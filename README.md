Arduino Project to send AS5047P magnetic rotary encoder value to CAN, based on @zorrobyte 's angle sensor project.

Parts:
1. Arduino (ATMega328P or ATMega32U4)
2. LS7366R
3. CAN Bus Shield or adapter
4. AS5047P magnetic encoder

Hook-up Guide:
1. Connect Arduino to CAN Bus Shield
2. Connect LS7366R to Arduino via SPI bus
3. Connect AS5047P module to LS7366R via A/B/I
4. Connect CAN Bus to application
