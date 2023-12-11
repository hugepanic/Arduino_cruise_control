

void drive_servo() {


  servopos = servopos   + offset;     //deactivated for testing purposes

  output = map(servopos, -255, 255, servo_idle, servo_full);


/////   limit output value
  if (servo_idle < servo_full) {
    output = constrain(output, servo_idle, servo_full);
    output_perc = map(output, servo_idle, servo_full, 0, 100);
  }
  else {
    output = constrain(output, servo_full, servo_idle);
    output_perc = map(output, servo_full, servo_idle,100,0);
  }

  myservo.write(output);
}




void calcspeed() {
  speed = speed_factor / interval;

  if (millis() - kst > 1500 & low_sec == 0) {
    speed = 0;
    stat = -1;
  }

//                    spped_low, speed_high, offset_low, offset_high
  offset = map(tspeed, 40,       70,          0,            90); // map(value, fromLow, fromHigh, toLow, toHigh)
  offset = constrain(offset, -20, 255);
}
