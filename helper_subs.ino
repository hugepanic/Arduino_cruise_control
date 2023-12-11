void choose_menu() {
  key = get_key();
  // wait_release();

  //Serial.print("read switches");
  //Serial.println(key);

  if (key == "r") {

    //delay(300);
    mode += 1;
    wait_release();
  }
  if (key == "l") {
    //  delay(300);
    mode -= 1;
    wait_release();
  }

  key = "n";


  if (mode < 0) {
    mode = 0;
  }
  if (mode > 3) {
    mode = 3;
  }
}


void brakeloop() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Press BRAKE");
  lcd.setCursor(0, 1);


  int x = 0;

  do {
    delay(100);
    x = analogRead(A1);
  } while (x < 100);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Release BRAKE");
  lcd.setCursor(0, 1);
  lcd.print("to unlock!");
  lcd.blink();

  do {
    delay(100);
    x = analogRead(A1);
  } while (x > 100);

}


void inter() {
  last_kst = kst;
  kst = millis();

  if (kst - last_kst > debouce_delay) {
    interval = kst - last_kst;
  }
  else {
    (last_kst = kst);
  }

}


void reset_PID() {



  myPID.SetOutputLimits(0.0, 1.0);  // Forces minimum up to 0.0
  myPID.SetOutputLimits(-1.0, 0.0);  // Forces maximum down to 0.0
  //PID.SetOutputLimits(PIDMinimum, PIDMaximum);  // Set the limits back to normal}
  myPID.SetOutputLimits(-255, 255);




  //PID myPID(&speed, &servopos, &tspeed, kp, ki * gap_factor, kd, P_ON_M, DIRECT);

  /*
    lcd.clear();
    lcd.setCursor(0, 0);


    lcd.print("RESET_PID");
    delay(250);

  */

}





void save_eeprom() {
  float kpo = EEPROM.read(1) / 10.0;
  float kio = EEPROM.read(2) / 200.0;
  float kdo = EEPROM.read(3) / 100.0;

  if (kpo != kp)  {
    EEPROM.write(1, int(kp * 10));
    //Serial.print("write_KP ");
    //    Serial.println(kp * 10);
  }

  if (kio != ki ) {

    EEPROM.write(2, int(ki * 200));
    //Serial.println("write_KI");
  }

  if (kdo != kd) {
    EEPROM.write(3, int(kd * 100));
    //Serial.println("write_KD");
  }



  if (angleoffset < 0)
  {
    if (EEPROM.read(4) == 0) {
      EEPROM.write(4, 1);
    }
  }
  else {
        if (EEPROM.read(4) == 1) {
      EEPROM.write(4, 0);
    }
  }
  EEPROM.write(5, abs(angleoffset) * 10);

}


void read_eeprom() {
  kp = EEPROM.read(1) / 10.0;
  ki = EEPROM.read(2) / 200.0;
  kd = EEPROM.read(3) / 100.0;


  angleoffset = EEPROM.read(5) / 10;
  if (EEPROM.read(4) == 1); {
    angleoffset *= -1;
  }


}





void get_angle() {


  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 3.09; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - 3.9; // AccErrorY ~(-1.58)

  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX + 3.1; // GyroErrorX ~(-0.56)
  GyroY = GyroY - .02; // GyroErrorY ~(2)
  GyroZ = GyroZ + 0.17; // GyroErrorZ ~ (-0.8)

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;

  // Complementary filter - combine acceleromter and gyro angle values
  //roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  //pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;


  float cfac = .04;  // TEST ml
  //roll =  (1-cfac)* gyroAngleX + cfac * accAngleX;  //TEST ml
  //pitch = (1-cfac) * gyroAngleY + cfac * accAngleY;  //TEST ml


  roll =  accAngleX;
  pitch = accAngleY;
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);

  delay(5000);
}


void draw_angles2() {
  lcd.clear();
  rest = 10 * (pitch - int(pitch));

  if (rest <= -8) {
    lcd.createChar(4, n8);
  }
  else if (rest <= -6) {
    lcd.createChar(4, n6);
  }
  else if (rest <= -4) {
    lcd.createChar(4, n4);
  }
  else if (rest <= -2) {
    lcd.createChar(4, n2);   //n2
  }
  else if (rest <= 2) {
    lcd.createChar(4, p2);
  }
  else if (rest <= 4) {
    lcd.createChar(4, p4);
  }
  else if (rest <= 6) {
    lcd.createChar(4, p6);
  }
  else if (rest <= 8) {
    lcd.createChar(4, p8);
  }

  //delay(50);


  if (pitch < 0) {
    if (rest <= -2) {
      lcd.setCursor(7 + int(pitch), 0);
      lcd.write(4);
    }
    if (pitch < -1) {
      for (int i = 8 + int(pitch); i <= 7; i++) {
        lcd.setCursor(i, 0);
        lcd.write(3);
      }
    }
  }
  else {   //pitch >0

    if (rest >= 2) {
      lcd.setCursor(8 + int(pitch), 0);
      lcd.write(4);
    }
    if (pitch > 1) {
      for (int i = 8; i <= 7 + int(pitch); i++) {
        lcd.setCursor(i, 0);
        lcd.write(3);
      }

    }

  }

  lcd.setCursor(0, 1);
  lcd.print("Angle: ");

  //  lcd.print(rest);

  if (pitch < 0) {
    lcd.setCursor(7, 1);
  }
  else {
    lcd.setCursor(8, 1);
  }
  lcd.print(0.1 * int(pitch * 10));
  //lcd.print(pitch);

}
