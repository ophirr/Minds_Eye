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
byte MaxBright = 50;
byte MinBright = 10;
byte BrightLength = 100;

// The following definitions are pqart of the NeuroSky sample code
// checksum variables
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte eegval = 0;
byte attention = 0;
byte meditation = 0;
byte attrcvd = 0;
byte medrcvd = 0;
byte eegvalready = 0;

byte oldeegval = 150;

long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;
// End NeuroSky sample vars
#if HARDSERIAL == 0
  SoftwareSerial BTSerial(RX,TX);
#endif

  // Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
  // The LED strips are 32 LEDs per meter but you can extend/cut the strip
  LPD8806 strip = LPD8806(8);
  
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
  
  if((currentMillis - previousMillis) > interval ) {
    // save the last time you blinked the LED 
    
    #if DEBUG >=1
    Serial.print("Interval Time exceeded:"); 
    Serial.println(currentMillis - previousMillis);
    #endif
    
    previousMillis = currentMillis;    
   
    if (eegvalready) {
      
      #if DEBUG >=1
      Serial.print("attention: ");
      Serial.println(attrcvd);
      Serial.print("meditation: ");
      Serial.println(medrcvd);
      #endif
      
      // TURN ON LEDS
      if ( medrcvd > 1 ) { fire(medrcvd); }
      //if ( meditation > 1 ) { rainbow(meditation,10); }
       
      // reset eegValReady until next time we have data
      eegvalready = 0;
    }
  }
    
    // Serial.println("Would have triggered Fire\n");
}

#if HARDSERIAL == 0
/* This passes commands from the Serial terminal in the
** Arduino IDE to the attached Bluetooth module and relays
** responses from the Bluteooth module back to the IDE.
** SoftwareSerial is required for this.
*/
void BluetoothConduit()
{
  int count = 0;
  if (Serial.available())
  {
    Serial.print("> ");
    while (Serial.available())
    {
      count++;
      char a = Serial.read();
      BTSerial.print(a);
      Serial.print(a);
    }
    BTSerial.print('\r');
    BTSerial.flush();
    Serial.println();
  }
  delay( 20 );
  count = 0;
  if (BTSerial.available())
  {
    Serial.print("< ");
    while (BTSerial.available())
    {
      count++;
      char a = BTSerial.read();
      Serial.print(a);
    }
    Serial.println();
    Serial.flush();
  }
}
#endif

/* Tries to manually connect to the MAC address, rather than
** programming the Bluetooth module. Good for one-shot solutions.
*/
void BTInit()
{
  Serial.flush();
  Serial.print( "$$$" );
  delay( 150 );
  
  Serial.flush();
  
  Serial.print( "C," );
  Serial.print( mac );
  Serial.print( '\r' );
  Serial.flush();
  
  Serial.print( "---" );
  Serial.print( '\r' );
  delay( 150 );
  #if DEBUG >= 1
    Serial.println("ON THE FLY: Init-ed\n");
  #endif
}

