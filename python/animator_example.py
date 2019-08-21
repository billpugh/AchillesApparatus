import animator
import board
import neopixel
import time


anim = animator.Animator()

leds = neopixel.NeoPixel(board.D29, 6, auto_write=False)

def draw_pattern_complete():
    print('DRAW PATTERN COMPLETE!')

def draw_pattern(color):
    for index, pixel in enumerate(leds):
        # loop over all 6 pixels and set the color
        for p in range(6):
            if index == p:
                pixel = color
            else:
                pixel = (0, 0, 0)
        leds.show()
        print('DRAW PIXEL {}'.format(index))
        yield 1 # wait 1 second

# start running the animation non-blocking
# first argument is the function that makes the animation
# second argument is a list of arguments to be passed to the function
# third arugment is a callback function to be called once the animation completes
anim.start_animation(draw_pattern, [(255, 0, 0)], draw_pattern_complete)

while True:
    anim.update()
    # you can do other things here, like i2c comms!