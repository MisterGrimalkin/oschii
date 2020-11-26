require 'restclient'
require 'rubyserial'
require 'json'
require 'byebug'

module Oschii
  BAUD_RATE = 115200

  class Device
    def initialize(connection)
      if (@ip = connection[:ip])
        @connection_type = :http
      elsif (@serial = connection[:serial])
        @connection_type = :serial
      else
        raise StandardError, 'Unknown connection type'
      end
      @osc_clients = {}
      connect
    end

    attr_reader :name, :ip, :osc_clients

    def send_osc(address, value = 1, port: 3333)
      address = "/#{address}" unless address[0] == '/'
      osc_client(port).send(OSC::Message.new(address, *value))
    end

    def osc_client(port)
      osc_clients[port] ||= OSC::Client.new(ip, port)
    end

    def send_http(path, value = 1, method: :post)
      path = "/#{path}" unless %w(/ :).include?(path[0])
      method = method.to_s.downcase.to_sym
      url = "http://#{ip}#{path}"
      if method == :get
        args = [url]
      else
        args = [url, value.to_s]
      end
      puts RestClient.send(method, *args)
    rescue RestClient::BadRequest => e
      puts "Oschii didn't like that: #{e.http_body}"
    end

    def connect
      case connection_type
        when :http
          @name = RestClient.get "http://#{ip}/name"
        when :serial
          purge_serial
          check_sync
          @name = serial_query 'name'
          @ip = serial_query 'ip'
        else
          puts 'wtf?'
      end
      self
    end

    def update(name: nil, ssid: nil, password: nil)
      case connection_type
        when :http
          update_via_http(name, ssid, password)
        when :serial
          update_via_serial(name, ssid, password)
      end
      connect
    end

    def upload_config_file(filename)
      upload_config JSON.parse(File.read(filename))
    end

    def upload_config(json)
      case connection_type
        when :http
          begin
            puts RestClient.post "http://#{ip}/config", json.to_json
          rescue RestClient::BadRequest => e
            puts "Error updating Oschii:\n\t#{e.http_body}"
          end
        when :serial
          if serial_query('set config') == 'Ready for config'
            puts serial_query(json.to_json)
          end
      end
      connect
    end

    def download_config
      if connection_type == :http
        JSON.parse RestClient.get "http://#{ip}/config"
      end
    end

    def update_via_http(name, ssid, password)
      if name
        puts RestClient.post "http://#{ip}/name", name
      end
      if ssid || password
        puts 'Changing wifi credentials over wifi is neither permitted nor sensible'
      end
      connect
    end

    def update_via_serial(name, ssid, password)
      purge_serial
      if name
        if serial_query('set name') == 'Ready for name'
          puts serial_query(name)
        end
      end
      if ssid && password
        if serial_query('start wifi') == 'Ready for ssid'
          if serial_query(ssid) == 'Ready for password'
            puts serial_query(password)
          end
        end
      end
    end

    def check_sync
      poke = serial_query('poke')
      while poke != 'Tickles!'
        serial_query("\n")
        purge_serial
        poke = serial_query('poke')
        sleep 0.5
      end
    end

    def purge_serial
      input = serial_port.read 100
      print input
      until input == ''
        input = serial_port.read 100
        print input
      end
    end

    def serial_query(query)
      print "#{query}/" unless query == "\n"
      serial_port.write (query.empty? ? "\n" : query)
      input = ''
      started = Time.now
      while input.empty? && (Time.now - started < 2)
        input = serial_port.read 100
        sleep 0.2
      end
      puts input
      input.strip
    end

    def inspect
      "<#{self.class.name}[#{connection_type==:serial ? '~' : ''}#{name}] #{ip}>"
    end

    attr_reader :connection_type, :serial

    def serial_port
      @serial_port ||= Serial.new serial, BAUD_RATE
    end
  end
end
