import time
import random
import neopixel
import board
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn
from eartohear import EarToHear

DEBUG = True

if DEBUG:
    myI2C = 0x72    # hexaplexor
else:
    myI2C = 0x74    # tilemaze

last_system_mode = None
last_day_segment = None
last_light_level = None
level = 0

# This will be multiplied by difficulty: 0.0 - 1.0
if DEBUG:
    shuffleReps = 5
else:
    shuffleReps = 1000

Dred = (15, 0, 0)
Dwhite = (15, 15, 15)
Dblue = (0, 0, 15)
Dyellow = (15, 15, 0)
Dgreen = (0, 15, 0)
Dpurple = (15, 0, 15)
off = (0, 0, 0)

brightness = 15
Nred = (brightness, 0, 0)
Nwhite = (brightness, brightness, brightness)
Nblue = (0, 0, brightness)
Nyellow = (brightness, brightness, 0)
Ngreen = (0, brightness, 0)
Npurple = (brightness, 0, brightness)

colorMove = Nred             # blink when the tile moves
colorPattern = Nblue         # the tile pattern matches the goal pattern
colorMatch = Nwhite          # the tile pattern does NOT match the goal pattern
colorShuffle = Ngreen       # blink before shuffle
colorOff = off                 # no lights
colorHole = Npurple

uplt = [True, False, False, True, True, False]
uprt = [False, False, True, True, True, False]
lolt = [True, False, False, False, True, True]
lort = [False, False, True, False, True, True]
vert = [False, False, False, True, True, True]
horz = [True, False, True, False, True, False]
blnk = [False, False, False, False, False, False]

patterns = [blnk, uplt, uprt, lolt, lort, vert, horz]

cross = [True, False, True, True, True, True]
crosshole = [True, False, True, True, False, True]
center = [False, False, False, False, True, False]
hole = cross

# sound clip index start and end values
soundReset = [0, 6]
soundLeft = [10, 16]
soundRight = [20, 26]
soundUp = [30, 36]
soundDown = [40, 46]
soundShuffle = [50, 57]
soundProgress = [70, 79]
soundSuccess = [80, 88]
soundError = [90, 93]

# -------------------------------------------------------
# Set up I2C with address assigned by Bill.
ear = EarToHear(board.SCL, board.SDA, myI2C)

# ----------------------------------------------
# Define the local reset button input
#  attach D1 to switch post near LED
#  attach GND to post on back of switch
# ----------------------------------------------
resetButton = DigitalInOut(board.D1)
resetButton.direction = Direction.INPUT
resetButton.pull = Pull.UP

# -----------------------------------------------------------
# Define the pull-up inputs that sense if a tile is in place
# -----------------------------------------------------------
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
sense[3][3] = DigitalInOut(board.D22)  # D20 and D21 are used by I2C
sense[3][4] = DigitalInOut(board.D23)
sense[4][0] = DigitalInOut(board.D24)
sense[4][1] = DigitalInOut(board.D25)
sense[4][2] = DigitalInOut(board.D26)
sense[4][3] = DigitalInOut(board.D27)
sense[4][4] = DigitalInOut(board.D28)

# Initialize inputs
for r in range(5):
    for c in range(5):
        sense[r][c].direction = Direction.INPUT
        sense[r][c].pull = Pull.UP

# ----------------------------------------------------
# For slider potentiometers:
# 1 = ground
# 2 = Analog I/O -> A1
# 3 = 3.3v
# Difficuly -- complexity of the maze
# Chaos -- shuffle multiplier
# ----------------------------------------------------
difficulty = AnalogIn(board.A1)
chaos = AnalogIn(board.A2)

# ---------------------------------------------------------------------------
# Define the tile light strips as a 2D array
# ---------------------------------------------------------------------------
tiles = [[0 for r in range(5)] for c in range(5)]

tiles[0] = [
  neopixel.NeoPixel(board.D29, 6, auto_write=False),
  neopixel.NeoPixel(board.D30, 6, auto_write=False),
  neopixel.NeoPixel(board.D31, 6, auto_write=False),
  neopixel.NeoPixel(board.D32, 6, auto_write=False),
  neopixel.NeoPixel(board.D33, 6, auto_write=False)]
tiles[1] = [
  neopixel.NeoPixel(board.D34, 6, auto_write=False),
  neopixel.NeoPixel(board.D35, 6, auto_write=False),
  neopixel.NeoPixel(board.D36, 6, auto_write=False),
  neopixel.NeoPixel(board.D37, 6, auto_write=False),
  neopixel.NeoPixel(board.D38, 6, auto_write=False)]
