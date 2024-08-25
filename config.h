/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "Juliorex23"
#define IO_KEY     

/******************************* WIFI **************************************/



#define WIFI_SSID "Julio"
#define WIFI_PASS "juliorex2310"


#include "AdafruitIO_WiFi.h"


AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

