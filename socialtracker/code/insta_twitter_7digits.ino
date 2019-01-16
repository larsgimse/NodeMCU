/*******************************************************************
 *  An compilation of library sample code written by Brian Lough            
 *  https://github.com/witnessmenow/        
 *                                                                  
 *  Smushed together with a dash of seven segment displays by Becky Stern
 *  https://www.instructables.com/id/EOGVPMLJD2FC7WM/
 *******************************************************************/
// requires the following libraries, search in Library Manager or download from github:

#include <Wire.h>                  // installed by default
#include <Adafruit_GFX.h>          // https://github.com/adafruit/Adafruit-GFX-Library
#include "Adafruit_LEDBackpack.h"  // https://github.com/adafruit/Adafruit_LED_Backpack
#include <TwitterApi.h>            // https://github.com/witnessmenow/arduino-twitter-api
#include <InstructablesApi.h>      // https://github.com/witnessmenow/arduino-instructables-api
#include <ArduinoJson.h>           // https://github.com/bblanchon/ArduinoJson
#include "InstagramStats.h"       // https://github.com/witnessmenow/arduino-instagram-stats
#include "JsonStreamingParser.h"  // https://github.com/squix78/json-streaming-parser
#include <TM1637Display.h>

#define CLK1 13 //D7
#define DIO1 12 //D6

#define CLK2 5 //D1
#define DIO2 16 //D0

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
  };

// these libraries are included with ESP8266 support
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "****";       // your network SSID (name)
char password[] = "****";  // your network key

// Normally we would use these to generate the bearer token but its not working yet :/
// Use steps on the readme to generate the Bearer Token

#define BEARER_TOKEN "****"

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

//Inputs

String screenName = "udirbetalab";  // Twitter
String userName = "udirbetalab";    // from their instagram url https://www.instagram.com/userName/

bool haveBearerToken = false;

TM1637Display display1(CLK1, DIO1);
TM1637Display display2(CLK2, DIO2);


void setup() {

  Serial.begin(115200);


  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
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
  //getTwitterStats(screenName);

}

void loop() {
  
 if (millis() > api_due_time)  {
//    getInstructablesStats();
    delay(200);
    if(haveBearerToken){
      getTwitterStats(screenName);
    }
    delay(200);
    getInstagramStatsForUser();
    api_due_time = millis() + api_delay;
  }

  
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
      display1.setBrightness(0x0f);
      display1.showNumberDec(followerCount, true);

    } else {
      Serial.println("Failed to parse Json");
    }
}

void getInstagramStatsForUser() {
//  display2.setSegments(SEG_DONE);
  Serial.println("Getting instagram user stats for " + userName );
  InstagramUserStats response = instaStats.getUserStats(userName);
  Serial.println("Response:");
  Serial.print("Number of followers: ");
  Serial.println(response.followedByCount);
  display2.setBrightness(0x0f);
  display2.showNumberDec(response.followedByCount, true);
}
