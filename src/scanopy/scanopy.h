#ifndef _Scanopy
#define _Scanopy

//GSM connection details
const char apn[]  = "";
const char user[] = "";
const char pass[] = "";

// Server details
const char server[] = "";
const char resource[] = "";
const int  port = 443; // for https

// Constants -- TODO: change/keep
#define DHTPIN 2
#define DHTTYPE DHT11

//Software serial 
const uint8_t gsmRX = 2, gsmTX = 3;
const uint8_t gpsRX = 5, gpsTX = 4;

//LCD 
const int rs = 10, en = 13, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
const int columns = 24, rows = 4;

// modem -
#define TINY_GSM_MODEM_SIM800

//esp8266 wifi
const char* ssid     = "";
const char* password = "";

//Gas Sensors
const int gas1Pin = 12;
const int gas1Average = 20;
const int gas1Safe = 4;

const int gas2Pin = 11;
const int gas2Average = 50;
const int gas2Safe = 10;

#endif

