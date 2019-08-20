import time

from eartohear import EarToHear

FRAME_DELAY = 1 / 30.0  # 30 FPS

def fade_x(frame, frame_time):
    # frame is an integer that counts up
    # frame_time is the time (in seconds) since effect started
    pass

def show_other(frame, frame_time):
    # draw other LED thing
    pass

def change_led_effect(effect, force=False):
    if effect != current_led_effect or force:
        effect_time = time.monotonic()
        frame = 0
    current_led_effect = effect

effect_time = 0
frame = 0
last_frame_time = time.monotonic()
current_led_effect = None
ear = EarToHear(board.SCL, board.SDA, 0x72)

while True:
    now = time.monotonic()
    ear.check_i2c()
    
    if ear.system_mode == EarToHear.MODE_DISCHARGING:
        change_led_effect(fade_x)

    # update LEDs
    if now - last_frame_time >= FRAME_DELAY:
        if current_led_effect is not None:
            current_led_effect(frame, now - effect_time)
            frame += 1
            last_frame_time = now