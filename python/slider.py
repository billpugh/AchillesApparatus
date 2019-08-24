import time
import board
from analogio import AnalogIn

slider = AnalogIn(board.A1)
slider2 = AnalogIn(board.A2)
# 1 = ground
# 2 = Analog I/O
# 3 = 3.3v

def get_voltage(pin):
    return (pin.value) / 65536

while True:
    print("1",(get_voltage(slider),))
    print("2",(get_voltage(slider2),))
    time.sleep(0.1)