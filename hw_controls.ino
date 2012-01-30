byte read_Pots () {

 // ORANGE = Bright_Pace
 int pot_orng = analogRead(A0);
 // BLACK = Bright_Length
 int pot_blk = analogRead(A1);
 
 #if DEBUG >= 2

 Serial.print("pot_orng: ");
 Serial.println(pot_orng);
  Serial.print("pot_blk: ");
 Serial.println(pot_blk);
 Serial.println();
 #endif
 
 
 pot_orng = map (pot_orng, 0, 682, 0, 6);
 pot_blk = map (pot_blk, 0, 682, 0, 6);
 
 #if DEBUG >= 2
  Serial.print("pot_orng: ");
 Serial.println(pot_orng);
  Serial.print("pot_blk: ");
 Serial.println(pot_blk);
 Serial.println();
 #endif
 
 default_BrPc = pot_orng;
 default_BrLn = pot_blk;
 
 #if DEBUG >= 2
 Serial.print("default_BrPc: ");
 Serial.println(default_BrPc);
 Serial.print("default_BrLn: ");
 Serial.println(default_BrLn);
 Serial.println();
 #endif 
 

}
