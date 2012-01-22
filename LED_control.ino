
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
  for (j=0; j <= 100; j++) {
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color((r*j)/100,(g*j)/100,(b*j)/100));
    }
  strip.show();
  }
  delay(wait);
}

void fadeDown(uint32_t r, uint32_t g, uint32_t b, uint32_t wait) {
  int i, j;
  for (j=100; j >= 0; j--) {
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
