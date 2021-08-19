#include <SPI.h>
#include <Encoder_Buffer.h>
#include "mcp2515_can.h"

const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 2;

mcp2515_can CAN(SPI_CS_PIN);

//ANGSENSOR
#define EncoderCS1 10
int32_t encoder1Reading = 0;
int32_t lastencoder1Reading = 0;
int32_t rate = 0;
Encoder_Buffer Encoder1(EncoderCS1);

unsigned char buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  CAN.begin(CAN_500KBPS);
  
  //INIT ANGSENSOR
  Encoder1.initEncoder();
}

void loop() {
  //ANGSENSOR
  encoder1Reading = Encoder1.readEncoder(); //READ the ANGSENSOR

  rate = abs(encoder1Reading) - abs(lastencoder1Reading);

  lastencoder1Reading = encoder1Reading;

  buff[0] = (encoder1Reading >> 24) & 0xFF; //Bitshift the ANGSENSOR (Cabana errors with 32 bit)
  buff[1] = (encoder1Reading >> 16) & 0xFF;
  buff[2] = (encoder1Reading >> 8) & 0xFF;
  buff[3] = (encoder1Reading >> 0) & 0xFF;
  buff[4] = (rate >> 16) & 0xFF;
  buff[5] = (rate >> 8) & 0xFF;
  buff[6] = (rate >> 0) & 0xFF;
  buff[7] = can_cksum (buff, 7, 0x230); //Toyota CAN CHECKSUM

  CAN.sendMsgBuf(0x23, 0, 8, buff);
  
  delay(10);
}

//TOYOTA CAN CHECKSUM
int can_cksum (uint8_t *dat, uint8_t len, uint16_t addr) {
  uint8_t checksum = 0;
  checksum = ((addr & 0xFF00) >> 8) + (addr & 0x00FF) + len + 1;
  //uint16_t temp_msg = msg;
  for (int ii = 0; ii < len; ii++) {
    checksum += (dat[ii]);
  }
  return checksum;
}
