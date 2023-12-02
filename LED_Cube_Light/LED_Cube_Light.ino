//  Tech Arrange
//wifi
#include "WiFi.h"
const char* ssid = "3DP_2.4G";    //  YOUR WIFI SSID / NAME
const char* password = "kingssel5262332";  // YOUR WIFI PASSWORD
int buttonState;
// setup server port 8080
WiFiServer server(8080);
int latchPin = 16;  // Latch pin (STCP腳位)
int clockPin = 17; // Clock pin (SHCP腳位)
int dataPin = 18;
int latchPin_2 = 33;  // Latch pin (STCP腳位)
int clockPin_2 = 32; // Clock pin (SHCP腳位)
int dataPin_2 = 19;
int LED_delay = 800;
int Scenes_delay = 4000;
int receive_data = 0x0;
String sReceive_tmp;
// setup core task
TaskHandle_t Task1;
const int column_size = 8;
int column[column_size] = {32};
int layer[4] = { 5, 23, 22, 21 };

int time_delay = 1500;

void setup() {
  Serial.begin(115200);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  pinMode(latchPin_2, OUTPUT);
  pinMode(dataPin_2, OUTPUT);  
  pinMode(clockPin_2, OUTPUT);

  //setting layers to output
  for(int i = 0; i<4; i++)
  {
    pinMode(layer[i], OUTPUT);
  }
  for (int i = 0; i < 4; i++) {
    digitalWrite(layer[i], LOW);
  }

  //seeding random for random pattern
  randomSeed(analogRead(10));

  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(Scenes_delay);
    Serial.println("Connecting to WiFi...");
  }

  server.begin();
  xTaskCreatePinnedToCore(Task1code, "Task1", 15000, NULL, 0, &Task1, 0);
}

void Task1code(void* pvParameters) {
  while (true) {
    IPAddress broadcastIP(255, 255, 255, 255);
    String mac = WiFi.macAddress();
    WiFiUDP udp;
    udp.begin(12345);
    // Send a broadcast message to all devices on the network
    for (size_t i = 0 ; i < 100; i++)
    {
      udp.beginPacket(broadcastIP, 12345);
      udp.print(mac.c_str());
      udp.endPacket();
      delay(250);
    }
    //Serial.println(mac.c_str());
    Serial.println(WiFi.localIP().toString().c_str());
    delay(10000);
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String data = client.readStringUntil('\r');
        Serial.print("received: ");
        if (data == sReceive_tmp)
        {
          //no change, do nothing
        }
        else
        {
          sReceive_tmp = data;
          receive_data = data.toInt();
        }
      }
    }
    client.stop();
  }

  // run special mode
  if (receive_data != 0)
  {
    int temp = 0, remainder = 0, data = receive_data;
    // 64
    remainder = data % 64;
    temp = data - remainder;
    data = remainder;
    temp /= 64;
    switch (temp)
    {
      case 1:
        warm();
        break;
      case 2:
        hot();
        break;
      default:
        cold();
        break;
    }
    delay(Scenes_delay);

    remainder = data % 16;
    temp = data - remainder;
    data = remainder;
    temp /= 16;
    // 16
    switch (temp)
    {
      case 1:
        strong_wind();
        break;
      case 2:
        no_wind();
        break;
      default:
        breeze();
        break;
    }
    delay(Scenes_delay);

    remainder = data % 4;
    temp = data - remainder;
    data = remainder;
    temp /= 4;
    // 4
    switch (temp)
    {
      case 1:
        cloudy();
        break;
      case 2:
        few_cloud();
        break;
      default:
        no_cloud();
        break;
    }
    delay(Scenes_delay);
  
    temp = data;
    // 1
    switch (temp)
    {
      case 1:
        heavy_rain();
        break;
      case 2:
        light_rain();
        break;
      default:
        no_rain();
        break;
    }
    delay(Scenes_delay);
  }
}

//turn all off

void turnEverythingOff() {
  for (int i = 0; i < column_size; i++) {
    digitalWrite(column[i], LOW);
  }
  for (int i = 0; i < 4; i++) {
    digitalWrite(layer[i], LOW);
  }
}

////////////////////////////////////////////////////////////turn all on
void turnEverythingOn() {
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, LSBFIRST, 255);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(LED_delay);
}

////////////////////////////////////////////////////// cold

