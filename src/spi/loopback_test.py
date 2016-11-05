import spidev
import random

spi = spidev.SpiDev()

spi.open(0,0)
spi.max_speed_hz = 3900000
spi.mode = 0b01

last_rand = None
inc = 0
for i in range(0,255):
    rand = i
    print(last_rand,"/",rand)
    temp = spi.xfer([rand,])
    print(last_rand,"<=>",temp[0])
    if last_rand:
        assert int(last_rand)+int(temp[0]) == 255
    last_rand = rand
spi.close()
