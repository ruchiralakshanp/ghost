/*
    RLP Ghost V6
    
    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D7
    CSN    ->   D8
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const uint64_t pipeIn = 0xE8E8F0F0E1LL; //Remember that this code is the same as in the transmitter

int l1=2;
int l2=4;
int r1=5;
int r2=9;
int len=3;
int ren= 6;
float leftSpeed = 0;
float rightSpeed = 0;
RF24 radio(7, 8); 

//We could use up to 32 channels
struct MyData {
byte throttle; //We define each byte of data input, in this case just 6 channels
byte yaw;
byte pitch;
byte roll;
byte AUX1;
byte AUX2;
};

MyData data;

void resetData()
{
//We define the inicial value of each data input
//3 potenciometers will be in the middle position so 127 is the middle from 254
data.throttle = 0;
data.yaw = 127;
data.pitch = 127;
data.roll = 127;
data.AUX1 = 0;
data.AUX2 = 0;

}

/**************************************************/

void setup()
{
Serial.begin(250000); //Set the speed to 9600 bauds if you want.
//You should always have the same speed selected in the serial monitor
resetData();
radio.begin();
radio.setAutoAck(false);
radio.setDataRate(RF24_250KBPS);

radio.openReadingPipe(1,pipeIn);
//we start the radio comunication
radio.startListening();

}

/**************************************************/

unsigned long lastRecvTime = 0;

void recvData()
{
while ( radio.available() ) {
radio.read(&data, sizeof(MyData));
lastRecvTime = millis(); //here we receive the data
}
}

/**************************************************/

void loop()
{
recvData();
unsigned long now = millis();
//Here we check if we've lost signal, if we did we reset the values 
if ( now - lastRecvTime > 1000 ) {
// Signal lost?
resetData();
}

Serial.print("Throttle: "); Serial.print(data.throttle);  Serial.print("    ");
Serial.print("Yaw: ");      Serial.print(data.yaw);       Serial.print("    ");
Serial.print("Pitch: ");    Serial.print(data.pitch);     Serial.print("    ");
Serial.print("Roll: ");     Serial.print(data.roll);      Serial.print("    ");
Serial.print("Aux1: ");     Serial.print(data.AUX1);      Serial.print("    ");
Serial.print("Aux2: ");     Serial.print(data.AUX2);      Serial.print("\n");

if (data.throttle < 125) {
    digitalWrite(l1, HIGH);
    digitalWrite(l2, LOW);
    leftSpeed = (129 - data.throttle) * 1.9;
    analogWrite(len, int(leftSpeed));

  } else if (data.throttle > 135) {
    digitalWrite(l1, LOW);
    digitalWrite(l2, HIGH);
    leftSpeed = ((data.throttle - 130) * 1.9);
    analogWrite(len, int(leftSpeed));

  } else {
    digitalWrite(l1, LOW);
    digitalWrite(l2, LOW);
    digitalWrite(len, 0);

  }

  if (data.pitch < 125) {
    digitalWrite(r1, HIGH);
    digitalWrite(r2, LOW);
    rightSpeed = (129 - data.pitch) * 1.9;
    analogWrite(ren, int(rightSpeed));

  } else if (data.pitch > 135) {
    digitalWrite(r1, LOW);
    digitalWrite(r2, HIGH);
    rightSpeed = ((data.pitch - 130) * 1.9);
    analogWrite(ren, int(rightSpeed));

  } else {
    digitalWrite(r2, LOW);
    digitalWrite(r1, LOW);
    digitalWrite(ren, 0);

  }


}

/**************************************************/
