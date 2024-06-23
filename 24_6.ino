/*
 * Initial Author: ryand1011 (https://github.com/ryand1011)
 *
 * Reads data written by a program such as "rfid_write_personal_data.ino"
 *
 * See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 *
 * Uses MIFARE RFID card using RFID-RC522 reader
 * Uses MFRC522 - Library
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
*/

#include <SPI.h>
#include <MFRC522.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RST_PIN_RFID         5           // Configurable, see typical pin layout above
#define SS_PIN_RFID          53          // Configurable, see typical pin layout above

#define OLED_MOSI     51
#define OLED_CLK      52
#define OLED_DC       50
#define OLED_CS       49
#define OLED_RST      24

int G_LED_1   =   31;
int G_LED_2   =   33;
int G_LED_3   =   35;
int G_LED_4   =   37;

char arr[4] = {G_LED_1, G_LED_2, G_LED_3, G_LED_4};


int led = 0;
int blinkcount = 10;
int blinkcounter = 0; 
int blinking[4] = {LOW, LOW, LOW ,LOW};
int ledState[4] = { LOW , LOW , LOW , LOW };
char str1[18];


//int ledState = LOW;

unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 500;  // interval at which to blink (milliseconds)


#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

// Create the OLED display
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

MFRC522 mfrc522(SS_PIN_RFID, RST_PIN_RFID);   // Create MFRC522 instance

