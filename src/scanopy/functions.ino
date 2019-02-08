// takes a LiquidCrystal instance, a column number, a row number and the data to print to screen as a string
void printToLCD(const LiquidCrystal &lcd, int column, int row, String value)
{
    lcd.setCursor(column, row);
    lcd.print(value);
}

// setup internet option
bool internetSetup()
{
    SerialMon.print(F("Waiting for network ..."));
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        delay(1000);
        return false;
    }

    SerialMon.println(" Ok");

    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);

    if (!modem.gprsConnect(apn, user, pass))
    {
        SerialMon.println(" failed");
        delay(1000);
        return false;
    }

    SerialMon.println(" Ok");

    return true;
}

// using PUT method instead, to efficiently create or update data
int makePostRequest(const HttpClient &http, const String requestURL, const JsonObject &data)
{
    const String contentType = "application/json";
    String dataStr;

    data.printTo(dataStr);

    SerialMon.print(F("Sending data to server ..."));

    http.put(requestURL, contentType, dataStr);

    int responseCode = http.responseStatusCode();

    return responseCode;
}

double *gpsCordinates(const TinyGPSPlus &gps, bool time)
{
    double latitude;
    double longitude;
    int currentTime;
    double cordinates[3];

    SerialMon.print(F("Checking location: ..."));

    if (gps.location.isValid())
    {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }

    if (gps.time.isValid())
    {

        currentTime = gps.time.value();
    }

    if (time)
    {
        cordinates[0] = latitude, cordinates[1] = longitude, cordinates[2] = currentTime;
    }
    else
    {
        cordinates[0] = latitude, cordinates[1] = longitude;
    }

    return cordinates;
}