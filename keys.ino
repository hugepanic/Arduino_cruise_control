String get_key() {
  

  //Serial.println("get_key");
  String key;
  // r=0   u= 130   d=305    l=478    sel=720  nothing = 1023
  int key_sig = analogRead(0);

  //Serial.println(key_sig);

  if (key_sig > 1000) {
    key = "n";
  }
  else if (key_sig > 550) {
    key = "s";
  }
  else if (key_sig > 350) {
    key = "l";
  }
  else if (key_sig > 220) {
    key = "d";
  }
  else if (key_sig > 80) {
    key = "u";
  }
  else  {
    key = "r";
  }


  return key;



}


void wait_release() {
  if (key != "n") {
    do {
      delay(10);
      //Serial.println("Waiting...");
    } while (analogRead(0) < 1000);
  }
}
