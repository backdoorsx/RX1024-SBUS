# RX1024 SBUS protocol

Arduino + NRF24 radio receiver 100mW:
  - 4x analog channel, 0-1023 remap to 1000-2000
  - 6x AUX channel, bool

Pin7 HIGH checks if the chip nrf24 is connected to the SPI bus.

Default settings for lost TX signal:
```
void ResetData() {
  data.TYPR[0] = 500; //[R]
  data.TYPR[1] = 500; //[P]
  data.TYPR[2] = 200; //[T]
  data.TYPR[3] = 500; //[Y]
  data.AUXS[0] = 0;
  data.AUXS[1] = 0;
  data.AUXS[2] = 0;
  data.AUXS[3] = 0;
  data.AUXS[4] = 0;
  data.AUXS[5] = 0;
}
```
![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/8d7e8c1f-b7fc-4eab-b5c8-3a3daf33f1d2)


```
Define SPI pins *(NANO & SUPERMINI):

  D15 - SCK  *(D13)
  D14 - MISO *(D12)
  D16 - MOSI *(D11)
  D10 - SCN ------+
  D9  - CE ----+  |
               |  |
    RF24 radio(9, 10);
```
```
Define output TX pin (SBUS pin):

Use HardwareSerial serial1 for arduino chip atmega32u4 Serial1.begin(100000, SERIAL_8E2);
For arduino chip atmega328p use Serial.begin(100000, SERIAL_8E2);
```
The pin TX0 from arduino is connected to the FC:
```
ARDUINO |  FC
--------+-----
  GND   | GND
  RAW   | 4V5
  TX    | R2
```
![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/79298bdb-e585-43c4-a280-e1579a6908ce)

![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/96c67c5c-45e3-4506-a620-ffb7eae4c5d6)
Betaflight configurator settings:

![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/8dc629f9-bb50-4c45-9a4b-5bf495dafa99)
![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/a94724c6-ffb4-482f-bb63-3245607efc16)


  

Arduino nano or pro micro (atmega328p or atmega32u4):

![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/c6f3156e-7ed8-4e9e-85e4-c01701db6c79)


nRF24L01P 2.4GHz 100mW or 500mW:

https://www.cdebyte.com/products/E01-ML01SP4 - 100mW
![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/52df6256-06ef-4a35-a71e-62a2324a68cb)

https://www.cdebyte.com/products/E01-2G4M27SX -500mW
![image](https://github.com/user-attachments/assets/6d117720-330f-4438-80a8-1c607f40daca)


3v3 Voltage Regulator Module:

![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/8b07d441-e9ae-42fb-baad-e9ccca79800e)
AMS1117 LDO 800MA:
![image](https://github.com/user-attachments/assets/dd9232b7-435e-41db-917b-55668d520364)



2.4GHz antenna:

![image](https://github.com/backdoorsx/RX1024-SBUS/assets/18431164/fe0aacec-f490-471d-9637-917829c43c26)
Flywoo ELRS 2.4g Minimortal PCB T Antenna UFL:
![image](https://github.com/user-attachments/assets/8fd22cc1-5be2-4990-97c0-bcf722e3d5b1)


# TX1024
https://github.com/backdoorsx/TX1024.git

## License

[WTFPL](http://www.wtfpl.net/)
