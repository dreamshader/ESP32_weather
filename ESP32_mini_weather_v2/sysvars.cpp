// #ifdef __cplusplus
// extern “C” {
// #endif

#include "FS.h"
#include "SPIFFS.h"

#include <Adafruit_GFX.h>    // Core graphics library by Adafruit
#include <Arduino_ST7789.h> // Hardware-specific library for ST7789

#include "sysvars.h"

char fileBuffer[FILE_BUFFER_MAX];
char dataBuffer[FILE_BUFFER_MAX];
struct _system_vars_ _envVars;

// #if defined(ARDUINO)

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
    _envVars.last_local_bme_humidity    = _envVars.local_bme_pressure = 
    _envVars.last_local_bme_pressure    = _envVars.local_bme_temperature;

    _envVars.http_timeoutTime         = HTTP_TIMEOUT_MILLIS;
    //
    _envVars.telnet_timeoutTime       = TELNET_TIMEOUT_MILLIS;
    //
    _envVars.wlan_timeoutTime         = WIFI_TIMEOUT_MILLIS;

    // new -> internals completed
    _envVars.pAdminPassword      = (char*) NULL;
    _envVars.networkMode         = NETWORK_MODE_STA;
    _envVars.wlanTimeout         = DEFAULT_WLAN_TIMEOUT;
    _envVars.pStaticIP           = (char*) NULL;
    _envVars.pWlanSSID           = (char*) NULL;
    _envVars.pWlanPassphrase     = (char*) NULL;
    //
    _envVars.uartMode            = UART_MODE_DISABLED;
    _envVars.uartAPI             = DEFAULT_HAS_UART_API;
    _envVars.uartAPIAccess       = UART_API_ACCESS_NONE;
    _envVars.pUartApiUser        = (char*) NULL;
    _envVars.pUartApiPassword    = (char*) NULL;
    _envVars.uartApiTimeout      = DEFAULT_UART_API_TIMEOUT;
    //
    _envVars.httpMode            = HTTP_MODE_DISABLED;
    _envVars.httpAPI             = DEFAULT_HAS_HTTP_API;
    _envVars.httpAPIAccess       = HTTP_API_ACCESS_NONE;
    _envVars.pHttpApiUser        = (char*) NULL;
    _envVars.pHttpApiPassword    = (char*) NULL;
    _envVars.httpApiTimeout      = DEFAULT_HTTP_API_TIMEOUT;
    //
    _envVars.telnetMode          = TELNET_MODE_DISABLED;
    _envVars.telnetAPI           = DEFAULT_HAS_TELNET_API;
    _envVars.telnetAPIAccess     = TELNET_API_ACCESS_NONE;
    _envVars.pTelnetApiUser      = (char*) NULL;
    _envVars.pTelnetApiPassword  = (char*) NULL;
    _envVars.telnetApiTimeout    = DEFAULT_TELNET_API_TIMEOUT;
    //
    _envVars.colorBackground     = BLACK;
    _envVars.colorForeground     = WHITE;
    _envVars.weekdayFormat       = WEEKDAY_DISABLED;
    _envVars.dateFormat          = DATE_DISABLED;
    _envVars.timeFormat          = TIME_LONG;
    _envVars.displayHumidity     = INTERNAL_DISPLAY_HUMIDITY_ENABLED;
    _envVars.displayTemperature  = INTERNAL_DISPLAY_TEMPERATURE_ENABLED;
    _envVars.displayPressure     = INTERNAL_DISPLAY_PRESSURE_ENABLED;
    _envVars.displayMoonphase    = DEFAULT_DISPLAY_MOON_PHASE;
    _envVars.displayLipo         = DISPLAY_LIPO_DISABLED;
    _envVars.displayIPAddress    = DEFAULT_DISPLAY_IP_ADDRESS;

    _envVars.humidityUpperLimit     = INTERNAL_HUMIDITY_UPPER_LIMIT;
    _envVars.humidityLowerLimit     = INTERNAL_HUMIDITY_LOWER_LIMIT;
    _envVars.temperatureUpperLimit  = INTERNAL_TEMPERATURE_UPPER_LIMIT;
    _envVars.temperatureLowerLimit  = INTERNAL_TEMPERATURE_LOWER_LIMIT;
    _envVars.pressureUpperLimit     = INTERNAL_PRESSURE_UPPER_LIMIT;
    _envVars.pressureLowerLimit     = INTERNAL_PRESSURE_LOWER_LIMIT;
}

//
// *****************************************************************
//
//


