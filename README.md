# As5048a Magnetic Encoder driver for Raspberry pi

## Dependencies

This code depends on the awesome _C library Broadcom BCM 2835_ which can be found and downloaded from this [page](http://www.airspayce.com/mikem/bcm2835/). The Installation instructions can also be found on the same page, but for the sake of your laziness, here you go:

```
# download the latest version of the library, say bcm2835-1.xx.tar.gz, then:
tar zxvf bcm2835-1.xx.tar.gz
cd bcm2835-1.xx
./configure
make
sudo make check
sudo make install
```

## Build and run

While compiling you also need to link the bcm2835 library.

`g++ my_example.cpp -lbcm2835 -o my_example_runnable`

The bcm2835 modules need to be initialized with root access when running :

`sudo ./my_example_runnable`

## API

* As5048a(int csn, int frequency_divider)

Constructor to initialize the Encoder Object takes two inputs, the SPI Chip select pin on the Rpi (choose between 0 and 1, refer [here](https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md)) and the clock frequency divider for the SPI. The datasheet of the encoder specifies a max clock speed of 10MHz, we try to keep a safe space and use a approx 2Mhz clock speed max, for which the divider is 128. Hence by default use 128 as the clock frequency divider.

* int Read(int reg_address)

This performs the read operation on the encoder chip, and returns the value of whatever register you pass as a parameter to read. For reading the angle of the magnet, you can pass `AS5048_CMD_ANGLE`, which is defined as a macro in the header file. This will return the hex value corresponding to the rotation of the magnet, the range of the value is between 0x0000 - 0x3FFF.
To get the angle in degrees you can use the func listed next.

* float Degrees(int hex_value)

This converts the hex_value recieved from the Read operation into Degrees. Currently, the observed Precision is approx up to (+-)0.065 degrees.

#### That's all folks
