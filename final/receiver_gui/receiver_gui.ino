#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <NimBLEDevice.h>

#define CS 5
#define DC 16
#define RST 17

#define SERVICE_UUID "12345678-1234-1234-1234-1234567890AB"
#define CHAR_UUID    "87654321-4321-4321-4321-BA0987654321"

U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI oled(U8G2_R0,CS,DC,RST);

float bgl[25];
int bglCount=0;
float currentBGL=0;
bool bleConnected=false;
bool finished=false;

class ClientCallbacks: public NimBLEClientCallbacks {
  void onConnect(NimBLEClient* client) {
    bleConnected=true;
    Serial.println("BLE Connected");
  }

  void onDisconnect(NimBLEClient* client,int reason) {
    bleConnected=false;
    Serial.println("BLE Disconnected");
  }
};

void notifyCallback(
  NimBLERemoteCharacteristic* chr,
  uint8_t* data,
  size_t len,
  bool isNotify) {

  if(len!=sizeof(float)) return;

  float value;
  memcpy(&value,data,sizeof(float));

  if(bglCount<25){
    bgl[bglCount]=value;
    currentBGL=value;
    bglCount++;

    Serial.print("Received ");
    Serial.print(bglCount);
    Serial.print(": ");
    Serial.println(value,1);

    if(bglCount>=25){
      finished=true;
      Serial.println("All BGL received");
    }
  }
}

void drawGraph(){
  int gx=0,gy=11,gw=84,gh=42;

  oled.drawFrame(gx,gy,gw,gh);

  if(bglCount<1) return;

  for(int i=0;i<bglCount-1;i++){
    int x1=gx+2+(i*(gw-4)/24.0);
    int x2=gx+2+((i+1)*(gw-4)/24.0);

    int y1=gy+gh-3-(bgl[i]-4.0)*(gh-6)/8.0;
    int y2=gy+gh-3-(bgl[i+1]-4.0)*(gh-6)/8.0;

    y1=constrain(y1,gy+2,gy+gh-2);
    y2=constrain(y2,gy+2,gy+gh-2);

    oled.drawLine(x1,y1,x2,y2);
  }

  oled.setFont(u8g2_font_5x7_tr);
  oled.drawStr(1,62,"0");
  oled.drawStr(19,62,"30");
  oled.drawStr(39,62,"60");
  oled.drawStr(58,62,"90");
  oled.drawStr(73,62,"120");
}

void drawGUI(){
  oled.clearBuffer();

  oled.setFont(u8g2_font_6x10_tr);
  oled.drawStr(2,9,"BGL");

  drawGraph();

  oled.drawVLine(86,0,64);

  char value[10];
  dtostrf(currentBGL,3,1,value);

  oled.setFont(u8g2_font_logisoso24_tr);
  oled.drawStr(88,29,value);

  oled.setFont(u8g2_font_5x7_tr);
  oled.drawStr(91,38,"mmol/L");

  if(bleConnected){
    oled.drawCircle(91,52,3,U8G2_DRAW_ALL);
    oled.drawStr(98,55,"BLE");
  }else{
    oled.drawCircle(91,52,3,U8G2_DRAW_NONE);
    oled.drawStr(98,55,"OFF");
  }

  oled.sendBuffer();
}

bool connectToServer(){
  NimBLEScan* scan=NimBLEDevice::getScan();
  scan->setActiveScan(true);

  Serial.println("Scanning...");

  NimBLEScanResults results=scan->getResults(5);

  for(int i=0;i<results.getCount();i++){
    NimBLEAdvertisedDevice* dev=results.getDevice(i);

    if(dev->getName()=="ESP32_BGL"){
      Serial.println("Found ESP32_BGL");

      NimBLEClient* client=NimBLEDevice::createClient();
      client->setClientCallbacks(new ClientCallbacks(),false);

      if(!client->connect(dev)){
        Serial.println("Connection failed");
        return false;
      }

      NimBLERemoteService* service=
        client->getService(SERVICE_UUID);

      if(!service){
        Serial.println("Service not found");
        client->disconnect();
        return false;
      }

      NimBLERemoteCharacteristic* chr=
        service->getCharacteristic(CHAR_UUID);

      if(!chr){
        Serial.println("Characteristic not found");
        client->disconnect();
        return false;
      }

      if(chr->canNotify()){
        chr->subscribe(true,notifyCallback);
        Serial.println("Notification enabled");
        return true;
      }
    }
  }

  return false;
}

void setup(){
  Serial.begin(115200);

  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tr);
  oled.drawStr(20,30,"BLE Scan...");
  oled.sendBuffer();

  NimBLEDevice::init("BGL_DISPLAY");

  connectToServer();

  drawGUI();
}

void loop(){
  static unsigned long lastGUI=0;
  static unsigned long lastReconnect=0;

  if(!bleConnected && millis()-lastReconnect>5000){
    lastReconnect=millis();
    connectToServer();
  }

  if(millis()-lastGUI>200){
    lastGUI=millis();
    drawGUI();
  }
}