//*****************************************************************************************//
void setup() {
  Serial.begin(19200);                                           // Initialize serial communications with the PC

  pinMode(G_LED_1,OUTPUT);
  pinMode(G_LED_2,OUTPUT);
  pinMode(G_LED_3,OUTPUT);
  pinMode(G_LED_4,OUTPUT);

  display.begin(0, true); // we dont use the i2c address but we will reset!

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  display.drawPixel(10, 10, SH110X_WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(300);
  display.clearDisplay();
/*
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Scan Hose's Tag:");
  display.display();

  delay (500);
  display.clearDisplay();
*/
  //SPI.begin();                                                  // Init SPI bus
  SPI.begin(); 

  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Scan Hose's Tag:"));    //shows in serial that it is ready to read

  display.begin(0, true); // we dont use the i2c address but we will reset!

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  str1[17] = '\0';
  str1[0] = '\0'; 
  
  //return 1;

}

void rtrim(char *str) {
    if (str == NULL) return; // Handle null pointer input

    size_t len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
    return str;
}

//*****************************************************************************************//
void loop() {
  
  
/*
  display.begin(0, true); // we dont use the i2c address but we will reset!

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(200);

  // Clear the buffer.
  display.clearDisplay();

*/
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
 

  //display.begin(0, true); // we dont use the i2c address but we will reset!

  MFRC522::StatusCode status;

  // ......................................

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Tag/Card Detected:**"));
  //Serial.println(F("\n"));
  display.println("**Tag/Card Detected:**");
  display.display();  
  //-------------------------------------------

  // mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  /*

  Serial.print(F("Instructions: connect to the Coupler with the Blinking Green LED"));
  display.println("Instructions: connect to the Coupler with the Blinking Green LED");
  display.display();

  */

  //------------------------------------------- GET FIRST Part
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }


  else 
  {

    byte buffer1[18];

    block = 4;
    len = 18;

    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    else {

      status = mfrc522.MIFARE_Read(block, buffer1, &len);

      Serial.println(F("Instructions: connect to the Coupler with the Blinking Green LED"));

      char str1 [18];
      memcpy(str1, buffer1, sizeof(buffer1));

      str1[sizeof (buffer1)] = '\0' ;

      

      //str1[sizeof (buffer1)] = '\0' ;
      str1[sizeof (buffer1) -1] = '\0' ;
      str1[sizeof (buffer1) -2] = '\0' ;
      
      

      //PRINT FIRST Part
      for (uint8_t x = 0; x < 16; x++)
      {
        if (buffer1[x] != 32)
        {
          Serial.write(buffer1[x]);
        //  (str1)[x] = (char)buffer1[x];
        }
      }


      Serial.print(" ");

      //---------------------------------------- GET Second Part

      byte buffer2[18];
      block = 1;

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }

      status = mfrc522.MIFARE_Read(block, buffer2, &len);
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Reading failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }
      
      char str2 [16];

      memcpy(str2, buffer2, 16);

      str2[sizeof (buffer2)] = '\0' ;
      str2[sizeof (buffer2) -1] = '\0' ;
      str2[sizeof (buffer2) -2] = '\0' ;
      char * pstr2 = NULL; 
      pstr2 [16] = &str2 ; 

      //PRINT LAST Part
      for (uint8_t y = 0; y < 16; y++) {
        Serial.write(buffer2[y] );
      //  (str2)[y] = (char)buffer2[y];
      
      }


      //rtrim(str1);
      //rtrim(str2);

      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }

      //stringReverse(str1);





      Serial.println("\n");

      Serial.println("str1");

      Serial.println(str1);

      Serial.println(" ");

      Serial.println(str1 [10]);

      Serial.println("str2");

      Serial.println(str2);

      Serial.println(F("\n**End Reading**\n"));
      Serial.println(str1);

      Serial.println("////////////////////////////////////////////////////");
      Led (str1);
      Serial.println("////////////////////////////////////////////////////");
      delay(200);

      

      //return *pstr1 , *pstr2;
    }
  }

  /*
  Serial.println(F("\n**End Reading**\n"));
  Serial.println(str1);

  Serial.println("////////////////////////////////////////////////////");
  Led (str1);
  Serial.println("////////////////////////////////////////////////////");  
  */


  //Serial.println(str1);
  //Serial.println(str2);
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  
}
void stringReverse(char* strR)
{
    int len = strlen(strR);
    // pointers to start and end
    char* start = strR;
    char* end = strR + len - 1;
 
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void Led (char* str) 
{

  Serial.println(str); 


  // check if empty string... if empty string don't change nothing 
  if (str[0] == '\0') {
    led = led;
    }
  else{
    rtrim(str);
    stringReverse(str);
    int Num = atoi(str) ; 
    led = Num - 1;
    blinking[led] = HIGH;}
   //N = Num - 1 
    Serial.println(str); 
   //Serial.println(Num); 
  
   //Serial.println(arr[led]);
   //int L = arr[Num-1];
   //Serial.println(L);
   //int ledState = LOW;
    unsigned long currentMillis = millis();
   //int ledState = LOW;
    int khara = 0;
    for (khara = 0; khara <= 20; khara++)
    { 
      Serial.println("terarararaa");

      digitalWrite(arr[led], ledState[led]);
      ledState[led] = !ledState[led];
      delay(500);
/*
      if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      Serial.println(ledState[led]);
      Serial.println(led);
      ledState[led] = !ledState[led];
      Serial.println(ledState[led]);
      Serial.println(led);
      

    

      //ledState[led] = !ledState[led];
      //Serial.println(ledState[led]);
      //Serial.println(led);
      // if the LED is off turn it on and vice-versa:
      if (ledState[led] == LOW) {
        blinkcounter++;
      } 

      if (blinkcounter >= blinkcount ){
        blinking[led] = false;
        digitalWrite(arr[led], LOW);
        blinkcounter = 0;
        // blincounter, blinkcount=10, blinking, global
      }


    }
*/
  }
  digitalWrite(arr[led], LOW);


  
}


/*int print (char *pstr1, char *pstr2)

  if ( blinking[0]&& currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState[Num-1] == LOW) {
      ledState[Num-1] = HIGH;
    } 
    else 
    {
      ledState[Num-1] = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(arr[Num-1], ledState[Num-1]);


{

  char v =  *pstr1 ;
  char b =  *pstr2 ;

  display.begin(0, true); // we dont use the i2c address but we will reset!

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(v);
  display.println(b);
  display.display();
  delay(500);
  display.clearDisplay();

  //Serial.println(v);
  //Serial.println(b);
}


//*****************************************************************************************/