int writeSysVars( struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
#if defined(ARDUINO)
    File pSettingsFile;
#else
    FILE *pSettingsFile;
#endif // defined(ARDUINO)



    if( pSysVars != (struct _system_vars_*) NULL )
    {
        if( _envVars.valid_FLASH == true )
        {

#if defined(ARDUINO)
            //
            if( SPIFFS.exists("/settings.dat") )
            {
                if( SPIFFS.exists("/settings.bak") )
                {
                    SPIFFS.remove("/settings.bak");
                }
                SPIFFS.rename("/settings.dat", "/settings.bak");
            }
            //
#endif // defined(ARDUINO)

#if defined(ARDUINO)
            if( pSettingsFile = SPIFFS.open("/settings.dat", FILE_WRITE) ) 
#else
            if( (pSettingsFile = fopen("settings.dat", "w+")) != (FILE*) NULL )
#endif // defined(ARDUINO)
            {

                sprintf( dataBuffer, "%d:%s", FIELD_ID_ADMIN_PASSWORD,
                                 pSysVars->pAdminPassword == (char*) NULL ? \
				  "" : pSysVars->pAdminPassword );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//           fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//           fprintf( stderr, "%s\n", dataBuffer );
//           fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

                sprintf( dataBuffer, "%d:%d", FIELD_ID_NETWORK_MODE, 
			         pSysVars->networkMode );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

                sprintf( dataBuffer, "%d:%lu", FIELD_ID_WLAN_TIMEOUT, 
			         pSysVars->wlanTimeout );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)


                sprintf( dataBuffer, "%d:%s", FIELD_ID_WLAN_STATIC_IP_ADDR, 
			         pSysVars->pStaticIP == (char*) NULL ? \
				 "" : pSysVars->pStaticIP );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)


                sprintf( dataBuffer, "%d:%s", FIELD_ID_WLAN_SSID, 
			          pSysVars->pWlanSSID == (char*) NULL ? \
				  "" : pSysVars->pWlanSSID );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)


                sprintf( dataBuffer, "%d:%s", FIELD_ID_WLAN_PASSPHRASE, 
			          pSysVars->pWlanPassphrase == (char*) NULL ? \
				  "" : pSysVars->pWlanPassphrase );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_UART_API,
                                 pSysVars->uartAPI ? 1 : 0 );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_UART_API_ACCESS,
                                 pSysVars->uartAPIAccess );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%s", FIELD_ID_UART_API_USER,
                                 pSysVars->pUartApiUser == (char*) NULL ? \
				  "" : pSysVars->pUartApiUser );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%s", FIELD_ID_UART_API_PASSWORD,
                                 pSysVars->pUartApiPassword == (char*) NULL ? \
				  "" : pSysVars->pUartApiPassword );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_UART_API_TIMEOUT,
                                 pSysVars->uartApiTimeout );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_UART_MODE,
                                 pSysVars->uartMode );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_HTTP_API,
                                 pSysVars->httpAPI ? 1 : 0 );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_HTTP_API_ACCESS,
                                 pSysVars->httpAPIAccess );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%s", FIELD_ID_HTTP_API_USER,
                                 pSysVars->pHttpApiUser == (char*) NULL ? \
				  "" : pSysVars->pHttpApiUser );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%s", FIELD_ID_HTTP_API_PASSWORD,
                                 pSysVars->pHttpApiPassword == (char*) NULL ? \
				  "" : pSysVars->pHttpApiPassword );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_HTTP_API_TIMEOUT,
				  pSysVars->httpApiTimeout );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_HTTP_MODE,
                                 pSysVars->httpMode );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_TELNET_API,
                                 pSysVars->telnetAPI ? 1 : 0 );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_TELNET_API_ACCESS,
                                 pSysVars->telnetAPIAccess );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%s", FIELD_ID_TELNET_API_USER,
                                 pSysVars->pTelnetApiUser == (char*) NULL ? \
				  "" : pSysVars->pTelnetApiUser );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%s", FIELD_ID_TELNET_API_PASSWORD,
                               pSysVars->pTelnetApiPassword == (char*) NULL ? \
				  "" : pSysVars->pTelnetApiPassword );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_TELNET_API_TIMEOUT,
                                 pSysVars->telnetApiTimeout );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// 
                sprintf( dataBuffer, "%d:%d", FIELD_ID_TELNET_MODE,
                                 pSysVars->telnetMode );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

//
// FIELD_ID_SEALEVEL_PRESSURE
// float bme_sealevel_hpa;
                sprintf( dataBuffer, "%d:%.2f", FIELD_ID_SEALEVEL_PRESSURE,
                                 pSysVars->bme_sealevel_hpa );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_BACKGROUND_COLOR
                sprintf( dataBuffer, "%d:%u", FIELD_ID_BACKGROUND_COLOR,
                                 pSysVars->colorBackground );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_FOREGROUND_COLOR
                sprintf( dataBuffer, "%d:%u", FIELD_ID_FOREGROUND_COLOR,
                                 pSysVars->colorForeground );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_WEEKDAY_FORMAT
                sprintf( dataBuffer, "%d:%d", FIELD_ID_WEEKDAY_FORMAT,
                                 pSysVars->weekdayFormat );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_DATE_FORMAT
                sprintf( dataBuffer, "%d:%d", FIELD_ID_DATE_FORMAT,
                                 pSysVars->dateFormat );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_TIME_FORMAT
                sprintf( dataBuffer, "%d:%d", FIELD_ID_TIME_FORMAT,
                                 pSysVars->timeFormat );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_HUMIDITY_INTERVAL
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_HUMIDITY_INTERVAL,
                                 pSysVars->local_bme_humidity_interval );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_TEMPERATURE_INTERVAL
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_TEMPERATURE_INTERVAL,
                                 pSysVars->local_bme_temperature_interval );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_PRESSURE_INTERVAL
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_PRESSURE_INTERVAL,
                                 pSysVars->local_bme_pressure_interval );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_DISPLAY_HUMIDITY
                sprintf( dataBuffer, "%d:%d", FIELD_ID_INTERNAL_DISPLAY_HUMIDITY,
                                 pSysVars->displayHumidity );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_HUMIDITY_UPPER_LIMIT
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_HUMIDITY_UPPER_LIMIT,
                                 pSysVars->humidityUpperLimit );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_HUMIDITY_LOWER_LIMIT
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_HUMIDITY_LOWER_LIMIT,
                                 pSysVars->humidityLowerLimit );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_DISPLAY_TEMPERATURE
                sprintf( dataBuffer, "%d:%d", FIELD_ID_INTERNAL_DISPLAY_TEMPERATURE,
                                 pSysVars->displayTemperature );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_TEMPERATURE_UPPER_LIMIT
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_TEMPERATURE_UPPER_LIMIT,
                                 pSysVars->temperatureUpperLimit );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_TEMPERATURE_LOWER_LIMIT
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_TEMPERATURE_LOWER_LIMIT,
                                 pSysVars->temperatureLowerLimit );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_DISPLAY_PRESSURE
                sprintf( dataBuffer, "%d:%d", FIELD_ID_INTERNAL_DISPLAY_PRESSURE,
                                 pSysVars->displayPressure );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_PRESSURE_UPPER_LIMIT
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_PRESSURE_UPPER_LIMIT,
                                 pSysVars->pressureUpperLimit );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_INTERNAL_PRESSURE_LOWER_LIMIT
                sprintf( dataBuffer, "%d:%lu", FIELD_ID_INTERNAL_PRESSURE_LOWER_LIMIT,
                                 pSysVars->pressureLowerLimit );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_DISPLAY_IP_ADDRESS
                sprintf( dataBuffer, "%d:%d", FIELD_ID_DISPLAY_IP_ADDRESS,
                                 pSysVars->displayIPAddress ? 1 : 0 );
    
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_DISPLAY_MOON_PHASE
                sprintf( dataBuffer, "%d:%d", FIELD_ID_DISPLAY_MOON_PHASE,
                                 pSysVars->displayMoonphase ? 1 : 0 );

                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)

