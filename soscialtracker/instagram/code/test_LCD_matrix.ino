#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

long period;
int offset=1,refresh=0;
int pinCS = 0; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 8; //default 8
int numberOfVerticalDisplays = 1;
String decodedMsg;
String insta;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);


int wait = 100; // In milliseconds default 250

int spacer = 2;
int width = 5 + spacer; // The font width is 5 pixels

void handle_msg() {
  insta = 1234;
                        
  matrix.fillScreen(LOW);
  refresh=1;
  String msg = "test " + insta;
  decodedMsg = msg;
  // Restore special characters that are misformed to %char by the client browser
  decodedMsg.replace("+", " ");      
  decodedMsg.replace("%21", "!");  
  decodedMsg.replace("%22", "");  
  decodedMsg.replace("%23", "#");
  decodedMsg.replace("%24", "$");
  decodedMsg.replace("%25", "%");  
  decodedMsg.replace("%26", "&");
  decodedMsg.replace("%27", "'");  
  decodedMsg.replace("%28", "(");
  decodedMsg.replace("%29", ")");
  decodedMsg.replace("%2A", "*");
  decodedMsg.replace("%2B", "+");  
  decodedMsg.replace("%2C", ",");  
  decodedMsg.replace("%2F", "/");   
  decodedMsg.replace("%3A", ":");    
  decodedMsg.replace("%3B", ";");  
  decodedMsg.replace("%3C", "<");  
  decodedMsg.replace("%3D", "=");  
  decodedMsg.replace("%3E", ">");
  decodedMsg.replace("%3F", "?");  
  decodedMsg.replace("%40", "@"); 
  //Serial.println(decodedMsg);                   // print original string to monitor
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

}

void loop() {
  handle_msg();


//decodedMsg = 1234;

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
