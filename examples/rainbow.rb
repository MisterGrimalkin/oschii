def rainbow(oschii, mode: 'pulse', time: 500, delay: 0)
  oschii.send_osc 'red', [mode, 'repeat', 100, time, 0, time+(delay*6)]
  oschii.send_osc 'orange', [mode, 'repeat', 0, delay, 100, time, 0, time+(delay*5)]
  oschii.send_osc 'yellow', [mode, 'repeat', 0, delay*2, 100, time, 0, time+(delay*4)]
  oschii.send_osc 'green', [mode, 'repeat', 0, delay*3, 100, time, 0, time+(delay*3)]
  oschii.send_osc 'blue', [mode, 'repeat', 0, delay*4, 100, time, 0, time+(delay*2)]
  oschii.send_osc 'indigo', [mode, 'repeat', 0, delay*5, 100, time, 0, time+delay]
  oschii.send_osc 'violet', [mode, 'repeat', 0, delay*6, 100, time, 0, time]
end