#include <NimBLEDevice.h>

#define SERVICE_UUID "12345678-1234-1234-1234-1234567890AB"
#define CHAR_UUID    "87654321-4321-4321-4321-BA0987654321"

float bgl[25]={
  8.8,8.4,7.9,7.2,6.4,
  6.8,7.5,8.1,8.7,9.2,
  10.2,9.8,9.1,8.5,7.8,
  7.2,6.9,7.4,8.0,8.8,
  9.5,10.1,9.6,8.9,9.2
};

NimBLECharacteristic *pChar;
unsigned long lastSend=0;
int indexBGL=0;

void setup(){
  Serial.begin(115200);

  NimBLEDevice::init("ESP32_BGL");

  NimBLEServer *server=NimBLEDevice::createServer();
  NimBLEService *service=server->createService(SERVICE_UUID);

  pChar=service->createCharacteristic(
    CHAR_UUID,
    NIMBLE_PROPERTY::READ|NIMBLE_PROPERTY::NOTIFY
  );

  service->start();

  NimBLEAdvertising *adv=NimBLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->start();

  Serial.println("BLE Ready");
}

void loop(){
  if(indexBGL<25 && millis()-lastSend>=5000){
    lastSend=millis();

    float value=bgl[indexBGL];

    pChar->setValue((uint8_t*)&value,sizeof(value));
    pChar->notify();

    Serial.print("Send [");
    Serial.print(indexBGL);
    Serial.print("] = ");
    Serial.println(value,1);

    indexBGL++;

    if(indexBGL>=25){
      Serial.println("All BGL data sent.");
    }
  }
}
