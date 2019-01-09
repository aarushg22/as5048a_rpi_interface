#include "as5048a_rpi_spi.h"
#include <unistd.h>

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

As5048a::~As5048a(){

}

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

const int As5048a::Read(int reg_address){
  int result;
  int cmd = reg_address | AS5048_READ;

  cmd = (cmd | ((EvenParityCalc(cmd)<<15) & 0x8000));

  result = send_over_spi(cmd,2);
  usleep(100);
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
  return result;
}
int main(){
  int result=0;

  /*!< 65536 = 3.814697260kHz on Rpi2, 6.1035156kHz on RPI3 */
  /*!< 32768 = 7.629394531kHz on Rpi2, 12.20703125kHz on RPI3 */
  /*!< 16384 = 15.25878906kHz on Rpi2, 24.4140625kHz on RPI3 */
  /*!< 8192 = 30.51757813kHz on Rpi2, 48.828125kHz on RPI3 */
  /*!< 4096 = 61.03515625kHz on Rpi2, 97.65625kHz on RPI3 */
  /*!< 2048 = 122.0703125kHz on Rpi2, 195.3125kHz on RPI3 */
  /*!< 1024 = 244.140625kHz on Rpi2, 390.625kHz on RPI3 */
  /*!< 512 = 488.28125kHz on Rpi2, 781.25kHz on RPI3 */
  /*!< 256 = 976.5625kHz on Rpi2, 1.5625MHz on RPI3 */
  /*!< 128 = 1.953125MHz on Rpi2, 3.125MHz on RPI3 */
  /*!< 64 = 3.90625MHz on Rpi2, 6.250MHz on RPI3 */
  /*!< 32 = 7.8125MHz on Rpi2, 12.5MHz on RPI3 */
  /*!< 16 = 15.625MHz on Rpi2, 25MHz on RPI3 */
  /*!< 8 = 31.25MHz on Rpi2, 50MHz on RPI3 */
  /*!< 4 = 62.5MHz on Rpi2, 100MHz on RPI3. Dont expect this speed to work reliably. */
  /*!< 2 = 125MHz on Rpi2, 200MHz on RPI3, fastest you can get. Dont expect this speed to work reliably.*/
  /*!< 1 = 3.814697260kHz on Rpi2, 6.1035156kHz on RPI3, same as 0/65536 */

  As5048a chalega_kya(0,128);
  result=chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
  sleep(1);
  result=chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
  sleep(1);
  while(1) {
    result=chalega_kya.Read(AS5048_CMD_ANGLE);
    if(result==-1){
      result=chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
    }
    else{
      std::cout << "Result : 0x"<< std::hex << result << '\n';
    }
    usleep(400000);
  }
  return 1;
}
