#ifndef AS5048A_RPI_SPI
#define AS5048A_RPI_SPI

#include <bcm2835.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>

#define AS5048_READ 0x4000
#define AS5048_CMD_ANGLE 0x3FFF
#define AS5048_CMD_MAGNITUDE 0x3FFE
#define AS5048_CMD_NOP 0x0000
#define AS5048_CMD_CLEAR_ERROR 0x4001

class As5048a {
public:
  As5048a(int csn, int frequency_divider);
  ~As5048a();

  const int Read(int reg_address);
  static int Degrees(int angle_hex);
  static void IntToCharBuffer(char* buffer, int value, int nbytes);
  static int CharBufferToInt(char* buffer, int nbytes);
  int EvenParityCalc(int value_16_bit);

private:
  int csn;
  int freq_divider;
};



#endif
