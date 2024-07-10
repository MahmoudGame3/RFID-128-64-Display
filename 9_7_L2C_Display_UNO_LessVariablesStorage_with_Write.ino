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
#include <Arduino.h>
#include <U8g2lib.h>

#include <SPI.h>
#include <MFRC522.h>


#include <Wire.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RST_PIN_RFID         9           // Configurable, see typical pin layout above
#define SS_PIN_RFID          10          // Configurable, see typical pin layout above
/*
#define OLED_MOSI     51
#define OLED_CLK      52
#define OLED_DC       50
#define OLED_CS       49
#define OLED_RST      24
*/

int G_LED_1   =   2;
int G_LED_2   =   3;
int G_LED_3   =   4;
int G_LED_4   =   5;

int Contact   =   7; 
int ContactRead = 0;

//int R_LED_1   =   33;
//int R_LED_2   =   32;
//int R_LED_3   =   34;
//int R_LED_4   =   36;

int Buzzer    =   6;

//int pbyeah    =   42;
//int pbno      =   44;

char arrG[4] = {G_LED_1, G_LED_2, G_LED_3, G_LED_4};


int led = 0;
int Counter = 0;

int ledStateG[4] = { LOW , LOW , LOW , LOW };
char str1[18];
char fDetails [18];


//int ledState = LOW;

//unsigned long previousMillis = 0;  // will store last time LED was updated
//const long Interval = 500;  // interval at which to blink (milliseconds)

char sCurrent [9] = "Scan Tag" ;
char sAuthF [14] = "Auth. Failed!";
char sReadF [14] = "Read. Failed!";
//char sAuthF1 [15] = "Authentication" ;
//char sAuthF2 [8] = "failed!" ;
//char sReadF1 [8] = "Reading" ;
//char sReadF2 [8] = "failed!" ;




MFRC522 mfrc522(SS_PIN_RFID, RST_PIN_RFID);   // Create MFRC522 instance

//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 16, /* data=*/ 17);   // ESP32 Thing, HW I2C with pin remapping
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 16, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C

