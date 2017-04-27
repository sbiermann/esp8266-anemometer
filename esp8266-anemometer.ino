
#include <ESP8266WiFi.h>
#include "myconfig.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

unsigned long  next_timestamp = 0;
volatile unsigned long i = 0;
float wind = 0;
volatile unsigned long last_micros;
long debouncing_time = 5; //in millis
int input_pin = 13;

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(input_pin, INPUT_PULLUP);//D3
  // We start by connecting to a WiFi network
  if(debugOutput){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(debugOutput) Serial.print(".");
  }
  if(debugOutput){
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  delay(500);
   //do_update();
  
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
    float rps = i/number_reed; //computing rounds per second 
    if(i == 0)
      wind = 0.0;
    else
      wind = 1.761 / (1 + rps) + 3.013 * rps;// found here: https://www.amazon.de/gp/customer-reviews/R3C68WVOLJ7ZTO/ref=cm_cr_getr_d_rvw_ttl?ie=UTF8&ASIN=B0018LBFG8 (in German)
    if(debugOutput){
      Serial.print("Wind: ");
      Serial.print(wind);
      Serial.println(" km/h");
    }
    i = 0;
    next_timestamp  = millis()+1000; //intervall is 1s
    attachInterrupt(input_pin,Interrupt,RISING);
  }
}

void do_update(){
  Serial.println("do update");
  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 80, update_uri, firmware_version);
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] no Update needed");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