tiles[2] = [
  neopixel.NeoPixel(board.D39, 6, auto_write=False),
  neopixel.NeoPixel(board.D40, 6, auto_write=False),
  neopixel.NeoPixel(board.D41, 6, auto_write=False),
  neopixel.NeoPixel(board.D42, 6, auto_write=False),
  neopixel.NeoPixel(board.D43, 6, auto_write=False)]
tiles[3] = [
  neopixel.NeoPixel(board.D44, 6, auto_write=False),
  neopixel.NeoPixel(board.D45, 6, auto_write=False),
  neopixel.NeoPixel(board.D46, 6, auto_write=False),
  neopixel.NeoPixel(board.D47, 6, auto_write=False),
  neopixel.NeoPixel(board.D48, 6, auto_write=False)]
tiles[4] = [
  neopixel.NeoPixel(board.D49, 6, auto_write=False),
  neopixel.NeoPixel(board.D50, 6, auto_write=False),
  neopixel.NeoPixel(board.D51, 6, auto_write=False),
  neopixel.NeoPixel(board.D52, 6, auto_write=False),
  neopixel.NeoPixel(board.D53, 6, auto_write=False)]

# ----------------------------------------------------------
# Define the edge light strip. NOTE: D54 -> A8
# edge = neopixel.NeoPixel(board.A15, 20, auto_write=False)
edge = neopixel.NeoPixel(board.A15, 20, auto_write=True)

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

def playSuccessLights(game):
    for x in range(10):
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    Dwhite)
        for e in range(20):
            edge[e] = Dblue
        time.sleep(0.25)
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    off)
        for e in range(20):
            edge[e] = off
        time.sleep(0.1)
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    Dblue)
        for e in range(20):
            edge[e] = Dwhite
        time.sleep(0.25)
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    off)
        for e in range(20):
            edge[e] = off
        time.sleep(0.1)
    clearBoard()

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
            if (matrix[r][c] == hole):
                print("XX  ", end="")

        print("")
    print("")

def clearBoard():
    if DEBUG:
        print("Clear board")
    for e in range(20):
        edge[e] = off
        edge.show
    for r in range(5):
        for c in range(5):
            showPattern(tiles[r][c], cross, off)

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
# shuffle the maze reps times
def shuffle(game, reps):
    printPattern(game)
    nR = 0
    nC = 0
    hR = 0
    hC = 0
    # find a hole
    for r in range(5):
        for c in range(5):
            if game[r][c] == hole:
                hR = r
                hC = c
            break
    # scale the requested repetitions by the chaos slider (0.0-1.0)
    chaosValue = chaos.value / 65536
    reps = int(reps * chaosValue)
    if DEBUG:
        print("Chaos Value, reps:", chaosValue, reps)
        time.sleep(3.0)
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
            game[nR][nC] = hole
            if (game[hR][hR] != blnk):
                if DEBUG:
                    print("Clip:",
                          random.randint(soundShuffle[0], soundShuffle[1]))
                ear.play_audio(
                    random.randint(soundShuffle[0], soundShuffle[1]))
                # blink the pattern in the old and new spaces
                showPattern(tiles[nR][nC], game[hR][hC], colorShuffle)
                showPattern(tiles[hR][hC], hole, colorHole)
                time.sleep(0.05)
                showPattern(tiles[nR][nC], hole, colorHole)
                showPattern(tiles[hR][hC], game[hR][hC], colorShuffle)
                time.sleep(0.2)
                showPattern(tiles[nR][nC], hole, off)
                showPattern(tiles[hR][hC], hole, off)
            hR = nR
            hC = nC
            printPattern(game)
    return hR, hC

# -----------------------------------------------------------
# Move tiles up and down
def moveRows(game, dist, ohR, ohC):
    # How many tiles moved? Which direction?
    dir = int(abs(dist)/dist)
    print("Dir & dist", dir, dist)
    # slide things up or down the correct distance
    for r in range(0, abs(dist)):
        game[ohR+r*dir][ohC] = \
            game[ohR+(r+1)*dir][ohC]
        game[ohR+(r+1)*dir][ohC] = hole
        showPattern(
            tiles[ohR+r*dir][ohC],
            game[ohR+r*dir][ohC],
            colorMove)
        if dir > 0:
            if DEBUG:
                print("Clip:", random.randint(soundUp[0], soundUp[1]))
            ear.play_audio(
                random.randint(soundUp[0], soundUp[1]))
        else:
            if DEBUG:
                print("Clip:", random.randint(soundDown[0], soundDown[1]))
            ear.play_audio(
                random.randint(soundDown[0], soundDown[1]))
        if DEBUG:
            printPattern(game)
        nhR = ohR+(r+1)*dir
    ohR = nhR

