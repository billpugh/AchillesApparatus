import time
import board
from analogio import AnalogIn

slider = AnalogIn(board.A1)

# 1 = ground
# 2 = Analog I/O
# 3 = 3.3v

def get_voltage(pin):
    return (pin.value * 3.3) / 65536

while True:
    print((get_voltage(slider),))
    time.sleep(0.1)