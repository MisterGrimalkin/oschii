require 'restclient'
require 'osc-ruby'
require 'osc-ruby/em_server'
require 'socket'

module Oschii
  class Cloud

    PING_PORT = 3333
    PING_ADDR = '/hello_oschii'
    RESPONSE_PORT = 3333
    RESPONSE_ADDR = '/i_am_oschii'

    def initialize
      @server = OSC::EMServer.new(RESPONSE_PORT)
      @oschii_data = []
      start_listening

      @http_handlers = {
          post: {}, get: {}, put: {}
      }

    end

    attr_reader :server, :oschii_data, :http_handlers

    def start_listening
      server.add_method RESPONSE_ADDR do |message|
        puts "#{message.ip_address} says '#{message.to_a.join(',')}'"
        oschii_data[message.ip_address] = message.to_a.first.split(':').last.strip
      end
      Thread.new do
        server.run
      end
      Thread.new do
        http_server = TCPServer.new 8000
        session = http_server.accept
        while true do
          method = nil
          path = nil
          value = nil
          while (request = session.gets&.chomp) do
            unless method && path
              method, path = request.split ' '
            end
            value = request
            session.puts "HTTP/1.1 200 OK\n\n"
          end
          if (block = http_handlers[method.downcase.to_sym][path])
            block.call value
          end
          session.close
          session = http_server.accept
        end
      end
    end

    def capture_osc(address, &block)
      actual_address = "/#{address}" unless address[0] == '/'
      server.add_method actual_address do |message|
        value = message.to_a.first&.to_i
        if block_given?
          yield value
        else
          puts "--> OSC #{address} [ #{value} ]"
        end
      end
    end

    def capture_http(path, method: :post, &block)
      actual_path = "/#{path}" unless path[0] == '/'
      unless block_given?
        block = ->(value) { puts "--> HTTP #{method.to_s.upcase} #{actual_path} [ #{value} ]" }
      end
      http_handlers[method.to_s.downcase.to_sym][actual_path] = block
    end

    def populate
      @oschii_data = {}
      puts 'Sending pings...'
      base_ip = local_ip_address.split('.')[0..2].join('.')
      (1..254).each do |i|
        target_ip = "#{base_ip}.#{i}"
        client = OSC::Client.new(target_ip, PING_PORT)
        client.send(OSC::Message.new(PING_ADDR, 1))
        sleep 0.001
      end
      puts 'DONE'

      puts 'Waiting for replies...'
      start_waiting = Time.now
      while Time.now - start_waiting < 3
        sleep 0.2
      end

      print_oschiis
      self
    end

    def print_oschiis
      puts
      puts 'IP Address       Name'
      sorted = oschii_data.to_a.sort_by { |_ip, name| name }.to_h
      sorted.each do |ip, name|
        puts "#{ip.ljust(16)} #{name}"
      end
      puts
    end

    def get(name)
      oschii_data.each do |ip, device_name|
        if device_name == name.to_s
          return Oschii::Device.new(ip: ip)
        end
      end
      puts "Cannot find Oschii named #{name}"
      nil
    end

    def inspect
      "<#{self.class.name} #{local_ip_address} devices: #{oschii_data.size}>"
    end

    def local_ip_address
      until (addr = Socket.ip_address_list.detect { |intf| intf.ipv4_private? })
      end
      addr.ip_address
    end
  end
end