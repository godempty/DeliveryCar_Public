#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoWebsockets.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems
#include "driver/gpio.h"
// pins definition for AI thinker board
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
// WiFi Config
WiFiMulti wifiMulti;
const char* ssid1 = "Your WiFi Name";
const char* password1 = "Your WiFi Password";
// Websockets Config
const char* host = "Your WiFi localIP";
const uint16_t port = 3001; //change this 
// file buffer
camera_fb_t* fb = NULL;
size_t _jpg_buf_len = 0;
uint8_t * _jpg_buf = NULL;
uint8_t state = 0;

using namespace websockets;
WebsocketsClient client;

void onMessageCallback(WebsocketsMessage  message){
  Serial.print("Got Message: ");
  Serial.println(message.data());
}
void onEventsCallback(WebsocketsEvent event, String data){
  if(event == WebsocketsEvent::ConnectionOpened){
    Serial.println("Connection opened");
    client.send("Cam connected");
  }
  else if(event == WebsocketsEvent::ConnectionClosed){
    Serial.println("Connection closed");
  }
  else{
    Serial.println("Get event haven't be set with callback");
  }
}

esp_err_t init_cam() {
  // Pin Settings
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  // configuration (set according to "https://www.uctronics.com/download/cam_module/OV2640DS.pdf" and "https://swf.com.tw/?p=1741")
  config.xclk_freq_hz = 20000000;  // 20M(maximum)
  config.frame_size = FRAMESIZE_HVGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 32;
  config.fb_count = 2;
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return err;
  }
  sensor_t* s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_HVGA);
  Serial.println("camera init done");
  return ESP_OK;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout problem
  // Wifi Connection
  wifiMulti.addAP(ssid1, password1);
  WiFi.setSleep(false);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi OK");
  Serial.println(WiFi.localIP());
  esp_err_t cam_state = init_cam();
  if (cam_state != ESP_OK) {
    Serial.println("camera err");
    Serial.println(cam_state);
  }
  // WebSocket Connection
  client.onMessage(onMessageCallback);
  bool connected = client.connect(host, port, "/");
  if(!connected){
    Serial.println("WebSocket connect failed");
  }
}
void loop() {
  if(client.available()){
    fb = esp_camera_fb_get();
    if(!fb){
      Serial.println("Get Picture Fail");
      esp_camera_fb_return(fb);
      ESP.restart();
    }
    client.sendBinary((const char*) fb->buf, fb->len);
    Serial.println("Picture sent");
    esp_camera_fb_return(fb);
    client.poll();
    delay(33);                 //fps 30
  }
}