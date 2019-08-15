import time
import random
import neopixel
import board
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn


Dred = (255, 0, 0)
Dwhite = (255, 255, 255)
Dblue = (0, 0, 255)
Dyellow = (255, 255, 0)
Dgreen = (0, 255, 0)
off = (0, 0, 0)

brightness = 50
Nred = (brightness, 0, 0)
Nwhite = (brightness, brightness, brightness)
Nblue = (0, 0, brightness)
Nyellow = (brightness, brightness, 0)
Ngreen = (0, brightness, 0)

colorMove = Nred             # blink when the tile moves
colorPattern = Nblue         # the tile pattern matches the goal pattern
colorMatch = Nwhite          # the tile pattern does NOT match the goal pattern
colorPreshuffle = Nyellow    # blink before shuffle
colorPostshuffle = Ngreen    # blink after shuffle
colorOff = off              # no lights

uplt = [True, False, False, True, True, False]
uprt = [False, False, True, True, True, False]
lolt = [True, False, False, False, True, True]
lort = [False, False, True, False, True, True]
vert = [False, False, False, True, True, True]
horz = [True, False, True, False, True, False]
blnk = [False, False, False, False, False, False]
hole = blnk

patterns = [blnk, uplt, uprt, lolt, lort, vert, horz]

cross = [True, False, True, True, True, True]
crosshole = [True, False, True, True, False, True]
center = [False, False, False, False, True, False]

# number of clips per sound type, 0 through X
soundReset = [0, 6]
soundLeft = [10, 16]
soundRight = [20, 26]
soundUp = [30, 36]
soundDown = [40, 46]
soundShuffle = [50, 57]
soundProgress = [70, 79]
soundSuccess = [80, 88]
soundError = [90, 93]


# ---------------------------------------------------------------------------
# Define the tile light strips as a 2D array
# ---------------------------------------------------------------------------
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

# ----------------------------------------------------------
# Define the edge light strip
edge = neopixel.NeoPixel(board.D53, 20, auto_write=False)

# -----------------------------------------------------------
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

# Initialize inputs
for r in range(5):
    for c in range(5):
        sense[r][c].direction = Direction.INPUT
        sense[r][c].pull = Pull.UP

#  attach D1 to switch post near LED
#  attach GND to post on back of switch
resetButton = DigitalInOut(board.D1)
resetButton.direction = Direction.INPUT
resetButton.pull = Pull.UP

# ----------------------------------------------------
# For slider potentiometer:
# 1 = ground
# 2 = Analog I/O
# 3 = 3.3v
# ----------------------------------------------------
slider = AnalogIn(board.A1)

def getValue(pin):
    return (pin.value) / 65536

# ------------------------------------------------------------------
# For the tile, set the LEDs to specifed pattern and indicated color
# ------------------------------------------------------------------
def showPattern(tile, pattern, color):
    for index, led in enumerate(pattern):
        if led:
            tile[index] = color
        else:
            tile[index] = off
    tile.show()

def playSound(sound):
    print("Sound: ", sound)

# Print configuration of matrix
def printPattern(matrix):
    for r in range(5):
        for c in range(5):
            if (matrix[r][c] == blnk):
                print("    ", end="")
            if (matrix[r][c] == uplt):
                print("UL  ", end="")
            if (matrix[r][c] == uprt):
                print("UR  ", end="")
            if (matrix[r][c] == lolt):
                print("LL  ", end="")
            if (matrix[r][c] == lort):
                print("LR  ", end="")
            if (matrix[r][c] == vert):
                print("VT  ", end="")
            if (matrix[r][c] == horz):
                print("HZ  ", end="")
        print("")
    print("")

# --------------------------------------------
# count the number of non-blank patterns
def countNonBlanks(game):
    nonBlank = 0
    for r in range(5):
        for c in range(5):
            if game[r][c] != blnk:
                nonBlank = nonBlank + 1
    return nonBlank

