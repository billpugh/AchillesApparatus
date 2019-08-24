import time
import random
import neopixel
import board
# import animator
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn
from eartohear import EarToHear
from mazes import goal, goalBegin, goalEnd, \
    uplt, uprt, lolt, lort, vert, horz, blnk, hole
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# TO DO
#   - Use time of day to set the light levels
#       + LIGHT_NIGHT
#       + LIGHT_NAUTICAL_TWILIGHT
#       + LIGHT_CIVIL_TWILIGHT
#       + LIGHT_DUSK_DAWN
#       + LIGHT_DAY
#   - Do something different with:
#       + QUIET (play random noises)
#       + ACTIVE (game play)
#       + CHARGED (outer to inner, faster & faster)
#       + DISCHARGING (inner to outer, faster & faster, flash, fade)
#       + RESET (stop game, flash code)
#       + NOT_RECEIVED (error code: Edge all red)
#   + Show pattern while shuffling
#   + Review audio
#   + Blink hole and selected tile if button is pressed for illegal move
#   + Mazes
#       + Level 0
#       + Level 0
#       + Level 1
#       + Level 2
#       + Level 3
#       + Level 4
#       + Level 5
#       + Level 6
#       + Level 7
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DEBUG = True

# This will be multiplied by difficulty: 0.0 - 1.0
shuffleScale = 100
    
# anim = animator.Animator()

if DEBUG:
    myI2C = 0x72    # hexaplexor
else:
    myI2C = 0x74    # tilemaze

last_system_mode = None
last_day_segment = None
last_light_level = None
level = 0

bright = 5
colorEntry = (bright, bright, 0)        # yellow
colorExit = (0, bright, 0)              # green
colorPattern = (0, 0, bright)           # blue (tile does NOT match goal)
colorMatch = (bright, bright, bright)   # white (tile matches goal)
colorMove = (bright, 0, 0)              # red
colorHole = (bright, 0, bright)         # purple
colorOff = (0, 0, 0)                    # no lights

# uplt = [True, False, False, True, True, False]
# uprt = [False, False, True, True, True, False]
# lolt = [True, False, False, False, True, True]
# lort = [False, False, True, False, True, True]
# vert = [False, False, False, True, True, True]
# horz = [True, False, True, False, True, False]
# blnk = [False, False, False, False, False, False]

cross = hole
top = [False, False, False, True, False, False]
bottom = [False, False, False, False, False, True]
left = [True, False, False, False, False, False]
right = [False, False, True, False, False, False]
crosshole = [True, False, True, True, False, True]
center = [False, False, False, False, True, False]


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
# default for sense[][].value = FALSE
# ----------------------------------------------
resetButton = DigitalInOut(board.D1)
resetButton.direction = Direction.INPUT
resetButton.pull = Pull.UP

# -----------------------------------------------------------
# Define the pull-up inputs that sense if a tile is in place
# small matrix switches: default for sense[][].value = TRUE
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
sense[2][1] = DigitalInOut(board.A14)   # D13 doesn't work
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
chaos = AnalogIn(board.A1)
difficulty = AnalogIn(board.A2)


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

def changeBrightness():
    global colorEntry, colorExit, colorMove
    global colorPattern, colorMatch, colorHole

    if last_light_level == EarToHear.LIGHT_DAY:
        bright = 250
    elif last_light_level == EarToHear.LIGHT_DUSK_DAWN:
        bright = 100
    elif last_light_level == EarToHear.CIVIL_TWILIGHT:
        bright = 50
    elif last_light_level == EarToHear.NAUTICAL_TWILIGHT:
        bright = 40
    elif last_light_level == EarToHear.NIGHT:
        bright = 25
    else:
        bright = 25
    if DEBUG:
        bright = 5

    colorEntry = (bright, bright, 0)        # yellow
    colorExit = (0, bright, 0)              # green
    colorPattern = (0, 0, bright)           # blue (tile does NOT match goal)
    colorMatch = (bright, bright, bright)   # white (tile matches goal)
    colorMove = (bright, 0, 0)              # red
    colorHole = (bright, 0, bright)         # purple

# ------------------------------------------------------------------
# For the tile, set the LEDs to specifed pattern and indicated color
# ------------------------------------------------------------------
def showPattern(tile, pattern, color):
    for index, led in enumerate(pattern):
        if led:
            tile[index] = color
        else:
            tile[index] = colorOff
    tile.show()

