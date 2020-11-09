/*
 ***********************************************************************
 *
 *  ESP32_mini_weather - compact weather station based on a
 *                       lolin D32 module
 *
 *  Copyright (C) 2020 Dreamshader (aka Dirk Schanz)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ***********************************************************************
 *
 * This project requires a graphical LCD of type 12864, a RTC of
 * type DS3231 and, of course, a hc-12 serial module.
 * Keep in mind to reduce power of the hc-12 to 3V3 to avoid overheating
 * the integrated LDO.
 * In addition, to control and interact with this firmware, you will 
 * need 5 tactile momentary push buttons.
 * For further information refer to the documentation.
 *
 * --------------------------------------------
 * History:
 * 20/10/16 - initial version
 *
 * Known issues:
 *
 ***********************************************************************
 */

//
// ----------- features ------------
//
#define ENABLE_NTP
// #undef ENABLE_NTP

#define ENABLE_SERIAL
// #undef ENABLE_SERIAL

#define ENABLE_WIFI
// #undef ENABLE_WIFI

#define ENABLE_HTTP
// #undef ENABLE_HTTP

#define ENABLE_TELNET
// #undef ENABLE_TELNET

#define ENABLE_MENU
// #undef ENABLE_MENU

#define ENABLE_SERIAL_MENU
// #undef ENABLE_SERIAL_MENU

#define ENABLE_TELNET_MENU
// #undef ENABLE_TELNET_MENU

#define ENABLE_HTTP_MENU
// #undef ENABLE_HTTP_MENU

#define ENABLE_BME280
// #undef ENABLE_BME280

// #define ENABLE_BMP280
#undef ENABLE_BMP280

#define ENABLE_DISPLAY
// #undef ENABLE_DISPLAY

#define DISPLAY_ST7789_130
// #define DISPLAY_ST7789_096
// #define DISPLAY_ILI9341_220
// #define DISPLAY_ILI9341_TOUCH_220

// #define ENABLE_BACKLIGHT_GPIO
#undef ENABLE_BACKLIGHT_GPIO

// #define ENABLE_PUSHBUTTONS
#undef ENABLE_PUSHBUTTONS

#define ENABLE_FLASH_STORE
// #undef ENABLE_FLASH_STORE
//
// ---------------------------------------------------------------------
//
#ifdef ENABLE_BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#endif // ENABLE_BME280
//
// ---------------------------------
//
#ifdef ENABLE_DISPLAY
    #ifdef DISPLAY_ST7789_130
#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library by Adafruit
#include <Arduino_ST7789.h> // Hardware-specific library for ST7789
    #endif // DISPLAY_ST7789_130
#endif // ENABLE_DISPLAY
//
// ---------------------------------
//
#ifdef ENABLE_NTP
#include <ezTime.h>
#endif // ENABLE_NTP
//
// ---------------------------------
//
#ifdef ENABLE_WIFI
#include <WiFi.h>
#endif // ENABLE_WIFI
//
// ---------------------------------
//
#ifdef ENABLE_FLASH_STORE
#include <SPIFFS.h>
#endif // ENABLE_FLASH_STORE

//
// ------------------------- FLASH configuration -----------------------
//
#define FLASH_DATA_FILE       "/bin-data.bin"

//
// ---------------------- pushbutton configuration ---------------------
//
#define PUSHBUTTON_BLACK   17
#define PUSHBUTTON_GREEN   16
#define PUSHBUTTON_RED      4

#define ESP32_NO_PIN       -1
//
// ----------------------- display configuration -----------------------
//
// ------- display interface -------
// 
#define ENABLE_SOFTWARE_SPI
// #undef ENABLE_SOFTWARE_SPI
// #define ENABLE_HARDWARE_SPI
#undef ENABLE_HARDWARE_SPI
//
// #define TFT_MISO  19   // for hardware SPI data pin (all of available pins)
#undef  TFT_MISO
#define TFT_MOSI  23   // for hardware SPI data pin (all of available pins)
#define TFT_SCLK  18   // for hardware SPI sclk pin (all of available pins)
#undef  TFT_CS
// #define TFT_CS     5   // only for displays with CS pin

#define TFT_DC    13
#define TFT_RST   15

#define TFT_WIDTH   240
#define TFT_HEIGHT  240

//
// --------- display entity --------
//
//or you can use software SPI on all available pins (slow)
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);

//
// -------------------- BME280 sensor configuration --------------------
//
// --------- related values --------
//
// https://de.wikipedia.org/wiki/Luftdruck
#define SEALEVELPRESSURE_HPA (1013.25)
#define BME_I2C_ADDR             0x76  
#define BME_LOCAL_SENSOR_ID        22
//
// --------- sensor entity ---------
//
Adafruit_BME280 bme; // I2C

//
// ----------------------- serial configuration ------------------------
//
#define SERIAL_BAUD   57600

//
// ------------------------ WIFI configuration ------------------------
//
// --------- related values --------
//

#define WLAN_SSID       "TP-LINK"
#define WLAN_PASSPHRASE "Das_ist_eine_1a_sichere_Passphrase"

// Replace with your network credentials
const char* ssid = WLAN_SSID;
const char* password = WLAN_PASSPHRASE;

// Set web server port number to 80
WiFiServer httpServer(80);

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//
// --------- WIFI entities ---------
//

// declare telnet server (do NOT put in setup())
WiFiServer telnetServer(23);
WiFiClient telnetClient;

//
// ---------------------- timezone configuration ----------------------
//
// ------- timezone entities -------
//
Timezone myTZ;


#define SUNDAY            1
#define MONDAY            2
#define TUESDAY           3
#define WEDNESDAY         4
#define THURSDAY          5
#define FRIDAY            6
#define SATURDAY          7

#define JANUARY           1
#define FEBRUARY          2
#define MARCH             3
#define APRIL             4
#define MAY               5
#define JUNE              6
#define JULY              7
#define AUGUST            8
#define SEPTEMBER         9
#define OCTOBER          10
#define NOVEMBER         11
#define DECEMBER         12


//
// ----------------------- system configuration -----------------------
//

#define VALUE_TYPE_FLOAT           'f'
#define VALUE_TYPE_LONG            'l'

struct _sensor_value_ {
    bool is_valid;
    bool has_changed;
    char value_type;
    unsigned long long_data;
    float float_data;
};

struct _system_vars_ {

    bool valid_serial;
    bool valid_display;
    bool valid_bme280;
    bool valid_wlan;
    bool valid_http;
    bool valid_telnet;
    bool valid_NTP;
    bool valid_FLASH;
    //
    float bme_sealevel_hpa;
    //
    unsigned long local_bme_temperature_interval;
    unsigned long local_bme_humidity_interval;
    unsigned long local_bme_pressure_interval;
    //
    unsigned long local_bme_last_temperature_read;
    unsigned long local_bme_last_humidity_read;
    unsigned long local_bme_last_pressure_read;
    //
    struct _sensor_value_ local_bme_temperature;
    struct _sensor_value_ last_local_bme_temperature;
    struct _sensor_value_ local_bme_humidity;
    struct _sensor_value_ last_local_bme_humidity;
    struct _sensor_value_ local_bme_pressure;
    struct _sensor_value_ last_local_bme_pressure;
    //
    unsigned long http_timeoutTime;
    //
    unsigned long telnet_timeoutTime;
    //
    unsigned long wlan_timeoutTime;

    // new -> internals completed
    char *pAdminPassword;
    byte networkMode;
    unsigned long wlanTimeout;
    char *pStaticIP;
    char *pWlanSSID;
    char *pWlanPassphrase;
    byte uartMode;
    bool uartAPI;
    byte uartAPIAccess;
    char *pUartApiUser;
    char *pUartApiPassword;
    unsigned long uartApiTimeout;
    byte httpMode;
    bool httpAPI;
    byte httpAPIAccess;
    char *pHttpApiUser;
    char *pHttpApiPassword;
    unsigned long httpApiTimeout;
    byte telnetMode;
    bool telnetAPI;
    byte telnetAPIAccess;
    char *pTelnetApiUser;
    char *pTelnetApiPassword;
    unsigned long telnetApiTimeout;
    uint16_t colorBackground;
    uint16_t colorForeground;
    byte weekdayFormat;
    byte dateFormat;
    byte timeFormat;
    byte displayHumidity;
    long humidityUpperLimit;
    long humidityLowerLimit;
    byte displayTemperature;
    long temperatureUpperLimit;
    long temperatureLowerLimit;
    byte displayPressure;
    long pressureUpperLimit;
    long pressureLowerLimit;
    bool displayIPAddress;
    bool displayMoonphase;
    byte displayLipo;
};

#define HTTP_TIMEOUT_MILLIS      2000
#define TELNET_TIMEOUT_MILLIS    2000
#define WIFI_TIMEOUT_MILLIS     30000

#define TIME_MINUTES_TO_SECONDS(M)	(M*60)
#define TIME_HOURS_TO_SECONDS(H)	(H*60*60)
#define TIME_DAYS_TO_SECONDS(D)		(D*24*60*60)

#define BME_TEMPERATURE_INTERVAL  TIME_MINUTES_TO_SECONDS(2)
#define BME_HUMIDITY_INTERVAL     TIME_MINUTES_TO_SECONDS(3)
#define BME_PRESSURE_INTERVAL     TIME_MINUTES_TO_SECONDS(4)

struct _system_vars_ _envVars;

//
// ************************** SYSTEM SETTINGS **************************
//


#define FIELD_ID_ADMIN_PASSWORD                            1
#define FIELD_ID_NETWORK_MODE                              2
#define FIELD_ID_WLAN_TIMEOUT                              3
#define FIELD_ID_WLAN_STATIC_IP_ADDR                       5
#define FIELD_ID_WLAN_SSID                                 6
#define FIELD_ID_WLAN_PASSPHRASE                           7
#define FIELD_ID_UART_MODE                                 8
#define FIELD_ID_UART_API                                  9
#define FIELD_ID_UART_API_ACCESS                          10
#define FIELD_ID_UART_API_USER                            11
#define FIELD_ID_UART_API_PASSWORD                        12
#define FIELD_ID_UART_API_TIMEOUT                         13
#define FIELD_ID_HTTP_MODE                                14
#define FIELD_ID_HTTP_API                                 15
#define FIELD_ID_HTTP_API_ACCESS                          16
#define FIELD_ID_HTTP_API_USER                            17
#define FIELD_ID_HTTP_API_PASSWORD                        18
#define FIELD_ID_HTTP_API_TIMEOUT                         19
#define FIELD_ID_TELNET_MODE                              20
#define FIELD_ID_TELNET_API                               21
#define FIELD_ID_TELNET_API_ACCESS                        22
#define FIELD_ID_TELNET_API_USER                          23
#define FIELD_ID_TELNET_API_PASSWORD                      24
#define FIELD_ID_TELNET_API_TIMEOUT                       25

#define FIELD_ID_SEALEVEL_PRESSURE                        26
#define FIELD_ID_BACKGROUND_COLOR                         27
#define FIELD_ID_FOREGROUND_COLOR                         28
#define FIELD_ID_WEEKDAY_FORMAT                           29
#define FIELD_ID_DATE_FORMAT                              30
#define FIELD_ID_TIME_FORMAT                              31
#define FIELD_ID_INTERNAL_HUMIDITY_INTERVAL               32
#define FIELD_ID_INTERNAL_TEMPERATURE_INTERVAL            33
#define FIELD_ID_INTERNAL_PRESSURE_INTERVAL               34
#define FIELD_ID_INTERNAL_DISPLAY_HUMIDITY                35
#define FIELD_ID_INTERNAL_HUMIDITY_UPPER_LIMIT            36
#define FIELD_ID_INTERNAL_HUMIDITY_LOWER_LIMIT            37
#define FIELD_ID_INTERNAL_DISPLAY_TEMPERATURE             38
#define FIELD_ID_INTERNAL_TEMPERATURE_UPPER_LIMIT         39
#define FIELD_ID_INTERNAL_TEMPERATURE_LOWER_LIMIT         40
#define FIELD_ID_INTERNAL_DISPLAY_PRESSURE                41
#define FIELD_ID_INTERNAL_PRESSURE_UPPER_LIMIT            42
#define FIELD_ID_INTERNAL_PRESSURE_LOWER_LIMIT            43
#define FIELD_ID_DISPLAY_IP_ADDRESS                       44
#define FIELD_ID_DISPLAY_MOON_PHASE                       46
#define FIELD_ID_DISPLAY_LIPO_MODE                        47

#define UART_MODE_DISABLED                                 0
#define UART_MODE_ENABLED                                  1
#define NETWORK_MODE_STA                                   1
#define NETWORK_MODE_AP                                    2
#define UART_API_DISABLED                                  0
#define UART_API_ENABLED                                   1
#define UART_API_ACCESS_USER                               1
#define UART_API_ACCESS_ANONYMOUS                          2
#define UART_API_ACCESS_NONE                               4
#define HTTP_MODE_DISABLED                                 0
#define HTTP_MODE_ENABLED                                  1
#define HTTP_API_DISABLED                                  0
#define HTTP_API_ENABLED                                   1
#define HTTP_API_ACCESS_USER                               1
#define HTTP_API_ACCESS_ANONYMOUS                          2
#define HTTP_API_ACCESS_NONE                               4
#define TELNET_MODE_DISABLED                               0
#define TELNET_MODE_ENABLED                                1
#define TELNET_API_DISABLED                                0
#define TELNET_API_ENABLED                                 1
#define TELNET_API_ACCESS_USER                             1
#define TELNET_API_ACCESS_ANONYMOUS                        2
#define TELNET_API_ACCESS_NONE                             4
#define WEEKDAY_DISABLED                                   0
#define WEEKDAY_ENABLED                                    1
#define DATE_DISABLED                                      0
#define DATE_ENABLED                                       1
#define TIME_DISABLED                                      0
#define TIME_SHORT                                         1
#define TIME_LONG                                          2
#define INTERNAL_DISPLAY_HUMIDITY_ENABLED                  1
#define INTERNAL_DISPLAY_HUMIDITY_TREND                    2
#define INTERNAL_DISPLAY_HUMIDITY_MONO                     4
#define INTERNAL_DISPLAY_HUMIDITY_COLOR                    8
#define INTERNAL_DISPLAY_TEMPERATURE_ENABLED               1
#define INTERNAL_DISPLAY_TEMPERATURE_TREND                 2
#define INTERNAL_DISPLAY_TEMPERATURE_MONO                  4
#define INTERNAL_DISPLAY_TEMPERATURE_COLOR                 8
#define INTERNAL_DISPLAY_PRESSURE_ENABLED                  1
#define INTERNAL_DISPLAY_PRESSURE_TREND                    2
#define INTERNAL_DISPLAY_PRESSURE_MONO                     4
#define INTERNAL_DISPLAY_PRESSURE_COLOR                    8
#define DISPLAY_LIPO_DISABLED                              0
#define DISPLAY_LIPO_CAPACITY                              1
#define DISPLAY_LIPO_RUNTIME                               2

#define INTERNAL_HUMIDITY_UPPER_LIMIT                     -1
#define INTERNAL_HUMIDITY_LOWER_LIMIT                     -1
#define INTERNAL_TEMPERATURE_UPPER_LIMIT                  -1
#define INTERNAL_TEMPERATURE_LOWER_LIMIT                  -1
#define INTERNAL_PRESSURE_UPPER_LIMIT                     -1
#define INTERNAL_PRESSURE_LOWER_LIMIT                     -1