# ----------------------------------------------
# shuffle the maze rep times
def shuffle(game, reps):
    nR = 0
    nC = 0
    # find a hole
    for r in range(5):
        for c in range(5):
            if game[r][c] == blnk:
                hR = r
                hC = c
            break
    for r in range(reps):
        d = random.randint(0, 3)
        # Move down
        if d == 0:
            if hR != 0:
                nR = hR-1
                nC = hC
        # Move up
        elif d == 1:
            if hR != 4:
                nR = hR+1
                nC = hC
        # Move right
        elif d == 2:
            if hC != 0:
                nR = hR
                nC = hC-1
        # Move left
        else:
            if hC != 4:
                nR = hR
                nC = hC+1
        if (nR != hR) or (nC != hC):
            game[hR][hC] = game[nR][nC]
            game[nR][nC] = blnk
            if (game[hR][hR] != blnk):

                # blink the pattern in the old space
                showPattern(tiles[nR][nC], game[hR][hC], colorPreshuffle)
                time.sleep(0.1)
                showPattern(tiles[nR][nC], game[hR][hC], off)

                # blink the pattern in the new space
                showPattern(tiles[hR][hC], game[hR][hC], colorPostshuffle)
                time.sleep(0.1)
                showPattern(tiles[hR][hC], game[hR][hC], off)
                playSound(random.randint(soundShuffle[0], soundShuffle[1]))
            hR = nR
            hC = nC
            printPattern(game)
    return hR, hC

# -----------------------------------------------------------
# Find the holes. Return location of last one found
def findHoles():
    hR = 0
    hC = 0
    hNum = 0
    for r in range(5):
        for c in range(5):
            if sense[r][c].value:
                hNum = hNum + 1
                hR = r
                hC = c
    return hNum, hR, hC

# -----------------------------------------------------------
# Move tiles up and down
def moveRows(game, dist, ohR, ohC):
    # How many tiles moved? Which direction?
    dir = int(abs(dist)/dist)
    # slide things up or down the correct distance
    for r in range(0, abs(dist)):
        game[ohR+r*dir][ohC] = \
            game[ohR+(r+1)*dir][ohC]
        showPattern(
            tiles[ohR+r*dir][ohC],
            game[ohR+r*dir][ohC],
            colorMove)
        if dir > 0:
            playSound(random.randint(soundUp[0], soundUp[1]))
        else:
            playSound(random.randint(soundDown[0], soundDown[1]))
        printPattern(game)

# ---------------------------------------------------------
# Move tiles right and left
def moveCols(game, dist, ohR, ohC):
    # slide things right or left the correct distance
    dir = int(abs(dist)/dist)
    for c in range(0, abs(dist)):
        game[ohR][ohC+c*dir] = \
            game[ohR][ohC+(c+1)*dir]
        showPattern(
            tiles[ohR][ohC+c*dir],
            game[ohR][ohC+c*dir],
            colorMove)
        if dir > 0:
            playSound(random.randint(soundLeft[0], soundLeft[1]))
        else:
            playSound(random.randint(soundRight[0], soundRight[1]))
        printPattern(game)

# -----------------------------------------------------------
# How's it going?
def matchRealityToGoal(game, target):
    m = 0       # Num matches
    p = 0       # Num non-blanks
    for r in range(5):
        for c in range(5):

            # count the game pieces
            if (game[r][c] != blnk):
                p = p + 1
            # do they match?
            if (game[r][c] == target[r][c]):
                showPattern(tiles[r][c], game[r][c], colorMatch)
                # and they are not blank?
                if (game[r][c] != blnk):
                    m = m + 1
            else:
                showPattern(tiles[r][c], game[r][c], colorPattern)

    # send back a percentage completion
    return p, m

# -----------------------------------------------------------
# Setup mazes
goal = [[0 for r in range(5)] for c in range(5)]
goal[0][0] = blnk
goal[0][1] = lort
goal[0][2] = lolt
goal[0][3] = uprt
goal[0][4] = lolt
goal[1][0] = horz
goal[1][1] = uplt
goal[1][2] = vert
goal[1][3] = blnk
goal[1][4] = vert
goal[2][0] = lort
goal[2][1] = lolt
goal[2][2] = vert
goal[2][3] = lort
goal[2][4] = uplt
goal[3][0] = vert
goal[3][1] = uprt
goal[3][2] = uplt
goal[3][3] = uprt
goal[3][4] = lolt
goal[4][0] = uprt
goal[4][1] = horz
goal[4][2] = horz
goal[4][3] = horz
goal[4][4] = uplt
goalBegin = 3
goalEnd = 18
goalComplexity = countNonBlanks(goal)

# ----------------------------------------
# Define misc stuff
# ----------------------------------------
countMultiHoles = 0
countMultiHolesLimit = 1_000