def playSuccessLights(game):
    for x in range(10):
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    colorMatch)
        for e in range(20):
            edge[e] = colorPattern
        time.sleep(0.25)
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    colorOff)
        for e in range(20):
            edge[e] = colorOff
        time.sleep(0.1)
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    colorPattern)
        for e in range(20):
            edge[e] = colorMatch
        time.sleep(0.25)
        for r in range(5):
            for c in range(5):
                showPattern(
                    tiles[r][c],
                    matrix[r][c],
                    colorOff)
        for e in range(20):
            edge[e] = colorOff
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
        edge[e] = colorOff
        edge.show
    for r in range(5):
        for c in range(5):
            showPattern(tiles[r][c], cross, colorOff)

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
def shuffle(game):
    printPattern(game)
    nR = 0
    nC = 0
    hR = 0
    hC = 0
    # find the hole
    for r in range(5):
        for c in range(5):
            if game[r][c] == hole:
                hR = r
                hC = c
    # flash the pattern 10 times
    for x in range(10):
        for r in range(5):
            for c in range(5):
                showPattern(tiles[r][c], game[r][c], colorOff)
        time.sleep(0.02)
        for r in range(5):
            for c in range(5):
                if game[r][c] == hole:
                    showPattern(tiles[r][c], game[r][c], colorHole)
                else:
                    showPattern(tiles[r][c], game[r][c], colorMatch)
        time.sleep(0.4)

    # scale the requested repetitions by the chaos slider (0.0-1.0)
    chaosValue = chaos.value / 65536
    reps = int(shuffleScale * chaosValue)
    if DEBUG:
        print("Chaos Value, reps:", chaosValue, reps)    
    if reps <= 4:
        reps = 5
        if DEBUG:
            print("Reset to min reps", reps)      
            
    # check to make sure something has moved (pieces != matches)
    p = 0
    m = 0
    while True:
        # shuffle reps times
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
                    ear.play_audio(
                        random.randint(soundShuffle[0], soundShuffle[1]))
                    # blink the pattern in the old and new spaces
                    showPattern(tiles[nR][nC], game[hR][hC], colorMove)
                    time.sleep(0.02)
                    showPattern(tiles[hR][hC], game[hR][hC], colorMove)
                    time.sleep(0.05)
                    showPattern(tiles[nR][nC], hole, colorHole)
                    if game[hR][hC] == goal[level][instance][hR][hC]:
                        showPattern(tiles[hR][hC], game[hR][hC], colorMatch)
                    else:
                        showPattern(tiles[hR][hC], game[hR][hC], colorPattern)
                    time.sleep(0.07)
                hR = nR
                hC = nC
                # printPattern(game)
        p, m = matchRealityToGoal(matrix, goal[level][instance])
        if DEBUG:
            print("pieces, matches", p, m)
        if (p != m):
            break
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

def getColor(tile):
    color = colorOff
    for index in range(6):
        if tile[index] != colorOff:
            color = tile[index]
    print("getcolor", color)
    return color

def blinkTile(tile, blinkPattern, color, reps, oldPattern, oldColor):
    for x in range(reps):
        showPattern(tile, blinkPattern, color)
        time.sleep(0.1)
        showPattern(tile, blinkPattern, colorOff)
        time.sleep(0.05)
    showPattern(tile, oldPattern, oldColor)

def processMasterReset():
    if DEBUG:
        print("system_mode = MASTER RESET")

    clearBoard
    # +++++ display big red X
    ear.set_points_bits(0)

def processLocalReset():
    if DEBUG:
        print("LOCAL RESET")
    ear.play_audio(
        random.randint(soundReset[0], soundReset[1]))
    clearBoard
    # ++++++++++ flash all white on & off

def checkReset():
    global current_system_mode
    localReset = False
    masterReset = False

    localReset = not resetButton.value
    if localReset:
        processLocalReset()

    ear.check_i2c()
    current_system_mode = ear.system_mode
    masterReset = ear.system_mode == EarToHear.MODE_RESET
    if masterReset:
        processMasterReset()

    return localReset or masterReset

# TEST THE LIGHTS
def showAllLights():
    for r in range(5):
        for c in range(5):
            showPattern(tiles[r][c], cross, colorMove)
    for e in range(20):
        edge[e] = colorPattern

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

goalPieces = [[0 for x in range(3)] for x in range(8)]

