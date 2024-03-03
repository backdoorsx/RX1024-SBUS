#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RC_CHANNEL_MIN 990
#define RC_CHANNEL_MAX 2010

#define SBUS_MIN_OFFSET 173
#define SBUS_MID_OFFSET 992
#define SBUS_MAX_OFFSET 1811
#define SBUS_CHANNEL_NUMBER 16
#define SBUS_PACKET_LENGTH 25
#define SBUS_FRAME_HEADER 0x0f
#define SBUS_FRAME_FOOTER 0x00
#define SBUS_FRAME_FOOTER_V2 0x04
#define SBUS_STATE_FAILSAFE 0x08
#define SBUS_STATE_SIGNALLOSS 0x04
#define SBUS_UPDATE_RATE 15  //ms

const uint64_t pipeOut = 0xA8A8F0F0E1LL;

struct MyData {
  // unsigned int throttle;
  // unsigned int yaw;
  // unsigned int pitch;
  // unsigned int roll;
  unsigned int TYPR[4];
  // bool AUX1;
  // bool AUX2;
  // bool AUX3;
  // bool AUX4;
  // bool AUX5;
  // bool AUX6;
  byte AUXS[6];
};

MyData data;

RF24 radio(9, 10);

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

void sbusPreparePacket(uint8_t packet[], int channels[], bool isSignalLoss, bool isFailsafe) {

  static int output[SBUS_CHANNEL_NUMBER] = { 0 };

  /*
  * Map 1000-2000 with middle at 1500 chanel values to
  * 173-1811 with middle at 992 S.BUS protocol requires
  */
  for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
    output[i] = map(channels[i], RC_CHANNEL_MIN, RC_CHANNEL_MAX, SBUS_MIN_OFFSET, SBUS_MAX_OFFSET);
    // Serial.print(i);
    // Serial.print(" : ");
    // Serial.println(output[i]);
  }

  uint8_t stateByte = 0x00;
  if (isSignalLoss) {
    stateByte |= SBUS_STATE_SIGNALLOSS;
  }
  if (isFailsafe) {
    stateByte |= SBUS_STATE_FAILSAFE;
  }
  packet[0] = SBUS_FRAME_HEADER;  //Header

  packet[1] = (uint8_t)(output[0] & 0x07FF);
  packet[2] = (uint8_t)((output[0] & 0x07FF) >> 8 | (output[1] & 0x07FF) << 3);
  packet[3] = (uint8_t)((output[1] & 0x07FF) >> 5 | (output[2] & 0x07FF) << 6);
  packet[4] = (uint8_t)((output[2] & 0x07FF) >> 2);
  packet[5] = (uint8_t)((output[2] & 0x07FF) >> 10 | (output[3] & 0x07FF) << 1);
  packet[6] = (uint8_t)((output[3] & 0x07FF) >> 7 | (output[4] & 0x07FF) << 4);
  packet[7] = (uint8_t)((output[4] & 0x07FF) >> 4 | (output[5] & 0x07FF) << 7);
  packet[8] = (uint8_t)((output[5] & 0x07FF) >> 1);
  packet[9] = (uint8_t)((output[5] & 0x07FF) >> 9 | (output[6] & 0x07FF) << 2);
  packet[10] = (uint8_t)((output[6] & 0x07FF) >> 6 | (output[7] & 0x07FF) << 5);
  packet[11] = (uint8_t)((output[7] & 0x07FF) >> 3);
  packet[12] = (uint8_t)((output[8] & 0x07FF));
  packet[13] = (uint8_t)((output[8] & 0x07FF) >> 8 | (output[9] & 0x07FF) << 3);
  packet[14] = (uint8_t)((output[9] & 0x07FF) >> 5 | (output[10] & 0x07FF) << 6);
  packet[15] = (uint8_t)((output[10] & 0x07FF) >> 2);
  packet[16] = (uint8_t)((output[10] & 0x07FF) >> 10 | (output[11] & 0x07FF) << 1);
  packet[17] = (uint8_t)((output[11] & 0x07FF) >> 7 | (output[12] & 0x07FF) << 4);
  packet[18] = (uint8_t)((output[12] & 0x07FF) >> 4 | (output[13] & 0x07FF) << 7);
  packet[19] = (uint8_t)((output[13] & 0x07FF) >> 1);
  packet[20] = (uint8_t)((output[13] & 0x07FF) >> 9 | (output[14] & 0x07FF) << 2);
  packet[21] = (uint8_t)((output[14] & 0x07FF) >> 6 | (output[15] & 0x07FF) << 5);
  packet[22] = (uint8_t)((output[15] & 0x07FF) >> 3);

  packet[23] = stateByte;          //Flags byte
  packet[24] = SBUS_FRAME_FOOTER;  //Footer
}