//U8G2_SSD1312_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 8);

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_UC1638_192X96_SW_I2C u8x8(/* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);  // u8g2 test board: I2C clock/data is same as SPI, I2C default address is 0x78


U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 	      


//const char COPYRIGHT_SYMBOL[] = {0xa9, '\0'};




//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC

  pinMode(G_LED_1,OUTPUT);
  pinMode(G_LED_2,OUTPUT);
  pinMode(G_LED_3,OUTPUT);
  pinMode(G_LED_4,OUTPUT);

  pinMode(Contact,INPUT);

  pinMode(Buzzer,OUTPUT);

//////////////////////////////////////////////////////////

  u8x8.begin();
  DPrintLogo();
  u8x8_prepare();
  //u8g2.clearBuffer();

//////////////////////////////////////////////////////////  
  
  //SPI.begin();                                                  // Init SPI bus
  //SPI.begin(); 
  SPI.begin();
  //SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("\nScan Hose's Tag:"));    //shows in serial that it is ready to read

//////////////////////////////////////////////////////////


  str1[17] = '\0';
  str1[0] = '\0'; 

  //DPrint (sCurrent);  


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
  
  //DPrint (sCurrent);
  u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);    
  u8x8.drawString(0, 0, sCurrent);
  
  //u8g2.clearBuffer();


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


  //-------------------------------------------

  // mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------


  Serial.println(F("**Tag/Card Detected**"));
  //Serial.println(F("\n"));
  char sDet [15] = "*Tag Detected*" ;
//  DPrint (sDet);
  u8x8.clear();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r); 
  u8x8.drawString(1, 0, sDet);

  delay(1500);

  //------------------------------------------- GET FIRST Part
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  //display.begin(0, true); // we dont use the i2c address but we will reset!
  //display.display();
  //display.clearDisplay();

  if (status != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));

/*
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);  
    u8x8.setCursor(1,0);
    u8x8.print(sAuthF1);   
    u8x8.setCursor(4,1);
    u8x8.print(sAuthF2);
*/
  
    DPrint (sAuthF);   
    //memcpy(fDetails, status, sizeof(status));
    //DPrintL (fDetails);

    delay(1500);
    
    u8x8.clear();

    u8x8.setFont(u8x8_font_px437wyse700b_2x2_r); 
    u8x8.drawString(0, 0, sCurrent);

    return;
  }



  else 
  {

    byte buffer1[18];

    block = 4;
    len = 18;

    if (status != MFRC522::STATUS_OK) {
      Serial.println(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      
      DPrint (sReadF);
      //memcpy(fDetails, status, sizeof(status));
      //DPrintL (fDetails);
      delay(1500);

      u8x8.clear();
      u8x8.setFont(u8x8_font_px437wyse700b_2x2_r); 
      u8x8.drawString(0, 0, sCurrent);
      return;
    }

    else {

      status = mfrc522.MIFARE_Read(block, buffer1, &len);


      char str1 [18];
      memcpy(str1, buffer1, sizeof(buffer1));

      str1[sizeof (buffer1)] = '\0' ;

      

      //str1[sizeof (buffer1)] = '\0' ;
      str1[sizeof (buffer1) -1] = '\0' ;
      str1[sizeof (buffer1) -2] = '\0' ;
      
      
      /*
      //PRINT FIRST Part
      for (uint8_t x = 0; x < 16; x++)
      {
        if (buffer1[x] != 32)
        {
          Serial.write(buffer1[x]);
        //  (str1)[x] = (char)buffer1[x];
        }
      }
      */

      Serial.println(F(" "));

      //---------------------------------------- GET Second Part

      byte buffer2[18];
      block = 1;

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
      if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));

        DPrint (sAuthF);
        //memcpy(fDetails, status, sizeof(status));
        //DPrintL (fDetails);
        delay(1500);

        u8x8.clear();
        u8x8.setFont(u8x8_font_px437wyse700b_2x2_r); 
        u8x8.drawString(0, 0, sCurrent);
     
        return;
      }

      status = mfrc522.MIFARE_Read(block, buffer2, &len);
      if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Reading failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));

        DPrint (sReadF);
        //memcpy(fDetails, status, sizeof(status));
        //DPrintL (fDetails);
        delay(1500);

        u8x8.clear();
        u8x8.setFont(u8x8_font_px437wyse700b_2x2_r); 
        u8x8.drawString(0, 0, sCurrent);

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

      /*
      for (uint8_t y = 0; y < 16; y++) {
        Serial.write(buffer2[y] );
      //  (str2)[y] = (char)buffer2[y];
      
      }
      */

      //rtrim(str1);
      //rtrim(str2);

      if (status != MFRC522::STATUS_OK) {
        Serial.println(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));

        DPrint (sAuthF);
        //memcpy(fDetails, status, sizeof(status));
        //DPrintL (fDetails);
        delay(1500);

        u8x8.clear();
        u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);    
        u8x8.drawString(0, 0, sCurrent);
        return;
      }

      //stringReverse(str1);




      /*
      Serial.println("\n");

      Serial.println("str1");

      Serial.println(str1);

      Serial.println(" ");

      Serial.println(str1 [10]);

      Serial.println("str2");

      Serial.println(str2);


      //display.clearDisplay();
      //display.println("**End Reading**");
      //display.display();
      //display.clearDisplay();

      Serial.println(str1);
      */
      Serial.println(F("////////////////////////////////////////////////////"));
      Ledd (str1, str2);
      Serial.println(F("////////////////////////////////////////////////////"));
      delay(200);

      Serial.println(F("Scan Hose's Tag:"));    //shows in serial that it is ready to read


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
  delay(500); //change value if you want to read cards faster
  u8x8.setFont(u8x8_font_px437wyse700b_2x2_r); 
  u8x8.drawString(0, 0, sCurrent);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  
}

void DPrintLogo(void)

{  
  u8x8.clear();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);  
  u8x8.setCursor(1,2);
  u8x8.print("Brought to you");
  u8x8.setCursor(7,4);
  u8x8.print("by:");
  delay(1500);

 
  u8x8.clear();
//  u8x8.setFont(u8x8_font_inb33_3x6_n);
//  u8x8.setCursor(1,2);

//  u8x8.setFont(u8x8_font_courB24_3x4_r);
  u8x8.setFont(u8x8_font_inb33_3x6_r);
  u8x8.setCursor(4,1);
  
  u8x8.print("CNH");

//  u8x8.draw2x2String(5,3,"CNH");
  delay(3000);
  u8x8.clear();
}
  //u8g2.clearBuffer();


