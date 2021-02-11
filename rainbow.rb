require_relative 'cloud/oschii'

ada.upload_config_file 'examples/ada_rainbow.json'

sleep 5

leds = %i[red orange yellow green blue indigo violet]

value = 0
delta = 10

while true
  leds.each { |led| ada.send_osc led, value }
  value += delta
  if value >= 100
    value = 100
    delta = -delta
  elsif value <= 0
    value = 0
    delta = -delta
  end
  sleep 0.1
end