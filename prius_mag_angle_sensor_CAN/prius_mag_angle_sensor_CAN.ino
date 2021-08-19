#include <Encoder_Buffer.h>
#include <can.h>
#include <mcp2515.h>
#include <SPI.h>

//CAN
struct can_frame canMsg1;
struct can_frame canMsg2;
MCP2515 mcp2515(17);

//ANGSENSOR
#define EncoderCS1 10
int32_t encoder1Reading = 0;
int32_t lastencoder1Reading = 0;
int32_t rate = 0;
Encoder_Buffer Encoder1(EncoderCS1);

void setup() {
  
  while (!Serial);
  Serial.begin(115200);
  SPI.begin();

  //INIT CAN
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //8Mhz oscillator
  mcp2515.setNormalMode();
  
  //INIT ANGSENSOR
  Encoder1.initEncoder();
}

void loop() {
  //ANGSENSOR
  encoder1Reading = Encoder1.readEncoder(); //READ the ANGSENSOR

  rate = abs(encoder1Reading) - abs(lastencoder1Reading);

  lastencoder1Reading = encoder1Reading;

  //CAN
  canMsg1.can_id  = 0x23;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = (encoder1Reading >> 24) & 0xFF; //Bitshift the ANGSENSOR (Cabana errors with 32 bit)
  canMsg1.data[1] = (encoder1Reading >> 16) & 0xFF;
  canMsg1.data[2] = (encoder1Reading >> 8) & 0xFF;
  canMsg1.data[3] = (encoder1Reading >> 0) & 0xFF;
  canMsg1.data[4] = (rate >> 16) & 0xFF;
  canMsg1.data[5] = (rate >> 8) & 0xFF;
  canMsg1.data[6] = (rate >> 0) & 0xFF;
  canMsg1.data[7] = can_cksum (canMsg1.data, 7, 0x230); //Toyota CAN CHECKSUM

  mcp2515.sendMessage(&canMsg1);
  
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