// FIELD_ID_DISPLAY_LIPO_MODE
                sprintf( dataBuffer, "%d:%d", FIELD_ID_DISPLAY_LIPO_MODE,
                                 pSysVars->displayLipo );
                sprintf( fileBuffer, "%3.3u:%s", strlen(dataBuffer), dataBuffer );
#if defined(ARDUINO)
                pSettingsFile.write( (uint8_t*) fileBuffer, strlen(fileBuffer) );
#else
                fprintf(pSettingsFile, "%s", fileBuffer);
//fprintf( stderr, "%u:\n", strlen(dataBuffer) );
//fprintf( stderr, "%s\n", dataBuffer );
//fprintf( stderr, "%3.3u:%s\n\n", strlen(dataBuffer), dataBuffer );
#endif // defined(ARDUINO)


#if defined(ARDUINO)
                pSettingsFile.close();
#else
                fclose(pSettingsFile);
#endif // defined(ARDUINO)

            }
            else
            {
                retVal = RETVAL_OPEN_FILE;
            }
        }
        else
        {
	    retVal = RETVAL_NO_FLASH;
        }
    }
    else
    {
        retVal = RETVAL_NULLP;
    }

    
    return( retVal );
}


int readSysVars( struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
#if defined(ARDUINO)
    File pSettingsFile;
#else    
    FILE *pSettingsFile;
#endif // defined(ARDUINO)
    
    char dataLen[FILE_DATA_LEN + 1];
    int recLen;
    bool stopReading;
    int dataOffset;
    int payLoadLen;

    if( pSysVars != (struct _system_vars_*) NULL )
    {
        if( _envVars.valid_FLASH == true )
        {
#if defined(ARDUINO)
            if( pSettingsFile = SPIFFS.open("/settings.dat", FILE_READ) ) 
#else
            if( (pSettingsFile = fopen("settings.dat", "r")) != (FILE*) NULL )
#endif // defined(ARDUINO)

            {
                stopReading = false;
                do
                {
                    memset(dataLen, '\0', sizeof( dataLen ));

#if defined(ARDUINO)
                    if(pSettingsFile.readBytes(dataLen, FILE_DATA_LEN ) == 
				FILE_DATA_LEN )
#else
                    if( fread( dataLen, sizeof(char), FILE_DATA_LEN, 
                           pSettingsFile ) == FILE_DATA_LEN )
#endif // defined(ARDUINO)
                    {
                        recLen = atoi(dataLen);
// fprintf( stderr, "LEN = %d\n", recLen );
                        memset( fileBuffer, '\0', sizeof(fileBuffer) );

#if defined(ARDUINO)
                    if(pSettingsFile.readBytes(fileBuffer, recLen ) == recLen )
#else
                        if( fread( fileBuffer, sizeof(char), recLen, 
                               pSettingsFile ) == recLen )
#endif // defined(ARDUINO)
                        {
// fprintf(stderr, "DATA=%s\n", fileBuffer);
                            switch( atoi(fileBuffer) )
                            {
                                case FIELD_ID_ADMIN_PASSWORD:
                                    dataOffset = 2;
                                    // pSysVars->pAdminPassword
                                    if( pSysVars->pAdminPassword != (char*) NULL )
                                    {
                                        free( pSysVars->pAdminPassword );
                                        pSysVars->pAdminPassword = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pAdminPassword = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pAdminPassword,
						    '\0', payLoadLen + 1 );
                                            strncpy(pSysVars->pAdminPassword, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pAdminPassword = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_NETWORK_MODE:
                                    dataOffset = 2;
                                    // pSysVars->networkMode
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->networkMode = 
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->networkMode = NETWORK_MODE_STA;
                                    }
                                    break;
                                case FIELD_ID_WLAN_TIMEOUT:
                                    dataOffset = 2;
                                    // pSysVars->wlanTimeout
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->wlanTimeout = 
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->wlanTimeout = DEFAULT_WLAN_TIMEOUT;
                                    }
                                    break;
                                case FIELD_ID_WLAN_STATIC_IP_ADDR:
                                    dataOffset = 2;
                                    // pSysVars->pStaticIP
                                    if( pSysVars->pStaticIP != (char*) NULL )
                                    {
                                        free( pSysVars->pStaticIP );
                                        pSysVars->pStaticIP = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pStaticIP = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pStaticIP, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pStaticIP, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pStaticIP = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_WLAN_SSID:
                                    dataOffset = 2;
                                    // pSysVars->pWlanSSID
                                    if( pSysVars->pWlanSSID != (char*) NULL )
                                    {
                                        free( pSysVars->pWlanSSID );
                                        pSysVars->pWlanSSID = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pWlanSSID = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pWlanSSID, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pWlanSSID, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pWlanSSID = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_WLAN_PASSPHRASE:
                                    dataOffset = 2;
                                    // pSysVars->pWlanPassphrase
                                    if( pSysVars->pWlanPassphrase != (char*) NULL )
                                    {
                                        free( pSysVars->pWlanPassphrase );
                                        pSysVars->pWlanPassphrase = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pWlanPassphrase = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pWlanPassphrase, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pWlanPassphrase, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pWlanPassphrase = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_UART_MODE:
                                    dataOffset = 2;
                                    // pSysVars->uartMode
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->uartMode =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->uartMode = UART_MODE_DISABLED;
                                    }
                                    break;
                                case FIELD_ID_UART_API:
                                    dataOffset = 2;
                                    // pSysVars->uartAPI
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( atoi(&fileBuffer[dataOffset]) > 0 )
                                        {
                                            pSysVars->uartAPI = true;
                                        }
                                        else
                                        {
                                            pSysVars->uartAPI = false;
                                        }
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->uartAPI = DEFAULT_HAS_UART_API;
                                    }
                                    break;
                                case FIELD_ID_UART_API_ACCESS:
                                    dataOffset = 3;
                                    // pSysVars->uartAPIAccess
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->uartAPIAccess =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->uartAPIAccess = 
                                                           UART_API_ACCESS_NONE;
                                    }
                                    break;
                                case FIELD_ID_UART_API_USER:
                                    dataOffset = 3;
                                    // pSysVars->pUartApiUser
                                    if( pSysVars->pUartApiUser != (char*) NULL )
                                    {
                                        free( pSysVars->pUartApiUser );
                                        pSysVars->pUartApiUser = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pUartApiUser = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pUartApiUser, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pUartApiUser, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pUartApiUser = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_UART_API_PASSWORD:
                                    dataOffset = 3;
                                    // pSysVars->pUartApiPassword
                                    if( pSysVars->pUartApiPassword != (char*) NULL )
                                    {
                                        free( pSysVars->pUartApiPassword );
                                        pSysVars->pUartApiPassword = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pUartApiPassword = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pUartApiPassword, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pUartApiPassword, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pUartApiPassword = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_UART_API_TIMEOUT:
                                    dataOffset = 3;
                                    // pSysVars->uartApiTimeout
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->uartApiTimeout =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->uartApiTimeout = 
					        DEFAULT_UART_API_TIMEOUT;
                                    }
                                    break;
                                case FIELD_ID_HTTP_MODE:
                                    dataOffset = 3;
                                    // pSysVars->httpMode
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
				         pSysVars->httpMode =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->httpMode = HTTP_MODE_DISABLED;
                                    }
                                    break;
                                case FIELD_ID_HTTP_API:
                                    dataOffset = 3;
                                    // pSysVars->httpAPI
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( atoi(&fileBuffer[dataOffset]) > 0 )
                                        {
                                            pSysVars->httpAPI = true;
                                        }
                                        else
                                        {
                                            pSysVars->httpAPI = false;
                                        }
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->httpAPI = DEFAULT_HAS_HTTP_API;
                                    }
                                    break;
                                case FIELD_ID_HTTP_API_ACCESS:
                                    dataOffset = 3;
                                    // pSysVars->httpAPIAccess
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
				         pSysVars->httpAPIAccess =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->httpAPIAccess = 
					        HTTP_API_ACCESS_NONE;
                                    }
                                    break;
                                case FIELD_ID_HTTP_API_USER:
                                    dataOffset = 3;
                                    // pSysVars->pHttpApiUser
                                    if( pSysVars->pHttpApiUser != (char*) NULL )
                                    {
                                        free( pSysVars->pHttpApiUser );
                                        pSysVars->pHttpApiUser = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pHttpApiUser = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pHttpApiUser, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pHttpApiUser, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pHttpApiUser = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_HTTP_API_PASSWORD:
                                    dataOffset = 3;
                                    // pSysVars->pHttpApiPassword
                                    if( pSysVars->pHttpApiPassword != (char*) NULL )
                                    {
                                        free( pSysVars->pHttpApiPassword );
                                        pSysVars->pHttpApiPassword = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pHttpApiPassword = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pHttpApiPassword, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pHttpApiPassword, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pHttpApiPassword = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_HTTP_API_TIMEOUT:
                                    dataOffset = 3;
                                    // pSysVars->httpApiTimeout
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->httpApiTimeout =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->httpApiTimeout = 
					        DEFAULT_HTTP_API_TIMEOUT;
                                    }
                                    break;
                                case FIELD_ID_TELNET_MODE:
                                    dataOffset = 3;
                                    // pSysVars->telnetMode
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
				         pSysVars->telnetMode =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->telnetMode = TELNET_MODE_DISABLED;
                                    }
                                    break;
                                case FIELD_ID_TELNET_API:
                                    dataOffset = 3;
                                    // pSysVars->telnetAPI
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( atoi(&fileBuffer[dataOffset]) > 0 )
                                        {
                                            pSysVars->telnetAPI = true;
                                        }
                                        else
                                        {
                                            pSysVars->telnetAPI = false;
                                        }
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->telnetAPI = 
					        DEFAULT_HAS_TELNET_API;
                                    }
                                    break;
                                case FIELD_ID_TELNET_API_ACCESS:
                                    dataOffset = 3;
                                    // pSysVars->telnetAPIAccess
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
				         pSysVars->telnetAPIAccess =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->telnetAPIAccess = 
					        TELNET_API_ACCESS_NONE;
                                    }
                                    break;
                                case FIELD_ID_TELNET_API_USER:
                                    dataOffset = 3;
                                    // pSysVars->pTelnetApiUser
                                    if( pSysVars->pTelnetApiUser != (char*) NULL )
                                    {
                                        free( pSysVars->pTelnetApiUser );
                                        pSysVars->pTelnetApiUser = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pTelnetApiUser = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pTelnetApiUser, '\0', 
                                                   payLoadLen + 1 );
                                            strncpy(pSysVars->pTelnetApiUser, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pTelnetApiUser = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_TELNET_API_PASSWORD:
                                    dataOffset = 3;
                                    // pSysVars->pTelnetApiPassword
                                    if(pSysVars->pTelnetApiPassword != (char*) NULL)
                                    {
                                        free( pSysVars->pTelnetApiPassword );
                                        pSysVars->pTelnetApiPassword = NULL;
                                    }
    
                                    if( (payLoadLen = 
                                         strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( (pSysVars->pTelnetApiPassword = (char*)
                                             malloc( payLoadLen + 1 )) != NULL )
                                        {
                                            memset(pSysVars->pTelnetApiPassword, 
						    '\0', payLoadLen + 1 );
                                            strncpy(pSysVars->pTelnetApiPassword, 
                                                    &fileBuffer[dataOffset], 
                                                    payLoadLen);
                                        }
                                        else
                                        {
                                            stopReading = true;
                                            retVal = RETVAL_MALLOC_NULL;
                                        }
                                    }
                                    else
                                    {
                                        pSysVars->pTelnetApiPassword = (char*) NULL;
                                    }
                                    break;
                                case FIELD_ID_TELNET_API_TIMEOUT:
                                    dataOffset = 3;
                                    // pSysVars->telnetApiTimeout
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->telnetApiTimeout =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->telnetApiTimeout = 
					        DEFAULT_TELNET_API_TIMEOUT;
                                    }
                                    break;
                                case FIELD_ID_SEALEVEL_PRESSURE:
                                    dataOffset = 3;
                                    // pSysVars->bme_sealevel_hpa
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->bme_sealevel_hpa =
                                                  atof(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->bme_sealevel_hpa = 
					        SEALEVELPRESSURE_HPA;
                                    }
                                    break;
                                case FIELD_ID_BACKGROUND_COLOR:
                                    dataOffset = 3;
                                    // pSysVars->colorBackground
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->colorBackground =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->colorBackground = BLACK;
                                    }
                                    break;
                                case FIELD_ID_FOREGROUND_COLOR:
                                    dataOffset = 3;
                                    // pSysVars->colorForeground
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->colorForeground =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->colorForeground = WHITE;
                                    }
                                    break;
                                case FIELD_ID_WEEKDAY_FORMAT:
                                    dataOffset = 3;
                                    // pSysVars->weekdayFormat
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->weekdayFormat =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->weekdayFormat = WEEKDAY_DISABLED;
                                    }
                                    break;
                                case FIELD_ID_DATE_FORMAT:
                                    dataOffset = 3;
                                    // pSysVars->dateFormat
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->dateFormat =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->dateFormat = DATE_DISABLED;
                                    }
                                    break;
                                case FIELD_ID_TIME_FORMAT:
                                    dataOffset = 3;
                                    // pSysVars->timeFormat
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->timeFormat =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->timeFormat = TIME_LONG;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_HUMIDITY_INTERVAL:
                                    dataOffset = 3;
                                    // pSysVars->local_bme_humidity_interval
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->local_bme_humidity_interval =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->local_bme_humidity_interval = 
					        BME_HUMIDITY_INTERVAL * 1000;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_TEMPERATURE_INTERVAL:
                                    dataOffset = 3;
                                    // pSysVars->local_bme_temperature_interval
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->local_bme_temperature_interval =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->local_bme_temperature_interval = 
					        BME_TEMPERATURE_INTERVAL * 1000;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_PRESSURE_INTERVAL:
                                    dataOffset = 3;
                                    // pSysVars->local_bme_pressure_interval
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->local_bme_pressure_interval =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->local_bme_pressure_interval = 
					        BME_PRESSURE_INTERVAL * 1000;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_DISPLAY_HUMIDITY:
                                    dataOffset = 3;
                                    // pSysVars->displayHumidity
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->displayHumidity =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->displayHumidity = 
					        INTERNAL_DISPLAY_HUMIDITY_ENABLED;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_HUMIDITY_UPPER_LIMIT:
                                    dataOffset = 3;
                                    // pSysVars->humidityUpperLimit
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->humidityUpperLimit =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->humidityUpperLimit = 
					        INTERNAL_HUMIDITY_UPPER_LIMIT;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_HUMIDITY_LOWER_LIMIT:
                                    dataOffset = 3;
                                    // pSysVars->humidityLowerLimit
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->humidityLowerLimit =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->humidityLowerLimit = 
					        INTERNAL_HUMIDITY_LOWER_LIMIT;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_DISPLAY_TEMPERATURE:
                                    dataOffset = 3;
                                    // pSysVars->displayTemperature
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->displayTemperature =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->displayTemperature = 
				             INTERNAL_DISPLAY_TEMPERATURE_ENABLED;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_TEMPERATURE_UPPER_LIMIT:
                                    dataOffset = 3;
                                    // pSysVars->temperatureUpperLimit
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->temperatureUpperLimit =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->temperatureUpperLimit = 
					        INTERNAL_TEMPERATURE_UPPER_LIMIT;
                                    }
    
                                    break;
                                case FIELD_ID_INTERNAL_TEMPERATURE_LOWER_LIMIT:
                                    dataOffset = 3;
                                    // pSysVars->temperatureLowerLimit
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->temperatureLowerLimit =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->temperatureLowerLimit = 
					        INTERNAL_TEMPERATURE_LOWER_LIMIT;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_DISPLAY_PRESSURE:
                                    dataOffset = 3;
                                    // pSysVars->displayPressure
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->displayPressure =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->displayPressure = 
					        INTERNAL_DISPLAY_PRESSURE_ENABLED;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_PRESSURE_UPPER_LIMIT:
                                    dataOffset = 3;
                                    // pSysVars->pressureUpperLimit
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->pressureUpperLimit =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->pressureUpperLimit = 
					        INTERNAL_PRESSURE_UPPER_LIMIT;
                                    }
                                    break;
                                case FIELD_ID_INTERNAL_PRESSURE_LOWER_LIMIT:
                                    dataOffset = 3;
                                    // pSysVars->pressureLowerLimit
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->pressureLowerLimit =
                                                  atol(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->pressureLowerLimit = 
					        INTERNAL_PRESSURE_LOWER_LIMIT;
                                    }
                                    break;
                                case FIELD_ID_DISPLAY_IP_ADDRESS:
                                    dataOffset = 3;
                                    // pSysVars->displayIPAddress
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( atoi(&fileBuffer[dataOffset]) > 0 )
                                        {
                                            pSysVars->displayIPAddress = true;
                                        }
                                        else
                                        {
                                            pSysVars->displayIPAddress = false;
                                        }
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->displayIPAddress = 
					        DEFAULT_DISPLAY_IP_ADDRESS;
                                    }
                                    break;
                                case FIELD_ID_DISPLAY_MOON_PHASE:
                                    dataOffset = 3;
                                    // pSysVars->displayMoonphase
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        if( atoi(&fileBuffer[dataOffset]) > 0 )
                                        {
                                            pSysVars->displayMoonphase = true;
                                        }
                                        else
                                        {
                                            pSysVars->displayMoonphase = 
						    DEFAULT_DISPLAY_MOON_PHASE;
                                        }
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->displayMoonphase = false;
                                    }
                                    break;
                                case FIELD_ID_DISPLAY_LIPO_MODE:
                                    dataOffset = 3;
                                    // pSysVars->displayLipo
                                    if( (payLoadLen = 
                                          strlen(&fileBuffer[dataOffset])) > 0 )
                                    {
                                        pSysVars->displayLipo =
                                                  atoi(&fileBuffer[dataOffset]);
                                    }
                                    else
                                    {
                                        // set to default
                                        pSysVars->displayLipo = 
					        DISPLAY_LIPO_DISABLED;
				    }
                                    break;
                                default:
                                    dataOffset = 0;
    fprintf(stderr, "oops!\n");
                                    break;
                            }
                        }
                        else
                        {
                            stopReading = true;
                            retVal = RETVAL_FILE_EOF;
                        }
                    }
                    else
                    {
                        stopReading = true;
                        retVal = RETVAL_FILE_EOF;
                    }
    
                } while( !stopReading );

