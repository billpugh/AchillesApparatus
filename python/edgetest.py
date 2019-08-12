import board
import neopixel
import time
from digitalio import DigitalInOut, Direction, Pull

red = (255, 0, 0)
white = (255, 255, 255)
blue = (0, 0, 255)
green = (0, 255, 0)
yellow = (255, 255, 0)
off = (0, 0, 0)


# Define the edge light strip
edge = neopixel.NeoPixel(board.D53, 20, auto_write=True)

while True:

    for l in range(20):
        edge[l] = blue
        edge[l-1] = off
        time.sleep(0.09)
        



