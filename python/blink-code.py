import board
import digitalio
import time

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
x = 1

while True:
    x = x + 1
    print("Hello, CircuitPython!")
    print(x)
    led.value = True
    time.sleep(1)
    led.value = False
    time.sleep(1)
