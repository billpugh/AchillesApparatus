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

blank = [False, False, False, False, False, False]
upleft = [True, False, False, True, True, False]
upright = [False, False, True, True, True, False]
lowleft = [True, False, False, False, True, True]
lowright = [False, False, True, False, True, True]
vert = [False, False, False, True, True, True]
horz = [True, False, True, False, True, False]

cross = [True, False, True, True, True, True]
crosshole = [True, False, True, True, False, True]
center = [False, False, False, False, True, False]


# For the specified tile, set the LEDs to the specifed pattern with the indicated color
def showPattern(tile, pattern, color):
    for index, led in enumerate(pattern):
        if led:
            tile[index] = color
        else:
            tile[index] = off
    tile.show()

# Fine the holes. If only one found, return the location
def findHoles(holeCount, holeRow, holeCol):
    x = 0
    for r in range (0,4):
        for c in range (0,4):
            if sense[r][c].value:
                x = x + 1
                holeRow = r
                holeCol = c
    holeCount = x


#-----------------------------------------------------------------------------------
# Define the tile light strips as a 2D array

tiles = [[0 for r in range(5)] for c in range(5)]

tiles[0] = [
  neopixel.NeoPixel(board.D28, 6, auto_write=False),
  neopixel.NeoPixel(board.D29, 6, auto_write=False),
  neopixel.NeoPixel(board.D30, 6, auto_write=False),
  neopixel.NeoPixel(board.D31, 6, auto_write=False),
  neopixel.NeoPixel(board.D32, 6, auto_write=False)]
tiles[1] = [
  neopixel.NeoPixel(board.D33, 6, auto_write=False),
  neopixel.NeoPixel(board.D34, 6, auto_write=False),
  neopixel.NeoPixel(board.D35, 6, auto_write=False),
  neopixel.NeoPixel(board.D36, 6, auto_write=False),
  neopixel.NeoPixel(board.D37, 6, auto_write=False)]
tiles[2] = [
  neopixel.NeoPixel(board.D38, 6, auto_write=False),
  neopixel.NeoPixel(board.D39, 6, auto_write=False),
  neopixel.NeoPixel(board.D40, 6, auto_write=False),
  neopixel.NeoPixel(board.D41, 6, auto_write=False),
  neopixel.NeoPixel(board.D42, 6, auto_write=False)]
tiles[3] = [
  neopixel.NeoPixel(board.D43, 6, auto_write=False),
  neopixel.NeoPixel(board.D44, 6, auto_write=False),
  neopixel.NeoPixel(board.D45, 6, auto_write=False),
  neopixel.NeoPixel(board.D46, 6, auto_write=False),
  neopixel.NeoPixel(board.D47, 6, auto_write=False)]
tiles[4] = [
  neopixel.NeoPixel(board.D48, 6, auto_write=False),
  neopixel.NeoPixel(board.D49, 6, auto_write=False),
  neopixel.NeoPixel(board.D50, 6, auto_write=False),
  neopixel.NeoPixel(board.D51, 6, auto_write=False),
  neopixel.NeoPixel(board.D52, 6, auto_write=False)]

# Define the edge light strip
edge = neopixel.NeoPixel(board.D53, 20, auto_write=False)

# Define the pull-up inputs that sense if a tile is in place
sense = [[0 for r in range(5)] for c in range(5)]

sense[0][0] = DigitalInOut(board.D2)
sense[0][1] = DigitalInOut(board.D3)
sense[0][2] = DigitalInOut(board.D4)
sense[0][3] = DigitalInOut(board.D5)
sense[0][4] = DigitalInOut(board.D6)
sense[1][0] = DigitalInOut(board.D7)
sense[1][1] = DigitalInOut(board.D8)
sense[1][2] = DigitalInOut(board.D9)
sense[1][3] = DigitalInOut(board.D10)
sense[1][4] = DigitalInOut(board.D11)
sense[2][0] = DigitalInOut(board.D12)
sense[2][1] = DigitalInOut(board.D13)
sense[2][2] = DigitalInOut(board.D14)
sense[2][3] = DigitalInOut(board.D15)
sense[2][4] = DigitalInOut(board.D16)
sense[3][0] = DigitalInOut(board.D17)
sense[3][1] = DigitalInOut(board.D18)
sense[3][2] = DigitalInOut(board.D19)
sense[3][3] = DigitalInOut(board.D20)
sense[3][4] = DigitalInOut(board.D21)
sense[4][0] = DigitalInOut(board.D23)
sense[4][1] = DigitalInOut(board.D24)
sense[4][2] = DigitalInOut(board.D25)
sense[4][3] = DigitalInOut(board.D26)
sense[4][4] = DigitalInOut(board.D27)

# Define the matrix which tracks the positions of patterns on the board
matrix = [[0 for r in range(5)] for c in range(5)]

# Initialize stuff
for r in range (0,4):
    for c in range (0,4):
        sense[r][c].direction = Direction.INPUT
        sense[r][c].pull = Pull.UP
        matrix[r][c] = blank

# Define misc stuff

countMultiHoles = 0
countMultiHolesLimit = 1_000

#----------------------------------------------------------
# Main loop
#----------------------------------------------------------
while True:

 
    if sense[1][1].value:
        showPattern(tiles[0][0],cross,red)
    else:
        showPattern(tiles[0][0],horz,blue)

    time.sleep(0.01)


