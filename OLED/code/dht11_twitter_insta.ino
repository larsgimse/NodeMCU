/*
Project: Wifi controlled LED matrix display
NodeMCU pins    -> EasyMatrix pins
MOSI-D7-GPIO13  -> DIN
CLK-D5-GPIO14   -> Clk
GPIO0-D3        -> LOAD

*/

#include "InstagramStats.h"       // https://github.com/witnessmenow/arduino-instagram-stats
#include <TwitterApi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"


// these libraries are included with ESP8266 support
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "****";       // your network SSID (name)
char password[] = "****";  // your network key

#define BEARER_TOKEN "****"
#define DHTTYPE DHT11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET LED_BUILTIN //NodeMCU
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Using curl to get bearer token
// curl -u "$CONSUMER_KEY:$CONSUMER_SECRET" \
//    --data 'grant_type=client_credentials' \
//    'https://api.twitter.com/oauth2/token'

WiFiClientSecure secureClient;
WiFiClient client;
TwitterApi TwitterStats(secureClient);
InstagramStats instaStats(secureClient);

unsigned long api_delay = 1 * 60000; //time between api requests (1mins)
unsigned long api_due_time;

String screenName = "larsgimse";  // Twitter
String userName = "larsgimse";    // from their instagram url https://www.instagram.com/userName/
String insta;
String twitter;
String oldinsta;
String temp;
String humi;
String instamsg;
String twittermsg;

bool haveBearerToken = false;

int wait = 100; // In milliseconds default 250

void setup() {
  dht.begin();
  
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  TwitterStats.setBearerToken(BEARER_TOKEN);
  haveBearerToken = true;

}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  temp = ("Temp: " + String(t) +  "C");
  humi = ("Humidity: " + String(h) + "%");

 if (millis() > api_due_time)  {
    delay(200);
    if(haveBearerToken){
      getTwitterStats(screenName);
    }
    delay(200);
    getInstagramStatsForUser();
    api_due_time = millis() + api_delay;
  }
  
  String instamsg = "Instagram: " + insta;
  String twittermsg = "Twitter: " + twitter;
  
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(1, 0);
  display.println(temp);
  display.setCursor(1, 10);
  display.println(humi);
  display.setCursor(1, 30);
  display.println("@larsgimse");
  display.setCursor(1, 40);
  display.println(instamsg);
  display.setCursor(1, 50);
  display.println(twittermsg);
  display.display();      // Show initial text
  delay(100);

  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  
}

void getTwitterStats(String name) {
//  display1.setSegments(SEG_DONE);
  Serial.println("Getting twitter stats for " + name);
   String responseString = TwitterStats.getUserStatistics(name);
    Serial.println(responseString);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& response = jsonBuffer.parseObject(responseString);
    if (response.success()) {
      Serial.println("parsed Json");
      // Use Arduino Json to parse the data
      int followerCount = response["followers_count"];
      Serial.println(followerCount);

      twitter = followerCount;

    } else {
      Serial.println("Failed to parse Json");
    }
}

void getInstagramStatsForUser() {
  Serial.println("Getting instagram user stats for " + userName );
  InstagramUserStats response = instaStats.getUserStats(userName);
  Serial.println("Response:");
  Serial.print("Number of followers: ");
  Serial.println(response.followedByCount);
  insta = response.followedByCount;

}