#if defined(ARDUINO)
                pSettingsFile.close();
#else
                fclose(pSettingsFile);
#endif // defined(ARDUINO)
            }
            else
            {
                retVal = RETVAL_OPEN_FILE;
            }
        }
        else
        {
	    retVal = RETVAL_NO_FLASH;
        }
    }
    else
    {
        retVal = RETVAL_NULLP;
    }

    return( retVal );

}


void freeSysVars( struct _system_vars_ *pSysVars )
{
    if( pSysVars != (struct _system_vars_*) NULL )
    {
        if( pSysVars->pAdminPassword != (char*) NULL )
        {
            free(pSysVars->pAdminPassword);
            pSysVars->pAdminPassword = (char*) NULL;
        }

        if( pSysVars->pStaticIP != (char*) NULL )
        {
            free(pSysVars->pStaticIP);
            pSysVars->pStaticIP = (char*) NULL;
        }

        if( pSysVars->pWlanSSID != (char*) NULL )
        {
            free(pSysVars->pWlanSSID);
            pSysVars->pWlanSSID  = (char*) NULL;
        }

        if( pSysVars->pWlanPassphrase != (char*) NULL )
        {
            free(pSysVars->pWlanPassphrase);
            pSysVars->pWlanPassphrase = (char*) NULL;
        }
        //
        if( pSysVars->pUartApiUser != (char*) NULL )
        {
            free(pSysVars->pUartApiUser);
            pSysVars->pUartApiUser = (char*) NULL;
        }

        if( pSysVars->pUartApiPassword != (char*) NULL )
        {
            free(pSysVars->pUartApiPassword);
            pSysVars->pUartApiPassword = (char*) NULL;
        }
        //
        if( pSysVars->pHttpApiUser != (char*) NULL )
        {
            free(pSysVars->pHttpApiUser);
            pSysVars->pHttpApiUser = (char*) NULL;
        }

        if( pSysVars->pHttpApiPassword != (char*) NULL )
        {
            free(pSysVars->pHttpApiPassword);
            pSysVars->pHttpApiPassword = (char*) NULL;
        }
        //
        if( pSysVars->pTelnetApiUser != (char*) NULL )
        {
            free(pSysVars->pTelnetApiUser);
            pSysVars->pTelnetApiUser = (char*) NULL;
        }

        if( pSysVars->pTelnetApiPassword != (char*) NULL )
        {
            free(pSysVars->pTelnetApiPassword);
            pSysVars->pTelnetApiPassword = (char*) NULL;
        }
    }
}




