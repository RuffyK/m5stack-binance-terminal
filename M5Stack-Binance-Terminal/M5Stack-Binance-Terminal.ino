#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "WiFiApi.h"

#include <WebSocketsClient.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#include <RingBuf.h>

#define USE_SERIAL Serial

#define CHART_WIDTH 300
#define CHART_HEIGHT 190
#define CHART_TOP 40
#define CHART_LEFT 0
#define SUB_CHART_HEIGHT 40
#define CANDLE_WIDTH 3
#define MOVING_AVG_MAX 100
#define CHART_POINTS (int(CHART_WIDTH / CANDLE_WIDTH) + MOVING_AVG_MAX)
#define KLINE_BUF_SIZE (CHART_POINTS * 290)

#define COLOR_RED 0xF22B
#define COLOR_GREEN 0x0E50

#define BOLL_MVA_LENGTH 21
#define BOLL_MULTIPLIER 2.2
#define BOLL_UP_COLOR 0x77FF
#define BOLL_DOWN_COLOR 0x77FF
#define BOLL_FILL_COLOR 0x10A5
#define BOLL_MVA_COLOR 0x0E50

#define EMA_1_LENGTH 50
#define EMA_1_COLOR 0xB5A8

#define EMA_2_LENGTH 100
#define EMA_2_COLOR 0xAB85

#define SYMBOL "btcusdt"

const char WiFiAPPSK[] = "ruffycrabby";
const char APNAME[] = "M5Stack-1";
AsyncWebServer server(80);

WebSocketsClient webSocket;
DynamicJsonDocument webSocketBuf(512);
DynamicJsonDocument klineBuf(KLINE_BUF_SIZE);

HTTPClient http;

float chart_price_max = 0;
float chart_price_min = 0;

struct chartDataPoint {
  unsigned long long openTime;
  float openPrice;
  float highPrice;
  float lowPrice;
  float closePrice;
  float volume;
  unsigned long long closeTime;
};

RingBuf<chartDataPoint, CHART_POINTS> chartBuffer;
TFT_eSprite chartSprite = TFT_eSprite(&M5.Lcd);

