#include <ESP8266WiFi.h>
#include "myconfig.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <PubSubClient.h>


unsigned long  next_timestamp = 0;
volatile unsigned long i = 0;
float wind = 0;
float last_wind = 0;
int count = 0;
volatile unsigned long last_micros;
long debouncing_time = 5; //in millis
int input_pin = 13;
char charBuffer[32];

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(input_pin, INPUT_PULLUP);//D7
  // We start by connecting to a WiFi network
  if(debugOutput){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
  WiFi.begin(ssid, password);
  int maxWait = 500;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(debugOutput) Serial.print(".");
    if(maxWait <= 0)
     ESP.restart();
    maxWait--;
  }
  if(debugOutput){
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  delay(500);
  do_update();
  client.setServer(mqtt_host, mqtt_port);
  reconnect();
  attachInterrupt(input_pin,Interrupt,RISING);
}



void Interrupt()
{
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    i++;
    last_micros = micros();
  }
}

void loop() 
{
  if (millis() > next_timestamp )    
  { 
    detachInterrupt(input_pin);
    count++; 
    float rps = i/number_reed; //computing rounds per second 
    if(i == 0)
      wind = 0.0;
    else
      wind = 1.761 / (1 + rps) + 3.013 * rps;// found here: https://www.amazon.de/gp/customer-reviews/R3C68WVOLJ7ZTO/ref=cm_cr_getr_d_rvw_ttl?ie=UTF8&ASIN=B0018LBFG8 (in German)
    if(last_wind - wind > 1 || wind - last_wind < -1 || count >= 10){
      if(debugOutput){
        Serial.print("Wind: ");
        Serial.print(wind);
        Serial.println(" km/h");
      }
      String strBuffer;
      strBuffer =  String(wind);
      strBuffer.toCharArray(charBuffer,10);
      if (!client.publish(mqtt_topic_prefix, charBuffer, false))
      {
        ESP.restart();
        delay(100);
      }
      count = 0;
    }
    i = 0;
    last_wind = wind;
    if(WiFi.status() != WL_CONNECTED) {
      ESP.restart();
      delay(100);
    }
    next_timestamp  = millis()+1000; //intervall is 1s
    attachInterrupt(input_pin,Interrupt,RISING);
  }
}

void reconnect() {
  while (!client.connected()) {
    if(debugOutput) Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_id)) {
      if(debugOutput) Serial.println("connected");
    } else {
      if(debugOutput){ 
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
      delay(5000);
    }
  }
}

void do_update(){
  if(debugOutput) Serial.println("do update");
  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 80, update_uri, firmware_version);
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        if(debugOutput) Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        if(debugOutput )Serial.println("[update] no Update needed");
        break;
    case HTTP_UPDATE_OK:
        if(debugOutput) Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