int setAdminPassword( char *pPassword, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pPassword != NULL )
	{
            if( pSysVars->pAdminPassword != (char*) NULL )
            {
                free(pSysVars->pAdminPassword);
	    }
	    if( (pSysVars->pAdminPassword = 
			    (char*) malloc(strlen(pPassword)+1)) != NULL )
	    {
	        strcpy( pSysVars->pAdminPassword, pPassword);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setStaticIP( char *pIPAddr, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pIPAddr != NULL )
	{
            if( pSysVars->pStaticIP != (char*) NULL )
            {
                free(pSysVars->pStaticIP);
	    }
	    if( (pSysVars->pStaticIP = 
			    (char*) malloc(strlen(pIPAddr)+1)) != NULL )
	    {
	        strcpy( pSysVars->pStaticIP, pIPAddr);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setWlanSSID( char *pSSID, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pSSID != NULL )
	{
            if( pSysVars->pWlanSSID != (char*) NULL )
            {
                free(pSysVars->pWlanSSID);
	    }
	    if( (pSysVars->pWlanSSID = 
			    (char*) malloc(strlen(pSSID)+1)) != NULL )
	    {
	        strcpy( pSysVars->pWlanSSID, pSSID);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setWlanPassphrase( char *pPassphrase, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pPassphrase != NULL )
	{
            if( pSysVars->pWlanPassphrase != (char*) NULL )
            {
                free(pSysVars->pWlanPassphrase);
	    }
	    if( (pSysVars->pWlanPassphrase = 
			    (char*) malloc(strlen(pPassphrase)+1)) != NULL )
	    {
	        strcpy( pSysVars->pWlanPassphrase, pPassphrase);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setUartApiUser( char *pUser, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pUser != NULL )
	{
            if( pSysVars->pUartApiUser != (char*) NULL )
            {
                free(pSysVars->pUartApiUser);
	    }
	    if( (pSysVars->pUartApiUser = 
			    (char*) malloc(strlen(pUser)+1)) != NULL )
	    {
	        strcpy( pSysVars->pUartApiUser, pUser);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setUartApiPassword( char *pPassword, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pPassword != NULL )
	{
            if( pSysVars->pUartApiPassword != (char*) NULL )
            {
                free(pSysVars->pUartApiPassword);
	    }
	    if( (pSysVars->pUartApiPassword = 
			    (char*) malloc(strlen(pPassword)+1)) != NULL )
	    {
	        strcpy( pSysVars->pUartApiPassword, pPassword);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setHttpApiUser( char *pUser, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pUser != NULL )
	{
            if( pSysVars->pHttpApiUser != (char*) NULL )
            {
                free(pSysVars->pHttpApiUser);
	    }
	    if( (pSysVars->pHttpApiUser = 
			    (char*) malloc(strlen(pUser)+1)) != NULL )
	    {
	        strcpy( pSysVars->pHttpApiUser, pUser);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setHttpApiPassword( char *pPassword, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pPassword != NULL )
	{
            if( pSysVars->pHttpApiPassword != (char*) NULL )
            {
                free(pSysVars->pHttpApiPassword);
	    }
	    if( (pSysVars->pHttpApiPassword = 
			    (char*) malloc(strlen(pPassword)+1)) != NULL )
	    {
	        strcpy( pSysVars->pHttpApiPassword, pPassword);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setTelnetApiUser( char *pUser, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pUser != NULL )
	{
            if( pSysVars->pTelnetApiUser != (char*) NULL )
            {
                free(pSysVars->pTelnetApiUser);
	    }
	    if( (pSysVars->pTelnetApiUser = 
			    (char*) malloc(strlen(pUser)+1)) != NULL )
	    {
	        strcpy( pSysVars->pTelnetApiUser, pUser);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}

int setTelnetApiPassword( char *pPassword, struct _system_vars_ *pSysVars )
{
    int retVal = RETVAL_OK;
    if( pSysVars != (struct _system_vars_*) NULL )
    {
	if( pPassword != NULL )
	{
            if( pSysVars->pTelnetApiPassword != (char*) NULL )
            {
                free(pSysVars->pTelnetApiPassword);
	    }
	    if( (pSysVars->pTelnetApiPassword = 
			    (char*) malloc(strlen(pPassword)+1)) != NULL )
	    {
	        strcpy( pSysVars->pTelnetApiPassword, pPassword);
	    }
	    else
	    {
		retVal = RETVAL_NULLP;
	    }
        }
	else
	{
	    retVal = RETVAL_NULLP;
	}
    }
    else
    {
        retVal = RETVAL_NULLP;
    }
    return(retVal);
}


// ---------------------------------------------------------------------
// void showSysVars( struct _system_vars_ *pSysVars )
// ------------
//  arguments: -
// ------------
//  purpose  : dump env vars
// ------------
//  returns  : nothing
// ---------------------------------------------------------------------
void showSysVars( struct _system_vars_ *pSysVars )
{

    if( pSysVars != (struct _system_vars_ *) NULL )
    {
#if defined(ARDUINO)
        Serial.println();
        Serial.println("ENV dump:");
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_serial .: ");
        Serial.println( pSysVars->valid_serial ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_display : ");
        Serial.println( pSysVars->valid_display ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_bme280 .: ");
        Serial.println( pSysVars->valid_bme280 ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_wlan ...: ");
        Serial.println( pSysVars->valid_wlan ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_http ...: ");
        Serial.println( pSysVars->valid_http ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_telnet .: ");
        Serial.println( pSysVars->valid_telnet ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_NTP ....: ");
        Serial.println( pSysVars->valid_NTP ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->valid_FLASH ..: ");
        Serial.println( pSysVars->valid_FLASH ? "true" : "false" );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->bme_sealevel_hpa ..............: ");
        Serial.println(pSysVars->bme_sealevel_hpa);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->local_bme_temperature_interval : ");
        Serial.println(pSysVars->local_bme_temperature_interval);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->local_bme_humidity_interval ...: ");
        Serial.println(pSysVars->local_bme_humidity_interval);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->local_bme_pressure_interval ...: ");
        Serial.println(pSysVars->local_bme_pressure_interval);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->http_timeoutTime ..............: ");
        Serial.println(pSysVars->http_timeoutTime);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->telnet_timeoutTime ............: ");
        Serial.println(pSysVars->telnet_timeoutTime);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->wlan_timeoutTime ..............: ");
        Serial.println(pSysVars->wlan_timeoutTime);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pAdminPassword ................: ");
        Serial.println( pSysVars->pAdminPassword == NULL ? "NULL" : pSysVars->pAdminPassword );
        
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->networkMode ...................: ");
        Serial.println(pSysVars->networkMode);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->wlanTimeout ...................: ");
        Serial.println(pSysVars->wlanTimeout);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pStaticIP .....................: ");
        Serial.println( pSysVars->pStaticIP == NULL ? "NULL" : pSysVars->pStaticIP );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pWlanSSID .....................: ");
        Serial.println( pSysVars->pWlanSSID == NULL ? "NULL" : pSysVars->pWlanSSID );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pWlanPassphrase ...............: ");
        Serial.println( pSysVars->pWlanPassphrase == NULL ? "NULL" : pSysVars->pWlanPassphrase );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->uartMode ......................: ");
        Serial.println(pSysVars->uartMode);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->uartAPI .......................: ");
        Serial.println(pSysVars->uartAPI);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->uartAPIAccess .................: ");
        Serial.println(pSysVars->uartAPIAccess);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pUartApiUser ..................: ");
        Serial.println( pSysVars->pUartApiUser == NULL ? "NULL" : pSysVars->pUartApiUser );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pUartApiPassword ..............: ");
        Serial.println( pSysVars->pUartApiPassword == NULL ? "NULL" : pSysVars->pUartApiPassword );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->uartApiTimeout ................: ");
        Serial.println(pSysVars->uartApiTimeout);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->httpMode ......................: ");
        Serial.println(pSysVars->httpMode);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->httpAPI .......................: ");
        Serial.println(pSysVars->httpAPI);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->httpAPIAccess .................: ");
        Serial.println(pSysVars->httpAPIAccess);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pHttpApiUser ..................: ");
        Serial.println( pSysVars->pHttpApiUser == NULL ? "NULL" : pSysVars->pHttpApiUser );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pHttpApiPassword ..............: ");
        Serial.println( pSysVars->pHttpApiPassword == NULL ? "NULL" : pSysVars->pHttpApiPassword);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->httpApiTimeout ................: ");
        Serial.println(pSysVars->httpApiTimeout);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->telnetMode ....................: ");
        Serial.println(pSysVars->telnetMode);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->telnetAPI .....................: ");
        Serial.println(pSysVars->telnetAPI);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->telnetAPIAccess ...............: ");
        Serial.println(pSysVars->telnetAPIAccess);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pTelnetApiUser ................: ");
        Serial.println(pSysVars->pTelnetApiUser == NULL ? "NULL" : pSysVars->pTelnetApiUser );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pTelnetApiPassword ............: ");
        Serial.println(pSysVars->pTelnetApiPassword == NULL ? "NULL" : pSysVars->pTelnetApiPassword );
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->telnetApiTimeout ..............: ");
        Serial.println(pSysVars->telnetApiTimeout);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->colorBackground ...............: ");
        Serial.println(pSysVars->colorBackground);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->colorForeground ...............: ");
        Serial.println(pSysVars->colorForeground);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->weekdayFormat .................: ");
        Serial.println(pSysVars->weekdayFormat);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->dateFormat ....................: ");
        Serial.println(pSysVars->dateFormat);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->timeFormat ....................: ");
        Serial.println(pSysVars->timeFormat);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->displayHumidity ...............: ");
        Serial.println(pSysVars->displayHumidity);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->displayTemperature ............: ");
        Serial.println(pSysVars->displayTemperature);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->displayPressure ...............: ");
        Serial.println(pSysVars->displayPressure);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->displayMoonphase ..............: ");
        Serial.println(pSysVars->displayMoonphase);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->displayLipo ...................: ");
        Serial.println(pSysVars->displayLipo);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->displayIPAddress ..............: ");
        Serial.println(pSysVars->displayIPAddress);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->humidityUpperLimit ............: ");
        Serial.println(pSysVars->humidityUpperLimit);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->humidityLowerLimit ............: ");
        Serial.println(pSysVars->humidityLowerLimit);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->temperatureUpperLimit .........: ");
        Serial.println(pSysVars->temperatureUpperLimit);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->temperatureLowerLimit .........: ");
        Serial.println(pSysVars->temperatureLowerLimit);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pressureUpperLimit ............: ");
        Serial.println(pSysVars->pressureUpperLimit);
#else
#endif // defined(ARDUINO)

#if defined(ARDUINO)
        Serial.print("pSysVars->pressureLowerLimit ............: ");
        Serial.println(pSysVars->pressureLowerLimit);
#else
#endif // defined(ARDUINO)

    }
}

#if !defined(ARDUINO)
//
// *****************************************************************
//

int main( int argc, char *argv[] )
{
    int retVal = 0;

    setupDefaults() ;
//    writeSysVars( &_envVars );

    readSysVars( &_envVars );

 setAdminPassword( (char*) "AdminPassword", &_envVars );
 setStaticIP( (char*) "staticIP", &_envVars );
 setWlanSSID( (char*) "SSID", &_envVars );
 setWlanPassphrase( (char*) "Passphrase", &_envVars );
 setUartApiUser( (char*) "ApiUser", &_envVars );
 setUartApiPassword( (char*) "ApiPassword", &_envVars );
 setHttpApiUser( (char*) "ApiUser", &_envVars );
 setHttpApiPassword( (char*) "ApiPassword", &_envVars );
 setTelnetApiUser( (char*) "ApiUser", &_envVars );
 setTelnetApiPassword( (char*) "ApiPassword", &_envVars );

    writeSysVars( &_envVars );

    freeSysVars( &_envVars );

    return( retVal );
}

#endif // !defined(ARDUINO)


// #ifdef __cplusplus
// }
// #endif
