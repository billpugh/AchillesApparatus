import board
from i2cslave import I2CSlave

regs = [0] * 16
index = 0

# see https://circuitpython.readthedocs.io/en/latest/shared-bindings/i2cslave/__init__.html
# see https://circuitpython.readthedocs.io/en/latest/shared-bindings/i2cslave/I2CSlave.html


with I2CSlave(board.SCL, board.SDA, (0x40, 0x41)) as slave:
    while True:
        r = slave.request()
        if not r:
            # Maybe do some housekeeping
            continue
        with r:  # Closes the transfer if necessary by sending a NACK or feeding the master dummy bytes
            if r.address == 0x40:
                if not r.is_read:  # Master write which is Slave read
                    b = r.read(1)
                    if not b or b[0] > 15:
                        break
                    index = b[0]
                    b = r.read(1)
                    if b:
                        regs[index] = b[0]
                elif r.is_restart:  # Combined transfer: This is the Master read message
                    n = r.write(bytes([regs[index]]))
                #else:
                    # A read transfer is not supported in this example
                    # If the Master tries, it will get 0xff byte(s) by the ctx manager (r.close())
            elif r.address == 0x41:
                if not r.is_read:
                    b = r.read(1)
                    if b and b[0] == 0xde:
                        # do something
                        pass
						
						
						