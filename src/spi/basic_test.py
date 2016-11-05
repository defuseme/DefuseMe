import spidev

spi = spidev.SpiDev()

spi.open(0,0)
spi.max_speed_hz = 50000
result = spi.xfer2([0x80,])
print("Xfer:",result)
length = spi.readbytes(1)
print(length)
result = spi.readbytes(length[0])
print(result)
spi.close()