# ----------------------------------------
# Define misc stuff
# ----------------------------------------
r = 0
c = 0
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
reset = False
press = False
levelsDone = [False, False, False, False, False, False, False, False]

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

    # =======================================================
    # Levels loop
    # =======================================================
    for level in range(8):
        # --------------------------------------------------------------
        # - clear board
        # - check for reset from master
        # - choose a game based on previous level and complexity slider
        # - assign the game to the work matrix
        # - shuffle the matrix depending on chaos slider

        clearBoard()
        ear.check_i2c()

        # Do nothing until reset is cleared
        while ear.system_mode == EarToHear.MODE_RESET:
            processMasterReset()
            ear.check_i2c()

        instance = random.randint(0, 2)

        for r in range(5):
            for c in range(5):
                matrix[r][c] = goal[level][instance][r][c]
        edge[goalBegin[level][instance]] = colorEntry
        edge[goalEnd[level][instance]] = colorExit

        # -------------------------------------------------------------------
        # shuffle the tiles
        # -------------------------------------------------------------------
        oldHoleRow, oldHoleCol = shuffle(matrix)
        # pieces, matches = matchRealityToGoal(matrix, goal[level][instance])
        if DEBUG:
            print("Old row & col:", oldHoleRow, oldHoleCol)
            print("Matrix")
            printPattern(matrix)
            print("Goal")
            printPattern(goal[level][instance])

        localReset = False
        # ========================================================
        # Game play loop
        # ========================================================
        while True:

            # Look for button presses. Also check for resets
            time.sleep(0.2)
            press = False
            reset = False
            while not (press or reset):
                for r in range(5):
                    for c in range(5):
                        press = not sense[r][c].value
                        if press:
                            newHoleRow = r
                            newHoleCol = c
                            break
                    if press:
                        break
                reset = checkReset()

            if reset:
                break

            rdist = newHoleRow - oldHoleRow
            cdist = newHoleCol - oldHoleCol
            currentColor = getColor(
                tiles[oldHoleRow][oldHoleCol])
            currentPattern = matrix[oldHoleRow][oldHoleCol]
            # Did anything move?
            print("Old:", oldHoleRow, oldHoleCol)
            print("New:", newHoleRow, newHoleCol)

            if (rdist == 0) and (cdist == 0):
                blinkTile(
                    tiles[oldHoleRow][oldHoleCol],
                    crosshole,
                    colorMove,
                    2,
                    currentPattern,
                    currentColor)
                if DEBUG:
                    print("No change:", oldHoleRow, ",", oldHoleCol)

            elif (rdist != 0) and (cdist != 0):
                # hole made impossible jump
                blinkTile(
                    tiles[newHoleRow][newHoleCol],
                    crosshole,
                    colorMove,
                    1,
                    currentPattern,
                    currentColor)
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
                showPattern(
                    tiles[newHoleRow][newHoleCol],
                    matrix[newHoleRow][newHoleCol],
                    colorOff)
                oldHoleRow = newHoleRow
                oldHoleCol = newHoleCol

            # How are we doing?
            pieces, matches = matchRealityToGoal(matrix, goal[level][instance])
            if DEBUG:
                print("Goal")
                printPattern(goal[level][instance])
                print("Progress: ", matches, "out of", pieces)

            # +++++++++++++ Find better clips to signify progress sounds
            progress = matches/pieces
            if (matches == pieces):
                if DEBUG:
                    print("Success:",
                          random.randint(soundSuccess[0], soundSuccess[1]))
                ear.play_audio(
                    random.randint(
                        soundSuccess[0], soundSuccess[1]))
                playSuccessLights(matrix)
                ear.set_points(level)
                break

            elif progress > 0.9:
                if DEBUG:
                    print("Progress 90%:",
                          random.randint(soundProgress[0], soundProgress[1]))
                ear.play_audio(
                    random.randint(
                        soundProgress[0], soundProgress[1]))
            elif progress > 0.75:
                if DEBUG:
                    print("Progress 75%:",
                          random.randint(soundProgress[0], soundProgress[1]))
                ear.play_audio(
                    random.randint(
                        soundProgress[0], soundProgress[1]))
            elif progress > 0.5:
                if DEBUG:
                    print("Progress 50%:",
                          random.randint(soundProgress[0], soundProgress[1]))
                ear.play_audio(
                    random.randint(
                        soundProgress[0], soundProgress[1]))