/* Programs the Bluetooth module to automatically go into
** Bluetooth master mode and scan for a given MAC address.
** After programming the Bluetooth module, the Arduino will
** have to be reprogrammed to interpret the data coming in.
** This is heavily modified from the NeuroSky sample code.
** (Portions (C) 2011 NeuroSky)
*/
void BTProgram()
{
#if HARDSERIAL == 0
  BTSerial.print( "$$$" );
  delay( 150 );
  
  BTSerial.print( "SU,57\r" );
  if ( !OKrcvd("CMD") ) { Serial.println("No SU"); return; }
  BTSerial.flush();
  
  BTSerial.print( "SR," );
  BTSerial.print( mac );
  BTSerial.print( '\r' );
  if ( !OKrcvd("AOK") ) { Serial.println("No SR"); return; }
  BTSerial.flush();
  
  BTSerial.print( "SP," );
  BTSerial.print( key );
  BTSerial.print( '\r' );
  if ( !OKrcvd("AOK") ) { Serial.println("No SP"); return; }
  BTSerial.flush();
  
  BTSerial.print( "SM,3\r" );
  if ( !OKrcvd("AOK") ) { Serial.println("No SM"); return; }
  BTSerial.flush();
  
  BTSerial.print( "---\r" );
  delay( 150 );
  #if DEBUG >= 1
    Serial.println("Soft Serial: Init-ed");
  #endif
#else
  Serial.print( "---\r" );
  Serial.print( "$$$" );
  delay( 150 );
  
  Serial.print( "SU,57\r" );
  if ( !OKrcvd("CMD") ) { Serial.println("No SU"); return; }
  Serial.flush();
  
  Serial.print( "SR," );
  Serial.print( mac );
  Serial.print( '\r' );
  if ( !OKrcvd("AOK") ) { Serial.println("No SR"); return; }
  Serial.flush();
  
  Serial.print( "SP," );
  Serial.print( key );
  Serial.print( '\r' );
  if ( !OKrcvd("AOK") ) { Serial.println("No SP"); return; }
  Serial.flush();
  
  Serial.print( "SM,3\r" );
  if ( !OKrcvd("AOK") ) { Serial.println("No SM"); return; }
  Serial.flush();
  
  Serial.print( "---\r" );
  delay( 150 );
  #if DEBUG >= 1
    Serial.println("Hard Serial: Init-ed");
  #endif
#endif
}

/* Checks to make sure a given three-character response is 
** received from the Bluetooth module. Heavily modified from
** NeuroSky sample code. (Portions (C) 2011 NeuroSky)
*/
boolean OKrcvd(char cmd[])
{
  int time = 0;
  #if HARDSERIAL != 0
    while( Serial.available() < 3 && time < 1000 ) { time++; delay( 1 ); }
  #else
    while( BTSerial.available() < 3 && time < 1000 ) { time++; delay( 1 ); }
  #endif
  if ( time >= 1000 )
  {
    digitalWrite( BTPWR, LOW );
    Serial.println( "Timeout on msg" );
    return false;
  }
  char str[3];
  for ( int i = 0; i < 3; i++ )
    #if HARDSERIAL != 0
      str[ i ] = (char) Serial.read();
    #else
      str[ i ] = (char) BTSerial.read();
    #endif
  if (str[ 0 ] != cmd[ 0 ] || str[ 1 ] != cmd[ 1 ] || str[ 2 ] != cmd[ 2 ] )
  {
    digitalWrite( BTPWR, LOW );
    Serial.print( "Got wrong msg: " );
    Serial.print( str );
    Serial.print( ", expected " );
    Serial.print( cmd );
    Serial.println();
    return false;
  }
  return true;
}

// A basic blink algorithm.
void blink( int times, int duration )
{
  for ( int i = 0; i < times; i++ )
  {
    digitalWrite( STATUS, HIGH );
    delay( duration );
    digitalWrite( STATUS, LOW );
    delay( duration );
  }
}

/* Reads in one byte from the chosen serial interface.
** Lifted almost directly from the NeuroSKy sample code.
** (Portions (C) 2011 NeuroSky)
*/
byte ReadOneByte() {
  int ByteRead;

  #if HARDSERIAL != 0
    while(!Serial.available());
    ByteRead = Serial.read();
  #else
    while(!BTSerial.available());
    ByteRead = BTSerial.read();
  #endif

  return ByteRead;
}

