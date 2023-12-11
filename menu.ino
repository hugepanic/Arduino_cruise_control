void level() {
  last_menu = "idle";


  // ### do something

  if (millis() - timer2 > 300) {

    get_angle();

    float mypitch = 0;
    mypitch += pitch;
    delay(50);
    get_angle();
    mypitch += pitch;
    delay(50);
    mypitch += pitch;
    delay(50);

    pitch = mypitch / 3 - angleoffset;

    //pitch = pitch - angleoffset;
    draw_angles2();
    timer2 = millis();
  }

  key = get_key();
  if (key != "n") {
    wait_release();
  }

  if (key == "u") {     //calibrate sensor

    get_angle();
    angleoffset = pitch;
  }
}



void idle() {

  last_menu = "idle";

  lcd.clear();
  lcd.setCursor(0, 0);

  if (stat == 1) {

    lcd.print("ON");
    lcd.setCursor(12, 0);
    lcd.print(int(speed - tspeed));

    lcd.setCursor(0, 1);
    lcd.print(int(servopos));
    lcd.setCursor(5, 1);
    lcd.print(int(offset));

    lcd.setCursor(12, 1);
    lcd.print(int(output_perc));


  }
  else {
    lcd.print("Idle");
    myservo.write(servo_idle);

  }

  lcd.setCursor(5, 0);
  lcd.print(int(speed));




  /*
    for (int i = 0; i <= int(servopos) / 16; i++) {
    // Serial.println(i);
    lcd.print("#");
    }
  */


  key = get_key();
  if (key != "n") {
    wait_release();
  }

  if (key == "s") {     //activate PID & SERVO

    reset_PID();      //reset PID???

    stat = stat * -1;
    //Serial.println("Activate!!");
    tspeed = speed;
  }

  if (key == "u") {
    tspeed += 1;
  }
  if (key == "d") {
    tspeed -= 1;
  }
}





void info() {


  last_menu = "Info";

  lcd.clear();
  lcd.setCursor(0, 0);

  if (stat == 1) {
    lcd.print("Active");
  }
  else {
    lcd.print("Sc=");
    lcd.print(int(speed));
  }


  lcd.setCursor(9, 0);
  lcd.print(speed);
  lcd.setCursor(13, 0);
  lcd.print(int(output));





  lcd.setCursor(0, 1);
  lcd.print(servopos);
  lcd.setCursor(9, 1);
  lcd.print(offset);




  key = get_key();
  if (key != "n") {
    wait_release();
  }

  if (key == "s") {     //activate PID & SERVO
    stat = stat * -1;
    //Serial.println("Activate!!");
    tspeed = speed;
    reset_PID();
  }

}



void settings() {

  if (last_menu != "setting") {
    lcd.blink();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set");
    //lcd.print(set_pos);

    lcd.setCursor(9, 0);
    lcd.print("Kp:");
    lcd.print(kp);

    lcd.setCursor(0, 1);
    lcd.print("Ki:");
    lcd.print(ki * 100); // ACHTUNG: Factor 100 USED  ---------------

    lcd.setCursor(9, 1);
    lcd.print("Kd:");
    lcd.print(kd);



  }
  last_menu = "setting";


  key = get_key();
  if (key != "n") {
    wait_release();
  }



  if (key == "s") {
    // Serial.println("s");
    set_pos = set_pos + 1;
    if (set_pos > 2) {
      set_pos = 0;
    }
  }

  switch (set_pos) {
    case 0:
      lcd.setCursor(9, 0);
      if (key == "u") {
        kp += .1;
        last_menu = "";
      }
      if (key == "d") {
        kp -= .1;
        last_menu = "";
      }
      break;

    case 1:
      lcd.setCursor(0, 1);
      if (key == "u") {
        ki += .001;
        last_menu = "";
      }
      if (key == "d") {
        ki -= .001;
        last_menu = "";
      }
      break;

    case 2:
      lcd.setCursor(9, 1);
      if (key == "u") {
        kd += .01;
        last_menu = "";
      }
      if (key == "d") {
        kd -= .01;
        last_menu = "";
      }
      break;

  }



}








void save_load() {

  last_menu = "save_load";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print("  = save");

  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.print("  = load");

  key = get_key();
  String old_key = key;

  if (key == "u" or key == "d") {
    lcd.clear();
    int i = 0;
    String operation;

    if (key == "u") {
      operation = "saving";
    }

    if (key == "d") {
      operation = "loading";
    }


    do {
      delay(2000 / 16);
      lcd.setCursor(0, 0);
      lcd.print(operation);
      lcd.setCursor(i, 1);
      lcd.print("#");
      i++;
      key = get_key();
    } while (key == old_key);

    if (i > 15) {
      lcd.clear();
      lcd.setCursor(0, 0);


      lcd.print("done ");
      lcd.print(operation);
      if (operation == "saving") {
        save_eeprom();
      } else {
        read_eeprom();

        //Serial.println("read");
      }

      delay(1500);
    }
  }

}
