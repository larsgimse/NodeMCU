/*
Project: Wifi controlled LED matrix display
NodeMCU pins    -> EasyMatrix pins
MOSI-D7-GPIO13  -> DIN
CLK-D5-GPIO14   -> Clk
GPIO0-D3        -> LOAD

*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <SPI.h>

// Facebook config.
#define FACEBOOK_HOST "graph.facebook.com"
#define FACEBOOK_PORT 443
#define PAGE_ID "***" // your page_id
#define ACCESS_TOKEN "***" // your access_token
// graph.facebook.com SHA1 fingerprint
const char* facebookGraphFingerPrint = "93 6F 91 2B AF AD 21 6F A5 15 25 6E 57 2C DC 35 A1 45 1A A5";

// Configuração de Wifi
#define WIFI_SSID "***"
#define WIFI_PASSWORD "***"

//#define REQUEST_INTERVAL 1000*60*1 // default time
#define REQUEST_INTERVAL 1000*10*1

Ticker tickerRequest;
bool requestNewState = true;
WiFiClientSecure tlsClient;

long period;
int offset=1,refresh=0;
int pinCS = 0; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 8; //default 8
int numberOfVerticalDisplays = 1;
String decodedMsg;
String face = "0";

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int wait = 100; // In milliseconds default 250

int spacer = 2;
int width = 5 + spacer; // The font width is 5 pixels


Ticker tickerDisplay;

int pageFansCount = 0;

void request(){
  requestNewState = true;
}


void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    tries++;
  
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}
}

String makeRequestGraph(){
  if (!tlsClient.connect(FACEBOOK_HOST, FACEBOOK_PORT)) {
    Serial.println("Host connection failed");
    return "";    
  }
  
  String params = "?pretty=0&fields=fan_count&access_token="+String(ACCESS_TOKEN);
  String path = "/v2.8/" + String(PAGE_ID);
  String url = path + params;
  Serial.print("requesting URL: ");
  Serial.println(url);

  String request = "GET " + url + " HTTP/1.1\r\n" +
    "Host: " + String(FACEBOOK_HOST) + "\r\n\r\n";
  
  tlsClient.print(request);

  String response = "";
  String chunk = "";  
  
  do {
    if (tlsClient.connected()) {
      delay(5);
      chunk = tlsClient.readStringUntil('\n');
      if(chunk.startsWith("{")){
        response += chunk;
      }
    }
  } while (chunk.length() > 0);
    
  Serial.print(" Message ");
  Serial.println(response);  

  return response;
}

int getPageFansCount(){
  String response = makeRequestGraph();  
  
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + 40;
  DynamicJsonBuffer jsonBuffer(bufferSize);  
  
  JsonObject& root = jsonBuffer.parseObject(response);
  
  int fanCount = root["fan_count"];

  return fanCount;
}

void handle_msg() {                       
  matrix.fillScreen(LOW);
  String msg = "Followers on Facebook:"  + face;
  decodedMsg = msg;
}

void number_matrix() {
  String matrix_txt = face; 
  int  y=0;
  for ( int i=0; i < matrix_txt.length(); i++ ) {
      int x= i * width;  // step horizontal by width of character and spacer
      matrix.drawChar(x, y, matrix_txt[ i ], HIGH, LOW, 1);   // draw a character from 'tape'
    }
   matrix.write(); // Send bitmap to display
}
  

void setup() {
  matrix.setIntensity(10); // Use a value between 0 and 15 for brightness
  
 /* matrix.setPosition(0, 7, 0); // The first display is at <0, 7>
  matrix.setPosition(1, 6, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 5, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 4, 0); // And the last display is at <3, 0>
  matrix.setPosition(4, 3, 0); // The first display is at <0, 0>
  matrix.setPosition(5, 2, 0); // The second display is at <1, 0>
  matrix.setPosition(6, 1, 0); // The third display is at <2, 0>
  matrix.setPosition(7, 0, 0); // And the last display is at <3, 0> */

  matrix.setRotation(0,1);
  matrix.setRotation(1,1);
  matrix.setRotation(2,1);
  matrix.setRotation(3,1);
  
  Serial.begin(9600);

  setupWifi();   
  
  // Registra o ticker para pegar dados novos de tempos em tempos
  tickerRequest.attach_ms(REQUEST_INTERVAL, request);

}

void loop() {

  number_matrix();

  // Apenas peça novos dados quando passar o tempo determinado
  if(requestNewState){
    Serial.println("Request new State");               
       
    int pageFansCountTemp = getPageFansCount();
    
    Serial.print("Page fans count: ");
    Serial.println(pageFansCountTemp);

    face = pageFansCountTemp;

    if(pageFansCountTemp <= 0){
      
      Serial.println("Error requesting data");    
      
    }else{
      pageFansCount = pageFansCountTemp;
      requestNewState = false; 

    }
  }
  
  delay(20);

  handle_msg();

  for ( int i = 0 ; i < width * decodedMsg.length() + matrix.width() - 1 - spacer; i++ ) {
    if (refresh==1) i=0;
    refresh=0;
    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically
 
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < decodedMsg.length() ) {
        matrix.drawChar(x, y, decodedMsg[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

    delay(wait);
  }
  
}