# ---------------------------------------------------------
# Move tiles right and left
def moveCols(game, dist, ohR, ohC):
    dir = int(abs(dist)/dist)
    print("Dir & dist", dir, dist)
    # slide things right or left the correct distance
    for c in range(0, abs(dist)):
        game[ohR][ohC+c*dir] = \
            game[ohR][ohC+(c+1)*dir]
        game[ohR][ohC+(c+1)*dir] = hole
        showPattern(
            tiles[ohR][ohC+c*dir],
            game[ohR][ohC+c*dir],
            colorMove)
        if dir > 0:
            if DEBUG:
                print("Clip:", random.randint(soundLeft[0], soundLeft[1]))
            ear.play_audio(
                random.randint(soundLeft[0], soundLeft[1]))
        else:
            if DEBUG:
                print("Clip:", random.randint(soundRight[0], soundRight[1]))
            ear.play_audio(
                random.randint(soundRight[0], soundRight[1]))
        if DEBUG:
            printPattern(game)
        nhC = ohC+(c+1)*dir
    ohC = nhC


# -----------------------------------------------------------
# How's it going?
def matchRealityToGoal(game, target):
    m = 0       # Num matches
    p = 0       # Num non-blanks
    for r in range(5):
        for c in range(5):
            # count the game pieces
            if (game[r][c] != blnk and game[r][c] != hole):
                p = p + 1
            # do they match?
            if (game[r][c] == hole):
                showPattern(tiles[r][c], game[r][c], colorHole)
            elif (game[r][c] == target[r][c]):
                showPattern(tiles[r][c], game[r][c], colorMatch)
                # and they are not blank?
                if (game[r][c] != blnk):
                    m = m + 1
            else:
                showPattern(tiles[r][c], game[r][c], colorPattern)

    # send back a percentage completion
    return p, m

def processMasterReset():
    if DEBUG:
        print("system_mode = MASTER RESET")
    # ??? ear.play_audio(soundSysReset)
    # play flashing red pattern then red X
    ear.set_points_bits(0)
    time.sleep(5.0)

def processLocalReset():
    if DEBUG:
        print("LOCAL RESET")
    ear.play_audio(
        random.randint(soundReset[0], soundReset[1]))
    clearBoard()
    # show a fancy pattern
    time.sleep(3.0)

def chooseGame(lev):
    # difficulty = value 0.0 = 1.0
    difficultyValue = difficulty.value / 65536
    lev = lev + 1 + int(difficultyValue * 8) - 3
    if lev > 7:
        lev = 7
    elif lev < 0:
        lev = 0
    instance = random.randint(0, 2)
    if DEBUG:
        print("Difficulty value, Level, instance:",
              difficultyValue, lev, instance)

    return lev, instance

def showAllLights():
    for r in range(5):
        for c in range(5):
            showPattern(tiles[r][c], cross, Dred)
    for e in range(20):
        edge[e] = Dblue

# -----------------------------------------------------------
# Setup mazes
# goal = desired ending pattern
# goalBegin = edge light indicator
# goalEnd = edge light indicator
# goalPieces = number of non-blank tiles
# d = difficulty level
# x = instance
# r = row
# c = column

goal = [[[[0 for c in range(5)]
        for r in range(5)]
        for x in range(3)]
        for d in range(8)]
goalBegin = [[0 for x in range(3)] for x in range(8)]
goalEnd = [[0 for x in range(3)] for x in range(8)]
goalPieces = [[0 for x in range(3)] for x in range(8)]

goal[7][0][0][0] = hole
goal[7][0][0][1] = lort
goal[7][0][0][2] = lolt
goal[7][0][0][3] = uprt
goal[7][0][0][4] = lolt
goal[7][0][1][0] = horz
goal[7][0][1][1] = uplt
goal[7][0][1][2] = vert
goal[7][0][1][3] = blnk
goal[7][0][1][4] = vert
goal[7][0][2][0] = lort
goal[7][0][2][1] = lolt
goal[7][0][2][2] = vert
goal[7][0][2][3] = lort
goal[7][0][2][4] = uplt
goal[7][0][3][0] = vert
goal[7][0][3][1] = uprt
goal[7][0][3][2] = uplt
goal[7][0][3][3] = uprt
goal[7][0][3][4] = lolt
goal[7][0][4][0] = uprt
goal[7][0][4][1] = horz
goal[7][0][4][2] = horz
goal[7][0][4][3] = horz
goal[7][0][4][4] = uplt

goalBegin[7][0] = 3
goalEnd[7][0] = 18
goalPieces[0][0] = countNonBlanks(goal[0][0])

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
localReset = False

