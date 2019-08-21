import animator
import board
import neopixel
import time


anim = animator.Animator()

tile = neopixel.NeoPixel(board.D29, 6, auto_write=False)
cross = [True, False, True, True, True, True]
colorOff = (0, 0, 0)

def draw_pattern(tile, pattern, color):
    for index, led in enumerate(pattern):
        if led:
            tile[index] = color
        else:
            tile[index] = colorOff
        tile.show()
        print('DRAW PIXEL {}'.format(index))
        yield 1  # wait 1 second

def draw_pattern_complete():
    print('DRAW PATTERN COMPLETE!')

# start running the animation non-blocking
# first argument is the function that makes the animation
# second argument is a list of arguments to be passed to the function
# third arugment is a callback function to be called once the animation completes

anim.start_animation(draw_pattern, [tile, cross, (255, 0, 0)], draw_pattern_complete)

while True:
    anim.update()

    # you can do other things here, like i2c comms!