uint8_t sbusPacket[SBUS_PACKET_LENGTH];
int rcChannels[SBUS_CHANNEL_NUMBER];

uint32_t currentMillis = 0;
uint32_t sbusTime = 0;

unsigned long lastRecvTime = 0;
unsigned long now = 0;

const int LED1 = 7;
void setup() {

  pinMode(LED1, OUTPUT);

  ResetData();  // Default channel value

  radio.begin();
  radio.openReadingPipe(1,pipeOut);
  radio.setChannel(99);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  delay(500);

  for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
    rcChannels[i] = 1501;
  }

  delay(500);

  // for arduino nano atmega328p use Serial.begin
  Serial1.begin(100000, SERIAL_8E1);
  //Serial.begin(115200);
  delay(500);

  // if is radio OK set pin7 to high, for physical checking
  if(radio.isChipConnected())
    digitalWrite(LED1, HIGH);
  else
    digitalWrite(LED1, LOW);
  

}

void print_data(){
  Serial.print(data.TYPR[0]);
  Serial.print("\t");
  Serial.print(data.TYPR[1]);
  Serial.print("\t");
  Serial.print(data.TYPR[2]);
  Serial.print("\t");
  Serial.println(data.TYPR[3]);
}

void print_channels(){
  Serial.print(rcChannels[0]);
  Serial.print("\t");
  Serial.print(rcChannels[1]);
  Serial.print("\t");
  Serial.print(rcChannels[2]);
  Serial.print("\t");
  Serial.println(rcChannels[3]);
}

void print_pactes(){
  Serial.print(rcChannels[0]);
  Serial.print("\t");
  Serial.print(rcChannels[1]);
  Serial.print("\t");
  Serial.print(rcChannels[2]);
  Serial.print("\t");
  Serial.println(rcChannels[3]);
}

void loop() {
  
  if (radio.available()) {
    radio.read(&data, sizeof(MyData));
    lastRecvTime = millis();
  } 
  
  now = millis();
  if (now - lastRecvTime > 1000) {
    ResetData();
  }

  rcChannels[0] = map(data.TYPR[0], 0, 1000, 1000, 2000);
  rcChannels[1] = map(data.TYPR[1], 0, 1000, 1000, 2000);
  rcChannels[2] = map(data.TYPR[2], 0, 1000, 1000, 2000);
  rcChannels[3] = map(data.TYPR[3], 0, 1000, 1000, 2000);

  rcChannels[4] = map(data.AUXS[0], 0, 1, 1000, 2000);
  rcChannels[5] = map(data.AUXS[1], 0, 1, 1000, 2000);
  rcChannels[6] = map(data.AUXS[2], 0, 1, 1000, 2000);
  rcChannels[7] = map(data.AUXS[3], 0, 1, 1000, 2000);
  rcChannels[8] = map(data.AUXS[4], 0, 1, 1000, 2000);
  rcChannels[9] = map(data.AUXS[5], 0, 1, 1000, 2000);
  rcChannels[10] = map(data.AUXS[5], 0, 1, 1000, 2000);
  rcChannels[11] = map(data.AUXS[5], 0, 1, 1000, 2000);

  //print_data();
  //print_channels();

  currentMillis = millis();

  if (currentMillis > sbusTime) {
    sbusPreparePacket(sbusPacket, rcChannels, false, false);
    Serial1.write(sbusPacket, SBUS_PACKET_LENGTH);
    sbusTime = currentMillis + SBUS_UPDATE_RATE;
  }
}