#include <Arduino.h>

#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <WiFiMulti.h>

using namespace websockets;

// WiFi Config
WiFiMulti wifiMulti;
const char* ssid1 = "Your WiFi Name";
const char* password1 = "Your WiFi Password";


// Websockets Config
const char* host = "Your WiFi localIP";
const uint16_t port = 3001; //change this 

// Pins
const byte p_l_0=32; // p_name, p->pin
const byte p_l_1=33;
const byte p_r_0=25;
const byte p_r_1=26;
const byte p_l_pwm=27;
const byte p_r_pwm=14;
const byte p_green_light=17;
const byte p_yellow_light=16;

// variables
int8_t l_motor, r_motor; // status unsigned
int8_t angle, rpm; //status signed


WebsocketsClient client;

void onMessageCallback(WebsocketsMessage msg){
  Serial.print("get message: ");
  String str = msg.data();
  Serial.println(str);
  if(str.substring(0,3)=="dc:"){
    uint8_t len = str.length(), idx = 0;
    bool inp=true;
    String tmp = "";
    for(uint8_t i = 3 ; i < len ; i++){
      if(str[i] == ','){
        if(idx == 0){
          l_motor=tmp.toInt();
        }
        else if(idx == 1){
          r_motor=tmp.toInt();
        }
        else if(idx == 2){
          angle=tmp.toInt();
        }
        else if(idx == 3){
          rpm=tmp.toInt();
        }
        else{
          Serial.println("decompose somehow get idx err");
        }
        idx++;
        tmp = "";
        continue;
      }
      tmp+=str[i];
      Serial.print("tmp:");
      Serial.println(tmp);
    }
    Serial.printf("l: %d r: %d angle: %d rpm: %d",l_motor,r_motor,angle,rpm);
    // L
    if(l_motor == 0){
      digitalWrite(p_l_0,LOW);
      digitalWrite(p_l_1,LOW);
    }
    else if(l_motor < 0){
      digitalWrite(p_l_0,LOW);
      digitalWrite(p_l_1,HIGH);
    }
    else{
      digitalWrite(p_l_0,HIGH);
      digitalWrite(p_l_1,LOW);
    }
    // R
    if(r_motor == 0){
      digitalWrite(p_r_0,LOW);
      digitalWrite(p_r_1,LOW);
    }
    else if(r_motor < 0){
      digitalWrite(p_r_0,LOW);
      digitalWrite(p_r_1,HIGH);
    }
    else{
      digitalWrite(p_r_0,HIGH);
      digitalWrite(p_r_1,LOW);
    }
  }
}
void onEventsCallback(WebsocketsEvent event, String data){
  if(event == WebsocketsEvent::ConnectionOpened){
    Serial.println("Connection opened");
  }
  else if(event == WebsocketsEvent::ConnectionClosed){
    Serial.println("Connection closed");
  }
  else{
    Serial.println("Get event haven't be set with callback");
  }
}

// not yet done
void onReturn(){
  String ret = String(l_motor)+','+String(r_motor)+','+String(angle)+','+String(rpm);
  client.send(ret);
}

void setup(){
  Serial.begin(115200);
  delay(500);
  // Wifi Connection
  wifiMulti.addAP(ssid1, password1);
  WiFi.setSleep(false);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi OK");
  Serial.println(WiFi.localIP());
  // WebClient
  bool connected = client.connect(host, port, "/");
  if(!connected){
    Serial.println("WebSocket connect failed");
  }
  else{
    client.send("esp32 connected");
    Serial.println("Websocket ok");
  }
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);

  // Pin Setting
  pinMode(p_l_0,OUTPUT);
  pinMode(p_l_1,OUTPUT);
  pinMode(p_r_0,OUTPUT);
  pinMode(p_r_1,OUTPUT);
  pinMode(p_l_pwm,OUTPUT);
  pinMode(p_r_pwm,OUTPUT);
  pinMode(p_green_light,OUTPUT);
  pinMode(p_yellow_light,OUTPUT);
}

void loop(){
  client.poll();
}