#ifndef _SYSVARS_H_
#define _SYSVARS_H_

#define NULL                  0L

#define FIELD_DATA_LEN          3
#define FIELD_DELIMITER_LEN     1
#define FILE_DATA_LEN          (FIELD_DATA_LEN + FIELD_DELIMITER_LEN)

#define DEFAULT_UART_API_TIMEOUT         0L
#define DEFAULT_HTTP_API_TIMEOUT         0L
#define DEFAULT_TELNET_API_TIMEOUT       0L
#define DEFAULT_WLAN_TIMEOUT             0L

#define HTTP_TIMEOUT_MILLIS      2000
#define TELNET_TIMEOUT_MILLIS    2000
#define WIFI_TIMEOUT_MILLIS     30000

//
// *****************************************************************
//

#define RETVAL_OPEN_FILE  -2
#define RETVAL_NULLP    -1
#define RETVAL_OK        0
#define FILE_BUFFER_MAX  128
#define RETVAL_FILE_EOF   -3
#define RETVAL_MALLOC_NULL   -4
#define RETVAL_NO_FLASH     -5

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

#define DEFAULT_HAS_UART_API      false
#define DEFAULT_HAS_HTTP_API      false
#define DEFAULT_HAS_TELNET_API      false
#define DEFAULT_DISPLAY_MOON_PHASE      false
#define DEFAULT_DISPLAY_IP_ADDRESS      false


#define VALUE_TYPE_FLOAT           'f'
#define VALUE_TYPE_LONG            'l'

struct _sensor_value_ {
    bool is_valid;
    bool has_changed;
    char value_type;
    unsigned long long_data;
    float float_data;
};

typedef unsigned char byte;
typedef unsigned short uint16_t;


#define SEALEVELPRESSURE_HPA (1013.25)

#define TIME_MINUTES_TO_SECONDS(M)  (M*60)
#define TIME_HOURS_TO_SECONDS(H)  (H*60*60)
#define TIME_DAYS_TO_SECONDS(D)   (D*24*60*60)


#define BME_TEMPERATURE_INTERVAL  TIME_MINUTES_TO_SECONDS(2)
#define BME_HUMIDITY_INTERVAL     TIME_MINUTES_TO_SECONDS(3)
#define BME_PRESSURE_INTERVAL     TIME_MINUTES_TO_SECONDS(4)

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

// #ifdef __cplusplus
// extern “C” {
// #endif

void setupDefaults(void);
int writeSysVars( struct _system_vars_ *pSysVars );
int readSysVars( struct _system_vars_ *pSysVars );
void freeSysVars( struct _system_vars_ *pSysVars );

int setAdminPassword( char *pPassword, struct _system_vars_ *pSysVars );
int setStaticIP( char *pIPAddr, struct _system_vars_ *pSysVars );
int setWlanSSID( char *pSSID, struct _system_vars_ *pSysVars );
int setWlanPassphrase( char *pPassphrase, struct _system_vars_ *pSysVars );
int setUartApiUser( char *pUser, struct _system_vars_ *pSysVars );
int setUartApiPassword( char *pPassword, struct _system_vars_ *pSysVars );
int setHttpApiUser( char *pUser, struct _system_vars_ *pSysVars );
int setHttpApiPassword( char *pPassword, struct _system_vars_ *pSysVars );
int setTelnetApiUser( char *pUser, struct _system_vars_ *pSysVars );
int setTelnetApiPassword( char *pPassword, struct _system_vars_ *pSysVars );
void showSysVars( struct _system_vars_ *pSysVars );

// #ifdef __cplusplus
// }
// #endif


#endif // _SYSVARS_H_