# Define the matrix which tracks the positions of patterns on the board
matrix = [[0 for r in range(5)] for c in range(5)]

# ===========================================================
# Main loop
#   We get to the beginning of the loop when
#       - reboot
#       - puzzle solved
#       - TM reset button pressed
#       - master RESET
# ===========================================================
while True:

    # --------------------------------------------------------------
    # - clear board
    # - check for reset from master
    # - choose a game based on previous level and complexity slider
    # - assign the game to the work matrix
    # - shuffle the matrix depending on chaos slider
    #
    # -
    clearBoard()
    ear.check_i2c()

    # Do nothing until reset is cleared
    while ear.system_mode == EarToHear.MODE_RESET:
        processMasterReset()
        ear.check_i2c()

    # Pick a game and copy to matrix
    level, instance = chooseGame(level)
    if DEBUG:
        level = 7
        instance = 0
    for r in range(5):
        for c in range(5):
            matrix[r][c] = goal[level][instance][r][c]
    edge[goalBegin[level][instance]] = Dwhite
    edge[goalEnd[level][instance]] = Dwhite
    # -------------------------------------------------------------------
    # shuffle the tiles
    oldHoleRow, oldHoleCol = shuffle(matrix, shuffleReps)
    if DEBUG:
        print("Old row & col:", oldHoleRow, oldHoleCol)

    # ---------------------------------
    # Check match against goal
    pieces, match = matchRealityToGoal(matrix, goal[level][instance])

    if DEBUG:
        print("Matrix")
        printPattern(matrix)
        print("Goal")
        printPattern(goal[level][instance])

    # ========================================================
    # Game play loop
    # ========================================================
    while True:

        localReset = False

        ear.check_i2c()

        current_system_mode = ear.system_mode
        current_day_segment = ear.day_time
        current_light_level = ear.light_level

        # Abort the game if master says RESET
        if ear.system_mode == EarToHear.MODE_RESET:
            processMasterReset()
            break

        # Abort the game if local reset button is pushed
        localReset = resetButton.value
        if localReset:
            processLocalReset()
            break

        if DEBUG:
            print("Old: ", oldHoleRow, ",", oldHoleCol)
            numHoles = 1
            newHoleRow = int(input("New row: "))
            newHoleCol = int(input("New col: "))
            print("New: ", newHoleRow, ",", newHoleCol)
        # else look for button push

        rdist = newHoleRow - oldHoleRow
        cdist = newHoleCol - oldHoleCol
        # Did anything move?
        if (rdist == 0) and (cdist == 0):
            if DEBUG:
                print("No change:", oldHoleRow, ",", oldHoleCol)
        elif (rdist != 0) and (cdist != 0):
            # hole made impossible jump
            ear.play_audio(
                random.randint(soundError[0], soundError[1]))
            if DEBUG:
                print(
                    "Error:",
                    random.randint(soundError[0], soundError[1]))

        else:
            if rdist != 0:
                moveRows(matrix, rdist, oldHoleRow, oldHoleCol)
            else:  # must be cdist > 0
                moveCols(matrix, cdist, oldHoleRow, oldHoleCol)
                #            matrix[newHoleRow][newHoleCol] = blnk
            showPattern(
                tiles[newHoleRow][newHoleCol],
                matrix[newHoleRow][newHoleCol],
                off)
            oldHoleRow = newHoleRow
            oldHoleCol = newHoleCol

        if DEBUG:
            print("Goal")
            printPattern(goal[level][instance])
        pieces, matches = matchRealityToGoal(matrix, goal[level][instance])
        if DEBUG:
            print("Progress: ", matches, "out of", pieces)

        # Is it solved yet?
        # +++++++++++++ Find better clips to signify progress sounds
        progress = matches/pieces
        if (matches == pieces):
            if DEBUG:
                print("Success:",
                      random.randint(soundSuccess[0], soundSuccess[1]))
            ear.play_audio(random.randint(soundSuccess[0], soundSuccess[1]))
            playSuccessLights(matrix)
            ear.set_points(level)

            break
        elif progress > 0.9:
            if DEBUG:
                print("Progress 90%:",
                      random.randint(soundProgress[0], soundProgress[1]))
            ear.play_audio(random.randint(soundProgress[0], soundProgress[1]))
        elif progress > 0.75:
            if DEBUG:
                print("Progress 75%:",
                      random.randint(soundProgress[0], soundProgress[1]))
            ear.play_audio(random.randint(soundProgress[0], soundProgress[1]))
        elif progress > 0.5:
            if DEBUG:
                print("Progress 50%:",
                      random.randint(soundProgress[0], soundProgress[1]))
            ear.play_audio(random.randint(soundProgress[0], soundProgress[1]))