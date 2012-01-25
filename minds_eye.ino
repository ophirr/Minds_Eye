/* Mind's Eye - EEG interface to a series of LED lights mounted
 ** in a windox box frame. EEG data via a Neurosky Bluetooth Headset
 
 ** Several portions of this code are borrowed and 
 ** modified from the NeuroSky Arduino project files and the Hex Machina
 ** Mindbullets code sample.
 */

#include "LPD8806.h"
#include <SPI.h>

#define MACADDR 0013ef0048b1
#define BTBAUDRATE 57600

// Number below which we consider the signal to be good, 1-255.
// Lower non-zero numbers denote better signal. 
#define POOR_QUALITY_THRESHOLD 200

// Set to 1 to program the BlueSMiRF to search automatically
#define PROGRAM 0

// Set to 1 to have the BlueSMiRF search just once on start
#define ONTHEFLY 0

// Set to 1 to enable pass-through terminal mode for querying 
// the BlueSMiRF
#define TERMINAL 0

// Set to 0 to use SoftwareSerial--note this is not recommended
// for gathering actual data from the EEG headset
#define HARDSERIAL 1

// Increase non-zero values for increasing levels of debug info
#define DEBUG 0

// Make sure you've downloaded and installed the 
// SoftwareSerial library prior to enabling
#if HARDSERIAL == 0
#include <SoftwareSerial.h>
#endif

char mac[13] = "0013ef0048b1";
char key[5] = "0000";
#if HARDSERIAL != 0
int RX = 0;
int TX = 1;
int dataPin = 2;
int clockPin = 3;
#else
int RX = 2;
int TX = 3;
#endif
int BTPWR = 7;
int ATT_LED = 10;
int MED_LED = 11;
int STATUS = 13;

int att;

// Brightness tuning (we'll want a photo resistor on the frame to make the LEDs brightness match ambient comfortably
int MaxBright = 50;
int MinBright = 10;

int BrightLength = 0;
int BrightPace = 0;

int default_BrLn = 0;
int default_BrPc = 0;

// The following definitions are pqart of the NeuroSky sample code
// checksum variables
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = {
  0};
byte poorQuality = 0;
byte eegval = 0;
byte attention = 0;
byte meditation = 0;

int attrcvd = 0;
int medrcvd = 0;
int eegvalready = 0;

byte oldeegval = 150;

long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 800;           // interval at which to blink (milliseconds)

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;
// End NeuroSky sample vars
#if HARDSERIAL == 0
SoftwareSerial BTSerial(RX,TX);
#endif

// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip
LPD8806 strip = LPD8806(34);

void setup()
{
  Serial.begin(57600);
#if HARDSERIAL == 0
  BTSerial.begin(BTBAUDRATE);
#endif
  pinMode( RX, INPUT );
  pinMode( TX, OUTPUT );
  pinMode( STATUS, OUTPUT );
  pinMode( BTPWR, OUTPUT );
  pinMode( ATT_LED, OUTPUT );
  pinMode( MED_LED, OUTPUT );

  digitalWrite( BTPWR, HIGH );
  delay( 500 );
#if TERMINAL != 1
#if PROGRAM == 1
  BTProgram();
#endif
#if ONTHEFLY == 1
  BTInit();
#endif
#endif
  blink( 3, 200 );
#if DEBUG >= 1
  Serial.println("Finished Setup. Ready for LOOP");
#endif

  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
  
  int i = 0;
  // Eyeball
  for (i=28; i < 30; i++) {
      //strip.setPixelColor(i, Wheel( 280));
      rainbow(3);
  }
 
}

void loop()
{
#if TERMINAL == 1 && HARDSERIAL == 0
  BTSerial.print("$$$");
#if DEBUG >= 1
  Serial.println("Issuing $$$");
#endif
  while ( true )
  {
    BluetoothConduit();
    delay( 2 );
  }
#endif

  unsigned long currentMillis = millis();
  
  readNeuroValues();

  //Serial.print("right after neurovalues meditation: ");
  // Serial.println(meditation);

#if DEBUG >= 2
  Serial.print("currentMillis: ");
  Serial.println(currentMillis);
  Serial.print("previousMillis: ");
  Serial.println(previousMillis);
  Serial.print("Interval: ");
  Serial.println(interval);
  Serial.println();
#endif

  //if((currentMillis - previousMillis) > interval ) {

  if((currentMillis - previousMillis) > (interval + (medrcvd * 10) )) {
   
    // save the last time we hit interval 
    previousMillis = currentMillis;    

 
#if DEBUG >=1
    Serial.print("Interval Time exceeded:"); 
    Serial.println(currentMillis - previousMillis);
#endif

    if (eegvalready) {
      
      // Read the pots for BrightLength and BrightPace
       read_Pots();
 

#if DEBUG >=1
      Serial.print("attention: ");
      Serial.println(attrcvd);
      Serial.print("meditation: ");
      Serial.println(medrcvd);
#endif

      Serial.print("meditation: ");
      Serial.println(medrcvd);
      BrightLength = (BrightLength + (medrcvd * default_BrLn));
      Serial.print("BrightLength: ");
      Serial.println(BrightLength);  
    // BrightPace = (BrightPace + (medrcvd / default_BrPc));
     BrightPace = (BrightPace + (medrcvd / 30));
      Serial.print("BrightPace: ");
      Serial.println(BrightPace);  
      Serial.println();

      // TURN ON LEDS
      if ( medrcvd > 1 ) { 
        burst(medrcvd); 
      }
      //if ( meditation > 1 ) { rainbow(meditation,10); }

      // reset eegValReady until next time we have data
      eegvalready = 0;
    }
  }

  // Reset BrightLength
  BrightLength = 0;
 
  // Reset BrightLength
  BrightPace = 0;
}