void drawChart() { 
  
  float local_min = 1000000000000;
  float local_max = 0;
  //float volume_min = 1000000000000;
  float volume_max = -1;
  int sub_charts = 0;
  uint16_t backColor = BLACK;

  bool draw_bollinger = true;
  bool draw_volume = true;
  bool draw_ema1 = true;
  bool draw_ema2 = true;
  
  int bollinger_mva_length = BOLL_MVA_LENGTH;
  float bollinger_multiplier = BOLL_MULTIPLIER;
  float bollinger_mva[chartBuffer.size() - MOVING_AVG_MAX];
  float bollinger_dev[chartBuffer.size() - MOVING_AVG_MAX];

  int ema_1_length = EMA_1_LENGTH;
  float ema_1[chartBuffer.size() - MOVING_AVG_MAX];

  int ema_2_length = EMA_2_LENGTH;
  float ema_2[chartBuffer.size() - MOVING_AVG_MAX];

  if(draw_bollinger) {
    for(int i = MOVING_AVG_MAX; i < chartBuffer.size(); i++) {
      float boll_mva = 0;
      for(int mva_i = 0; mva_i <  bollinger_mva_length; mva_i++){
        boll_mva += chartBuffer[mva_i + i - bollinger_mva_length].closePrice;
      }
      boll_mva = boll_mva / bollinger_mva_length;
  
      float stddev = 0;
      for(int mva_i = 0; mva_i <  bollinger_mva_length; mva_i++){
        stddev += sqrt(pow(chartBuffer[mva_i + i - bollinger_mva_length].closePrice - boll_mva, 2));
      }
      stddev = stddev / (bollinger_mva_length-1);
      bollinger_mva[i - MOVING_AVG_MAX] = boll_mva;
      bollinger_dev[i - MOVING_AVG_MAX] = stddev;
    }
  }

  if(draw_volume) {
    sub_charts += 1;
  }

  if(draw_ema1) {
    int ema_length = ema_1_length;
    
    float k = 2.0f / (ema_length + 1);
    float ema = 0;
    for(int i = 0; i < ema_length; i++) {
      ema += chartBuffer[MOVING_AVG_MAX - i - 1].closePrice;
    }
    ema = ema / ema_length;
    for(int i = MOVING_AVG_MAX; i < chartBuffer.size(); i++) {
      int ema_i = i - MOVING_AVG_MAX;
      if(ema_i > 0) {
        ema = (k * (chartBuffer[i].closePrice - ema)) + ema;
      }

      ema_1[ema_i] = ema;
    }
  }

  if(draw_ema2) {
    int ema_length = ema_2_length;
    
    float k = 2.0f / (ema_length + 1);
    float ema = 0;
    for(int i = 0; i < ema_length; i++) {
      ema += chartBuffer[MOVING_AVG_MAX - i - 1].closePrice;
    }
    ema = ema / ema_length;
    for(int i = MOVING_AVG_MAX; i < chartBuffer.size(); i++) {
      int ema_i = i - MOVING_AVG_MAX;
      if(ema_i > 0) {
        ema = (k * (chartBuffer[i].closePrice - ema)) + ema;
      }

      ema_2[ema_i] = ema;
    }
  }
  // Start at MOVING_AVG_MAX, as all previous data points
  // are only needed for moving avg calculation and will
  // not be drawn on the chart
  for(int i = MOVING_AVG_MAX; i< chartBuffer.size(); i++) {
    chartDataPoint point = chartBuffer[i];
    if(point.highPrice > local_max) {
      local_max = point.highPrice;
    }
    if(point.lowPrice < local_min) {
      local_min = point.lowPrice;
    }
    
    if(draw_bollinger){
      float boll_mva = bollinger_mva[i - MOVING_AVG_MAX];
      float boll_dev = bollinger_dev[i - MOVING_AVG_MAX];
      if(boll_mva + (boll_dev * bollinger_multiplier) > local_max) {
        local_max = boll_mva + (boll_dev * bollinger_multiplier);
      }
      if(boll_mva - (boll_dev * bollinger_multiplier) < local_min) {
        local_min = boll_mva - (boll_dev * bollinger_multiplier);
      }
    }

    if(draw_volume) {
      if(point.volume > volume_max){
        volume_max = point.volume;
      }
      /*if(point.volume < volume_min) {
        volume_min = point.volume;
      }*/
    }

    if(draw_ema1) {
      if(ema_1[i - MOVING_AVG_MAX] > local_max) {
        local_max = ema_1[i - MOVING_AVG_MAX];
      }
      if(ema_1[i - MOVING_AVG_MAX] < local_min){
        local_min = ema_1[i - MOVING_AVG_MAX];
      }
    }

    if(draw_ema2) {
      if(ema_2[i - MOVING_AVG_MAX] > local_max) {
        local_max = ema_2[i - MOVING_AVG_MAX];
      }
      if(ema_2[i - MOVING_AVG_MAX] < local_min){
        local_min = ema_2[i - MOVING_AVG_MAX];
      }
    }
    
  }
  chart_price_max = local_max;
  chart_price_min = local_min;

  int sub_chart_height = SUB_CHART_HEIGHT;
  int main_chart_height = CHART_HEIGHT - (sub_charts * sub_chart_height);
  
  const float price_scale = main_chart_height / (chart_price_max - chart_price_min);
  const float volume_scale = sub_chart_height / volume_max;

  for(int i = MOVING_AVG_MAX; i< chartBuffer.size(); i++) {
    chartSprite.fillSprite(backColor);
    
    int index = i - MOVING_AVG_MAX;
    chartDataPoint candle = chartBuffer[i];
    bool isLoss = candle.closePrice < candle.openPrice;
   
    
    /*********************************/
    /*   Bollinger Bands             */
    /*********************************/

    if(draw_bollinger && index < chartBuffer.size() - MOVING_AVG_MAX - 1) {
      int boll_prev_mva_point = main_chart_height - int((bollinger_mva[index] - chart_price_min) * price_scale);
      int boll_curr_mva_point = main_chart_height - int((bollinger_mva[index+1] - chart_price_min) * price_scale);
      int boll_prev_up_point = main_chart_height - int((bollinger_mva[index] + (bollinger_dev[index] * bollinger_multiplier) - chart_price_min) * price_scale);
      int boll_curr_up_point = main_chart_height - int((bollinger_mva[index+1] + (bollinger_dev[index+1] * bollinger_multiplier) - chart_price_min) * price_scale);
      int boll_prev_down_point = main_chart_height - int((bollinger_mva[index] - (bollinger_dev[index] * bollinger_multiplier) - chart_price_min) * price_scale);
      int boll_curr_down_point = main_chart_height - int((bollinger_mva[index+1] - (bollinger_dev[index+1] * bollinger_multiplier) - chart_price_min) * price_scale);

      // Bollinger Bands filled area
      chartSprite.fillRect(0, max(boll_prev_up_point, boll_curr_up_point), CANDLE_WIDTH, min(boll_prev_down_point, boll_curr_down_point) - max(boll_prev_up_point, boll_curr_up_point), BOLL_FILL_COLOR);

      // Bollinger Lines
      chartSprite.drawLine(0, boll_prev_mva_point, CANDLE_WIDTH, boll_curr_mva_point, BOLL_MVA_COLOR);
      chartSprite.drawLine(0, boll_prev_up_point, CANDLE_WIDTH, boll_curr_up_point, BOLL_UP_COLOR);
      chartSprite.drawLine(0, boll_prev_down_point, CANDLE_WIDTH, boll_curr_down_point, BOLL_DOWN_COLOR);
    }

    /*********************************/
    /*   EMA                         */
    /*********************************/

    if(draw_ema1 && index < chartBuffer.size() - MOVING_AVG_MAX - 1) {
      int ema_point = main_chart_height - int((ema_1[index] - chart_price_min) * price_scale);
      int next_ema_point = main_chart_height - int((ema_1[index+1] - chart_price_min) * price_scale);
      chartSprite.drawLine(0, ema_point, CANDLE_WIDTH, next_ema_point, EMA_1_COLOR);
    }
    
    if(draw_ema2 && index < chartBuffer.size() - MOVING_AVG_MAX - 1) {
      int ema_point = main_chart_height - int((ema_2[index] - chart_price_min) * price_scale);
      int next_ema_point = main_chart_height - int((ema_2[index+1] - chart_price_min) * price_scale);
      chartSprite.drawLine(0, ema_point, CANDLE_WIDTH, next_ema_point, EMA_2_COLOR);
    }

    
    /*********************************/
    /*   Candles                     */
    /*********************************/
    uint16_t color = COLOR_GREEN;
    if(isLoss) {
      color = COLOR_RED;
    }
    int highPoint = main_chart_height - int((candle.highPrice - chart_price_min) * price_scale);
    int lowPoint = main_chart_height - int((candle.lowPrice - chart_price_min) * price_scale);
    int openPoint = main_chart_height - int((candle.openPrice - chart_price_min) * price_scale);
    int closePoint = main_chart_height - int((candle.closePrice - chart_price_min) * price_scale);
    int highLowLineX = int(CANDLE_WIDTH / 2);
    chartSprite.drawLine(highLowLineX, lowPoint, highLowLineX, highPoint, color);

    if(isLoss) {
      chartSprite.fillRect(0, openPoint, CANDLE_WIDTH, closePoint - openPoint, color);
    }
    else {
      chartSprite.fillRect(0, closePoint, CANDLE_WIDTH, openPoint - closePoint, color);
    }
    
    /*********************************/
    /*   Volume subplot              */
    /*********************************/
    
    if(draw_volume) {
      int volume_scaled = int(candle.volume * volume_scale);
      chartSprite.fillRect(0,  main_chart_height + sub_chart_height - volume_scaled, CANDLE_WIDTH, volume_scaled, color);
    }
    
    chartSprite.pushSprite(CHART_LEFT + (index * CANDLE_WIDTH), CHART_TOP);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED: {
            USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
          }
          break;
        case WStype_TEXT: {
            //USE_SERIAL.printf("[WSc] get text: %s\n", payload);
            deserializeJson(webSocketBuf, payload);
            JsonObject response = webSocketBuf.as<JsonObject>();
            
            M5.Lcd.setTextColor(WHITE, BLACK);
            M5.Lcd.setCursor(0,12);
            M5.Lcd.setTextSize(3);
            M5.Lcd.print(response["s"].as<String>());
            float openPrice = response["k"]["o"].as<float>();
            float closePrice = response["k"]["c"].as<float>();
            float highPrice = response["k"]["h"].as<float>();
            float lowPrice = response["k"]["l"].as<float>();
            float volume = response["k"]["v"].as<float>();
            unsigned long long openTime = response["k"]["t"].as<unsigned long long>();

            int mostRecentIndex = chartBuffer.size()-1;
            if(chartBuffer[mostRecentIndex].openTime == openTime) {
              chartBuffer[mostRecentIndex].lowPrice = lowPrice;
              chartBuffer[mostRecentIndex].highPrice = highPrice;
              chartBuffer[mostRecentIndex].openPrice = openPrice;
              chartBuffer[mostRecentIndex].closePrice = closePrice;
              chartBuffer[mostRecentIndex].volume = volume;
            }
            else {
              chartDataPoint discarded;
              chartBuffer.pop(discarded);
              chartBuffer.push({
                openTime,
                openPrice,
                highPrice,
                lowPrice,
                closePrice,
                volume,
                response["k"]["T"].as<unsigned long long>()
              });
            }
            drawChart();
            
            if(closePrice > openPrice){
              M5.Lcd.setTextColor(COLOR_GREEN, BLACK);
            }
            else if (closePrice < openPrice) {
              M5.Lcd.setTextColor(COLOR_RED, BLACK);
            }
            //M5.Lcd.setCursor(0,84);
            //M5.Lcd.setTextSize(6);
            M5.Lcd.print(" ");
            if(closePrice >= 100000){
              M5.Lcd.println(closePrice, 0);
            }
            else if(closePrice >= 10000) {
              M5.Lcd.println(closePrice, 1);
            }
            else if(closePrice >= 1000) {
              M5.Lcd.println(closePrice, 2);
            }
            else if(closePrice >= 100) {
              M5.Lcd.println(closePrice, 3);
            }
            else if(closePrice >= 10) {
              M5.Lcd.println(closePrice, 4);
            }
            else if(closePrice >= 1) {
              M5.Lcd.println(closePrice, 5);
            }
            else{
              M5.Lcd.println(closePrice, 6);
            }
          }
          break;
    case WStype_BIN:
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    }

}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case SYSTEM_EVENT_WIFI_READY: 
      Serial.println("WiFi interface ready");
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:{
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
        M5.Lcd.setCursor(0,0);
        M5.Lcd.print(WiFi.localIP());

        Serial.print("CHART_POINTS: ");
        Serial.println(CHART_POINTS);
        
        http.begin("https://fapi.binance.com/fapi/v1/klines?interval=1m&limit="+ String(CHART_POINTS) +"&symbol=" + SYMBOL);
        Serial.print("http status code: ");
        Serial.println(http.GET());
        deserializeJson(klineBuf, http.getString());
        JsonArray response = klineBuf.as<JsonArray>();
        chartBuffer.clear();
        for (int i = 0; i < response.size(); i++) {
          JsonArray row = response[i].as<JsonArray>();
          chartBuffer.push({
              row[0].as<unsigned long long>(),
              row[1].as<float>(),
              row[2].as<float>(),
              row[3].as<float>(),
              row[4].as<float>(),
              row[5].as<float>(),
              row[6].as<unsigned long long>(),
            });
        }
        delay(20);
        drawChart();
        webSocket.beginSSL("fstream.binance.com", 443, "/ws/" + String(SYMBOL) + "@kline_1m");
      }
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case SYSTEM_EVENT_AP_START:
      Serial.println("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      Serial.println("WiFi access point  stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      Serial.println("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      Serial.println("Received probe request");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      Serial.println("IPv6 is preferred");
      break;
    case SYSTEM_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("Ethernet disconnected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.println("Obtained IP address");
      break;
    default: break;
  }
}


void setup(){
  Serial.begin(115200);
  if(!SPIFFS.begin()){
    Serial.println("SPIFFS Mount Failed");
  }
  
  M5.begin(); //Init M5Core. Initialize M5Core
  M5.Power.begin(); //Init Power module. Initialize the power module


  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(APNAME, WiFiAPPSK);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin();
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  useWiFiApi(&server);
  
  server.serveStatic("/", SPIFFS, "/wwwroot/")
    .setDefaultFile("index.html");
  server.begin();

  webSocket.onEvent(webSocketEvent);

  
  // Ram not big enough, must make chart with 2 sprites :(
  chartSprite.createSprite(CANDLE_WIDTH, CHART_HEIGHT);
  //chartSprite.setColorDepth(8);
}

const long status_interval = 5000;
static long status_millis_last = status_interval * -1;

const int wifi_icon_x = 295;
const int wifi_icon_y = 0;
const int bat_icon_x = 313;
const int bat_icon_y = 0;

void loop() {
  webSocket.loop();
  M5.update();
  if (millis() - status_millis_last >= status_interval) {
    status_millis_last += status_interval;
    // WIFI
    Serial.print("[Status] WiFi RSSI: ");
    int wifi_rssi = WiFi.RSSI();
    Serial.println(wifi_rssi);
    if(wifi_rssi <= -90) {
      M5.Lcd.drawBmpFile(SPIFFS, "/wifi/rssi_worse.bmp",wifi_icon_x,wifi_icon_y);
    }
    else if(wifi_rssi <= -80) {
      M5.Lcd.drawBmpFile(SPIFFS, "/wifi/rssi_bad.bmp",wifi_icon_x,wifi_icon_y);
    }
    else if(wifi_rssi <= -70) {
      M5.Lcd.drawBmpFile(SPIFFS, "/wifi/rssi_ok.bmp",wifi_icon_x,wifi_icon_y);
    }
    else if(wifi_rssi <= -60) {
      M5.Lcd.drawBmpFile(SPIFFS, "/wifi/rssi_good.bmp",wifi_icon_x,wifi_icon_y);
    }
    else if(wifi_rssi < 0) {
      M5.Lcd.drawBmpFile(SPIFFS, "/wifi/rssi_excellent.bmp",wifi_icon_x,wifi_icon_y);
    }
    else {
      M5.Lcd.drawBmpFile(SPIFFS, "/wifi/rssi_none.bmp",wifi_icon_x,wifi_icon_y);
    }

    // BATTERY
    Serial.print("[Status] Battery: ");
    if(M5.Power.isCharging()){
      M5.Lcd.drawBmpFile(SPIFFS, "/battery/bat_charge.bmp",bat_icon_x,bat_icon_y);
      Serial.println("Charging...");
    }
    else{
      int bat_lvl = M5.Power.getBatteryLevel();
      Serial.print(bat_lvl);
      Serial.println("%");
      if(bat_lvl < 20){
        M5.Lcd.drawBmpFile(SPIFFS, "/battery/bat_empty.bmp",bat_icon_x,bat_icon_y);
      }
      else if(bat_lvl < 40){
        M5.Lcd.drawBmpFile(SPIFFS, "/battery/bat_low.bmp",bat_icon_x,bat_icon_y);
      }
      else if(bat_lvl < 60){
        M5.Lcd.drawBmpFile(SPIFFS, "/battery/bat_mid.bmp",bat_icon_x,bat_icon_y);
      }
      else if(bat_lvl < 80){
        M5.Lcd.drawBmpFile(SPIFFS, "/battery/bat_high.bmp",bat_icon_x,bat_icon_y);
      }
      else {
        M5.Lcd.drawBmpFile(SPIFFS, "/battery/bat_full.bmp",bat_icon_x,bat_icon_y);
      }
    }
  }
  
}
