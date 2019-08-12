import board
import neopixel
import time
from digitalio import DigitalInOut, Direction, Pull

red = (255, 0, 0)    # the tile moved
white = (255, 255, 255)  # the tile pattern matches the goal pattern
blue = (0, 0, 255)  # the tile pattern does NOT match the goal pattern
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


# For the tile, set the LEDs to specifed pattern and indicated color
def showPattern(tile, pattern, color):
    for index, led in enumerate(pattern):
        if led:
            tile[index] = color
        else:
            tile[index] = off
    tile.show()

# Print configuration of matrix
def printPattern(matrix):
    for r in range(0, 4):
        print(matrix[r])


# Find the holes. If only one found, return the location
def findHoles(holeCount, holeRow, holeCol):
    x = 0
    for r in range(0, 4):
        for c in range(0, 4):
            if sense[r][c].value:
                x = x + 1
                holeRow = r
                holeCol = c
    holeCount = x

# How's it going?
def matchRealityToGoal(matrix, goal, result, progress):
    progress = 0.0
    pieces = 0.0
    result = True
    for r in range(0, 4):
        for c in range(0, 4):

            # count the game pieces
            if (matrix[r][c] != blank):
                pieces = pieces + 1.0

            # do they match?
            if (matrix[r][c] == goal[r][c]):
                showPattern(tiles[r][c], matrix[r][c], white)

                # and they are not blank?
                if (matrix[r][c] != blank):
                    progress = progress + 1.0
            else:
                showPattern(tiles[r][c], matrix[r][c], blue)
                result = False

    # send back a percentage completion
    progress = pieces / progress

# ---------------------------------------------------------------------------
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
for r in range(0, 4):
    for c in range(0, 4):
        sense[r][c].direction = Direction.INPUT
        sense[r][c].pull = Pull.UP
        matrix[r][c] = blank

# Define misc stuff

countMultiHoles = 0
countMultiHolesLimit = 1_000

numHoles = 0
oldHoleRow = 0
oldHoleCol = 0
newHoleRow = 0
newHoleCol = 0
rdist = 0
vdist = 0
result = False
progress = 0.0

# ----------------------------------------------------------
# Main loop
# ----------------------------------------------------------
while True:

    # look for holes in the matrix
    findHoles(numHoles, newHoleRow, newHoleCol)

    # If the number of holes is more then one, mis-alignment or errors.
    # Count 'em for awhile and then complain
    if (numHoles > 1):
        countMultiHoles = countMultiHoles + 1
        if (countMultiHoles > countMultiHolesLimit):
            print("Hey, fix the tiles!!!")
    else:
        countMultiHoles = 0

        # Did anything move?
        if (oldHoleRow == newHoleRow) and (oldHoleCol == newHoleCol):
            print("No change:", oldHoleRow, ",", oldHoleCol)
        else:
            # Figure out how many tiles moved... could be more than one
            rdist = oldHoleRow - newHoleRow
            cdist = oldHoleCol - newHoleCol

            # Note: if rdist != 0 && vdist != 0 we have a big problem!!!

            if rdist != 0:
                # slide things up or down the correct distance
                for r in range(0, abs(vdist)-1):
                    matrix[oldHoleRow+r][oldHoleCol] = \
                        matrix[oldHoleRow+r+abs(rdist)/rdist][oldHoleCol]
                    showPattern(
                        tiles[oldHoleRow+r][oldHoleCol],
                        matrix[oldHoleRow+r][oldHoleCol],
                        red)
            else:  # must be hdist > 0
                # slide things right or left the correct distance
                for c in range(0, abs(cdist)-1):
                    matrix[oldHoleRow][oldHoleCol+c] = \
                        matrix[oldHoleRow][oldHoleCol+c+(abs(vdist)/vdist)]
                    showPattern(
                        tiles[oldHoleRow][oldHoleCol+c],
                        matrix[oldHoleRow][oldHoleCol+c],
                        red)

            matrix[newHoleRow][newHoleCol] = blank
            showPattern(
                tiles[newHoleRow][newHoleCol],
                matrix[newHoleRow][newHoleCol],
                off)
            print(
                "Moved: Old Hole:",
                oldHoleRow, ",",
                oldHoleCol, "  New Hole:",
                newHoleRow, ",",
                newHoleCol)
            oldHoleRow = newHoleRow
            oldHoleCol = newHoleCol

    # matchRealityToGoal(matrix, goal, result, progress)

    if result:
        print("We won!")

    if progress > .9:
        print("Almost there")
    elif progress > .75:
        print("Three-quartered")
    elif progress > 0.50:
        print("Half-way there!")
    elif progress > 0.25:
        print("Progress!")

    if sense[1][1].value:
        showPattern(tiles[0][0], vert, red)
    else:
        showPattern(tiles[0][0], horz, blue)

    time.sleep(0.01)