numHoles = 0
oldHoleRow = 0
oldHoleCol = 0
newHoleRow = 0
newHoleCol = 0
rdist = 0
cdist = 0
pieces = 0
matches = 0
progress = 0.0

shuffleReps = 10

# Define the matrix which tracks the positions of patterns on the board
matrix = [[0 for r in range(5)] for c in range(5)]

# -----------------------------------------------------------
# Main loop, eventually
# -----------------------------------------------------------

# copy goal to matrix
for r in range(5):
    for c in range(5):
        matrix[r][c] = goal[r][c]

# ----------------------------
# Determine difficulty 0-8
difficulty = int(getValue(slider) * 9)

# ----------------------------------
# Figure out reset button
# ------------------------

# -------------------------------------------------------------------
# shuffle
oldHoleRow, oldHoleCol = shuffle(matrix, shuffleReps)

# ----------------------------------------------------
# Find physical hole and move virtual hole to match
# numHoles, newHoleRow, newHoleCol = findHoles()
numHoles = 1
newHoleRow = int(input("Physical hole row: "))
newHoleCol = int(input("Physical hole col: "))

if numHoles == 0:
    playSound("No holes!!!!")
elif numHoles > 1:
    playSound("Tiles not aligned")
else:
    if newHoleRow-oldHoleRow != 0:
        moveRows(matrix, newHoleRow-oldHoleRow, oldHoleRow, oldHoleCol)
        oldHoleRow = newHoleRow
        matrix[oldHoleRow][oldHoleCol] = blnk
    if newHoleCol-oldHoleCol != 0:
        moveCols(matrix, newHoleCol-oldHoleCol, oldHoleRow, oldHoleCol)
        oldHoleCol = newHoleCol
        matrix[oldHoleRow][oldHoleCol] = blnk

# ---------------------------------
# Check match agains goal
pieces, match = matchRealityToGoal(matrix, goal)

print("Matrix")
printPattern(matrix)
print("Goal")
printPattern(goal)

# ----------------------------------------------------------
# Play loop
# ----------------------------------------------------------
while True:

    # look for physical holes in the game table
    # numHoles, newHoleRow, newHoleCol = findHoles()
    # +++++++ do it manually for now
    print("Old: ", oldHoleRow, ",", oldHoleCol)
    numHoles = 1
    newHoleRow = int(input("New row: "))
    newHoleCol = int(input("New col: "))

    # If num holes > 1, then mis-alignment or errors.
    # Count 'em for awhile and then complain
    if (numHoles > 1):
        countMultiHoles = countMultiHoles + 1
        if (countMultiHoles > countMultiHolesLimit):
            playSound(random.randint(soundError[0], soundError[1]))
    else:
        countMultiHoles = 0
        print("Old: ", oldHoleRow, ",", oldHoleCol)
        print("New: ", newHoleRow, ",", newHoleCol)
        rdist = newHoleRow - oldHoleRow
        cdist = newHoleCol - oldHoleCol
        # Did anything move?
        if (rdist == 0) and (cdist == 0):
            print("No change:", oldHoleRow, ",", oldHoleCol)
        else:
            if (rdist != 0) and (cdist != 0):
                playSound(random.randint(soundError[0], soundError[1]))
            if rdist != 0:
                moveRows(matrix, rdist, oldHoleRow, oldHoleCol)
            else:  # must be cdist > 0
                moveCols(matrix, cdist, oldHoleRow, oldHoleCol)
            matrix[newHoleRow][newHoleCol] = blnk
            showPattern(
                tiles[newHoleRow][newHoleCol],
                matrix[newHoleRow][newHoleCol],
                off)
            oldHoleRow = newHoleRow
            oldHoleCol = newHoleCol

    print("Matrix")
    printPattern(matrix)
    print("goal")
    printPattern(goal)
    pieces, matches = matchRealityToGoal(matrix, goal)
    print("Progress: ", matches, "out of", pieces)

    progress = matches/pieces
    if (matches == pieces):
        playSound(random.randint(soundSuccess[0], soundSuccess[1]))
    elif progress > 0.9:
        playSound(random.randint(soundProgress[0], soundProgress[1]))
    elif progress > 0.75:
        playSound(random.randint(soundProgress[0], soundProgress[1]))
    elif progress > 0.5:
        playSound(random.randint(soundProgress[0], soundProgress[1]))


