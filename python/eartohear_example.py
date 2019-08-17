import board
import time

from eartohear import EarToHear

# quiet, active, fully-charged, discharging
last_system_mode = None

# section of day
last_day_time = None

last_light_level = None

print('Achilles Apparatus TileMaze')

# This sets up the connection to the Achilles Central
# Make sure the address is not 72!
ear = EarToHear(board.SCL, board.SDA, 0x72)

while True:
    # This call is needed in your main loop
    ear.check_i2c()
    
    time.sleep(0.1)
    # This call will set the points for this wedge
    ear.set_points(int(time.monotonic()) % 9)
    ear.set_points(4)
    #  ear.set_points_bits(129)

    print('{} {} {}'.format(
        ear.get_system_mode_name(),
        ear.get_day_time_name(),
        ear.get_light_level_name()))

    # see if the mode is current RESET
    if ear.system_mode == EarToHear.MODE_RESET:
        print("IN RESET MODE!")
    # Tlooking to see if any of the system info (mode, time, light level)
    # have changed, and if so, print some info and play a sound
    if last_system_mode != ear.system_mode or \
       last_day_time != ear.day_time or \
       last_light_level != ear.light_level:

            print('{} {} {}'.format(
                ear.get_system_mode_name(),
                ear.get_day_time_name(),
                ear.get_light_level_name()))

            # the second argument:play the sound global (defaults to false)
            ear.play_audio(1, True)
            # control the playing audio, fade it out
            ear.control_audio(EarToHear.AUDIO_FADE_LONG)

    last_system_mode = ear.system_mode
    last_day_time = ear.day_time
    last_light_level = ear.light_level

# in the worst case that the main loop exits, close the I2C connection
ear.close()