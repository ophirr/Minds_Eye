
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

void fadeUp(uint32_t r, uint32_t g, uint32_t b, uint32_t wait) {
  int i, j;
  for (j=MinBright; j <= MaxBright; j++) {
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color((r*j)/100,(g*j)/100,(b*j)/100));
    }
  strip.show();
  }
  delay(wait);
}

void fadeDown(uint32_t r, uint32_t g, uint32_t b, uint32_t wait) {
  int i, j;
  for (j=MaxBright; j >= MinBright; j--) {
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color((r*j)/100,(g*j)/100,(b*j)/100));  // turn all pixels on
    }
  strip.show();
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

void rainbow(uint8_t eegval, uint8_t wait) {
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


