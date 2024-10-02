#include <SPI.h>
#include <Encoder_Buffer.h>
#include "mcp2515_can.h"
#include "mcp_can.h"
#define CAN_2515
#define TARGET_ID 0x23

const int CAN_CS_PIN = 17;
const int ENC_CS_PIN = 10;

mcp2515_can CAN(CAN_CS_PIN);
Encoder_Buffer Encoder(ENC_CS_PIN);

int32_t encoderReading = 0;

unsigned char SensorMsg[8] = {};

void setup() {
  Serial.begin(115200);
  if (CAN.begin(CAN_500KBPS) == CAN_OK) {
    Serial.println("CAN OK");
  } else {
    Serial.println("CAN FUCKED");
  }

  Encoder.initEncoder();
}

void loop() {
  // parse sensor
  encoderReading = (Encoder.readEncoder());  //* 253.7);
  // parse CAN message
  SensorMsg[0] = (encoderReading >> 24) & 0xFF;
  SensorMsg[1] = (encoderReading >> 16) & 0xFF;
  SensorMsg[2] = (encoderReading >> 8) & 0xFF;
  SensorMsg[3] = (encoderReading >> 0) & 0xFF;
  SensorMsg[7] = can_cksum(SensorMsg, 4, TARGET_ID);

  if (millis() % 12 == 0) {
    if (CAN.sendMsgBuf(TARGET_ID, 0, 5, SensorMsg) == CAN_OK) {
      Serial.print(millis());
      Serial.print(" at ");
      Serial.print(encoderReading);
      Serial.print('\n');
    }
  }
}

// TOYOTA CAN CHECKSUM
int can_cksum(uint8_t* dat, uint8_t len, uint16_t addr) {
  uint8_t checksum = 0;
  checksum = ((addr & 0xFF00) >> 8) + (addr & 0x00FF) + len + 1;
  //uint16_t temp_msg = msg;
  for (int ii = 0; ii < len; ii++) {
    checksum += (dat[ii]);
  }
  return checksum;
}
