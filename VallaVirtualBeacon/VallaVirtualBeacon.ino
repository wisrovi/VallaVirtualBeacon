#define macBuscar "90e2028d7b20"



#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BLEBeacon.h"

int scanTime = 7; //In seconds
BLEScan* pBLEScan;
boolean dispositivoEncontrado = false;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      std::string strManufacturerData = advertisedDevice.getManufacturerData();
      if (strManufacturerData.length() == 25) {
        uint8_t cManufacturerData[100];
        strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
        String adr = advertisedDevice.getAddress().toString().c_str();
        if (cManufacturerData[0] == 0x4C  && cManufacturerData[1] == 0x00 ) {
          BLEBeacon oBeacon = BLEBeacon();
          oBeacon.setData(strManufacturerData);
          adr.replace(":", "");
          String uuid_unico = oBeacon.getProximityUUID().toString().c_str();
          String macIntoUuid = uuid_unico.substring(24, 26) + uuid_unico.substring(21, 23) + uuid_unico.substring(19, 21) ;
          macIntoUuid = macIntoUuid + uuid_unico.substring(16, 18) + uuid_unico.substring(14, 16) + uuid_unico.substring(11, 13) ;
          if (adr.equals(macIntoUuid)) {
            if (uuid_unico.substring(26).equals("8667700000")) {
              dispositivoEncontrado = true;
              if(macIntoUuid.equals(macBuscar)){
                digitalWrite(2, HIGH);
              }
              
              Serial.print("uuid_unico: "); Serial.println(  uuid_unico  );
              Serial.println("**********************************************************************************");
            }
          }
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(21, OUTPUT);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

byte conteoSinPresencia = 0;
void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  if (dispositivoEncontrado) {
    digitalWrite(21, HIGH);
    conteoSinPresencia = 0;
  } else {
    conteoSinPresencia++;
    if(conteoSinPresencia>=3){
      conteoSinPresencia = 0;
      digitalWrite(2, LOW);
      Serial.println("No se encontro MAC a buscar");
    }
    digitalWrite(21, LOW);
  }
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  dispositivoEncontrado = false;
  delay(500);
}
