require_relative 'device'
require_relative 'cloud'
require_relative 'osc_monitor'

module Oschii
  LOGO = '
  ╔═╗┌─┐┌─┐┬ ┬┬┬
  ║ ║└─┐│  ├─┤││  Sensor/Driver Interface
  ╚═╝└─┘└─┘┴ ┴┴┴
  '
  def self.included(base)
    base.class_eval do
      puts LOGO
    end
  end

  def cloud
    @cloud ||= Cloud.new.populate
  end

  def populate
    @cloud.nil? ? cloud : cloud.populate
  end

  def serial
    @serial ||= Device.find_serial
  end

  def method_missing(m, *args, &block)
    return if @cloud.nil?

    cloud.get(m)
  end
end

include Oschii

cloud
