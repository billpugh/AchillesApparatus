import board
import struct
import time
from i2cslave import I2CSlave


print('Achilles Aparatus TileMaze')

with I2CSlave(board.SCL, board.SDA, (0x72,)) as slave:
    while True:
        request = slave.request()

        if not request:
            # Maybe do some housekeeping
            continue
        else:
            try:
                with request:
                    print('{} {} {}'.format(time.monotonic(), request.is_read, request.is_restart))
                    if not request.is_read:
                        packet = request.read()
                        print('Received {} bytes: {}'.format(len(packet), packet))
                    else:
                        print("Wrote {} byte(s)".format(request.write(bytearray("abcdefghi"))))


            except OSError as e:
                print(e)