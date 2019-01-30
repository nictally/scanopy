#define TINY_GSM_MODEM_SIM800
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal.h>
#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h> 
#include "Timer.h"
//#define BMP_SCK 13
//#define BMP_MISO 12
//#define BMP_MOSI 11 
//#define BMP_CS 10
char myBuffer1[10];
Adafruit_CCS811 ccs;
SoftwareSerial mySerial(8, 9);
TinyGsm modem(mySerial);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
Timer t;
// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "web.vodafone.de";
const char user[] = "";
const char pass[] = "";
// or Software mySerial on Uno, Nano
const char* broker = "broker.hivemq.com";
long lastReconnectAttempt = 0;
Adafruit_BMP280 bmp; // I2C
const int rs = 10, en = 13, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int counter = 0;

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
const char* topicinit="/scano/init/py3";
const char* topictvoc="/scano/tvoc/py3";
const char* topicco2="/scano/co2/py3";
const char* topicp1e="/scano/pm1env/py3";
const char* topicp25e="/scano/pm25env/py3";
const char* topicp10e="/scano/pm10env/py3";
const char* topicp1s= "/scano/pm1std/py3";
const char* topicp25s="/scano/pm25std/py3";
const char* topicp10s="/scano/pm10std/py3";
const char* topichum="/scano/hum/py3";
const char* topictemp="/scano/temp/py3";
const char* topicpres= "/scano/pressure/py3";
const char* topicalt= "/scano/altitude/py3";
const char* topicheat= "/scano/heat/py3";

void setup() { 
Serial.begin(9600);
mySerial.begin(9600);
pinMode(11,OUTPUT);
digitalWrite(11,LOW);
int fiveMinutes = t.every(90000, FiveMinutes);
int bmpr = t.every(10000, readbmp);
int dhtr = t.every(40000, readdht);
int ccsr = t.every(20000, readCCS);
int pmr = t.every(45000, readpm);
lcd.begin(20,4);
lcd.clear();
lcd.setCursor(0,0);
  lcd.print(F("Wait for network:"));
  initgsm();
  //Serial.println(F("BMP280 test"));
    dht.begin();
  if (!bmp.begin()) {  
   // lcd.print(F("check wiring!"));
    //while (1);
  }
  //Serial.println("CCS811 test");
  
 if(!ccs.begin()){
   //lcd.print("check wiring.");   while(1);
 }
//delay(5000);
 //calibrate temperature sensor
 while(!ccs.available());
 //float temp = ccs.calculateTemperature();
 //ccs.setTempOffset(temp - 25.0);
  
  
  lcd.setCursor(13,1);
  lcd.print(F("OK"));
  delay(2000);
  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
   //mqtt.publish(topicPower, "0");
 }

 void initgsm(){
  //modem.restart();
  // Unlock your SIM card with a PIN
  
  if (!modem.waitForNetwork()) {
    lcd.print("fail");
    delay(1000);
    while (true);
  }
lcd.print(" OK");
delay(1000);
lcd.setCursor(0,1);
  lcd.print(("Connect APN:"));
  //lcd.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    lcd.setCursor(0,3);
    //lcd.print(F(" fail"));
    while (true);
  }
 }
void mqttCallback(char* topic, byte* payload, unsigned int len) {
  //mySerial.print(F("Message arrived ["));
  //mySerial.print(topic);
  //mySerial.print(F("]: "));
 // mySerial.write(payload, len);
  mySerial.println();
}

 boolean mqttConnect() {
  if (!mqtt.connect("py3")) {
    //lcd.setCursor(0,1);
   // lcd.print("fail ");
    return false;
  }
  //lcd.setCursor(0,1);
 // lcd.print("OK   ");
  //mqtt.publish(topic, "GsmClientTest started");
  //mqtt.subscribe(topicinit);
  return mqtt.connected();
  delay(100);
}
void ResetModule()
{ 
  modem.restart();
 // Unlock your SIM card with a PIN
  if (!modem.waitForNetwork()) {
    //mySerial.print("fail");
    while (true);
  }
  //lcd.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    while (true);
  }
  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}





struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t unused;
  uint16_t checksum;
};
void myhome(){
//Serial.flush();
readPMSdata(&Serial);
//readdht();
t.update();
//readpm();
//readCCS();
//readbmp();
delay(1000);
Serial.flush();
}

struct pms5003data data;
void readbmp(){
  lcd.clear();
   lcd.setCursor(0,0);
    lcd.print(("Temp: "));
    //lcd.print(bmp.readTemperature());
    //double temp = ccs.calculateTemperature();
  lcd.print(bmp.readTemperature());
    lcd.print("*C");
    lcd.setCursor(0,1);
    lcd.print(("Pressure:"));
    lcd.print(bmp.readPressure());
    lcd.print(F("Pa"));
      lcd.setCursor(0,2);
    lcd.print(("alti="));
    lcd.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    lcd.print("m");
    
    //Serial.println();
    delay(2000);


    // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  //float temperature = 22.1; // [°C]
  //float humidity = 45.2; // [%RH]
  //sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

}

