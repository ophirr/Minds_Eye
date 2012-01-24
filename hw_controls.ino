byte read_Pots () {

 // ORANGE = Bright_Pace
 int pot_orng = analogRead(A0);
 // BLACK = Bright_Length
 int pot_blk = analogRead(A1);
 
 #if DEBUG >= 1
 Serial.print("pot1: ");
 Serial.println(pot_blk);
 Serial.print("pot2: ");
 Serial.println(pot_orng);
 Serial.println();
 #endif
 
 
 pot_orng = map (pot_orng, 10, 16, 0, 682);
 pot_blk = map (pot_orng, 0, 201, 0, 682);
 
 if (default_BrLn != pot_blk) {
   Serial.print("default_BrLn: ");
   Serial.println(default_BrLn);
   Serial.println();
 } else if (default_BrPc != pot_orng) {
   Serial.print("default_BrPc: ");
   Serial.println(default_BrPc);
   Serial.println();
 }
 
 default_BrPc = pot_orng;
 default_BrLn = pot_blk;
 
 

}