void DPrint(char *str)
{
  u8x8.clear();
// u8x8_font_chroma48medium8_r 
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  u8x8.setFont(u8x8_font_chroma48medium8_r);    
  u8x8.setCursor(0,0);
  u8x8.print(str);
  delay(500);
  //u8g2.clearBuffer();
}

void DPrintL(char *str)
{
//  u8g2.clearBuffer();
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  u8x8.setFont(u8x8_font_chroma48medium8_r);   
  u8x8.setCursor(0,1);
  u8x8.print(str);
  delay(500);
//  u8g2.clearBuffer();
}

void DPrintLL(char *str)
{
//  u8g2.clearBuffer();
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  u8x8.setFont(u8x8_font_chroma48medium8_r); 
  u8x8.setCursor(0,2);
  u8x8.print(str);
  delay(500);
//  u8g2.clearBuffer();

}
void DPrintLLL(char *str)
{
//  u8g2.clearBuffer();
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,3);
  u8x8.print(str);
  delay(500);
//  u8g2.clearBuffer();

}

void DPrintLLLL(char *str)
{
//  u8g2.clearBuffer();
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  u8x8.setFont(u8x8_font_chroma48medium8_r);   
  u8x8.setCursor(0,4);
  u8x8.print(str);
  delay(500);
//  u8g2.clearBuffer();

}

void DPrintLFL(char *str)
{
  //u8g2.clearBuffer();
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  u8x8.setFont(u8x8_font_chroma48medium8_r);   
  u8x8.setCursor(0,5);
  u8x8.print(str);
  delay(500);
//  u8g2.clearBuffer();
}

void DPrintLFR(char *str)
{
  //u8g2.clearBuffer();
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    
  rtrim(str);
  Serial.println(str);

  stringReverse(str);
  rtrim(str);
  stringReverse(str);
  Serial.println(str);
  char strN [7];
  strN[6] = '\0';
  for (uint8_t coun = 0; coun < 6; coun ++)
  {
    strN[coun] = str[coun];
  }
//  strN[3] = '\0';
//  memcpy(strN, str, 2); Gives very bad results
  Serial.println(strN);

  rtrim(strN);
  Serial.println(strN);

//  int LenS [] = strlen(str);
//  strN[0] = str[0];
//  strN[1] = str[1];

//  Serial.println(strN);

//  rtrim(str1);
//  stringReverse(str);
  
//  Serial.println(str);
//  strN[2] = str[0];
//  Serial.println(str);
 
  u8x8.setFont(u8x8_font_chroma48medium8_r);   
  u8x8.setCursor(10,6);
  u8x8.print(strN);
  delay(500);
//  u8g2.clearBuffer();
}