/* Reads in data coming from the NeuroSky headset, checks 
** for integrity, and then assigns values to the attention
** and meditation values if everything checks out. Debugging
** and LED status added by Hex Machina, all else is NeuroSky 
** sample code. (Portions (C) 2011 NeuroSky)
*/
void readNeuroValues() {
     #if DEBUG >= 2
          Serial.print( "Entered readNeuroValues\n" );
    #endif
  // Look for sync bytes
  if(ReadOneByte() == 170) {
    #if DEBUG >= 2
      Serial.print("-");
    #endif
    if(ReadOneByte() == 170) {
      #if DEBUG >= 2
        Serial.println("+");
      #endif

      payloadLength = ReadOneByte();
      if(payloadLength > 169)                      //Payload length can not be greater than 169
      {
        #if DEBUG >= 1
          Serial.print( "Payload length excessive: " );
          Serial.println( payloadLength );
        #endif
        return;
      }

      generatedChecksum = 0;        
      #if DEBUG >= 3
        Serial.print( "Reading payload... " );
      #endif
      for(int i = 0; i < payloadLength; i++) {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }
      #if DEBUG >= 3
        Serial.println( "Done" );
      #endif

      checksum = ReadOneByte();                      //Read checksum byte from stream      
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

      if(checksum == generatedChecksum) {

        poorQuality = POOR_QUALITY_THRESHOLD;
        attention = 0;
        meditation = 0;

        for(int i = 0; i < payloadLength; i++) {    // Parse the payload
          switch (payloadData[i]) {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            #if DEBUG >= 1
              Serial.print( poorQuality, DEC );
              Serial.print( ": " );
            #endif
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;
            break;
          default:
            break;
          } // switch
        } // for loop

        bigPacket = false;
      }
      else {
        #if DEBUG >= 2
          Serial.println("Bad checksum!");
        #endif
      }  // end if else for checksum
      
      #if DEBUG >= 1
        if ( attention > 0 || meditation > 0 )
        {
          Serial.print( attention, DEC );
          Serial.print( ", " );
          Serial.println( meditation, DEC );
        }
      #endif
    
      // calculate LED 
      //att = attention * 2 + 55;
      att = attention;
      #if DEBUG >= 2
        if (att > 55) {
        // Adjust status LEDs
        analogWrite( ATT_LED, att );
        analogWrite( MED_LED, meditation * 2 + 55 );
        Serial.print ("attention LED: ");
        Serial.println(att);
        }
      #endif  
    } // end if read 0xAA byte
  } // end if read 0xAA byte
     #if DEBUG >= 2
          Serial.print( "Leaving readNeuroValues\n" );
    #endif
    if (attention > 0 || meditation > 0) { 
        eegvalready = 1; 
        attrcvd = attention;
        medrcvd = meditation;    
    }
//    Serial.println("readneurovalues");
//    Serial.print("attention: ");
//    Serial.println(attention);
//    Serial.print("meditation: ");
//    Serial.println(meditation);
}

/* Fire the Nerf Stampede. Because of the two-relay system,
** the ground connection between the trigger and the motor 
** must be broken first (FIRE_READY is brought high), and
** then the fire command is issued (FIRE is brought high).
** To stop, the ground connection between the trigger and
** motor must be re-enabled (FIRE_READY and FIRE both 
** brought low simultaneously).
*/
void fire(int eegval)
{
   #if DEBUG >=1
   Serial.print ("Got eegval: ");
   Serial.println(eegval);
   #endif
   
      if (eegval <= 15 ) {
    fadeUp(127,0,127,BrightLength);		// violet
    fadeDown(127,0,127,BrightLength);		// violet
   } else if (eegval <= 30) {
    fadeUp(0, 0,127, BrightLength);		// blue
    fadeDown(0, 0,127, BrightLength);		// blue
   } else if (eegval <= 45) {
     fadeUp(0, 127,127, BrightLength);
     fadeDown(0, 127,127, BrightLength);		// teal
   } else if (eegval <= 60) {
     fadeUp(0, 127,0, BrightLength);
     fadeDown(0, 127,0, BrightLength);		// green
   } else if (eegval <= 75) {
     fadeUp(127, 127,0, BrightLength);
     fadeDown(127, 127,0, BrightLength);		// orange
   } else {
     fadeUp(127,0,0, BrightLength);		// red
     fadeDown(127,0,0, BrightLength);		// red
  }

}


