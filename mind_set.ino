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
  if ( !OKrcvd("CMD") ) { 
    Serial.println("No SU"); 
    return; 
  }
  BTSerial.flush();

  BTSerial.print( "SR," );
  BTSerial.print( mac );
  BTSerial.print( '\r' );
  if ( !OKrcvd("AOK") ) { 
    Serial.println("No SR"); 
    return; 
  }
  BTSerial.flush();

  BTSerial.print( "SP," );
  BTSerial.print( key );
  BTSerial.print( '\r' );
  if ( !OKrcvd("AOK") ) { 
    Serial.println("No SP"); 
    return; 
  }
  BTSerial.flush();

  BTSerial.print( "SM,3\r" );
  if ( !OKrcvd("AOK") ) { 
    Serial.println("No SM"); 
    return; 
  }
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
  if ( !OKrcvd("CMD") ) { 
    Serial.println("No SU"); 
    return; 
  }
  Serial.flush();

  Serial.print( "SR," );
  Serial.print( mac );
  Serial.print( '\r' );
  if ( !OKrcvd("AOK") ) { 
    Serial.println("No SR"); 
    return; 
  }
  Serial.flush();

  Serial.print( "SP," );
  Serial.print( key );
  Serial.print( '\r' );
  if ( !OKrcvd("AOK") ) { 
    Serial.println("No SP"); 
    return; 
  }
  Serial.flush();

  Serial.print( "SM,3\r" );
  if ( !OKrcvd("AOK") ) { 
    Serial.println("No SM"); 
    return; 
  }
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
  while( Serial.available() < 3 && time < 1000 ) { 
    time++; 
    delay( 1 ); 
  }
#else
  while( BTSerial.available() < 3 && time < 1000 ) { 
    time++; 
    delay( 1 ); 
  }
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


