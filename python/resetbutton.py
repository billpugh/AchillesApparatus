import time
import board
from digitalio import DigitalInOut, Direction, Pull

		
resetButton = DigitalInOut(board.D1)
resetButton.direction = Direction.INPUT
resetButton.pull = Pull.UP
# ----------------------------------------------------------
# Main loop
# ----------------------------------------------------------
while True:

    if resetButton.value:
        print("on")
    else:
        print("off")

    time.sleep(.1)
