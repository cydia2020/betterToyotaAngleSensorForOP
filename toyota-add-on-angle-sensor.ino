#include <SPI.h>
#include <Encoder_Buffer.h>
#include "mcp2515_can.h"
#include "mcp_can.h"
#define CAN_2515

const int SPI_CS_PIN = 17;

mcp2515_can CAN(SPI_CS_PIN);

int32_t encoder1Reading = 0;
int32_t lastencoder1Reading = 0;
int32_t rate = 0;
Encoder_Buffer Encoder1(10);

unsigned char SensorMsg[8] = {};
unsigned long lastMessageTime = 0;
const unsigned long messageInterval = 12;

void setup() {
  CAN.begin(CAN_500KBPS);
  Encoder1.initEncoder();
}

void loop() {
  // parse sensor
  encoder1Reading = (Encoder1.readEncoder() * 253.7);
  rate = abs(encoder1Reading) - abs(lastencoder1Reading);
  lastencoder1Reading = encoder1Reading;

  // parse CAN message
  SensorMsg[0] = (encoder1Reading >> 24) & 0xFF;
  SensorMsg[1] = (encoder1Reading >> 16) & 0xFF;
  SensorMsg[2] = (encoder1Reading >> 8) & 0xFF;
  SensorMsg[3] = (encoder1Reading >> 0) & 0xFF;
  SensorMsg[4] = (rate >> 16) & 0xFF;
  SensorMsg[5] = (rate >> 8) & 0xFF;
  SensorMsg[6] = (rate >> 0) & 0xFF;
  SensorMsg[7] = can_cksum(SensorMsg, 7, 0x23);

  unsigned long currentTime = millis();

  // send CAN message at the same rate as stock STEER_ANGLE_SENSOR
  if (currentTime - lastMessageTime >= messageInterval) {
    CAN.sendMsgBuf(0x23, 0, 8, SensorMsg);
    lastMessageTime = currentTime;
  }
}

// TOYOTA CAN CHECKSUM
int can_cksum(uint8_t *dat, uint8_t len, uint16_t addr) {
  uint8_t checksum = 0;
  checksum = ((addr & 0xFF00) >> 8) + (addr & 0x00FF) + len + 1;
  //uint16_t temp_msg = msg;
  for (int ii = 0; ii < len; ii++) {
    checksum += (dat[ii]);
  }
  return checksum;
}
