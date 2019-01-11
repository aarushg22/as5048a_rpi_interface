#include "as5048a_rpi_spi.h"


int main(){
  int result=0;
  float result_deg=0.0;


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
  //128 tested, works fine, by default use 128

  As5048a chalega_kya(0,128);

  //Clear any Error on initialization
  result=chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
  sleep(1);

  //start read loop
  while(1) {
    //Get rotation value in hex
    result=chalega_kya.Read(AS5048_CMD_ANGLE);

    // Get result in degrees
    result_deg=chalega_kya.Degrees(result);

    if(result==-1){
      //Clear Error
      result=chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
    }
    else{
      std::cout << "Result : 0x"<< std::hex << result << '\n' << "Result in degress : " << result_deg << '\n';
    }
    // 0.4 sec delay between each read
    usleep(400000);
  }
  return 1;
}
