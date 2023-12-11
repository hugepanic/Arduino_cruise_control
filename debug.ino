void debug() {
  Serial.print("output: ");
  Serial.println(output);
}


void debug1() {


  Serial.print("Debug: ");
  Serial.print(stat);
  Serial.print("\t");
  Serial.print(mode);
  Serial.print("\t");
  Serial.print(key);
  Serial.print("\t");
  Serial.print(speed);
  Serial.print("\t");
  Serial.print(servopos);
  Serial.print("\t");


  Serial.print(kp);
  Serial.print("\t");

  Serial.print(ki * 10);
  Serial.print("\t");

  Serial.print(kd);
  Serial.print("\t");

  Serial.println(millis());

}