void u8x8_prepare(void) 
{
  //u8g2.setFontMode(1);
  u8x8.setFont(u8x8_font_chroma48medium8_r);    
  u8x8.clear();
  //u8x8.setCursor(3,1);

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

void Ledd (char* str1, char*str2) 
{
  
  Serial.println(str1);
  Serial.println(str2);   


  //int ledStateG[4] = { LOW , LOW , LOW , LOW };


  // check if empty string... if empty string don't change nothing 
  if (str1[0] == '\0') {
    led = led;
    Serial.println(F("Empty Tag! You should save the coupler's data first."));
    
    u8x8.clear();
  
    char sEmpty1 [11] = "Empty Tag!" ;
    char sEmpty2 [11] = "It must be" ;
    char sEmpty3 [13] = "saved first." ;
    DPrint (sEmpty1);
    DPrintL (sEmpty2);
    DPrintLL (sEmpty3);    
    delay (3000);

    u8x8.clear();
    
    Serial.println(F("You want to assign it a location?")); 
    

    char sQtoOp1 [16] = "Do you want to" ;
    char sQtoOp2 [10] = "save it?" ;
    DPrint (sQtoOp1);
    DPrintL (sQtoOp2);
    
    delay (1500);

    u8x8.clear();

    Serial.println(F("Press Green Push Button for YES, or Red one for NO"));
    

    char sChoice1 [12]= "Press Green" ;
    char sChoice2 [15]= "Button for YES" ;
    //char sChoice1 [16]= "G >> Y , R >> N" ;
    //char sChoice2 [15]= "for YES, or" ;    
    char sChoice3 [14]= "or Red for NO" ;
//    char sChoice3 [20]= "NO" ;

    DPrint (sChoice1);
    DPrintL (sChoice2);
    DPrintLL (sChoice3);    

    delay (1500);

    u8x8.clear();
  }
 // if no answer in 10 seconds >> 1 - Base RED LED and Buzzer flashing. 
 // 2- Serial.println("Timeout! ") 
 // 3- Exit Function and Return to loop

 //if Answer No >> Exit Function and Return to loop 

 // if YES >> Trigger DataWrite Function
  else 
  {

    Serial.println(F("Instructions: connect to the Coupler with the Blinking Green LED"));
    char sInst1 [21] = "Connect to the" ;
    char sInst2 [20] = "Coupler with the" ;
    char sInst3 [15] = "Blinking Green" ;
    char sInst4 [5] = "LED." ;


    DPrint(sInst1);
    DPrintL(sInst2);
    DPrintLL(sInst3);
    DPrintLLL(sInst4);

    Serial.println(str1);
    Serial.println(str2);

    DPrintLFL(str1);
    DPrintLFR(str2);

    rtrim(str1);
    stringReverse(str1);
    int Num = atoi(str1) ; 
    led = Num - 1;
    //blinking[led] = HIGH;
   //N = Num - 1 
   //Serial.println(str1); 
   //Serial.println(Num); 
  
   //Serial.println(arrG[led]);
   //int L = arrG[Num-1];
   //Serial.println(L);
   //int ledState = LOW;
    //unsigned long currentMillis = millis();
    //previousMillis = 0;
    Counter = 0;
   //int ledState = LOW;
    ContactRead = digitalRead(Contact);
    while (ContactRead == LOW)
    {
      ContactRead = digitalRead(Contact);

      // if the LED is off turn it on and vice-versa:
      ledStateG[led] = !ledStateG[led]; 
      digitalWrite(arrG[led], ledStateG[led]);
      delay (500); 

      ledStateG[led] = !ledStateG[led];
      digitalWrite(arrG[led], ledStateG[led]);
      delay (500); 

      
      Counter ++; 

      if ( Counter >= 10 && Counter <= 15) // currentMillis - previousMillis >= Interval/2 && currentMillis - previousMillis < Interval)
      {
      digitalWrite(Buzzer, HIGH); 
      delay(400);
      digitalWrite(Buzzer, LOW); 
      delay(100);
      }
          

      if (Counter >= 15) //(currentMillis - previousMillis >= Interval - 1)
      {
      Serial.println(F("Timeout! Please connect fast for longer device life,"));

      char sDecOne1 [9] = "Timeout!" ;
      char sDecOne2 [16] = "Please Connect" ;
      //char sDecOne1 [16] = "Timeout! Please" ;
      //char sDecOne2 [16] = "Connect Faster!" ;      
      char sDecOne3 [13] = "Faster for a" ;  
      char sDecOne4 [14] = "longer device" ; 
      char sDecOne5 [6] = "life," ;
      DPrint (sDecOne1);
      DPrintL (sDecOne2);
      DPrintLL (sDecOne3);
      DPrintLLL (sDecOne4);
      DPrintLLLL (sDecOne5);

      delay (1500); // Change later to 3000 or 2000 ||| 5000 is just for presentation

      u8x8.clear();
     
      Serial.println(F("... and less energy consumption"));

      char sDecTwo1 [13] = "... and less" ;
      char sDecTwo2 [7] = "energy" ;
      char sDecTwo3 [13] = "consumption." ;

      DPrint (sDecTwo1);
      DPrintL (sDecTwo2);
      DPrintLL (sDecTwo3);

      delay (1500); // Change later to 3000 or 2000 ||| 5000 is just for presentation

      u8x8.clear();
      
      return; 
      }
    }
    
    digitalWrite(arrG[led], LOW);


    Serial.println(F("Successfully Connected!"));
    char stars [17] = "****************" ;
    char sConn1 [13] = "Successfully" ;
    char sConn2 [11] = "Connected!" ;
 

    DPrint (stars);

    u8x8.drawString(2, 1, sConn1);
    u8x8.drawString(3, 2, sConn2);


//    DPrintL (sConn);
    DPrintLLL (stars); 
    delay(1500);
    //return;

    u8x8.clear();

  }
  
  Serial.println(F("\n**End Reading**\n"));
  
  char sEndR [16] = "**End Reading**";
  DPrint (sEndR);
  delay(1500);  

  u8x8.clear();
}


