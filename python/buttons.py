import time
import board
from digitalio import DigitalInOut, Direction, Pull

print("starting")
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
sense[2][1] = DigitalInOut(board.A8)    # D13 doesnt work right
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
for r in range(5):
    for c in range(5):
        sense[r][c].direction = Direction.INPUT
        sense[r][c].pull = Pull.UP

resetButton = DigitalInOut(board.D1)
resetButton.direction = Direction.INPUT
resetButton.pull = Pull.UP
# ----------------------------------------------------------
# Main loop
# ----------------------------------------------------------
while True:

    if resetButton.value:
        print("R:on")
    else:
        print("R:off")

    press = False
    for r in range(5):
        for c in range(5):
            if not sense[r][c].value:
                print(r, c)
                press = True
                break
        if press:
                break

    time.sleep(.1)