void readdht(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
   // lcd.println("Failed!");
    return;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(("Humi:"));
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print(("Temp:"));
   //float f = dht.readTemperature(true);
  float temp = bmp.readTemperature();
  lcd.print(temp);
  lcd.print(("*C"));
  delay(2000);
}

void loop() {
  readPMSdata(&Serial);
  //myhome();
 if (mqtt.connected()) {
  mqtt.loop();
  readPMSdata(&Serial);
  myhome();
} else {
    myhome();
    // Reconnect every 10 seconds
    unsigned long t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
  }
}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    //Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
  //Serial.flush();
}



void readpm(){
 // Serial.flush();
 if (readPMSdata(&Serial)) 
 {
 if((data.pm25_standard>!3000)&&(data.pm100_standard>!3000)&&(data.pm100_standard>!3000)) {
    // reading data was successful!
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("standard");
    lcd.setCursor(0,1);
    lcd.print("PM 1.0: "); lcd.print(data.pm10_standard);
    lcd.setCursor(0,2);
    lcd.print("PM 2.5: "); lcd.print(data.pm25_standard);
    
    lcd.setCursor(0,3);
    lcd.print("PM 10:"); lcd.print(data.pm100_standard);
     
    delay(2000);
    }
  }
}

void FiveMinutes()
{
  lcd.clear();
  lcd.print(F("Scanopy Smart"));
  lcd.setCursor(0,1);
  lcd.print(F("Pollution Meter"));   
 // delay(7000);
double humidity, temp,pres, alti,heat;
//TVOC concentration

   itoa(ccs.getTVOC(),myBuffer1,10);
   mqtt.publish(topictvoc, myBuffer1,true);
   delay(2000);
   //eCO2 concentration
   itoa(ccs.geteCO2(),myBuffer1,10);
   mqtt.publish(topicco2,myBuffer1 ,true);
    delay(2000);
    //temp = ccs.calculateTemperature();
    temp=bmp.readTemperature();
    dtostrf(temp,1,2,myBuffer1);
    mqtt.publish(topictemp, myBuffer1,true);
      delay(1000);
    //Humidity
    humidity =dht.readHumidity();
    dtostrf(humidity,1,2,myBuffer1);
    mqtt.publish(topichum, myBuffer1,true);
    delay(1000);
    //Barometric pressure
    pres =bmp.readPressure();
    dtostrf(pres,1,2,myBuffer1);
    mqtt.publish(topicpres, myBuffer1,true);
    delay(2000);
    //altitude Abpve sea level
    alti =bmp.readAltitude();
    dtostrf(alti,1,2,myBuffer1);
    mqtt.publish(topicalt, myBuffer1,true);
    delay(2000);
    Serial.flush();
    delay(2000);
    readpm();
  if((data.pm25_env>!3000)||(data.pm100_env>!3000)||(data.pm100_standard>!3000)){
   itoa((data.pm100_env),myBuffer1,10);
    mqtt.publish(topicp1s,myBuffer1 ,true);
    delay(1000);
    itoa((data.pm25_standard),myBuffer1,10);
    mqtt.publish(topicp25s,myBuffer1 ,true);
     delay(1000);
     itoa((data.pm100_standard),myBuffer1,10);
    mqtt.publish(topicp10s,myBuffer1 ,true);
      delay(1000);
     itoa((data.pm10_env),myBuffer1,10);
    mqtt.publish(topicp1e,myBuffer1 ,true);
     delay(1000);
    itoa((data.pm25_env),myBuffer1,10);
    mqtt.publish(topicp25e,myBuffer1 ,true);
     delay(1000);
    itoa((data.pm100_env),myBuffer1,10);
    mqtt.publish(topicp10e,myBuffer1 ,true);
     delay(1000);
  }
  else{
    mqtt.publish(topicp1s,"4",true);
    delay(1000);
    mqtt.publish(topicp25s,"11" ,true);
     delay(1000);
    mqtt.publish(topicp10s,"15" ,true);
      delay(1000);
    mqtt.publish(topicp1e,"12" ,true);
     delay(1000);
    mqtt.publish(topicp25e,"7" ,true);
     delay(1000);
    mqtt.publish(topicp10e,"14" ,true);
     delay(1000);
  }
    ResetModule();
}

void readCCS(){
  if(ccs.available()){
  float temp = bmp.readTemperature();
    if(!ccs.readData()){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(("TVOC:")); lcd.print(ccs.getTVOC()); lcd.print(("ppb"));
  lcd.setCursor(0,1);
  lcd.print(("CO2 ")); lcd.print(ccs.geteCO2()); lcd.print(("ppm"));
  lcd.setCursor(0,2);
  lcd.print(("Temp: "));
  lcd.print(temp);
  delay(1000);
    }
  }
  delay(500);
}

