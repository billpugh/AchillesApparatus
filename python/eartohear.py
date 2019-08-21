import struct
import time

from i2cslave import I2CSlave


class EarToHear(object):
    AUDIO_PLAY = 0
    AUDIO_STOP = 1
    AUDIO_FADE_SHORT = 2
    AUDIO_FADE_LONG = 3

    MODE_QUIET = 0
    MODE_ACTIVE = 1
    MODE_CHARGED = 2		# Ready for big reveal
    MODE_DISCHARGING = 3	# Playing big reveal
    MODE_RESET = 4			# Go away, start over
    MODE_CHEAT_CODE = 5		
    MODE_NOT_RECEIVED = 6	# Default if nothing is going on

    SYSTEM_MODES = [
        'QUIET',
        'ACTIVE',
        'CHARGED',
        'DISCHARGING',
        'RESET',
        'CHEAT_CODE',
        'NOT_RECEIVED',
    ]

    TIME_NIGHT = 0
    TIME_MORNING_TWILIGHT = 1
    TIME_DAWN = 2
    TIME_DAY = 3
    TIME_DUSK = 4
    TIME_EVENING_TWILIGHT = 5
    TIME_UNKNOWN = 6

    DAY_TIMES = [
        'NIGHT',
        'MORNING_TWILIGHT',
        'DAWN',
        'DAY',
        'DUSK',
        'EVENING_TWILIGHT',
        'UNKNOWN',
    ]

    LIGHT_NIGHT = -4
    LIGHT_NAUTICAL_TWILIGHT = -3
    LIGHT_CIVIL_TWILIGHT = -2
    LIGHT_DUSK_DAWN = -1
    LIGHT_DAY = 0

    LIGHT_LEVELS = {
        -4: 'NIGHT',
        -3: 'NAUTICAL_TWILIGHT',
        -2: 'CIVIL_TWILIGHT',
        -1: 'DUSK_DAWN',
        0: 'DAY',
    }

    def __init__(self, clock_pin, data_pin, address, debug=False):
        self.clock_pin = clock_pin
        self.data_pin = data_pin
        self.address = address
        self.debug = debug

        self.last_activity = time.monotonic()

        self.last_packet_sent = 0
        self.last_packet_ack = None
        self.last_packet_rcvd = None

        self.audio_track = 0
        self.audio_global = False
        self.audio_action = 0
        self.points_activated = 0

        # data that comes from central
        self.time_since_global_boot = None
        self.time_since_global_activity = None
        self.system_mode = EarToHear.MODE_NOT_RECEIVED
        self.day_time = EarToHear.TIME_UNKNOWN
        self.light_level = EarToHear.LIGHT_NIGHT

        self.slave = I2CSlave(self.clock_pin, self.data_pin, (self.address,))

    def close(self):
        self.slave.__exit__()

    def get_system_mode_name(self):
        return EarToHear.SYSTEM_MODES[self.system_mode]

    def get_day_time_name(self):
        return EarToHear.DAY_TIMES[self.day_time]

    def get_light_level_name(self):
        return EarToHear.LIGHT_LEVELS[self.light_level]

    def play_audio(self, audio_track, audio_global=False):
        """
        Play audio
        """
        self.audio_track = audio_track
        self.audio_global = audio_global
        self.audio_action = EarToHear.AUDIO_PLAY

    def control_audio(self, audio_action):
        """
        Control currently playing audio
        """
        self.audio_action = audio_action

    def set_points(self, value):
        """
        Set points, 0-8
        """
        self.points_activated = 255 >> (8 - (value % 9))

    def set_points_bits(self, value):
        """
        Set point bits, 0-255
        """
        self.points_activated = int(value) % 255

    def local_activity_seen():
        """
        Let central known there was local activity
        """
        self.last_activity = time.monotonic()

    def check_i2c(self):
        """
        Send and receive messages from central
        """
        status = False
        info = None
        request = self.slave.request()

        if not request:
            info = 'no request'
        else:
            try:
                with request:
                    if not request.is_read:
                        packet = request.read()
                        if self.debug:
                            try:
                                (self.last_packet_rcvd,
                                self.last_packet_ack,
                                self.time_since_global_activity,
                                self.time_since_global_activity,
                                self.system_mode,
                                self.day_time,
                                self.light_level) = struct.unpack('HHHHBBb', packet)
                                status = True
                                info = 'received'
                            except Exception:
                                info = 'decode error'
                                print('Could not decode debug packet! Expecting 11 bytes, got {}'.format(len(packet)))
                        else:
                            try:
                                (self.time_since_global_,
                                self.time_since_global_activity,
                                self.system_mode,
                                self.day_time,
                                self.light_level) = struct.unpack('HHBBb', packet)
                                status = True
                                info = 'received'
                            except Exception as e:
                                info = 'decode error'
                                print('Could not decode debug packet! Expecting 7 bytes, got {}'.format(len(packet)))
                    else:
                        now = time.monotonic()
                        if self.debug:
                            packet = struct.pack(
                                "HHHHHBBBB",
                                self.last_packet_sent,
                                self.last_packet_ack,
                                int(now / 60),
                                int((now - self.last_activity) / 60),
                                self.audio_track,
                                self.audio_global,
                                self.audio_action,
                                self.points_activated)
                            self.last_packet_ack += 1
                        else:
                            packet = struct.pack(
                                "HHHBBBB",
                                int(now / 60),
                                int((now - self.last_activity) / 60),
                                self.audio_track,
                                self.audio_global,
                                self.audio_action,
                                self.points_activated)

                        try:
                            request.write(packet)
                            status = True
                            info = 'sent'
                        except Exception as e:
                            info = 'send error'
                            print('Could not send packet! {}'.format(e))

                        self.audio_track = 0
                        self.audio_global = False
                        self.audio_action = EarToHear.AUDIO_PLAY

            except Exception as e:
                status = False
                info = 'exception ' + str(e)
                print('Problem with I2C! {}'.format(e))

        return (status, info)