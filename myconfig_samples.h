/*************************************************/
/* Debugging                                     */
/*************************************************/
const bool debugOutput = true;  // set to true for serial OUTPUT

/*************************************************/
/* Settings for WLAN                             */
/*************************************************/
const char* ssid = "SSID";
const char* password = "ergerg123erg";

/*************************************************/
/* Settings for number of reed contacts in       */
/* abenometer                                    */
/*************************************************/
const float number_reed = 4;

/*************************************************/
/* Update settings                               */
/*************************************************/ 
const char* firmware_version = "myanemometer_0.0.1";
const char* update_server = "myhost";
const char* update_uri = "/path/update.php";

/*************************************************/
/* MQTTCloud data                               */
/*************************************************/
const char* mqtt_host = "mqtthost";
const char* mqtt_id = "ESP8266-Anemometer";
const char* mqtt_topic_prefix = "anemometer/wind";
const int mqtt_port = 1883;