//
// *************************** FUNCTIONALITY ***************************
//

// ---------------------------------------------------------------------
//  bool setupDisplay(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize display entity
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool setupDisplay(void)
{
    bool retVal = true;

    Serial.println("Init ST7789 display");

    // initialize a ST7789 chip, 240x240 pixels
    tft.init(TFT_WIDTH, TFT_HEIGHT);   
    tft.setRotation(3);

    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor(0, 0);
    tft.setTextWrap(false);
    tft.setTextSize(2);

    return(retVal);
}

// ---------------------------------------------------------------------
//  bool setupBME280(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize BME 280 sensor entity
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool setupBME280(void)
{
    bool retVal = true;

    Serial.println("Init BME280");

    if( !bme.begin(BME_I2C_ADDR) )
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        retVal = false;
    }

    return(retVal);
}

// ---------------------------------------------------------------------
//  bool setupPins(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize I/Os 
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool setupPins(void)
{
    bool retVal = true;

    Serial.println("Setup pins");

    // Initialize the output variables as outputs
    pinMode(PUSHBUTTON_BLACK, INPUT);
    pinMode(PUSHBUTTON_GREEN, INPUT);
    pinMode(PUSHBUTTON_RED,   INPUT);

    pinMode(LED_BUILTIN,      OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off 

    return(retVal);
}


// ---------------------------------------------------------------------
//  void printDirectory(File dir, int numTabs) {
// ------------
//  arguments: File dir
//             int numTabs
// ------------
//  purpose  : initialize flash memory
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
void printDirectory(File dir, int numTabs) 
{
  while (true) 
  {
 
    File entry =  dir.openNextFile();
    if (! entry) 
    {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) 
    {
      Serial.print('\t');
    }

    Serial.print(entry.name());
    if (entry.isDirectory()) 
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } 
    else 
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


// ---------------------------------------------------------------------
//  bool setupFlash(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize flash memory
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool setupFlash(void)
{
    bool retVal = true;

    Serial.println("Setting up flash filesystem ...");

    if( !SPIFFS.begin(true) ) 
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        retVal = false;
    }
    else
    {
        // To format all space in SPIFFS
        // SPIFFS.format()
 
        // Get all information of your SPIFFS
 
        unsigned int totalBytes = SPIFFS.totalBytes();
        unsigned int usedBytes = SPIFFS.usedBytes();
 
        Serial.println("File system info.");
 
        Serial.print("Total space:      ");
        Serial.print(totalBytes);
        Serial.println(" byte");
 
        Serial.print("Total space used: ");
        Serial.print(usedBytes);
        Serial.println(" byte");
 
        Serial.println();
 
        // Open dir folder
        File dir = SPIFFS.open("/");
        // Cycle all the content
        printDirectory(dir, 0);
    }

    return(retVal);
}


// WiFi.status() ==
// const char *str_status[]= {
//         "WL_IDLE_STATUS",
//         "WL_NO_SSID_AVAIL",
//         "WL_SCAN_COMPLETED",
//         "WL_CONNECTED",
//         "WL_CONNECT_FAILED",
//         "WL_CONNECTION_LOST",
//         "WL_DISCONNECTED"
// };

// ---------------------------------------------------------------------
//  bool connectWIFI() 
// ------------
//  arguments: -
// ------------
//  purpose  : connect to WLAN
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool connectWIFI(void)
{
    bool retVal = true;
    bool timeout = false;
    unsigned long millisBeginConnect;
    unsigned long tmout_millis;

// provide text for the WiFi mode
// const char *str_mode[]= { "WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA" };
// use in case of mode problem
// WiFi.disconnect();
// switch to Station mode
// if (WiFi.getMode() != WIFI_STA) 
// {
//     WiFi.mode(WIFI_STA);
// }

    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setup WIFI ...");

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    millisBeginConnect = millis();
    tmout_millis = _envVars.wlan_timeoutTime;
    timeout = false;

    while( !timeout && WiFi.status() != WL_CONNECTED )
    {
        if( millis() - millisBeginConnect >= tmout_millis )
        {
            retVal = false;
            timeout = true;
        }
        else
        {
            delay(500);
            Serial.print(".");
        }
    }

    if( !timeout )
    {
        // Print local IP address and start web server
        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("TIMEOUT connecting WIFI!");
    }

    return(retVal);

}

// ---------------------------------------------------------------------
//  bool setupTelnet(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize telnet server
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool setupTelnet(void)
{
    bool retVal = true;

    Serial.println("Setup telnet ...");

    telnetServer.begin();
    telnetServer.setNoDelay(true);

    httpServer.begin();
    _envVars.valid_http   = true;
    _envVars.valid_telnet = false;

    return(retVal);
}

// ---------------------------------------------------------------------
//  bool setupNTP(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : make ready to manage time based on NTP
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------


bool setupNTP(void)
{
    bool retVal = true;

    Serial.println("Setup timezone ...");

    // Uncomment the line below to see what it does behind the scenes
//    setDebug(INFO);
	
    waitForSync();

    // Or country codes for countries that do not span multiple timezones
    myTZ.setLocation("de");
//    Serial.print("Germany:         ");
//    Serial.println(myTZ.dateTime());

    return(retVal);
}

// ---------------------------------------------------------------------
//  bool setupHTTP(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : setup http server
// ------------
//  returns  : true on success, false otherwise
// ---------------------------------------------------------------------
bool setupHTTP(void)
{
    bool retVal = true;

    Serial.println("Setup http ...");

    httpServer.begin();

    return(retVal);
}

// ---------------------------------------------------------------------
//  void setupDefaults(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize all system vars with defaults
// ------------
//  returns  : nothing
// ---------------------------------------------------------------------
void setupDefaults(void) 
{
    Serial.println("Setup defaults ...");

    _envVars.valid_serial             = false;
    _envVars.valid_display            = false;
    _envVars.valid_bme280             = false;
    _envVars.valid_wlan               = false;
    _envVars.valid_http               = false;
    _envVars.valid_telnet             = false;
    _envVars.valid_NTP                = false;
    _envVars.valid_FLASH              = false;
    //
    _envVars.bme_sealevel_hpa         = SEALEVELPRESSURE_HPA;

    _envVars.local_bme_temperature_interval = BME_TEMPERATURE_INTERVAL * 1000;
    _envVars.local_bme_humidity_interval    = BME_HUMIDITY_INTERVAL * 1000;
    _envVars.local_bme_pressure_interval    = BME_PRESSURE_INTERVAL * 1000;

    _envVars.local_bme_last_temperature_read   = 0;
    _envVars.local_bme_last_humidity_read      = 0;
    _envVars.local_bme_last_pressure_read      = 0;

    _envVars.local_bme_temperature.is_valid    = false;
    _envVars.local_bme_temperature.has_changed = false;
    _envVars.local_bme_temperature.value_type  = VALUE_TYPE_FLOAT;
    _envVars.local_bme_temperature.long_data   = 0;
    _envVars.local_bme_temperature.float_data  = 0.0;

    _envVars.last_local_bme_temperature = _envVars.local_bme_humidity = 
    _envVars.last_local_bme_humidity = _envVars.local_bme_pressure = 
    _envVars.last_local_bme_pressure = _envVars.local_bme_temperature;

    _envVars.http_timeoutTime         = HTTP_TIMEOUT_MILLIS;
    //
    _envVars.telnet_timeoutTime       = TELNET_TIMEOUT_MILLIS;
    //
    _envVars.wlan_timeoutTime         = WIFI_TIMEOUT_MILLIS;

    // new -> internals completed
    _envVars.pAdminPassword         = (char*) NULL;
    _envVars.networkMode            = NETWORK_MODE_STA;
    _envVars.wlanTimeout            = 0L;
    _envVars.pStaticIP              = (char*) NULL;
    _envVars.pWlanSSID              = (char*) NULL;
    _envVars.pWlanPassphrase        = (char*) NULL;
    //
    _envVars.uartMode               = UART_MODE_DISABLED;
    _envVars.uartAPI                = false;
    _envVars.uartAPIAccess          = UART_API_ACCESS_NONE;
    _envVars.pUartApiUser           = (char*) NULL;
    _envVars.pUartApiPassword       = (char*) NULL;
    _envVars.uartApiTimeout          = 0L;
    //
    _envVars.httpMode               = HTTP_MODE_DISABLED;
    _envVars.httpAPI                = false;
    _envVars.httpAPIAccess          = HTTP_API_ACCESS_NONE;
    _envVars.pHttpApiUser           = (char*) NULL;
    _envVars.pHttpApiPassword       = (char*) NULL;
    _envVars.httpApiTimeout         = 0L;
    //
    _envVars.telnetMode             = TELNET_MODE_DISABLED;
    _envVars.telnetAPI              = false;
    _envVars.telnetAPIAccess        = TELNET_API_ACCESS_NONE;
    _envVars.pTelnetApiUser         = (char*) NULL;
    _envVars.pTelnetApiPassword     = (char*) NULL;
    _envVars.telnetApiTimeout       = 0L;
    //
    _envVars.colorBackground        = BLACK;
    _envVars.colorForeground        = WHITE;
    _envVars.weekdayFormat          = WEEKDAY_DISABLED;
    _envVars.dateFormat             = DATE_DISABLED;
    _envVars.timeFormat             = TIME_LONG;
    _envVars.displayHumidity        = INTERNAL_DISPLAY_HUMIDITY_ENABLED;
    _envVars.displayTemperature     = INTERNAL_DISPLAY_TEMPERATURE_ENABLED;
    _envVars.displayPressure        = INTERNAL_DISPLAY_PRESSURE_ENABLED;
    _envVars.displayMoonphase       = false;
    _envVars.displayLipo            = DISPLAY_LIPO_DISABLED;
    _envVars.displayIPAddress       = false;

    _envVars.humidityUpperLimit     = INTERNAL_HUMIDITY_UPPER_LIMIT;
    _envVars.humidityLowerLimit     = INTERNAL_HUMIDITY_LOWER_LIMIT;
    _envVars.temperatureUpperLimit  = INTERNAL_TEMPERATURE_UPPER_LIMIT;
    _envVars.temperatureLowerLimit  = INTERNAL_TEMPERATURE_LOWER_LIMIT;
    _envVars.pressureUpperLimit     = INTERNAL_PRESSURE_UPPER_LIMIT;
    _envVars.pressureLowerLimit     = INTERNAL_PRESSURE_LOWER_LIMIT;
}

// ---------------------------------------------------------------------
//  void showEnvVars(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : dump env vars
// ------------
//  returns  : nothing
// ---------------------------------------------------------------------
void showEnvVars(void) 
{

    Serial.println();
    Serial.println("ENV dump:");

    Serial.print("_envVars.valid_serial .: ");
    Serial.println( _envVars.valid_serial ? "true" : "false" );

    Serial.print("_envVars.valid_display : ");
    Serial.println( _envVars.valid_display ? "true" : "false" );

    Serial.print("_envVars.valid_bme280 .: ");
    Serial.println( _envVars.valid_bme280 ? "true" : "false" );

    Serial.print("_envVars.valid_wlan ...: ");
    Serial.println( _envVars.valid_wlan ? "true" : "false" );

    Serial.print("_envVars.valid_http ...: ");
    Serial.println( _envVars.valid_http ? "true" : "false" );

    Serial.print("_envVars.valid_telnet .: ");
    Serial.println( _envVars.valid_telnet ? "true" : "false" );

    Serial.print("_envVars.valid_NTP ....: ");
    Serial.println( _envVars.valid_NTP ? "true" : "false" );

    Serial.print("_envVars.valid_FLASH ..: ");
    Serial.println( _envVars.valid_FLASH ? "true" : "false" );

    Serial.print("_envVars.bme_sealevel_hpa ..............: ");
    Serial.println(_envVars.bme_sealevel_hpa);

    Serial.print("_envVars.local_bme_temperature_interval : ");
    Serial.println(_envVars.local_bme_temperature_interval);

    Serial.print("_envVars.local_bme_humidity_interval ...: ");
    Serial.println(_envVars.local_bme_humidity_interval);

    Serial.print("_envVars.local_bme_pressure_interval ...: ");
    Serial.println(_envVars.local_bme_pressure_interval);

    Serial.print("_envVars.http_timeoutTime ..............: ");
    Serial.println(_envVars.http_timeoutTime);

    Serial.print("_envVars.telnet_timeoutTime ............: ");
    Serial.println(_envVars.telnet_timeoutTime);

    Serial.print("_envVars.wlan_timeoutTime ..............: ");
    Serial.println(_envVars.wlan_timeoutTime);

}




bool writeData( char *pPath, float sensorValue, 
                unsigned char dataType, unsigned char sensorId )
{
    bool retVal = false;
    time_t timeStamp;
    File file;
    unsigned char *pData;
    int dataLen;
    int recordPosition;

    short   pressure;
    unsigned char humidity;
    unsigned char temp;
    unsigned char recType;
    unsigned char sensId;

    if( (retVal = _envVars.valid_FLASH) == true )
    {
        if( pPath != NULL )
        {
            file = SPIFFS.open(pPath, FILE_APPEND);

            if (!file) 
            {
                retVal = false;
// Serial.println("Error opening file for writing");
            }
            else
            {
                timeStamp = myTZ.now();
                recordPosition = file.position();

                pData = (unsigned char*) &sensorValue;
                dataLen = sizeof(sensorValue);

                if( file.write(&dataType, sizeof(dataType)) == 
                                sizeof(dataType) )
                {
// Serial.println("successfully wrote record Id");

                    if( file.write(&sensId, sizeof(sensId)) == 
                                    sizeof(sensId) )
                    {
// Serial.println("successfully wrote sensor Id");

                        if( file.write( (unsigned char*) &timeStamp, sizeof(timeStamp)) == 
                                        sizeof(timeStamp) )
                        {
// Serial.println("successfully wrote timestamp");

                            if( file.write(pData, dataLen) == dataLen )
                            {
                                retVal = true;
// Serial.println("successfully wrote record");
                            }
                            else
                            {
                                file.seek(recordPosition, SeekSet);
                                retVal = false;
// Serial.println("failed to write data");
                            }
                        }
                        else
                        {
                            file.seek(recordPosition, SeekSet);
                            retVal = false;
// Serial.println("failed to write timestamp");
                        }
                    }
                    else
                    {
                        file.seek(recordPosition, SeekSet);
                        retVal = false;
// Serial.println("failed to write sensor Id");
                    }
                }
                else
                {
                    file.seek(recordPosition, SeekSet);
                    retVal = false;
// Serial.println("failed to write record Id");
                }
    
                file.close();
            }
        }
        else
        {
            retVal = false;
// Serial.println("Path is NULL!");
        }
    }
    else
    {
        retVal = false;
// Serial.println("NO valid SPIFFS");
    }

    return( retVal );
}



// ---------------------------------------------------------------------
//  bool restoreSettings(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : read previous stored system settings from flash
// ------------
//  returns  : nothing
// ---------------------------------------------------------------------
bool restoreSettings(void) 
{
    bool retVal = true;

    if( (retVal = _envVars.valid_FLASH) == true )
    {
//        if( SPIFFS.exists("/test.txt") )
//        {
//            SPIFFS.remove("/test.txt");
//        }
//
//        File file = SPIFFS.open("/test.txt", FILE_WRITE);
//
//        if (!file) 
//        {
//            retVal = false;
//            Serial.println("There was an error opening the file for writing");
//        }
//        else
//        {
//            if (file.print("TEST")) 
//            {
//                Serial.println("File was written");
//            } 
//            else 
//            {
//                retVal = false;
//                Serial.println("File write failed");
//            }
//
//            file.close();
//        }
    }
    else
    {
        Serial.println("NO valid SPIFFS");
    }

    return(retVal);
}


//
// *********************************************************************


// ---------------------------------------------------------------------
//  void setup(void) 
// ------------
//  arguments: -
// ------------
//  purpose  : initialize system
// ------------
//  returns  : nothing
// ---------------------------------------------------------------------
void setup(void) 
{
    setupDefaults(); 

    Serial.begin(SERIAL_BAUD);
    _envVars.valid_serial = true;

//    showEnvVars();

    _envVars.valid_FLASH  = setupFlash();

    restoreSettings(); 

    _envVars.valid_display = setupDisplay();

    _envVars.valid_bme280 = setupBME280();

    _envVars.valid_wlan = connectWIFI();

    _envVars.valid_http = setupHTTP();

    _envVars.valid_telnet = setupTelnet();

    _envVars.valid_NTP = setupNTP();

    setupPins();
 
//    showEnvVars();

}

// ****************************** BITMAPS *********************************

#define arrow_down_width 18
#define arrow_down_height 24
static unsigned char arrow_down_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x1c, 0x00, 0x00,
   0x1c, 0x00, 0x00, 0x38, 0x00, 0x00, 0x78, 0x00, 0x00, 0x70, 0x00, 0x00,
   0xe0, 0x00, 0x00, 0xe0, 0x01, 0x00, 0xc0, 0x01, 0x00, 0x80, 0xe3, 0x00,
   0x80, 0xe7, 0x00, 0x00, 0xe7, 0x00, 0x00, 0xff, 0x00, 0x00, 0xfe, 0x00,
   0x00, 0xfe, 0x00, 0xe0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0xe0, 0xff, 0x00,
   0x00, 0xfe, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define arrow_right_down_width 18
#define arrow_right_down_height 24
static unsigned char arrow_right_down_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x3e, 0x00, 0x00,
   0x3e, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00,
   0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x07, 0x00,
   0x38, 0x1f, 0x00, 0x38, 0x3c, 0x00, 0x38, 0x70, 0x00, 0xf8, 0xff, 0x00,
   0xf8, 0xff, 0x01, 0xf8, 0xff, 0x00, 0x00, 0x70, 0x00, 0x00, 0x3c, 0x00,
   0x00, 0x0f, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define arrow_right_up_width 18
#define arrow_right_up_height 24
static unsigned char arrow_right_up_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x0f, 0x00,
   0x00, 0x3c, 0x00, 0x00, 0x70, 0x00, 0xf8, 0xff, 0x00, 0xf8, 0xff, 0x01,
   0xf8, 0xff, 0x00, 0x38, 0x70, 0x00, 0x38, 0x3c, 0x00, 0x38, 0x1f, 0x00,
   0x38, 0x07, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00,
   0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 0x3e, 0x00, 0x00,
   0x3e, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define arrow_right_width 18
#define arrow_right_height 24
static unsigned char arrow_right_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0xc0, 0x03, 0x00,
   0x00, 0x0f, 0x00, 0x00, 0x3f, 0x00, 0xfe, 0xff, 0x00, 0xfe, 0xff, 0x01,
   0xfe, 0xff, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x0f, 0x00, 0xc0, 0x03, 0x00,
   0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define arrow_up_width 18
#define arrow_up_height 24
static unsigned char arrow_up_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0xfe, 0x00,
   0xe0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0x00, 0xfe, 0x00,
   0x00, 0xfe, 0x00, 0x00, 0xff, 0x00, 0x00, 0xe7, 0x00, 0x80, 0xe7, 0x00,
   0x80, 0xe3, 0x00, 0xc0, 0x01, 0x00, 0xe0, 0x01, 0x00, 0xe0, 0x00, 0x00,
   0x70, 0x00, 0x00, 0x78, 0x00, 0x00, 0x38, 0x00, 0x00, 0x1c, 0x00, 0x00,
   0x1c, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define moon_full_width 18
#define moon_full_height 24
static unsigned char moon_full_bits[] = {
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03,
   0x1f, 0xe0, 0x03, 0x07, 0x80, 0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03,
   0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02,
   0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02,
   0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x07, 0x80, 0x03, 0x1f, 0xe0, 0x03,
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03};
#define moon_half_dec_width 18
#define moon_half_dec_height 24
static unsigned char moon_half_dec_bits[] = {
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03,
   0x1f, 0xfe, 0x03, 0x07, 0xfe, 0x03, 0x03, 0xfe, 0x03, 0x03, 0xfe, 0x03,
   0x01, 0xfe, 0x03, 0x01, 0xfe, 0x03, 0x01, 0xfe, 0x03, 0x01, 0xfe, 0x03,
   0x01, 0xfe, 0x03, 0x01, 0xfe, 0x03, 0x01, 0xfe, 0x03, 0x01, 0xfe, 0x03,
   0x03, 0xfe, 0x03, 0x03, 0xfe, 0x03, 0x07, 0xfe, 0x03, 0x1f, 0xfe, 0x03,
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03};
#define moon_half_inc_width 18
#define moon_half_inc_height 24
static unsigned char moon_half_inc_bits[] = {
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03,
   0xff, 0xe1, 0x03, 0xff, 0x81, 0x03, 0xff, 0x01, 0x03, 0xff, 0x01, 0x03,
   0xff, 0x01, 0x02, 0xff, 0x01, 0x02, 0xff, 0x01, 0x02, 0xff, 0x01, 0x02,
   0xff, 0x01, 0x02, 0xff, 0x01, 0x02, 0xff, 0x01, 0x02, 0xff, 0x01, 0x02,
   0xff, 0x01, 0x03, 0xff, 0x01, 0x03, 0xff, 0x81, 0x03, 0xff, 0xe1, 0x03,
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03};
#define moon_quarter_dec_width 18
#define moon_quarter_dec_height 24
static unsigned char moon_quarter_dec_bits[] = {
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03,
   0x9f, 0xff, 0x03, 0xc7, 0xff, 0x03, 0xe3, 0xff, 0x03, 0xf3, 0xff, 0x03,
   0xf1, 0xff, 0x03, 0xf1, 0xff, 0x03, 0xf1, 0xff, 0x03, 0xf1, 0xff, 0x03,
   0xf1, 0xff, 0x03, 0xf1, 0xff, 0x03, 0xf1, 0xff, 0x03, 0xf1, 0xff, 0x03,
   0xf3, 0xff, 0x03, 0xe3, 0xff, 0x03, 0xc7, 0xff, 0x03, 0x9f, 0xff, 0x03,
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03};
#define moon_quarter_inc_width 18
#define moon_quarter_inc_height 24
static unsigned char moon_quarter_inc_bits[] = {
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03,
   0xff, 0xe7, 0x03, 0xff, 0x8f, 0x03, 0xff, 0x1f, 0x03, 0xff, 0x3f, 0x03,
   0xff, 0x3f, 0x02, 0xff, 0x3f, 0x02, 0xff, 0x3f, 0x02, 0xff, 0x3f, 0x02,
   0xff, 0x3f, 0x02, 0xff, 0x3f, 0x02, 0xff, 0x3f, 0x02, 0xff, 0x3f, 0x02,
   0xff, 0x3f, 0x03, 0xff, 0x1f, 0x03, 0xff, 0x8f, 0x03, 0xff, 0xe7, 0x03,
   0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x03};


// *************************** END OF BITMAPS ******************************

int decodeBits( unsigned char byte2decode, int width2decode, int xPos, int yPos, uint16_t color )
{
    int shiftcount, shiftwidth;

    shiftwidth = width2decode >= 8 ? 8 : width2decode ;

    for( shiftcount = 0; shiftcount < shiftwidth; shiftcount++ )
    {
        if( (byte2decode >> shiftcount) & 0x01 )
        {
            tft.drawPixel(xPos + shiftcount, yPos, color);
        }
    }
    return( shiftwidth );
}



int decodeLine( int xPos, int yPos, int width, uint16_t color, unsigned char pBitmap[] )
{
    int bytes, pixel;

    bytes = ( width % 8 ) ? ( width / 8 ) + 1 : ( width / 8 );
//    Serial.print("Decoding ");
//    Serial.print(bytes);
//    Serial.print(" bytes:");

    for( int x = 0; x < bytes; x++ )
    {
//        Serial.print(" ");
//        Serial.print(pBitmap[x], HEX);
//        Serial.print(" -> use ");
//        Serial.print(width);
//        Serial.print(" bits, ");
        pixel = decodeBits( pBitmap[x], width, xPos, yPos, color );
        width -= pixel;
        xPos += pixel;
    }
//    Serial.println();

    return(3);

}

void drawXBM( int xPos, int yPos, int width, int height, uint16_t color, unsigned char pBitmap[] )
{
    int byteOffset;
    int currLine;

    if( (unsigned char*) pBitmap != NULL )
    {
        if( width > 0 && height > 0 )
        {
//            Serial.println();
            byteOffset = 0;
            for( currLine = 0; currLine < height; currLine++ )
            {
                byteOffset += decodeLine( xPos, yPos + currLine, 
                                           width, color, &pBitmap[byteOffset] );
            }
        }
    }
}










/* *********************************************************************
 * void httpHandler(void)
 * ---------------------------------
 *  arguments: -
 * ------------
 * purpose: telnet handling
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/
void httpHandler(void)
{

  // Variable to store the HTTP request
  static String header;

  WiFiClient client = httpServer.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
//  else
//  {
//    delay(500);
//  }

}

/* *********************************************************************
 * void telnetHandler(void)
 * ---------------------------------
 *  arguments: -
 * ------------
 * purpose: telnet handling
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/
void telnetHandler(void)
{

  if(telnetServer.hasClient()) 
  {
    if (!telnetClient || !telnetClient.connected()) 
    {
      if (telnetClient) 
      {
        telnetClient.stop();
        Serial.println("Telnet Client Stop");
      }
      telnetClient = telnetServer.available();
      Serial.println("New Telnet client");
      telnetClient.flush(); 
    }
  }

  while(telnetClient.available()) 
  {
    Serial.write(telnetClient.read());
  }
  
  if (telnetClient && telnetClient.connected()) 
  {
    telnetClient.print("Telnet Test, millis: ");
    telnetClient.println(millis());
    telnetClient.print("Free Heap RAM: ");
    telnetClient.println(ESP.getFreeHeap());
  }

}

/* *********************************************************************
 * void localSensorHandler(void)
 * ---------------------------------
 *  arguments: -
 * ------------
 * purpose: local sensor handling
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/
void localSensorHandler(void)
{
    float sensorValue;

    if( _envVars.valid_bme280 )
    {
        if( _envVars.local_bme_last_temperature_read == 0 ||
            millis() - _envVars.local_bme_last_temperature_read >= 
            _envVars.local_bme_temperature_interval )
        {
// Serial.println("local_bme_temperature_interval reached.");
            _envVars.local_bme_temperature.value_type = VALUE_TYPE_FLOAT;
            _envVars.local_bme_temperature.long_data = 0;
            sensorValue = bme.readTemperature();
            if( sensorValue != _envVars.local_bme_temperature.float_data )
            {
// Serial.println("temperature changed.");
                _envVars.last_local_bme_temperature = _envVars.local_bme_temperature;

                _envVars.local_bme_temperature.float_data = sensorValue;
                _envVars.local_bme_temperature.has_changed = true;
                _envVars.local_bme_temperature.is_valid = true;
            }
            else
            {
                _envVars.local_bme_temperature.has_changed = false;
                _envVars.local_bme_temperature.is_valid = false;
            }
            _envVars.local_bme_last_temperature_read = millis();
        }

        if( _envVars.local_bme_last_humidity_read  == 0 ||
            millis() - _envVars.local_bme_last_humidity_read >= 
            _envVars.local_bme_humidity_interval )
        {
// Serial.println("local_bme_humidity_interval reached.");
            _envVars.local_bme_humidity.value_type = VALUE_TYPE_FLOAT;
            _envVars.local_bme_humidity.long_data = 0;

            sensorValue = bme.readHumidity();

            if( sensorValue != _envVars.local_bme_humidity.float_data )
            {
// Serial.println("humidity changed.");
                _envVars.last_local_bme_humidity = _envVars.local_bme_humidity;

                _envVars.local_bme_humidity.float_data = sensorValue;
                _envVars.local_bme_humidity.is_valid = true;
                _envVars.local_bme_humidity.has_changed = true;
            }
            else
            {
                _envVars.local_bme_humidity.has_changed = false;
                _envVars.local_bme_humidity.is_valid = false;
            }
            _envVars.local_bme_last_humidity_read = millis();
        }

        if( _envVars.local_bme_last_pressure_read == 0 ||
            millis() - _envVars.local_bme_last_pressure_read >= 
            _envVars.local_bme_pressure_interval )
        {
// Serial.println("local_bme_pressure_interval reached.");
            _envVars.local_bme_pressure.value_type = VALUE_TYPE_FLOAT;
            _envVars.local_bme_pressure.long_data = 0;

            sensorValue = bme.readPressure();  // Pa, div by 100 for hPa

            if( sensorValue != _envVars.local_bme_pressure.float_data )
            {
// Serial.println("pressure changed.");
                _envVars.last_local_bme_pressure = _envVars.local_bme_pressure;

                _envVars.local_bme_pressure.float_data = sensorValue;
                _envVars.local_bme_pressure.is_valid = true;
                _envVars.local_bme_pressure.has_changed = true;
            }
            else
            {
                _envVars.local_bme_pressure.has_changed = false;
                _envVars.local_bme_pressure.is_valid = false;
            }
            _envVars.local_bme_last_pressure_read = millis();
        }
    }
}

#define degree_18x24_width 18
#define degree_18x24_height 24
static unsigned char degree_18x24_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0xf8, 0x03, 0x00,
   0x18, 0x03, 0x00, 0x0c, 0x06, 0x00, 0x0c, 0x06, 0x00, 0x0c, 0x06, 0x00,
   0x18, 0x03, 0x00, 0xf8, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define arrow_right_down_24x24_width 24
#define arrow_right_down_24x24_height 24
static unsigned char arrow_right_down_24x24_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x0f, 0x00, 0xfe, 0x0f, 0x00,
   0xfe, 0x0f, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00,
   0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0xce, 0x01,
   0x00, 0xce, 0x03, 0x00, 0x0e, 0x0f, 0x00, 0x0e, 0x1c, 0x00, 0xfe, 0x3f,
   0x00, 0xfe, 0x7f, 0x00, 0xfe, 0x3f, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x0f,
   0x00, 0xc0, 0x03, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define arrow_right_up_24x24_width 24
#define arrow_right_up_24x24_height 24
static unsigned char arrow_right_up_24x24_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0xc0, 0x03,
   0x00, 0x00, 0x0f, 0x00, 0x00, 0x1c, 0x00, 0xfe, 0x3f, 0x00, 0xfe, 0x7f,
   0x00, 0xfe, 0x3f, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x0f, 0x00, 0xce, 0x03,
   0x00, 0xce, 0x01, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00,
   0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0xfe, 0x0f, 0x00,
   0xfe, 0x0f, 0x00, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define arrow_right_24x24_width 24
#define arrow_right_24x24_height 24
static unsigned char arrow_right_24x24_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x78, 0x00,
   0x00, 0xf0, 0x00, 0x00, 0xe0, 0x01, 0x00, 0xc0, 0x03, 0x00, 0x80, 0x07,
   0x00, 0x00, 0x0f, 0x00, 0x00, 0x1e, 0xfe, 0xff, 0x3f, 0xfe, 0xff, 0x7f,
   0xfe, 0xff, 0x7f, 0xfe, 0xff, 0x3f, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x0f,
   0x00, 0x80, 0x07, 0x00, 0xc0, 0x03, 0x00, 0xe0, 0x01, 0x00, 0xf0, 0x00,
   0x00, 0x78, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



/* *********************************************************************
 * void sensorDataHandler(void)
 * ---------------------------------
 *  arguments: -
 * ------------
 * purpose: display / log sensor data
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/
void sensorDataHandler(void)
{

    if( _envVars.valid_FLASH )
    {
        unsigned char dataType;
        unsigned char sensorId = BME_LOCAL_SENSOR_ID;
        float sensorValue;

        if( _envVars.local_bme_temperature.is_valid )
        {
            if( _envVars.local_bme_temperature.has_changed )
            {
                sensorValue = _envVars.local_bme_temperature.float_data;
                dataType = 'T';
                writeData( FLASH_DATA_FILE, sensorValue, dataType, sensorId );
            }
        }

        if( _envVars.local_bme_humidity.is_valid )
        {
            if( _envVars.local_bme_humidity.has_changed )
            {
                sensorValue = _envVars.local_bme_humidity.float_data;
                dataType = 'H';
                writeData( FLASH_DATA_FILE, sensorValue, dataType, sensorId );
            }
        }

        if( _envVars.local_bme_pressure.is_valid )
        {
            if( _envVars.local_bme_pressure.has_changed )
            {
                sensorValue = _envVars.local_bme_pressure.float_data;
                dataType = 'P';
                writeData( FLASH_DATA_FILE, sensorValue, dataType, sensorId );
            }
        }
    }

    if( _envVars.valid_display )
    {
        int16_t  x1, y1;
        uint16_t w, h;
        char sBuffer[30];
        char floatString[10];

        tft.setTextWrap(false);
        tft.setTextSize(3);
        tft.setTextColor(YELLOW);

#define V_OFFSET     12
#define LINE_1       0+V_OFFSET
#define LINE_2       24+2+V_OFFSET
#define LINE_3       48+4+V_OFFSET
#define LINE_4       72+6+V_OFFSET
#define LINE_5       96+6+V_OFFSET
#define LINE_6       120+8+V_OFFSET
#define LINE_7       144+10+V_OFFSET
#define LINE_8       168+12+V_OFFSET
#define LINE_9       192+12+V_OFFSET
#define LINE_10      216+12+V_OFFSET

// 11*18=198

        if( _envVars.local_bme_humidity.is_valid )
        {
            if( _envVars.local_bme_humidity.has_changed )
            {
                int xPos = 36;
                int iconXPos = 198;

                dtostrf(_envVars.local_bme_humidity.float_data,3,2,floatString);
                sprintf(sBuffer, "%s %%", floatString);

                tft.getTextBounds(sBuffer, xPos, LINE_5, &x1, &y1, &w, &h);
                tft.fillRect(x1, y1, w, h, BLACK);
                tft.setCursor(xPos, LINE_5);
                tft.print(sBuffer);
                drawXBM( iconXPos, LINE_5, arrow_right_down_24x24_width, 
                         arrow_right_down_24x24_height, YELLOW, 
                         arrow_right_down_24x24_bits );

//                tft.print("Hum  = ");
//                tft.print(_envVars.local_bme_humidity.float_data);
//                tft.println("%");
            }
        }

        if( _envVars.local_bme_temperature.is_valid )
        {
            if( _envVars.local_bme_temperature.has_changed )
            {
                int xPos = 36;
                int iconXPos = 198;

                dtostrf( _envVars.local_bme_temperature.float_data,
                       3,2,floatString);

                sprintf(sBuffer, "%s  C", floatString );

                tft.getTextBounds(sBuffer, xPos, LINE_6, &x1, &y1, &w, &h);
                tft.fillRect(x1, y1, w, h, BLACK);
                tft.setCursor(xPos, LINE_6);
                tft.print(sBuffer);
                
                drawXBM( 144, LINE_6, degree_18x24_width, 
                         degree_18x24_height, YELLOW, 
                         degree_18x24_bits );

                drawXBM( iconXPos, LINE_6, arrow_right_up_24x24_width, 
                         arrow_right_up_24x24_height, YELLOW, 
                         arrow_right_up_24x24_bits );
//                tft.print("Temp = ");
//                tft.print(_envVars.local_bme_temperature.float_data);
//                tft.println(" *C");
            }
        }

        if( _envVars.local_bme_pressure.is_valid )
        {
            if( _envVars.local_bme_pressure.has_changed )
            {
                int xPos = 0;
                int iconXPos = 198;

                dtostrf( _envVars.local_bme_pressure.float_data / 100.0F,
                         4,2,floatString);

                sprintf(sBuffer, "%s hPa", floatString );
                
                if( strlen(sBuffer) < 11 )
                {
                    xPos += 18;
                }

                tft.getTextBounds(sBuffer, xPos, LINE_7, &x1, &y1, &w, &h);
                tft.fillRect(x1, y1, w, h, BLACK);
                tft.setCursor(xPos, LINE_7);
                tft.print(sBuffer);

                drawXBM( iconXPos, LINE_7, arrow_right_24x24_width, 
                         arrow_right_24x24_height, YELLOW, 
                         arrow_right_24x24_bits );
//                tft.print("Pres = ");
//                tft.print(_envVars.local_bme_pressure.float_data / 100.0F);
//                tft.println(" hPa");
            }
        }
// "Montag"
// "Dienstag"
// "Mittwoch"
// "Donnerstag"
// "Freitag"
// "Samstag"
// "Sonntag"


        sprintf(sBuffer, "Donnerstag"); // 10 Zeichen = 180 Pix
        tft.setCursor(30, LINE_1);  // 1 1/2 Zeichen nach rechts
        tft.print(sBuffer);

        sprintf(sBuffer, "20.10.2020"); // 10 Zeichen = 180 Pix
        tft.setCursor(30, LINE_2);  // 1 1/2 Zeichen nach rechts
        tft.print(sBuffer);

        sprintf(sBuffer, "20:22.20"); // 8 Zeichen = 144 Pix
        tft.setCursor(48, LINE_3);
        tft.print(sBuffer);

        sprintf(sBuffer, "%s", WiFi.localIP().toString().c_str());
        
        tft.setTextSize(2);
        tft.setCursor( ((240 - (strlen(sBuffer)*12))/2), LINE_9);
        tft.print(sBuffer);
        tft.setTextSize(3);

        tft.drawLine( 10, ( LINE_4 + 12 ) , 220, ( LINE_4 + 12 ), WHITE);
        tft.drawLine( 10, ( LINE_8 + 12 ) , 220, ( LINE_8 + 12 ), WHITE);


//        tft.print("Approx. Altitude = ");
//        tft.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//        tft.println(" m");
    }

#ifdef LOG_TO_SERIAL

    if( _envVars.valid_serial )
    {
        if( _envVars.local_bme_humidity.is_valid )
        {
            if( _envVars.local_bme_humidity.has_changed )
            {
                Serial.print("Last humidity = ");
                Serial.print(_envVars.last_local_bme_humidity.float_data);
                Serial.print(", new humidity = ");
                Serial.println(_envVars.local_bme_humidity.float_data);

                Serial.print("Humidity = ");
                Serial.print(_envVars.local_bme_humidity.float_data);
                Serial.println(" %");
            }
        }

        if( _envVars.local_bme_temperature.is_valid )
        {
            if( _envVars.local_bme_temperature.has_changed )
            {
                Serial.print("Last temperature = ");
                Serial.print(_envVars.last_local_bme_temperature.float_data);
                Serial.print(", new temperature = ");
                Serial.println(_envVars.local_bme_temperature.float_data);

                Serial.print("Temperature = ");
                Serial.print(_envVars.local_bme_temperature.float_data);
                Serial.println(" *C");
            }
        }

        if( _envVars.local_bme_pressure.is_valid )
        {
            if( _envVars.local_bme_pressure.has_changed )
            {
                Serial.print("Last pressure = ");
                Serial.print(_envVars.last_local_bme_pressure.float_data / 100.0F);
                Serial.print(", new pressure = ");
                Serial.println(_envVars.local_bme_pressure.float_data / 100.0F);

                Serial.print("Pressure = ");
                Serial.print(_envVars.local_bme_pressure.float_data / 100.0F);
                Serial.println(" hPa");
            }
        }


//        Serial.print("Approx. Altitude = ");
//        Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//        Serial.println(" m");

//      Serial.println();

//      Serial.print("hour:minute.second = ");
//      Serial.print(myTZ.hour());
//      Serial.print(":");
//      Serial.print(myTZ.minute());
//      Serial.print(".");
//      Serial.println(myTZ.second());
//
//      Serial.print("weekday = ");
//      Serial.println(myTZ.weekday());
//
//      Serial.print("day.month.year = ");
//      Serial.print(myTZ.day());
//      Serial.print(".");
//      Serial.print(myTZ.month());
//      Serial.print(".");
//      Serial.println(myTZ.year());
    }

#endif // LOG_TO_SERIAL


//  if( _envVars.valid_display )
//  {
//      drawXBM( 0, 0, arrow_down_width, arrow_down_height, WHITE, 
//               arrow_down_bits );
//      drawXBM( 20, 0, arrow_right_down_width, arrow_right_down_height, WHITE,
//                arrow_right_down_bits );
//      drawXBM( 40, 0, arrow_right_up_width, arrow_right_up_height, WHITE,
//                arrow_right_up_bits );
//      drawXBM( 60, 0, arrow_right_width, arrow_right_height, WHITE,
//                arrow_right_bits );
//      drawXBM( 80, 0, arrow_up_width, arrow_up_height, WHITE, arrow_up_bits );
//      drawXBM( 100, 0, moon_full_width, moon_full_height, WHITE,
//                moon_full_bits );
//      drawXBM( 120, 0, moon_half_dec_width, moon_half_dec_height, WHITE,
//                moon_half_dec_bits );
//      drawXBM( 140, 0, moon_half_inc_width, moon_half_inc_height, WHITE,
//                moon_half_inc_bits );
//      drawXBM( 160, 0, moon_quarter_dec_width, moon_quarter_dec_height, WHITE,
//                moon_quarter_dec_bits );
//      drawXBM( 180, 0, moon_quarter_inc_width, moon_quarter_inc_height,
//                WHITE, moon_quarter_inc_bits );
//
//  }

// int16_t  x1, y1;
// uint16_t w, h;

// tft.getTextBounds(string, x, y, &x1, &y1, &w, &h);
// void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
//
//    // In global declarations:
//    GFXcanvas1 canvas(128, 32); // 128x32 pixel canvas
//     
//    // In code later:
//    canvas.println("I like cake");
//    tft.drawBitmap(x, y, canvas, 128, 32, foreground, background); // Copy to screen
//

    _envVars.local_bme_temperature.is_valid = false;
    _envVars.local_bme_temperature.has_changed = false;

    _envVars.local_bme_humidity.is_valid = false;
    _envVars.local_bme_humidity.has_changed = false;

    _envVars.local_bme_pressure.is_valid = false;
    _envVars.local_bme_pressure.has_changed = false;

}



//
// Einstellungen:
// Format der Ausgabe (ein, zwei, keine Nachkomma-Stellen)
// abhaengig davon Vergleich (auf bzw. abrunden, ohne Nachkomma vergleichen)
//
// Farbe der Ausgabe (alle weiss oder rot, gelb, grün und Grenzwerte dazu)
// Beschriftung der Felder
//
// Trend-Anzeige
// Mondphasen
// IP Adresse
//
// Lese- unf Anzeige-Intervalle
//
// Format und Farbe des Datums und der Uhrzeit
//

// | Seite 1:
// | ========
// | Hintergrund-Farbe
// | Schrift-Farbe
// | Wochentag
// | Datum
// | Uhrzeit
// |   -- Stunden:Minuten.Sekunden
// |     -- Update Intervall Sekunde
// |   -- Stunden:Minuten
// |     -- Update Intervall Minute
// | 
// | Luftfeuchtigkeit
// |   -- 2 Nachkommastellen
// |   -- 1 Nachkommastelle
// |   -- 0 Nachkommastellen
// |   -- Update Intervall
// |   -- Trend-Anzeige
// |   -- Schrift-Farbe
// |      -- Mono
// |      -- farbige Anzeige
// |         -- unterer Grenzwert
// |         -- oberer Grenzwert
// | Temperatur
// |   -- 2 Nachkommastellen
// |   -- 1 Nachkommastelle
// |   -- 0 Nachkommastellen
// |   -- Update Intervall
// |   -- Trend-Anzeige
// |   -- Schrift-Farbe
// |      -- Mono
// |      -- farbige Anzeige
// |         -- unterer Grenzwert
// |         -- oberer Grenzwert
// | Luftdruck
// |   -- 2 Nachkommastellen
// |   -- 1 Nachkommastelle
// |   -- 0 Nachkommastellen
// |   -- Update Intervall
// |   -- Trend-Anzeige
// |   -- Schrift-Farbe
// |      -- Mono
// |      -- farbige Anzeige
// |         -- unterer Grenzwert
// |         -- oberer Grenzwert
// | IP-Adresse
// | 
// | Seite 2:
// | ========
// | Hintergrund-Farbe
// | Schrift-Farbe
// |   -- Wochentag
// |   -- Datum
// |   -- Uhrzeit
// |      -- Stunden:Minuten.Sekunden
// |        -- Update Intervall Sekunde
// |      -- Stunden:Minuten
// |        -- Update Intervall Minute
// | Mondphase
// | Alternative Sensoren
// | Akku Kapazitaet
// | Restlaufzeit
// | 
// | 
// | Seite 3:
// | ========
// | SSID
// | PASSPHRASE
// | Feste IP
// | Admin Passwort
// | 
// | 
// ************************************************************


enum { key_left, key_right, key_accept, key_invalid, key_no_key };

enum { action_next_page, action_prev_page, action_prev_char, action_del_char, action_next_char, action_accept, action_cancel, action_no_action }; 

#define TFT_CHAR_WIDTH                12
#define TFT_CHAR_HEIGHT               16

#define INPUT_DATA_MAX_LEN            60
#define DISPLAY_ITEM_MAX_LEN          20



int whichKeyPressed(void)
{

    int retVal = key_no_key;
    int incomingByte = 0; // for incoming serial data

    if (Serial.available() > 0) 
    {
        // read the incoming byte:

        switch( (char) (incomingByte = Serial.read()) )
        {
            case 'l':
            case 'L':
                retVal = key_left;
                Serial.print("Got -> ");
                Serial.print( (char) incomingByte);
                Serial.println("means key_left.");
                break;
            case 'r':
            case 'R':
                retVal = key_right;
                Serial.print("Got -> ");
                Serial.print( (char) incomingByte);
                Serial.println("means key_right.");
                break;
            case 'a':
            case 'A':
                retVal = key_accept;
                Serial.print("Got -> ");
                Serial.print( (char) incomingByte);
                Serial.println("means key_accept.");
                break;
            default:
                retVal = key_invalid;
                Serial.print("Got -> ");
                Serial.print( (char) incomingByte);
                Serial.println("means key_invalid.");
                break;
        }

        while( Serial.available() )
        {
            Serial.read();
        }
    }

    return(retVal);
}


#define CHAR_PAGE_1_REGULAR_ITEMS          52
#define CHAR_PAGE_2_REGULAR_ITEMS          42

#define MINIMUM_REGULAR_ITEMS     CHAR_PAGE_2_REGULAR_ITEMS

#define CHAR_PAGE_1_COMMAND_ITEMS           6
#define CHAR_PAGE_2_COMMAND_ITEMS           6

#define CHAR_PAGE_1_MAX_ITEMS           (CHAR_PAGE_1_REGULAR_ITEMS + \
                                         CHAR_PAGE_1_COMMAND_ITEMS)
#define CHAR_PAGE_2_MAX_ITEMS           (CHAR_PAGE_2_REGULAR_ITEMS + \
                                         CHAR_PAGE_2_COMMAND_ITEMS)

#define INPUT_ACCEPTED      '0'
#define INPUT_FAILED        'F'


char *pCharPage1[] = {
    "",
    "",
    "",
    "",
    "--------------------",
    "a b c d e f g h i j",
    "k l m n o p q r s t",
    "u v w x y z A B C D",
    "E F G H I J K L M N",
    "O P Q R S T U V W X",
    "Y Z",
    "--------------------",
    "       [NEXT]",
    "   [<] [DEL] [>]",
    "   [OK] [CANCEL]",
    NULL
};
//
char *pCharPage2[] = {
    "",
    "",
    "",
    "",
    "",
    "--------------------",
    "0 1 2 3 4 5 6 7 8 9",
    "! \" § $ % & / ( ) =",
    "? ; : _ , . - # \' +",
    "* ^ ~ @ \\ } ] [ { <",
    "> |",
    "--------------------",
    "       [PREV]",
    "   [<] [DEL] [>]",
    "   [OK] [CANCEL]",
    NULL
};


void startCharPage1( char *pValueName, uint16_t bgColor, uint16_t fgColor )
{
    if( _envVars.valid_display )
    {
        if( pValueName != NULL )
        {
            tft.fillScreen(bgColor);
            tft.setTextColor(fgColor);
            tft.setTextWrap(false);
            tft.setTextSize(2);

            tft.setCursor(0,  0*TFT_CHAR_HEIGHT);
            tft.print(pValueName);

            for( int i = 1; pCharPage1[i] != NULL; i++ )
            {
                tft.setCursor(0,  i * TFT_CHAR_HEIGHT);
                tft.print(pCharPage1[i]);
            }
        }
    }
}



void startCharPage2( char *pValueName, uint16_t bgColor, uint16_t fgColor )
{
    if( _envVars.valid_display )
    {
        if( pValueName != NULL )
        {
            tft.fillScreen(bgColor);
            tft.setTextColor(fgColor);
            tft.setTextWrap(false);
            tft.setTextSize(2);

            tft.setCursor(0,  0*TFT_CHAR_HEIGHT);
            tft.print(pValueName);

            for( int i = 1; pCharPage2[i] != NULL; i++ )
            {
                tft.setCursor(0,  i * TFT_CHAR_HEIGHT);
                tft.print(pCharPage2[i]);
            }
        }
    }
}


/* *********************************************************************
 * void selectStringParam( char* pSetupWhat, char *pResult, int maxLen )
 * ---------------------------------
 *  arguments: char* pSetupWhat - title to display
 *             char* pResult    - pointer to store result
 *             int maxLen       - max. amount of chars
 * ------------
 * purpose: enter a text by choosing each single characters
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/
void selectStringParam( char* pSetupWhat, char *pResult, int maxLen )
{
    int keyPressed;
    int doAction;

    int currXPos, currYPos;
    int prevXPos, prevYPos;
    int currItem, prevItem;
    int currItemColumn, currItemRow;
    int itemLen;
    char pCurrItem[DISPLAY_ITEM_MAX_LEN];
    char pPrevItem[DISPLAY_ITEM_MAX_LEN];
    int startRow;
    int prevColumn, prevRow;
    int currInputPage, prevInputPage;
    uint16_t bgColor, fgColor;
    bool inputDone;
    char inputResult;
    int maxItem;
    bool isCurrAction;
    int currAction;
    int inputIndex;
    char pInputData[INPUT_DATA_MAX_LEN+1];

    if( pSetupWhat != NULL && pResult != NULL && maxLen > 0 )
    {

        bgColor = BLACK;
        fgColor = WHITE;

        currXPos = 0;
        currYPos = 0;
        currItem = 0;
        prevItem = 0;

        prevXPos = -1;
        prevYPos = -1;
        prevInputPage = -1;
        currInputPage = 1;

        itemLen = 0;
        memset( pCurrItem, '\0', sizeof( pCurrItem ) );
        memset( pPrevItem, '\0', sizeof( pPrevItem ) );

        startRow = 0;
        prevColumn = 0;
        prevRow = 0;

        if( _envVars.valid_display )
        {
            tft.fillScreen(bgColor);
            tft.setTextColor(fgColor);
            tft.setTextWrap(false);
            tft.setTextSize(2);

            inputDone = false;
            inputResult = INPUT_ACCEPTED;

            itemLen = 0;
            memset( pCurrItem, '\0', sizeof( pCurrItem ) );
            memset( pPrevItem, '\0', sizeof( pPrevItem ) );

            inputIndex = 0;
            memset( pInputData, '\0', sizeof( pInputData ) );
    
            startCharPage1( pSetupWhat, bgColor, fgColor );

            do 
            {
                isCurrAction = false;
                currAction = action_no_action;

                if( currInputPage == 1 )
                {
                    maxItem = CHAR_PAGE_1_MAX_ITEMS;
                }
                else
                {
                    if( currInputPage == 2 )
                    {
                        maxItem = CHAR_PAGE_2_MAX_ITEMS;
                    }
                }
  
                switch( keyPressed = whichKeyPressed() )
                {
                    case key_left:
Serial.print("key_left item = ");
                        if( currItem > 0 )
                        {
                            prevItem = currItem;
                            currItem--;
                        }
                        else
                        {
                            // wrap
                            prevItem = currItem;
                            currItem = maxItem - 1;
                        }
Serial.println(currItem);
                        break;
                    case key_right:
Serial.print("key_right item = ");
                        if( currItem < (maxItem - 1) )
                        {
                            prevItem = currItem;
                            currItem++;
                        }
                        else
                        {
                            // wrap
                            prevItem = currItem;
                            currItem = 0;
                        }
Serial.println(currItem);
                        break;
                    case key_accept:
Serial.println("key_accept");
                        isCurrAction = true;
                        inputResult = INPUT_ACCEPTED;
                        break;
                    case key_no_key:
//Serial.println("NO key");
                        break;
                    default:
Serial.println("Unknown key");
                        inputDone = true;
                        inputResult = INPUT_FAILED;
                        break;
                }

                if( keyPressed != key_no_key )
                {
                    if( !inputDone )
                    {
                        if( currInputPage == 1 )
                        {
                            startRow = 5;
                        }
        
                        if( currInputPage == 2 )
                        {
                            startRow = 6;
                        }
        
        
                        if( currItem < MINIMUM_REGULAR_ITEMS )
                        {
                            itemLen = 1;
                            currItemColumn = (currItem % 10) * 2;
                            currItemRow = startRow + (currItem / 10);
                        }
                        else
                        {
                            if( currInputPage == 1 )
                            {
                                if( currItem < CHAR_PAGE_1_REGULAR_ITEMS )
                                {
                                    itemLen = 1;
                                    currItemColumn = (currItem % 10) * 2;
                                    currItemRow = startRow + (currItem / 10);
                                }
                                else
                                {
                                    switch( currItem )
                                    {
                                        case CHAR_PAGE_1_REGULAR_ITEMS:
                                            currItemRow = 12;
                                            currItemColumn = 8;
                                            itemLen = 4;
                                            isCurrAction = true;
                                            currAction = action_next_page;
                                            break;
                                        case CHAR_PAGE_1_REGULAR_ITEMS+1:
                                            currItemRow = 13;
                                            currItemColumn = 4;
                                            itemLen = 1;
                                            isCurrAction = true;
                                            currAction = action_prev_char;
                                            break;
                                        case CHAR_PAGE_1_REGULAR_ITEMS+2:
                                            currItemRow = 13;
                                            currItemColumn = 8;
                                            itemLen = 3;
                                            isCurrAction = true;
                                            currAction = action_del_char;
                                            break;
                                        case CHAR_PAGE_1_REGULAR_ITEMS+3:
                                            currItemRow = 13;
                                            currItemColumn = 14;
                                            itemLen = 1;
                                            isCurrAction = true;
                                            currAction = action_next_char;
                                            break;
                                        case CHAR_PAGE_1_REGULAR_ITEMS+4:
                                            currItemRow = 14;
                                            currItemColumn = 4; 
                                            itemLen = 2;
                                            isCurrAction = true;
                                            currAction = action_accept;
                                            break;
                                        case CHAR_PAGE_1_REGULAR_ITEMS+5:
                                            currItemRow = 14;
                                            currItemColumn = 9;
                                            itemLen = 6;
                                            isCurrAction = true;
                                            currAction = action_cancel;
                                            break;
                                        default:
Serial.print("Current item = ");
Serial.println(currItem);

                                            inputDone = true;
                                            inputResult = INPUT_FAILED;
                                            itemLen = 0;
                                            break;
                                    }
                                }
                            }
                            else
                            {
                                if( currItem < CHAR_PAGE_2_REGULAR_ITEMS )
                                {
                                    itemLen = 1;
                                    currItemColumn = (currItem % 10) * 2;
                                    currItemRow = startRow + (currItem / 10);
                                }
                                else
                                {
                                    if( currInputPage == 2 )
                                    {
                                        switch( currItem )
                                        {
                                            case CHAR_PAGE_2_REGULAR_ITEMS:
                                                currItemRow = 12;
                                                currItemColumn = 8;
                                                itemLen = 4;
                                                isCurrAction = true;
                                                currAction = action_prev_page;
                                                break;
                                            case CHAR_PAGE_2_REGULAR_ITEMS+1:
                                                currItemRow = 13;
                                                currItemColumn = 4;
                                                itemLen = 1;
                                                isCurrAction = true;
                                                currAction = action_prev_char;
                                                break;
                                            case CHAR_PAGE_2_REGULAR_ITEMS+2:
                                                currItemRow = 13;
                                                currItemColumn = 8;
                                                itemLen = 3;
                                                isCurrAction = true;
                                                currAction = action_del_char;
                                                break;
                                            case CHAR_PAGE_2_REGULAR_ITEMS+3:
                                                currItemRow = 13;
                                                currItemColumn = 14;
                                                itemLen = 1;
                                                isCurrAction = true;
                                                currAction = action_next_char;
                                                break;
                                            case CHAR_PAGE_2_REGULAR_ITEMS+4:
                                                currItemRow = 14;
                                                currItemColumn = 4; 
                                                itemLen = 2;
                                                isCurrAction = true;
                                                currAction = action_accept;
                                                break;
                                            case CHAR_PAGE_2_REGULAR_ITEMS+5:
                                                currItemRow = 14;
                                                currItemColumn = 9;
                                                itemLen = 6;
                                                isCurrAction = true;
                                                currAction = action_cancel;
                                                break;
                                            default:
Serial.print("Current item = ");
Serial.println(currItem);
                                                inputDone = true;
                                                inputResult = INPUT_FAILED;
                                                itemLen = 0;
                                                break;
                                        }
                                    }
                                    else
                                    {
                                        inputDone = true;
                                        inputResult = INPUT_FAILED;
                                    }
                                }
                            }
                        }
    
                        if( isCurrAction )
                        {
                            Serial.print("ACTION! currAction = ");
    
                            switch( currAction )
                            {
                                case action_no_action:
                                    Serial.println("NOP");
                                    break;
                                case action_next_page:
                                    if( keyPressed == key_accept )
                                    {
                                        currItem = CHAR_PAGE_2_REGULAR_ITEMS;
                                        currInputPage = 2;
                                        startCharPage2( pSetupWhat, bgColor, 
                                                     fgColor );
                                        Serial.println("NEXT PAGE");
                                    }
//                                    else
//                                    {
//                                        Serial.print(" -> NOT PERFORMED <-");
//                                        Serial.println("NEXT PAGE");
//                                    }
                                    break;
                                case action_prev_page:
                                    if( keyPressed == key_accept )
                                    {
                                        currItem = CHAR_PAGE_1_REGULAR_ITEMS;
                                        currInputPage = 1;
                                        startCharPage1( pSetupWhat, bgColor, 
                                                     fgColor );
                                        Serial.println("PAGE BACK");
                                    }
//                                    else
//                                    {
//                                        Serial.print(" -> NOT PERFORMED <-");
//                                        Serial.println("NEXT PAGE");
//                                    }
                                    break;
                                case action_prev_char:
                                    Serial.println("PREV");
                                    break;
                                case action_del_char:
                                    Serial.println("DEL");
                                    break;
                                case action_next_char:
                                    Serial.println("NEXT");
                                    break;
                                case action_accept:
                                    if( keyPressed == key_accept )
                                    {
                                        inputDone = true;
                                        inputResult = INPUT_ACCEPTED;
                                        Serial.println("OK");
                                    }
//                                    else
//                                    {
//                                        Serial.print(" -> NOT PERFORMED <-");
//                                        Serial.println("OK");
//                                    }
                                    break;
                                case action_cancel:
                                    if( keyPressed == key_accept )
                                    {
                                        inputDone = true;
                                        inputResult = INPUT_ACCEPTED;
                                        Serial.println("CANCEL");
                                    }
//                                    else
//                                    {
//                                        Serial.print(" -> NOT PERFORMED <-");
//                                        Serial.println("CANCEL");
//                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
    
                        if( !inputDone )
                        {
    
                            prevXPos = currXPos;
                            prevYPos = currYPos;
        
                            prevColumn = currItemColumn;
                            prevRow  = currItemRow;
        
    
                            currXPos = currItemColumn * TFT_CHAR_WIDTH;
                            currYPos = currItemRow * TFT_CHAR_HEIGHT;
    
                            strcpy( pPrevItem, pCurrItem );
                            memset( pCurrItem, '\0', sizeof( pCurrItem ) );
        
                            if( currInputPage == 1 )
                            {
                                strncpy( pCurrItem, 
                                         &pCharPage1[currItemRow][currItemColumn],
                                         itemLen );
                            }
                            else
                            {
                                strncpy( pCurrItem, 
                                         &pCharPage2[currItemRow][currItemColumn],
                                         itemLen );
                            }
        
                            tft.setCursor( prevXPos, prevYPos );
                            tft.setTextColor(fgColor, bgColor);
                            tft.print( pPrevItem );
    
                            tft.setCursor( currXPos, currYPos );
                            tft.setTextColor(bgColor, fgColor);
                            tft.print( pCurrItem );

                            if( inputResult == INPUT_ACCEPTED )
                            {
                                if( keyPressed == key_accept )
                                {
                                    if( currInputPage == 1 )
                                    {
                                        pInputData[inputIndex] =
                                            pCharPage1[currItemRow][currItemColumn];

                                        if( inputIndex + 1 < sizeof(pInputData))
                                        {
                                            inputIndex++;
                                        }
                                    }
                                    else
                                    {
                                        pInputData[inputIndex] =
                                            pCharPage2[currItemRow][currItemColumn];

                                        if( inputIndex + 1 < sizeof(pInputData))
                                        {
                                            inputIndex++;
                                        }
                                    }
                                    Serial.print("Input is now: ");
                                    Serial.println(pInputData);
                                }
                            }
                        }
                    }
                }
    
            } while( !inputDone );
    
            if( inputResult == INPUT_ACCEPTED )
            {
            }
            else
            {
                if( inputResult == INPUT_FAILED )
                {
                }
            }
        }
    }
    else
    {
        if( pSetupWhat == NULL )
        {
            Serial.println("pSetupWhat must NOT be NULL");
        }

        if( pResult == NULL )
        {
            Serial.println("pResult must NOT be NULL");
        }

        if( maxLen <= 0 )
        {
            Serial.print("maxlen is ");
            Serial.println(maxLen);
        }
    }
}



char *pColorSettings[] = {
    "--- Color Setup ----",
    "Schrift",
    "",
    "",
    "",
    "Hintergrund",
    "",
    "",
    "",
    "Vorschau",
    "",
    "",
    "",
    "--------------------",
    "   [OK] [CANCEL]",
    NULL
};

#define XPOS_FG_BLACK_COLOR_SETTINGS          2
#define YPOS_FG_BLACK_COLOR_SETTINGS          2

#define XPOS_FG_BLUE_COLOR_SETTINGS           6
#define YPOS_FG_BLUE_COLOR_SETTINGS           2

#define XPOS_FG_RED_COLOR_SETTINGS           10
#define YPOS_FG_RED_COLOR_SETTINGS            2

#define XPOS_FG_GREEN_COLOR_SETTINGS         14
#define YPOS_FG_GREEN_COLOR_SETTINGS          2

#define XPOS_FG_CYAN_COLOR_SETTINGS           2
#define YPOS_FG_CYAN_COLOR_SETTINGS           3

#define XPOS_FG_MAGENTA_COLOR_SETTINGS        6
#define YPOS_FG_MAGENTA_COLOR_SETTINGS        3

#define XPOS_FG_YELLOW_COLOR_SETTINGS        10
#define YPOS_FG_YELLOW_COLOR_SETTINGS         3

#define XPOS_FG_WHITE_COLOR_SETTINGS         14
#define YPOS_FG_WHITE_COLOR_SETTINGS          3

#define XPOS_BG_BLACK_COLOR_SETTINGS          2
#define YPOS_BG_BLACK_COLOR_SETTINGS          6

#define XPOS_BG_BLUE_COLOR_SETTINGS           6
#define YPOS_BG_BLUE_COLOR_SETTINGS           6

#define XPOS_BG_RED_COLOR_SETTINGS           10
#define YPOS_BG_RED_COLOR_SETTINGS            6

#define XPOS_BG_GREEN_COLOR_SETTINGS         14
#define YPOS_BG_GREEN_COLOR_SETTINGS          6

#define XPOS_BG_CYAN_COLOR_SETTINGS           2
#define YPOS_BG_CYAN_COLOR_SETTINGS           7

#define XPOS_BG_MAGENTA_COLOR_SETTINGS        6
#define YPOS_BG_MAGENTA_COLOR_SETTINGS        7

#define XPOS_BG_YELLOW_COLOR_SETTINGS        10
#define YPOS_BG_YELLOW_COLOR_SETTINGS         7

#define XPOS_BG_WHITE_COLOR_SETTINGS         14
#define YPOS_BG_WHITE_COLOR_SETTINGS          7

#define NUM_FG_COLORS_IN_SETTINGS             8
#define NUM_BG_COLORS_IN_SETTINGS             8
#define NUM_COLORS_IN_COLOR_SETTINGS      NUM_FG_COLORS_IN_SETTINGS + \
                                          NUM_BG_COLORS_IN_SETTINGS

#define XPOS_BG_PREVIEW_COLOR_SETTINGS        2
#define YPOS_BG_PREVIEW_COLOR_SETTINGS       10
#define WIDTH_BG_PREVIEW_COLOR_SETTINGS      16
#define HEIGHT_BG_PREVIEW_COLOR_SETTINGS      3

#define XPOS_TEXT_PREVIEW_COLOR_SETTINGS      5
#define YPOS_TEXT_PREVIEW_COLOR_SETTINGS     11

#define YPOS_ITEM_ROW_0_COLOR_SETTINGS        2
#define YPOS_ITEM_ROW_1_COLOR_SETTINGS        3
#define YPOS_ITEM_ROW_2_COLOR_SETTINGS        6
#define YPOS_ITEM_ROW_3_COLOR_SETTINGS        7

#define XPOS_ITEM_COLUMN_0_COLOR_SETTINGS     2
#define XPOS_ITEM_COLUMN_1_COLOR_SETTINGS     6
#define XPOS_ITEM_COLUMN_2_COLOR_SETTINGS    10
#define XPOS_ITEM_COLUMN_3_COLOR_SETTINGS    14

#define XPOS_OK_COMMAND_COLOR_SETTINGS        4
#define YPOS_OK_COMMAND_COLOR_SETTINGS       14
#define XPOS_CANCEL_COMMAND_COLOR_SETTINGS    9
#define YPOS_CANCEL_COMMAND_COLOR_SETTINGS   14

#define BOX_WIDTH_COLOR_SETTINGS              3
#define BOX_HEIGHT_COLOR_SETTINGS             1

#define MAX_ITEMS_COLOR_SETTINGS          NUM_FG_COLORS_IN_SETTINGS + \
                                          NUM_BG_COLORS_IN_SETTINGS + 2

#define ITEM_OK_COMMAND_COLOR_SETTINGS        NUM_COLORS_IN_COLOR_SETTINGS
#define ITEM_CANCEL_COMMAND_COLOR_SETTINGS    NUM_COLORS_IN_COLOR_SETTINGS + 1

#define MAX_LEN_COLOR_SETTINGS_COMMAND       10

#define INPUT_RESULT_OK                       7
#define INPUT_RESULT_CANCEL                   8
#define INPUT_RESULT_NO_RESULT                0


void colorSelection( uint16_t bgColor, uint16_t fgColor,
                     uint16_t *pNewBGColor, uint16_t *pNewFGColor )
{
    if( _envVars.valid_display )
    {
        if( pNewBGColor != NULL && pNewFGColor != NULL )
        {
            int keyPressed;
            int maxItem = MAX_ITEMS_COLOR_SETTINGS;
            int currItem, prevItem;
            int currXPos, currYPos;
            int prevXPos, prevYPos;
            int currItemColumn, currItemRow;
            int prevItemColumn, prevItemRow;
            uint16_t prevItemColor, currItemColor;
            uint16_t selectedFGColor, selectedBGColor;

            char pCurrCommand[MAX_LEN_COLOR_SETTINGS_COMMAND];
            char pPrevCommand[MAX_LEN_COLOR_SETTINGS_COMMAND];
            int currCommandLen;
            bool isCommand;
            int inputResult;
            bool inputDone;
           
            tft.fillScreen(bgColor);
            tft.setTextColor(fgColor);
            tft.setTextWrap(false);
            tft.setTextSize(2);

            for( int i = 0; pColorSettings[i] != NULL; i++ )
            {
                tft.setCursor(0,  i * TFT_CHAR_HEIGHT);
                tft.print(pColorSettings[i]);
            }

            tft.fillRect( (XPOS_FG_BLACK_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_BLACK_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          BLACK );

            tft.fillRect( (XPOS_FG_BLUE_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_BLUE_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          BLUE );

            tft.fillRect( (XPOS_FG_RED_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_RED_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          RED );

            tft.fillRect( (XPOS_FG_GREEN_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_GREEN_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          GREEN );

            tft.fillRect( (XPOS_FG_CYAN_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_CYAN_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          CYAN );

            tft.fillRect( (XPOS_FG_MAGENTA_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_MAGENTA_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          MAGENTA );

            tft.fillRect( (XPOS_FG_YELLOW_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_YELLOW_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          YELLOW );
    
            tft.fillRect( (XPOS_FG_WHITE_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_WHITE_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          WHITE );

            tft.fillRect( (XPOS_BG_BLACK_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_BLACK_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          BLACK );

            tft.fillRect( (XPOS_BG_BLUE_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_BLUE_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          BLUE );

            tft.fillRect( (XPOS_BG_RED_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_RED_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          RED );

            tft.fillRect( (XPOS_BG_GREEN_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_GREEN_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          GREEN );

            tft.fillRect( (XPOS_BG_CYAN_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_CYAN_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          CYAN );

            tft.fillRect( (XPOS_BG_MAGENTA_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_MAGENTA_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          MAGENTA );

            tft.fillRect( (XPOS_BG_YELLOW_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_YELLOW_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                              YELLOW );

            tft.fillRect( (XPOS_BG_WHITE_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_WHITE_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          WHITE );

            tft.fillRect( (XPOS_BG_PREVIEW_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_BG_PREVIEW_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (WIDTH_BG_PREVIEW_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (HEIGHT_BG_PREVIEW_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          bgColor );

            tft.setCursor( XPOS_TEXT_PREVIEW_COLOR_SETTINGS * TFT_CHAR_WIDTH,
                           YPOS_TEXT_PREVIEW_COLOR_SETTINGS * TFT_CHAR_HEIGHT );
            tft.setTextColor(fgColor);
            tft.print("Beispieltext");

            isCommand = inputDone = false;

            memset( pCurrCommand, '\0', sizeof( pCurrCommand ) );
            memset( pPrevCommand, '\0', sizeof( pPrevCommand ) );
            currCommandLen = 0;

            inputResult = INPUT_RESULT_NO_RESULT;

            currItem = prevItem = 0;
            prevXPos = currXPos = currItem * TFT_CHAR_WIDTH;
            prevYPos = currYPos = currItem * TFT_CHAR_HEIGHT;
            currItemColumn = prevItemColumn = XPOS_FG_BLACK_COLOR_SETTINGS;
            currItemRow = prevItemRow = YPOS_FG_BLACK_COLOR_SETTINGS;
    
            selectedFGColor = fgColor;
            selectedBGColor = bgColor;

            do
            {
                switch( keyPressed = whichKeyPressed() )
                {
                    case key_left:
Serial.print("key_left item = ");
                        if( currItem > 0 )
                        {
                            prevItem = currItem;
                            currItem--;
                        }
                        else
                        {
                            // wrap
                            prevItem = currItem;
                            currItem = maxItem - 1;
                        }
Serial.println(currItem);
                        break;
                    case key_right:
Serial.print("key_right item = ");
                        if( currItem < (maxItem - 1) )
                        {
                            prevItem = currItem;
                            currItem++;
                        }
                        else
                        {
                            // wrap
                            prevItem = currItem;
                            currItem = 0;
                        }
Serial.println(currItem);
                        break;
                    case key_accept:
Serial.println("key_accept");
                        break;
                    case key_no_key:
                        break;
                    default:
Serial.println("Unknown key");
                        break;
                }

                if( prevItem == 0 && currItem == 0 &&
                    keyPressed == key_no_key )
                {
                    tft.drawRect( 
                          (XPOS_FG_BLACK_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (YPOS_FG_BLACK_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          (BOX_WIDTH_COLOR_SETTINGS * TFT_CHAR_WIDTH),
                          (BOX_HEIGHT_COLOR_SETTINGS * TFT_CHAR_HEIGHT),
                          ~BLACK );
                     currItemColor = BLACK;
                     currXPos = XPOS_FG_BLACK_COLOR_SETTINGS * TFT_CHAR_WIDTH;
                     currYPos = YPOS_FG_BLACK_COLOR_SETTINGS * TFT_CHAR_HEIGHT;
                     currItemColumn = XPOS_FG_BLACK_COLOR_SETTINGS;
                     currItemRow = YPOS_FG_BLACK_COLOR_SETTINGS;
                }

                if( keyPressed != key_no_key )
                {

                    if( keyPressed == key_left ||
                        keyPressed == key_right )
                    {
                        prevXPos = currXPos;
                        prevYPos = currYPos;
                        prevItemColumn = currItemColumn;
                        prevItemRow = currItemRow;
                        prevItemColor = currItemColor;
                        strcpy( pPrevCommand, pCurrCommand );

                        switch( currItem )
                        {
                            case ITEM_OK_COMMAND_COLOR_SETTINGS:
                                memset( pCurrCommand, '\0', 
                                    sizeof( pCurrCommand ) );
                                strcpy( pCurrCommand, "OK");

                                currItemColumn = 
                                           XPOS_OK_COMMAND_COLOR_SETTINGS;
                                currItemRow = 
                                           YPOS_OK_COMMAND_COLOR_SETTINGS;
                                break;
                            case ITEM_CANCEL_COMMAND_COLOR_SETTINGS:
                                memset( pCurrCommand, '\0', 
                                    sizeof( pCurrCommand ) );
                                strcpy( pCurrCommand, "CANCEL");

                                currItemColumn = 
                                           XPOS_CANCEL_COMMAND_COLOR_SETTINGS;
                                currItemRow = 
                                           YPOS_CANCEL_COMMAND_COLOR_SETTINGS;
                                break;
                            default:
                                switch( currItem / 4 )
                                {
                                    // relative Zeile = 0, 1, 2, 3,
                                    case 0:
                                        currItemRow = 
                                             YPOS_ITEM_ROW_0_COLOR_SETTINGS;
                                        break;
                                    case 1:
                                        currItemRow = 
                                             YPOS_ITEM_ROW_1_COLOR_SETTINGS;
                                        break;
                                    case 2:
                                        currItemRow = 
                                             YPOS_ITEM_ROW_2_COLOR_SETTINGS;
                                        break;
                                    case 3:
                                        currItemRow = 
                                             YPOS_ITEM_ROW_3_COLOR_SETTINGS;
                                        break;
                                    default:
                                        break;
                                }

                                switch( currItem % 4 )
                                {
                                    // relative Spalte = 0, 1, 2, 3
                                    case 0:
                                        currItemColumn = 
                                             XPOS_ITEM_COLUMN_0_COLOR_SETTINGS;
                                        if( currItemRow ==
                                             YPOS_ITEM_ROW_0_COLOR_SETTINGS ||
                                            currItemRow ==
                                             YPOS_ITEM_ROW_2_COLOR_SETTINGS )
                                        {
                                            currItemColor = BLACK;
                                        }
                                        else
                                        {
                                            currItemColor = CYAN;
                                        }
                                        break;
                                    case 1:
                                        currItemColumn = 
                                             XPOS_ITEM_COLUMN_1_COLOR_SETTINGS;
                                        if( currItemRow ==
                                             YPOS_ITEM_ROW_0_COLOR_SETTINGS ||
                                            currItemRow ==
                                             YPOS_ITEM_ROW_2_COLOR_SETTINGS )
                                        {
                                            currItemColor = BLUE;
                                        }
                                        else
                                        {
                                            currItemColor = MAGENTA;
                                        }
                                        break;
                                    case 2:
                                        currItemColumn = 
                                             XPOS_ITEM_COLUMN_2_COLOR_SETTINGS;
                                        if( currItemRow ==
                                             YPOS_ITEM_ROW_0_COLOR_SETTINGS ||
                                            currItemRow ==
                                             YPOS_ITEM_ROW_2_COLOR_SETTINGS )
                                        {
                                            currItemColor = RED;
                                        }
                                        else
                                        {
                                            currItemColor = YELLOW;
                                        }
                                        break;
                                    case 3:
                                        currItemColumn = 
                                             XPOS_ITEM_COLUMN_3_COLOR_SETTINGS;
                                        if( currItemRow ==
                                             YPOS_ITEM_ROW_0_COLOR_SETTINGS ||
                                            currItemRow ==
                                             YPOS_ITEM_ROW_2_COLOR_SETTINGS )
                                        {
                                            currItemColor = GREEN;
                                        }
                                        else
                                        {
                                            currItemColor = WHITE;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                break;
                        }

                        currXPos = currItemColumn * TFT_CHAR_WIDTH;
                        currYPos = currItemRow * TFT_CHAR_HEIGHT;

                        switch( prevItem )
                        {
                            case ITEM_OK_COMMAND_COLOR_SETTINGS:
                            case ITEM_CANCEL_COMMAND_COLOR_SETTINGS:
                                if( prevXPos != 0 &&
                                    prevYPos != 0 )
                                {
                                    tft.setCursor( prevXPos, prevYPos );
                                    tft.setTextColor(fgColor, bgColor);
                                    tft.print( pPrevCommand );
                                }
                                break;
                            default:
                                if( prevXPos != 0 &&
                                    prevYPos != 0 )
                                {
                                    tft.fillRect( prevXPos, prevYPos,
                                            (BOX_WIDTH_COLOR_SETTINGS *
                                             TFT_CHAR_WIDTH),
                                            (BOX_HEIGHT_COLOR_SETTINGS *
                                             TFT_CHAR_HEIGHT),
                                            prevItemColor );
                                }
                                break;
                        }

                        switch( currItem )
                        {
                            case ITEM_OK_COMMAND_COLOR_SETTINGS:
                            case ITEM_CANCEL_COMMAND_COLOR_SETTINGS:
                                tft.setCursor( currXPos, currYPos );
                                tft.setTextColor(bgColor, fgColor);
                                tft.print( pCurrCommand );
                                break;
                            default:
                                tft.drawRect( currXPos, currYPos,
                                            (BOX_WIDTH_COLOR_SETTINGS *
                                             TFT_CHAR_WIDTH),
                                            (BOX_HEIGHT_COLOR_SETTINGS *
                                             TFT_CHAR_HEIGHT),
                                            ~currItemColor );
                                break;
                        }
                    }
                    else
                    {
                        // key_accept
                        switch( currItem )
                        {
                            case ITEM_OK_COMMAND_COLOR_SETTINGS:
                                inputDone = true;
                                inputResult = INPUT_RESULT_OK;
                                break;
                            case ITEM_CANCEL_COMMAND_COLOR_SETTINGS:
                                inputDone = true;
                                inputResult = INPUT_RESULT_CANCEL;
                                break;
                            default:
Serial.println("default in accept");
                            
                                if( currItem < NUM_COLORS_IN_COLOR_SETTINGS )
                                {
                                    if( currItem < NUM_FG_COLORS_IN_SETTINGS )
                                    {
                                        selectedFGColor = currItemColor;
                                    }
                                    else
                                    {
                                        selectedBGColor = currItemColor;
                                    }
                                }
                                tft.fillRect( (XPOS_BG_PREVIEW_COLOR_SETTINGS *
                                               TFT_CHAR_WIDTH),
                                             (YPOS_BG_PREVIEW_COLOR_SETTINGS *
                                              TFT_CHAR_HEIGHT),
                                             (WIDTH_BG_PREVIEW_COLOR_SETTINGS *
                                              TFT_CHAR_WIDTH),
                                             (HEIGHT_BG_PREVIEW_COLOR_SETTINGS *
                                              TFT_CHAR_HEIGHT),
                                             selectedBGColor );

                                tft.setCursor(XPOS_TEXT_PREVIEW_COLOR_SETTINGS *
                                              TFT_CHAR_WIDTH,
                                              YPOS_TEXT_PREVIEW_COLOR_SETTINGS *
                                              TFT_CHAR_HEIGHT );
                                tft.setTextColor(selectedFGColor);
                                tft.print("Beispieltext");
                                break;
                        }
                    }

                }

            } while( !inputDone );

            switch( inputResult )
            {
                case INPUT_RESULT_NO_RESULT:
                    break;
                case INPUT_RESULT_OK:
                    break;
                case INPUT_RESULT_CANCEL:
                    break;
                default:
                    break;
            }

        }
        else
        {
Serial.println("pointers must NOT be NULL!");
        }
    }
    else
    {
Serial.println("NO valid display!");
    }
}




//
// ********************************
//

char *pNumericInput[] = {
    "",
    "",
    "",
    "",
    "      0 1 2 3",
    "      4 5 6 7",
    "        8 9",
    "       . , :",
    "",
    "",
    "",
    "",
    "--------------------",
    "   [<] [DEL] [>]",
    "   [OK] [CANCEL]",
    NULL
};

#define NUMERIC_INPUT_ZERO_ROW               4
#define NUMERIC_INPUT_ZERO_COLUMN            6
#define NUMERIC_INPUT_ITEM_ZERO              0

#define NUMERIC_INPUT_ONE_ROW                4
#define NUMERIC_INPUT_ONE_COLUMN             8
#define NUMERIC_INPUT_ITEM_ONE               1

#define NUMERIC_INPUT_TWO_ROW                4
#define NUMERIC_INPUT_TWO_COLUMN            10
#define NUMERIC_INPUT_ITEM_TWO               2

#define NUMERIC_INPUT_THREE_ROW              4
#define NUMERIC_INPUT_THREE_COLUMN          12
#define NUMERIC_INPUT_ITEM_THREE             3

#define NUMERIC_INPUT_FOUR_ROW               5
#define NUMERIC_INPUT_FOUR_COLUMN            6
#define NUMERIC_INPUT_ITEM_FOUR              4

#define NUMERIC_INPUT_FIVE_ROW               5
#define NUMERIC_INPUT_FIVE_COLUMN            8
#define NUMERIC_INPUT_ITEM_FIVE              5

#define NUMERIC_INPUT_SIX_ROW                5
#define NUMERIC_INPUT_SIX_COLUMN            10
#define NUMERIC_INPUT_ITEM_SIX               6

#define NUMERIC_INPUT_SEVEN_ROW              5
#define NUMERIC_INPUT_SEVEN_COLUMN          12
#define NUMERIC_INPUT_ITEM_SEVEN             7

#define NUMERIC_INPUT_EIGHT_ROW              6
#define NUMERIC_INPUT_EIGHT_COLUMN           8
#define NUMERIC_INPUT_ITEM_EIGHT             8

#define NUMERIC_INPUT_NINE_ROW               6
#define NUMERIC_INPUT_NINE_COLUMN           10
#define NUMERIC_INPUT_ITEM_NINE              9

#define NUMERIC_INPUT_DOT_ROW                7
#define NUMERIC_INPUT_DOT_COLUMN             7
#define NUMERIC_INPUT_ITEM_DOT              10

#define NUMERIC_INPUT_COMMA_ROW              7
#define NUMERIC_INPUT_COMMA_COLUMN           9
#define NUMERIC_INPUT_ITEM_COMMA            11

#define NUMERIC_INPUT_COLON_ROW              7
#define NUMERIC_INPUT_COLON_COLUMN          11
#define NUMERIC_INPUT_ITEM_COLON            12

#define NUMERIC_INPUT_LEFT_ROW              13
#define NUMERIC_INPUT_LEFT_COLUMN            4
#define NUMERIC_INPUT_ITEM_LEFT             13

#define NUMERIC_INPUT_DEL_ROW               13
#define NUMERIC_INPUT_DEL_COLUMN             8
#define NUMERIC_INPUT_ITEM_DEL              14

#define NUMERIC_INPUT_RIGHT_ROW             13
#define NUMERIC_INPUT_RIGHT_COLUMN          14
#define NUMERIC_INPUT_ITEM_RIGHT            15

#define NUMERIC_INPUT_OK_ROW                14
#define NUMERIC_INPUT_OK_COLUMN              4
#define NUMERIC_INPUT_ITEM_OK               16

#define NUMERIC_INPUT_CANCEL_ROW            14
#define NUMERIC_INPUT_CANCEL_COLUMN          9
#define NUMERIC_INPUT_ITEM_CANCEL           17

#define MAX_ITEMS_NUMERIC_INPUT             18
#define MAX_LEN_NUMERIC_INPUT_ITEM           6


//
// ********************************
//



void numericInput( uint16_t bgColor, uint16_t fgColor, char *pWhat,
                     char *pResult, int maxLen )
{
    if( _envVars.valid_display )
    {
        if( pWhat != NULL && pResult != NULL )
        {
            int keyPressed;
            int maxItem = MAX_ITEMS_NUMERIC_INPUT;
            int currItem, prevItem;
            int currXPos, currYPos;
            int prevXPos, prevYPos;
            int currItemColumn, currItemRow;
            int prevItemColumn, prevItemRow;

            char pCurrItem[MAX_LEN_NUMERIC_INPUT_ITEM + 1];
            char pPrevItem[MAX_LEN_NUMERIC_INPUT_ITEM + 1];
            int currInputLen;
            bool isCommand;
            int inputResult;
            bool inputDone;
           
            tft.fillScreen(bgColor);
            tft.setTextColor(fgColor);
            tft.setTextWrap(false);
            tft.setTextSize(2);

            tft.setCursor(0, 0);
            tft.print(pWhat);
            tft.print("=");

            for( int i = 0; pNumericInput[i] != NULL; i++ )
            {
                tft.setCursor(0,  i * TFT_CHAR_HEIGHT);
                tft.print(pNumericInput[i]);
            }

            isCommand = inputDone = false;

            memset( pResult, '\0', maxLen );
            currInputLen = 0;

            memset( pCurrItem, '\0', sizeof( pCurrItem ) );
            memset( pPrevItem, '\0', sizeof( pPrevItem ) );

            inputResult = INPUT_RESULT_NO_RESULT;

            currItem = prevItem = 0;
            prevXPos = currXPos = currItem * TFT_CHAR_WIDTH;
            prevYPos = currYPos = currItem * TFT_CHAR_HEIGHT;
            currItemColumn = prevItemColumn = NUMERIC_INPUT_ZERO_COLUMN;
            currItemRow = prevItemRow = NUMERIC_INPUT_ZERO_ROW;
    
            do
            {
                switch( keyPressed = whichKeyPressed() )
                {
                    case key_left:
Serial.print("key_left item = ");
                        if( currItem > 0 )
                        {
                            prevItem = currItem;
                            currItem--;
                        }
                        else
                        {
                            // wrap
                            prevItem = currItem;
                            currItem = maxItem - 1;
                        }
Serial.println(currItem);
                        break;
                    case key_right:
Serial.print("key_right item = ");
                        if( currItem < (maxItem - 1) )
                        {
                            prevItem = currItem;
                            currItem++;
                        }
                        else
                        {
                            // wrap
                            prevItem = currItem;
                            currItem = 0;
                        }
Serial.println(currItem);
                        break;
                    case key_accept:
Serial.println("key_accept");
                        break;
                    case key_no_key:
                        break;
                    default:
Serial.println("Unknown key");
                        break;
                }


                if( keyPressed == key_no_key &&
                    prevXPos == 0 && prevYPos == 0 )
                {
                    strcpy( pCurrItem, "0" );
                    currXPos = currItemColumn * TFT_CHAR_WIDTH;
                    currYPos = currItemRow * TFT_CHAR_HEIGHT;

                    tft.setCursor( currXPos, currYPos );
                    tft.setTextColor(bgColor, fgColor);
                    tft.print( pCurrItem );
                }


                if( keyPressed != key_no_key )
                {
                    if( keyPressed == key_left ||
                        keyPressed == key_right )
                    {
                        prevXPos = currXPos;
                        prevYPos = currYPos;
                        prevItemColumn = currItemColumn;
                        prevItemRow = currItemRow;
                        strcpy( pPrevItem, pCurrItem );

                        switch( currItem )
                        {
                            case NUMERIC_INPUT_ITEM_ZERO:
                                currItemRow = NUMERIC_INPUT_ZERO_ROW;
                                currItemColumn = NUMERIC_INPUT_ZERO_COLUMN;
                                strcpy( pCurrItem, "0" );
                                break;
                            case NUMERIC_INPUT_ITEM_ONE:
                                currItemRow = NUMERIC_INPUT_ONE_ROW;
                                currItemColumn = NUMERIC_INPUT_ONE_COLUMN;
                                strcpy( pCurrItem, "1" );
                                break;
                            case NUMERIC_INPUT_ITEM_TWO:
                                currItemRow = NUMERIC_INPUT_TWO_ROW;
                                currItemColumn = NUMERIC_INPUT_TWO_COLUMN;
                                strcpy( pCurrItem, "2" );
                                break;
                            case NUMERIC_INPUT_ITEM_THREE:
                                currItemRow = NUMERIC_INPUT_THREE_ROW;
                                currItemColumn = NUMERIC_INPUT_THREE_COLUMN;
                                strcpy( pCurrItem, "3" );
                                break;
                            case NUMERIC_INPUT_ITEM_FOUR:
                                currItemRow = NUMERIC_INPUT_FOUR_ROW;
                                currItemColumn = NUMERIC_INPUT_FOUR_COLUMN;
                                strcpy( pCurrItem, "4" );
                                break;
                            case NUMERIC_INPUT_ITEM_FIVE:
                                currItemRow = NUMERIC_INPUT_FIVE_ROW;
                                currItemColumn = NUMERIC_INPUT_FIVE_COLUMN;
                                strcpy( pCurrItem, "5" );
                                break;
                            case NUMERIC_INPUT_ITEM_SIX:
                                currItemRow = NUMERIC_INPUT_SIX_ROW;
                                currItemColumn = NUMERIC_INPUT_SIX_COLUMN;
                                strcpy( pCurrItem, "6" );
                                break;
                            case NUMERIC_INPUT_ITEM_SEVEN:
                                currItemRow = NUMERIC_INPUT_SEVEN_ROW;
                                currItemColumn = NUMERIC_INPUT_SEVEN_COLUMN;
                                strcpy( pCurrItem, "7" );
                                break;
                            case NUMERIC_INPUT_ITEM_EIGHT:
                                currItemRow = NUMERIC_INPUT_EIGHT_ROW;
                                currItemColumn = NUMERIC_INPUT_EIGHT_COLUMN;
                                strcpy( pCurrItem, "8" );
                                break;
                            case NUMERIC_INPUT_ITEM_NINE:
                                currItemRow = NUMERIC_INPUT_NINE_ROW;
                                currItemColumn = NUMERIC_INPUT_NINE_COLUMN;
                                strcpy( pCurrItem, "9" );
                                break;
                            case NUMERIC_INPUT_ITEM_DOT:
                                currItemRow = NUMERIC_INPUT_DOT_ROW;
                                currItemColumn = NUMERIC_INPUT_DOT_COLUMN;
                                strcpy( pCurrItem, "." );
                                break;
                            case NUMERIC_INPUT_ITEM_COMMA:
                                currItemRow = NUMERIC_INPUT_COMMA_ROW;
                                currItemColumn = NUMERIC_INPUT_COMMA_COLUMN;
                                strcpy( pCurrItem, "," );
                                break;
                            case NUMERIC_INPUT_ITEM_COLON:
                                currItemRow = NUMERIC_INPUT_COLON_ROW;
                                currItemColumn = NUMERIC_INPUT_COLON_COLUMN;
                                strcpy( pCurrItem, ":" );
                                break;
                            case NUMERIC_INPUT_ITEM_LEFT:
                                currItemRow = NUMERIC_INPUT_LEFT_ROW;
                                currItemColumn = NUMERIC_INPUT_LEFT_COLUMN;
                                strcpy( pCurrItem, "<" );
                                break;
                            case NUMERIC_INPUT_ITEM_DEL:
                                currItemRow = NUMERIC_INPUT_DEL_ROW;
                                currItemColumn = NUMERIC_INPUT_DEL_COLUMN;
                                strcpy( pCurrItem, "DEL" );
                                break;
                            case NUMERIC_INPUT_ITEM_RIGHT:
                                currItemRow = NUMERIC_INPUT_RIGHT_ROW;
                                currItemColumn = NUMERIC_INPUT_RIGHT_COLUMN;
                                strcpy( pCurrItem, ">" );
                                break;
                            case NUMERIC_INPUT_ITEM_OK:
                                currItemRow = NUMERIC_INPUT_OK_ROW;
                                currItemColumn = NUMERIC_INPUT_OK_COLUMN;
                                strcpy( pCurrItem, "OK" );
                                break;
                            case NUMERIC_INPUT_ITEM_CANCEL:
                                currItemRow = NUMERIC_INPUT_CANCEL_ROW;
                                currItemColumn = NUMERIC_INPUT_CANCEL_COLUMN;
                                strcpy( pCurrItem, "CANCEL" );
                                break;
                            default:
                                break;
                        }

                        currXPos = currItemColumn * TFT_CHAR_WIDTH;
                        currYPos = currItemRow * TFT_CHAR_HEIGHT;

                        //
                        tft.setCursor( prevXPos, prevYPos );
                        tft.setTextColor(fgColor, bgColor);
                        tft.print( pPrevItem );
                        // 
                        tft.setCursor( currXPos, currYPos );
                        tft.setTextColor(bgColor, fgColor);
                        tft.print( pCurrItem );
                        //

                    }
                    else
                    {
                        // key_accept
                        switch( currItem )
                        {
                            case NUMERIC_INPUT_ITEM_OK:
                                inputDone = true;
                                inputResult = INPUT_RESULT_OK;
                                break;
                            case NUMERIC_INPUT_ITEM_CANCEL:
                                inputDone = true;
                                inputResult = INPUT_RESULT_CANCEL;
                                break;
                            case NUMERIC_INPUT_ITEM_LEFT:
                            case NUMERIC_INPUT_ITEM_DEL:
                            case NUMERIC_INPUT_ITEM_RIGHT:
                                break;
                            default:
Serial.println("default in accept");
                                if( currInputLen + 1 < maxLen )
                                {
                                    tft.setTextWrap( true );
                                    strcat(pResult, pCurrItem);
                                    currInputLen++;
                                    
                                    tft.setTextColor(fgColor, bgColor);
                                    tft.setCursor( strlen(pWhat)*TFT_CHAR_WIDTH,
                                                   0 );
                                                   
                                    tft.print( pResult );
                                    tft.setTextWrap( false );
                                }
                                // NUMERIC_INPUT_ITEM_DOT:
                                // NUMERIC_INPUT_ITEM_COMMA:
                                // NUMERIC_INPUT_ITEM_COLON:
                                break;
                        }
                    }

                }

            } while( !inputDone );

            switch( inputResult )
            {
                case INPUT_RESULT_NO_RESULT:
                    break;
                case INPUT_RESULT_OK:
                    break;
                case INPUT_RESULT_CANCEL:
                    memset( pResult, '\0', maxLen );
                    break;
                default:
                    break;
            }
        }
        else
        {
Serial.println("pWhat and/or pResult must NOT be NULL!");
        }
    }
    else
    {
Serial.println("NO valid display!");
    }
}





/* *********************************************************************
 * void setChangedFlag(void)
 * ---------------------------------
 *  arguments: -
 * ------------
 * purpose: set changed flag to force display of data
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/
void setChangedFlag(void)
{
    if( _envVars.valid_bme280 )
    {
        _envVars.local_bme_temperature.has_changed = true;
        _envVars.local_bme_temperature.is_valid = true;
        _envVars.local_bme_humidity.is_valid = true;
        _envVars.local_bme_humidity.has_changed = true;
        _envVars.local_bme_pressure.is_valid = true;
        _envVars.local_bme_pressure.has_changed = true;
    }
}


/* *********************************************************************
 * void loop()
 * ---------------------------------
 *  arguments: -
 * ------------
 * purpose: main loop
 * ---------------------------------
 * returns: nothing
 ***********************************************************************
*/

#define RESULT_STRING_MAXLEN    30

void loop()
{
  uint16_t newFG, newBG;
  char resultString[RESULT_STRING_MAXLEN + 1];

  events();  // ezTime!

  if (Serial.available() > 0) 
  {
      char incomingByte;

      if( (incomingByte = (char) (Serial.read() & 0xFF) ) == 's' || 
           incomingByte  == 'S' )
      {
          while(Serial.available() )
          {
              Serial.read();
          }

          numericInput( BLACK, WHITE, "IP-Addr=", 
                        resultString, RESULT_STRING_MAXLEN);

//          colorSelection( BLACK, WHITE, &newBG, &newFG );

//          memset( resultString, '\0', sizeof(resultString) );
//          selectStringParam("SSID=", resultString, RESULT_STRING_MAXLEN);
//          memset( resultString, '\0', sizeof(resultString) );
//          selectStringParam("PASS=", resultString, RESULT_STRING_MAXLEN);

          tft.fillScreen(BLACK);
          tft.setTextColor(WHITE);
          tft.setCursor(0, 0);
          tft.setTextWrap(false);
          setChangedFlag();
      }
  }

  localSensorHandler();

  sensorDataHandler();

  telnetHandler();

  httpHandler();

}

#ifdef NEVERDEF

//
// ********************************
//

char *pSettingsMenu[] = {
    "--------------------",
    "Settings Seite 1",
    "Settings Seite 2",
    "Settings Seite 3",
    "--------------------",
    "Settings SSID",
    "Settings Passphrase",
    "Settings Admin",
    "Settings IP",
    "",
    "",
    "Speichern",
    "Werkseinstellungen",
    "--------------------",
    "       [EXIT]",
    NULL
};

#define XPOS_PAGE_1_SETTINGS_MENU          1
#define YPOS_PAGE_1_SETTINGS_MENU          1

#define XPOS_PAGE_2_SETTINGS_MENU          1
#define YPOS_PAGE_2_SETTINGS_MENU          2

#define XPOS_PAGE_3_SETTINGS_MENU          1
#define YPOS_PAGE_3_SETTINGS_MENU          3

#define XPOS_SSID_SETTINGS_MENU            1
#define YPOS_SSID_SETTINGS_MENU            5

#define XPOS_PASS_SETTINGS_MENU            1
#define YPOS_PASS_SETTINGS_MENU            6

#define XPOS_ADMIN_SETTINGS_MENU           1
#define YPOS_ADMIN_SETTINGS_MENU           7

#define XPOS_FIX_IP_SETTINGS_MENU          1
#define YPOS_FIX_IP_SETTINGS_MENU          8

#define XPOS_EXIT_SETTINGS_MENU            8
#define YPOS_EXIT_SETTINGS_MENU           14

// Einstellungen:
// Format der Ausgabe (ein, zwei, keine Nachkomma-Stellen)
// abhaengig davon Vergleich (auf bzw. abrunden, ohne Nachkomma vergleichen)
//
// Farbe der Ausgabe (alle weiss oder rot, gelb, grün und Grenzwerte dazu)
// Beschriftung der Felder
//
// Trend-Anzeige
// Mondphasen
// IP Adresse
//
// Lese- unf Anzeige-Intervalle
//
// Format und Farbe des Datums und der Uhrzeit
//

//
// ********************************
//

char *pSettingsPage1[] = {
    "----- Setup 1 ------",
    "[ ] Wochentag",
    "[ ] Datum",
    "[ ] Uhrzeit",
    "  [ ] Sekunden",
    "--------------------",
    "[ ] Interner Sensor",
    "  [ ] Luftfeuchte",
    "  [ ] Temperatur",
    "  [ ] Luftdruck",
    "  [ ] Trend",
    "--------------------",
    "[ ] IP Adresse",
    "--------------------",
    "   [OK] [CANCEL]",
    NULL
};

#define XPOS_WEEKDAY_SETTINGS_PAGE_1       1
#define YPOS_WEEKDAY_SETTINGS_PAGE_1       1

#define XPOS_DATE_SETTINGS_PAGE_1          1
#define YPOS_DATE_SETTINGS_PAGE_1          2

#define XPOS_TIME_SETTINGS_PAGE_1          1
#define YPOS_TIME_SETTINGS_PAGE_1          3

#define XPOS_TIME_SEC_SETTINGS_PAGE_1      3
#define YPOS_TIME_SEC_SETTINGS_PAGE_1      4

#define XPOS_INTERNAL_SETTINGS_PAGE_1      1
#define YPOS_INTERNAL_SETTINGS_PAGE_1      6

#define XPOS_HUMIDITY_SETTINGS_PAGE_1      3
#define YPOS_HUMIDITY_SETTINGS_PAGE_1      7

#define XPOS_TEMPERATURE_SETTINGS_PAGE_1   3
#define YPOS_TEMPERATURE_SETTINGS_PAGE_1   8

#define XPOS_PRESSURE_SETTINGS_PAGE_1      3
#define YPOS_PRESSURE_SETTINGS_PAGE_1      9

#define XPOS_TREND_SETTINGS_PAGE_1         3
#define YPOS_TREND_SETTINGS_PAGE_1        10

#define XPOS_IP_ADDR_SETTINGS_PAGE_1       1
#define YPOS_IP_ADDR_SETTINGS_PAGE_1      12

#define XPOS_IP_ADDR_SETTINGS_PAGE_1       4
#define YPOS_IP_ADDR_SETTINGS_PAGE_1      12

//
// ********************************
//

char *pSettingsPage2[] = {
    "----- Setup 2 ------",
    "Luftfeuchte",
    "[ ] Mono [ ] Farbe",
    "  oberer  xxxxx",
    "  unterer xxxxx",
    "Temperatur",
    "[ ] Mono [ ] Farbe",
    "  oberer  xxxxx",
    "  unterer xxxxx",
    "Luftdruck",
    "[ ] Mono [ ] Farbe",
    "  oberer  xxxxx",
    "  unterer xxxxx",
    "--------------------",
    "   [OK] [CANCEL]",
    NULL
};

#define XPOS_HUM_MONO_SETTINGS_PAGE_2      1
#define YPOS_HUM_MONO_SETTINGS_PAGE_2      2
#define XPOS_HUM_COLORSETTINGS_PAGE_2     10
#define YPOS_HUM_COLORSETTINGS_PAGE_2      2
#define XPOS_HUM_UPPER_SETTINGS_PAGE_2    10
#define YPOS_HUM_UPPER_SETTINGS_PAGE_2     3
#define XPOS_HUM_LOWER_SETTINGS_PAGE_2    10
#define YPOS_HUM_LOWER_SETTINGS_PAGE_2     4

#define XPOS_TEMP_MONO_SETTINGS_PAGE_2     1
#define YPOS_TEMP_MONO_SETTINGS_PAGE_2     6
#define XPOS_TEMP_COLORSETTINGS_PAGE_2    10
#define YPOS_TEMP_COLORSETTINGS_PAGE_2     6
#define XPOS_TEMP_UPPER_SETTINGS_PAGE_2   10
#define YPOS_TEMP_UPPER_SETTINGS_PAGE_2    7
#define XPOS_TEMP_LOWER_SETTINGS_PAGE_2   10
#define YPOS_TEMP_LOWER_SETTINGS_PAGE_2    8

#define XPOS_PRESS_MONO_SETTINGS_PAGE_2    1
#define YPOS_PRESS_MONO_SETTINGS_PAGE_2   10
#define XPOS_PRESS_COLORSETTINGS_PAGE_2   10
#define YPOS_PRESS_COLORSETTINGS_PAGE_2   10
#define XPOS_PRESS_UPPER_SETTINGS_PAGE_2  10
#define YPOS_PRESS_UPPER_SETTINGS_PAGE_2  11
#define XPOS_PRESS_LOWER_SETTINGS_PAGE_2  10
#define YPOS_PRESS_LOWER_SETTINGS_PAGE_2  12

//
// ********************************
//

char *pSettingsPage3[] = {
    "----- Setup 3 ------",
    "[ ] HTTP",
    "[ ] TELNET",
    "[ ] NTP",
    "[ ] RTC",
    "[ ] MOND",
    "[ ] LiPo",
    "[ ] runtime",
    "Background = ",
    "Foreground = ",
    "
    "
    "
    "--------------------",
    "   [OK] [CANCEL]",
    NULL
};

#define XPOS_HTTP_SETTINGS_PAGE_3          1
#define YPOS_HTTP_SETTINGS_PAGE_3          1

//
// ********************************
//

#define XPOS_OK_SETTINGS_PAGE              4
#define YPOS_OK_SETTINGS_PAGE             14

#define XPOS_CANCEL_SETTINGS_PAGE          9
#define YPOS_CANCEL_SETTINGS_PAGE         14

//
// ********************************
//

//
// ********************************
//

// |   -- Stunden:Minuten.Sekunden
// |     -- Update Intervall Sekunde
// |   -- Stunden:Minuten
// |     -- Update Intervall Minute
// | 
// | Luftfeuchtigkeit
// |   -- 2 Nachkommastellen
// |   -- 1 Nachkommastelle
// |   -- 0 Nachkommastellen
// |   -- Update Intervall
// |   -- Trend-Anzeige
// |   -- Schrift-Farbe
// |      -- Mono
// |      -- farbige Anzeige
// |         -- unterer Grenzwert
// |         -- oberer Grenzwert
// | Temperatur
// |   -- 2 Nachkommastellen
// |   -- 1 Nachkommastelle
// |   -- 0 Nachkommastellen
// |   -- Update Intervall
// |   -- Trend-Anzeige
// |   -- Schrift-Farbe
// |      -- Mono
// |      -- farbige Anzeige
// |         -- unterer Grenzwert
// |         -- oberer Grenzwert
// | Luftdruck
// |   -- 2 Nachkommastellen
// |   -- 1 Nachkommastelle
// |   -- 0 Nachkommastellen
// |   -- Update Intervall
// |   -- Trend-Anzeige
// |   -- Schrift-Farbe
// |      -- Mono
// |      -- farbige Anzeige
// |         -- unterer Grenzwert
// |         -- oberer Grenzwert
// | IP-Adresse
// | 
// | Seite 2:
// | ========
// | Hintergrund-Farbe
// | Schrift-Farbe
// |   -- Wochentag
// |   -- Datum
// |   -- Uhrzeit
// |      -- Stunden:Minuten.Sekunden
// |        -- Update Intervall Sekunde
// |      -- Stunden:Minuten
// |        -- Update Intervall Minute
// | Mondphase
// | Alternative Sensoren
// | Akku Kapazitaet
// | Restlaufzeit
// | 
// | 
// | Seite 3:
// | ========
// | SSID
// | PASSPHRASE
// | Feste IP
// | Admin Passwort
// | 
// | 



#endif // NEVERDEF
//
