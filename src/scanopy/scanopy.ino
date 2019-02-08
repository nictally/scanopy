#include "scanopy.h"
#include "GasSensor.h"

#include <Wire.h>
#include <DHT.h>
#include <TinyGsmClient.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

#define SerialMon Serial

SoftwareSerial Serialgsm(gsmRX, gsmTX);
SoftwareSerial Serialgps(gpsRX, gpsTX);

TinyGsm modem(Serialgsm);
TinyGsmClient client(modem);

HttpClient http(client, server, port);
TinyGPSPlus gps;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

GasSensor Gas_1(gas1Pin, gas1Average, gas1Safe);
GasSensor Gas_2(gas2Pin, gas2Average, gas2Safe);

/**
 *  create an instance of jsonBuffer and set root JSON
     * create a nested array in the root JSON.
     *  Final data => { "data":[{sensor: 'gps', time:123, lat: 123, long: 124}, {sensor: 'dht', time: 123, temp: 232, hum:234}]}
     * */
DynamicJsonBuffer jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
JsonArray &data = root.createNestedArray("data");

void setup()
{
    SerialMon.begin(115200);
    delay(10);

    //GMS module baud
    Serialgsm.begin(115200);
    Serialgps.begin(4800);
    delay(300);

    SerialMon.println(F("Init modem ..."));
    modem.init();

    dht.begin();
    lcd.begin(columns, rows);

    Gas_1.begin();
}

void loop()
{
    // internetSetup must remain first in loop()
    bool gotInternet = internetSetup();
    if (!gotInternet)
    {
        return;
    }

    //should the code proceed when gps is not available ?
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
        Serial.println(F("No GPS detected: check wiring."));
        //remove return if system should continue without gps
        return;
    }

    double *gpsValues = gpsCordinates(gps, true);

    //complete JSON for sending data to server
    //gps
    JsonObject &gpsData = jsonBuffer.createObject();
    gpsData["sensor"] = "gps";
    gpsData["lat"] = gpsValues[0];
    gpsData["lng"] = gpsValues[1];
    gpsData["time"] = gpsValues[2];
    data.add(gpsData);

    //dht
    JsonObject &dhtData = jsonBuffer.createObject();
    dhtData["sensor"] = "DHT11";
    dhtData["temp"] = dht.readTemperature();
    dhtData["hum"] = dht.readHumidity();
    data.add(dhtData);

    //gasSensors
    //Gas_1 dummy
    JsonObject &Gas_1Data = jsonBuffer.createObject();
    Gas_1Data["sensor"] = "someSensor";
    Gas_1Data["safety"] = Gas_1.readSafety();
    Gas_1Data["ppm"] = Gas_1.readConcentration();
    data.add(Gas_1Data);

    //Gas_2 dummy
    JsonObject &Gas_2Data = jsonBuffer.createObject();
    Gas_2Data["sensor"] = "someSensor";
    Gas_2Data["safety"] = Gas_2.readSafety();
    Gas_2Data["ppm"] = Gas_2.readConcentration();
    data.add(Gas_2Data);

    // post request must come after all other code
    int statusCode = makePostRequest(http, resource, root);
    if (statusCode != 200 || statusCode != 204)
    {
        SerialMon.println(" failed to send data");
        return;
    }
    delay(1000);
}
