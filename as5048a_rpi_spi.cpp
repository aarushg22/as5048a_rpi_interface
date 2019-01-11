#include "as5048a_rpi_spi.h"

#define DEBUG 1

As5048a::As5048a(int chip_select_spi, int frequency_divider) : csn(chip_select_spi),freq_divider(frequency_divider) {

  //Let's initialize SPI using the bcm2835 library

  if (!bcm2835_init())
  {
    std::cout<<"bcm2835_init failed. Are you running as root??";
    std::exit(EXIT_FAILURE);
  }

  if (!bcm2835_spi_begin())
  {
    std::cout<<"bcm2835_spi_begin failed. Are you running as root??\n";
    std::exit(EXIT_FAILURE);
  }

  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);

  //Set the Chip Select pin, using the value passed in the constructor

  if(csn){
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
  }
  else{
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  }


  //@TODO = create a check to make sure the divider isn't invalid number
  bcm2835_spi_setClockDivider(freq_divider);
  std::cout<<"Initialization done"<<'\n';
}

As5048a::~As5048a(){}

// Int to Char Buffer and Char buffer to int translators

// void As5048a::IntToCharBuffer(char* buffer, int value, int nbytes) {
//   char mask = 0xFF;
//   for(int i=0; i < nbytes; i++){
// //    *buffer[i]=(value & (mask<<(i*8)));
//   }
// }
//
//
// int As5048a::CharBufferToInt(char* buffer, int nbytes) {
//   char mask = 0xFF;
//   int value=0;
//   for(int i=0; i < nbytes; i++){
//     value<<=(i*8);
// //    value |= *buffer[i];
//   }
//   return value;
// }


//The bcm2835 library takes input of character buffer
//so to do the required conversions and pass the data over SPI

int send_over_spi(int cmd, int nbytes){
  char  send_buff[nbytes];
  char  recv_buff[nbytes];
  char mask = 0xFF;
  int value=0;
  for(int i=0; i < nbytes; i++){
    send_buff[nbytes-i-1]=((cmd>>(i*8)) & mask);
  }

  bcm2835_spi_transfernb(&send_buff[0],&recv_buff[0],nbytes);

  #ifdef DEBUG
    printf("Sent to SPI: 0x%02X %02X. Read back from SPI: 0x%02X 0x%02X.\n", send_buff[0],send_buff[1], recv_buff[0],recv_buff[1]);
  #endif

  for(int i=0; i < nbytes; i++){
    value<<=(i*8);
    value |= recv_buff[i];
  }
  return value;
}

//The 15th bit(MSB) is always the parity bit with Even parity
//Hence to insert or validate the parity bit

int As5048a::EvenParityCalc(int value_16_bit) {

  int parity=0;
  for(int i = 0; i <= 14; ++i){
    parity = (((value_16_bit >> i) & 0x0001) ^ parity);
  }

  return parity;
}

float As5048a::Degrees(int hex_value){
  return ((float)hex_value * (360 / 0x4000));
}

const int As5048a::Read(int reg_address){
  int result;
  int cmd = reg_address | AS5048_READ;

  cmd = (cmd | ((EvenParityCalc(cmd)<<15) & 0x8000));

  result = send_over_spi(cmd,2);
  usleep(1000);
  #ifdef DEBUG
    std::cout << "Result : 0x"<< std::hex << result << '\n';
  #endif

  result = send_over_spi(AS5048_CMD_NOP,2);
  usleep(100);
  #ifdef DEBUG
    std::cout << "Result : 0x"<< std::hex << result << '\n';
  #endif
  printf("Result from SPI: 0x%04X\n", result);
  int parity_chk=EvenParityCalc(result);

  #ifdef DEBUG
    printf("parity chk: 0x%04X, result bit : 0x%04X\n", parity_chk,((result >> 15) & 0x0001));
  #endif

  if(((result >> 15) & 0x0001) != parity_chk){
    std::cout<<"Parity is wrong, stupid Magnetic Encoder chip, burn everything, burrrrrnnnnnnnnnnnnnnnnn!!!"<< '\n';
    return -1;
  }
  if(((result >> 14) & 0x0001) == 0x0001){
    std::cout<<"Error flag set, stupid Magnetic Encoder chip, burn everything, burrrrrrrrnnnnnnnnnnnnnnn!!!"<< '\n';
    return -1;
  }

  return (result & 0x3FFF);
}
