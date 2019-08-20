# import neopixel
import time

# CONSTANTS
FRAME_RATE = 30
FRAME_DELAY = 1.0 / FRAME_RATE
LED_PIN = 2
NUM_LEDS = 10

# VARIABLES
effect_time = 0
frame = 0
last_frame_time = time.monotonic()
current_led_effect = None
pixels = [(0, 0, 0), (0, 0, 0)] #neopixel.NeoPixel(LED_PIN, NUM_LEDS)
try:
    time.monotonic()
    get_now = time.monotonic
except:
    get_now = time.time

# FUNCTIONS
def fade_in(frame, frame_time):
    value = int(frame_time * 255) % 255 # since frame_time is seconds, this will make the fade take 1 second

    if value > 255:
        change_led_effect(fade_out)
        current_led_effect(0, 0)

    else:
        for i in range(len(pixels)):
            pixels[i] = (value, 0, 0)
        if value == 255:
            change_led_effect(fade_out)

def fade_out(frame, frame_time):
    value = int(frame_time * 255) % 255 # since frame_time is seconds, this will make the fade take 1 second

    for i in range(len(pixels)):
        pixels[i] = (255 - value, 0, 0)
    if value == 255:
        change_led_effect(fade_in)

    print("OUT {}".format(pixels))

def run_current_led_effect():
    global frame

    current_led_effect(frame, now - effect_time)
    frame += 1
    last_frame_time = now

def change_led_effect(effect, force=False):
    global current_led_effect
    global get_now

    if effect != current_led_effect or force:
        effect_time = get_now()
        frame = 0
    current_led_effect = effect

# INITIALIZATION
change_led_effect(fade_in)

# MAIN LOOP
while True:
    now = get_now()
   
    # update LEDs
    if now - last_frame_time >= FRAME_DELAY:
        if current_led_effect is not None:
            run_current_led_effect(now)