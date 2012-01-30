
// Burst LEDs

void burst(int eegval)
{
#if DEBUG >=1
  Serial.print ("Got eegval: ");
  Serial.println(eegval);
#endif

  if (eegval <= 15 ) {
    fadeUp(127,0,0, BrightLength);		// red
    fadeDown(127,0,0, BrightLength);		// red
  } 
  else if (eegval <= 30) {
    fadeUp(127, 127,0, BrightLength);
    fadeDown(127, 127,0, BrightLength);		// orange
  } 
  else if (eegval <= 45) {
    fadeUp(0, 127,0, BrightLength);
    fadeDown(0, 127,0, BrightLength);		// green
  } 
  else if (eegval <= 60) {
    fadeUp(0, 127,127, BrightLength);
    fadeDown(0, 127,127, BrightLength);		// teal
  } 
  else if (eegval <= 75) {
    fadeUp(0, 0,127, BrightLength);		// blue
    fadeDown(0, 0,127, BrightLength);		// blue
  } 
  else {
    fadeUp(127,0,127,BrightLength);		// violet
    fadeDown(127,0,127,BrightLength);		// violet
  }

}


void fadeUp(uint32_t r, uint32_t g, uint32_t b, uint32_t wait) {
  int i, j;
  for (j=MinBright; j <= MaxBright; j++) {
    for (i=0; i < (strip.numPixels()-10); i++) {
      strip.setPixelColor(i, strip.Color((r*j)/100,(g*j)/100,(b*j)/100));
    }
  strip.show();
  delay(BrightPace);
  }
  delay(wait);
}

void fadeDown(uint32_t r, uint32_t g, uint32_t b, uint32_t wait) {
  int i, j;
  for (j=MaxBright; j >= MinBright; j--) {
    for (i=0; i < (strip.numPixels()-10); i++) {
      strip.setPixelColor(i, strip.Color((r*j)/100,(g*j)/100,(b*j)/100));  // turn all pixels on
    }
  strip.show();
  delay(BrightPace);
  }
  delay(wait);
}

void turnAllOn(uint32_t c, uint32_t wait) {
  int i;  
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);  // turn all pixels on
  }
  strip.show();   // write all the pixels out
  delay(wait); 
}

void newrainbow(uint8_t eegval, uint8_t wait) {
	int i, j;

    Serial.print("realttention: ");
    Serial.println(eegval);
    eegval = map(eegval, 0, 101, 385, 0);
  
    Serial.print("eegval: ");
    Serial.println(eegval);
    Serial.print("oldeegval: ");
    Serial.println(oldeegval);
    Serial.println();
    
    if (oldeegval >= eegval) {
	for (j=oldeegval; j > eegval; j--) {			// 3 cycles of all 384 colors in the wheel
		for (i=0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel( j % 384));
		}	 
		strip.show();		// write all the pixels out
		delay(wait);
	}
    } else if (oldeegval <= eegval) {
      for (j=oldeegval; j < eegval; j++) {			// 3 cycles of all 384 colors in the wheel
		for (i=0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel( j % 384));
		}	 
		strip.show();		// write all the pixels out
		//delay(wait);
      }
    }
  oldeegval = eegval;
}

void rainbow(uint8_t wait) {
	int i, j;
	 
	for (j=0; j < 384; j++) {			// 3 cycles of all 384 colors in the wheel
		for (i=28; i < 30; i++) {
			strip.setPixelColor(i, Wheel( (i + j) % 384));
		}	 
		strip.show();		// write all the pixels out
		delay(wait);
	}
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  } 
  
  for (i=0; i < strip.numPixels()-10; i++) {
    
      strip.setPixelColor(i, c);
      if (i == 0) { 
        strip.setPixelColor(strip.numPixels()-1, 0);
      } else {
        strip.setPixelColor(i-1, 0);
      }
      strip.show();
      delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  // for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
   for (j=0; j < 384; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

void eyeball() {
 int i = 0;
  // Eyeball
  for (i=28; i < 30; i++) {
     strip.setPixelColor(i, Wheel( 280));
     // rainbow(3);
  }
   strip.show();
}

void fillUpTheEye(uint32_t c, uint8_t wait) {
  int i,j;
      strip.setPixelColor(80, c);
      delay(wait);
         for (i = 1; i <= 20; i++) {
           strip.setPixelColor(80+i, c);
           strip.setPixelColor(80-i, c);
           strip.show();   // write all the pixels out
           delay(wait);
           }
         for (j=0; j<=29; j++) {
           strip.setPixelColor(j, c);
           strip.setPixelColor(59-j, c);
           strip.show();   // write all the pixels out
           delay(wait);
           }  
}

