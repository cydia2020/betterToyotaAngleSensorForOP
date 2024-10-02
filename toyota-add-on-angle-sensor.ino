#include <SPI.h>
#include <Encoder_Buffer.h>
#include "mcp2515_can.h"
#include "mcp_can.h"
#define CAN_2515

const int CAN_CS_PIN = D1;
const int ENC_CS_PIN = D2;
const unsigned long messageInterval = 12;
unsigned long lastCanSendTime = 0;
int32_t encoderReading = 0;
int32_t lastEncoderReading = 0;
int32_t rate = 0;
byte SensorMsg[] = {};

mcp2515_can CAN(CAN_CS_PIN);
Encoder_Buffer Encoder(ENC_CS_PIN);

void setup() {
  CAN.begin(CAN_500KBPS, MCP_8MHz);
  CAN.setMode(MODE_NORMAL);
  Encoder.initEncoder();
}

void loop() {
  // parse sensor
  encoderReading = Encoder.readEncoder();  //* 253.7);

  rate = abs(encoderReading) - abs(lastEncoderReading);
  lastEncoderReading = encoderReading;

  // parse CAN message
  SensorMsg[0] = (encoderReading >> 24) & 0xFF;
  SensorMsg[1] = (encoderReading >> 16) & 0xFF;
  SensorMsg[2] = (encoderReading >> 8) & 0xFF;
  SensorMsg[3] = (encoderReading >> 0) & 0xFF;
  SensorMsg[4] = (rate >> 16) & 0xFF;
  SensorMsg[5] = (rate >> 8) & 0xFF;
  SensorMsg[6] = (rate >> 0) & 0xFF;
  SensorMsg[7] = toyota_checksum(SensorMsg, 7, 0x23);  //Toyota CAN CHECKSUM

  if (millis() - lastCanSendTime >= messageInterval) {
    lastCanSendTime = millis();
    CAN.sendMsgBuf(0x23, 0, 8, SensorMsg);
  }
}

// TOYOTA CAN CHECKSUM
int toyota_checksum(uint8_t* dat, uint8_t len, uint16_t addr) {
  uint8_t checksum = 0;
  checksum = ((addr & 0xFF00) >> 8) + (addr & 0x00FF) + len + 1;
  //uint16_t temp_msg = msg;
  for (int ii = 0; ii < len; ii++) {
    checksum += (dat[ii]);
  }
  return checksum;
}