void cold(){
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], LOW);
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, LSBFIRST, 153);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 153);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 102);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 102);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 6);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 96);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// hot
void hot() {
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 6);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 96);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 102);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 102);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 255);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 255);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], LOW);
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// warm 
void warm() {
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 96);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 6);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 144);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 9);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 96);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 6);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 6);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 96);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// no wind
void no_wind() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], LOW);

  int value = 128;
  for (int i = 0 ; i < 4 ; i++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, value);  //85就是2進位的01010101
    digitalWrite(latchPin, HIGH);
    value/=2;
    delay(LED_delay);
  }
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 1);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 16);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  value = 1;
  for (int i = 0 ; i < 4 ; i++)
  {
    digitalWrite(latchPin_2, LOW);
    shiftOut(dataPin_2, clockPin_2, LSBFIRST, value);  //85就是2進位的01010101
    digitalWrite(latchPin_2, HIGH);
    value*=2;
    delay(LED_delay);
  }

  digitalWrite(latchPin_2, LOW);
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 128);  //85就是2進位的01010101
  digitalWrite(latchPin_2, HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 8);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// breeze
void breeze() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], LOW);

  int value = 192;
  for (int i = 0 ; i < 3 ; i++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, value);  //85就是2進位的01010101
    digitalWrite(latchPin, HIGH);
    value/=2;
    delay(LED_delay);
  }
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 17);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 1);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 16);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 17);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  value = 3;
  for (int i = 0 ; i < 3 ; i++)
  {
    digitalWrite(latchPin_2, LOW);
    shiftOut(dataPin_2, clockPin_2, LSBFIRST, value);  //85就是2進位的01010101
    digitalWrite(latchPin_2, HIGH);
    value*=2;
    delay(LED_delay);
  }

  digitalWrite(latchPin_2, LOW);
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 136);  //85就是2進位的01010101
  digitalWrite(latchPin_2, HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 8);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 128);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 136);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// no cloud
void no_cloud() {
  int led1[12] = {160, 80, 33, 16, 1, 0, 0, 0, 0, 8, 128, 72};
  int led2[12] = {0, 0, 0, 16, 1, 18, 5, 10, 132, 8, 128, 0};

  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);

  for (int i = 0 ; i < 12 ; i++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led1[i]);  //85就是2進位的01010101
    digitalWrite(latchPin, HIGH);
    delay(10);
    digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
    shiftOut(dataPin_2, clockPin_2, LSBFIRST, led2[i]);  //送出資料，170就是2進位的10101010
    digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
    delay(LED_delay);
  }

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// few cloud
void few_cloud() {
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  int led1[12] = {192, 96, 48, 17, 1, 0, 0, 0, 0, 0, 8, 136};
  int led2[12] = {0, 0, 0, 0, 8, 17, 3, 6, 12, 136, 128, 0};

  for (int i = 0 ; i < 12 ; i++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led1[i]);  //85就是2進位的01010101
    digitalWrite(latchPin, HIGH);
    delay(10);
    digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
    shiftOut(dataPin_2, clockPin_2, LSBFIRST, led2[i]);  //送出資料，170就是2進位的10101010
    digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
    delay(LED_delay);
  }

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// cloudy
void cloudy() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 112);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 1);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 17);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 15);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 8);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 136);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  delay(LED_delay);

  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], LOW);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 112);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 1);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 17);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 15);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 8);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 136);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// heavy rain
void heavy_rain() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, LSBFIRST, 255);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 255);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], LOW);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// light rain
void light_rain() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, LSBFIRST, 6);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 96);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], LOW);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// no rain
void no_rain() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, LSBFIRST, 4);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 32);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], HIGH);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], LOW);
  delay(LED_delay);
  digitalWrite(layer[0], HIGH);
  digitalWrite(layer[1], HIGH);
  digitalWrite(layer[2], HIGH);
  digitalWrite(layer[3], HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

////////////////////////////////////////////////////// strong wind
void strong_wind() {
  digitalWrite(layer[0], LOW);
  digitalWrite(layer[1], LOW);
  digitalWrite(layer[2], LOW);
  digitalWrite(layer[3], LOW);
  int led1[12] = {240, 113, 49, 9, 1, 0, 0, 0, 8, 136, 200, 136};
  int led2[12] = {0, 0, 8, 9, 19, 23, 15, 142, 140, 136, 128, 0};

  for (int i = 0 ; i < 12 ; i++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led1[i]);  //85就是2進位的01010101
    digitalWrite(latchPin, HIGH);
    delay(10);
    digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
    shiftOut(dataPin_2, clockPin_2, LSBFIRST, led2[i]);  //送出資料，170就是2進位的10101010
    digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
    delay(LED_delay);
  }

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  //85就是2進位的01010101
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin_2, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin_2, clockPin_2, LSBFIRST, 0);  //送出資料，170就是2進位的10101010
  digitalWrite(latchPin_2, HIGH); // 送完資料後要把 latchPin 設成高電位
  delay(LED_delay);
}

