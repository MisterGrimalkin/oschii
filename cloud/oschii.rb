require_relative 'device'
require_relative 'cloud'

module Oschii
  LOGO = '
  ╔═╗┌─┐┌─┐┬ ┬┬┬
  ║ ║└─┐│  ├─┤││  Sensor/Actuator Driver
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
end

include Oschii
