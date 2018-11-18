/*
Project: Wifi controlled LED matrix display
NodeMCU pins    -> EasyMatrix pins
MOSI-D7-GPIO13  -> DIN
CLK-D5-GPIO14   -> Clk
GPIO0-D3        -> LOAD

*/

#include "InstagramStats.h"       // https://github.com/witnessmenow/arduino-instagram-stats
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>


// these libraries are included with ESP8266 support
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "YODA";       // your network SSID (name)
char password[] = "dollytimmy123";  // your network key

WiFiClientSecure secureClient;
WiFiClient client;
InstagramStats instaStats(secureClient);

unsigned long api_delay = 1 * 60000; //time between api requests (1mins)
unsigned long api_due_time;

String userName = "larsgimse";    // from their instagram url https://www.instagram.com/userName/

long period;
int offset=1,refresh=0;
int pinCS = 0; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 8; //default 8
int numberOfVerticalDisplays = 1;
String decodedMsg;
String insta;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

//String tape = "Arduino";
int wait = 100; // In milliseconds default 250

int spacer = 2;
int width = 5 + spacer; // The font width is 5 pixels

void handle_msg() {                       
  matrix.fillScreen(LOW);
  String msg = "Instagram: " + insta;
  decodedMsg = msg;
} 

void setup() {
  matrix.setIntensity(10); // Use a value between 0 and 15 for brightness
  
  matrix.setPosition(0, 7, 0); // The first display is at <0, 7>
  matrix.setPosition(1, 6, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 5, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 4, 0); // And the last display is at <3, 0>
  matrix.setPosition(4, 3, 0); // The first display is at <0, 0>
  matrix.setPosition(5, 2, 0); // The second display is at <1, 0>
  matrix.setPosition(6, 1, 0); // The third display is at <2, 0>
  matrix.setPosition(7, 0, 0); // And the last display is at <3, 0>

  matrix.setRotation(0,3);
  matrix.setRotation(1,3);
  matrix.setRotation(2,3);
  matrix.setRotation(3,3);

  Serial.begin(115200);

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

}

void loop() {
  
 if (millis() > api_due_time)  {
    getInstagramStatsForUser();
    api_due_time = millis() + api_delay;
  }

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

void getInstagramStatsForUser() {
  Serial.println("Getting instagram user stats for " + userName );
  InstagramUserStats response = instaStats.getUserStats(userName);
  Serial.println("Response:");
  Serial.print("Number of followers: ");
  Serial.println(response.followedByCount);
  insta = response.followedByCount;

}
