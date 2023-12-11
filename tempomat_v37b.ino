#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Servo.h>
#include <PID_v1.h>
#include <Wire.h>



//    v21 ---> PID + Control
//    v24 ---> rework with new interface
//    v25 ---> working with new servo!!
//    v27  --> new offset method  ---> is working, but the offset is moving with warm engine
//    v30.0 -> new head-off version since 25 to 28.2 are all mixed and fucked up
//    updated parameters and offsets
//    updated PID RESET
//    v35 --> addition of level sensor hard & software
//    v35.1  ---> small updates and corrections
//    v36   ---> addition of up/down tspeed adjustment
//    v37 ---> new Servo
String ver = "v37b";



//stuff for level sensor
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;
int rest = 0;
float angleoffset = 0;
// end of stuff for level sensor



Servo myservo;


int offset;
int output;
int output_perc;


int mode = 0;
int key_sig;
String key;

int stat = -1;

String last_menu;



float kp = 2;
float ki = 10;
float kd = 0;

int set_pos = 0;



double servopos = 0;
int debouce_delay = 20;  // = speed_factor / max_speed
unsigned long interval = 0;

int servo_idle = 140;    //was 30
int servo_full = 10;   // was 170

long timer = 0;
long timer2 = 0;

unsigned long volatile kst = 0;
unsigned long  last_kst = 0;



double speed = 0;
double tspeed = 80;


double speed_factor = 2465 / 1.1 ;


float gap_factor = 1;
float pos_gf = 0.3;     //not used --> 0.3
float neg_gf = 1;



int brake = 0;


byte up[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};


byte down[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};


byte p2[8] = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000
};

byte p4[8] = {
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000
};

byte p6[8] = {
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100
};

byte p8[8] = {
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110
};


byte p10[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte n2[8] = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001
};


byte n4[8] = {
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00011
};

byte n6[8] = {
  0b00111,
  0b00111,
  0b00111,
  0b00111,
  0b00111,
  0b00111,
  0b00111,
  0b00111
};

byte n8[8] = {
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01111
};



const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


PID myPID(&speed, &servopos, &tspeed, kp, ki*gap_factor, kd, P_ON_M, DIRECT);


int low_sec = 0;    //################################################

void setup() {
  lcd.begin(16, 2);
  lcd.clear();

  lcd.createChar(1, up);
  lcd.createChar(2, down);
  lcd.createChar(3, p10);



  lcd.setCursor(0, 0);
  lcd.print("SW: ");
  lcd.print(ver);

  delay(1000);


  read_eeprom();


  pinMode(2, INPUT);    // hall_sensor on digital-pin2
  pinMode(A1, INPUT);  //IVECO brake-contact on Analog-Pin1

  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(2), inter, RISING);
  myservo.attach(3);
  myservo.write(servo_idle);

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-255, 255);


  // -------------  level sensor
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission

  // Call this function if you need to get the IMU error values for your module
  //calculate_IMU_error();
  // --------------  level sensor

  if (low_sec == 0) {
    brakeloop();
    delay(1000);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("LOW SEC MODE");
    delay(2000);
  }
}



void loop() {

  if (analogRead(A1) > 100) {
    brake = 1;
    stat = -1;
  }
  else {
    brake = 0;
  }

  choose_menu();

  if (millis() - timer > 100) {
    noInterrupts();
    calcspeed();

    if (stat == 1 & mode < 2) {
      if (speed - tspeed)
      {
        gap_factor = pos_gf;
      }
      else
      {
        gap_factor = neg_gf;
      }

      myPID.Compute();                      //update PID
      drive_servo();                         // activate servo each loop
    }
    else {
      myservo.write(servo_idle);     //servo goto idle
      tspeed = speed;
      myPID.Compute();
    }

    timer = millis();

    //debug();

    interrupts();

    switch (mode) {
      case 0:
        idle();
        lcd.noBlink();
        break;
      case 1:
        //Serial.println("case1");
        level();
        lcd.noBlink();
        break;
      case 2:
        //Serial.println("case2");
        settings();
        break;
      case 3:
        //Serial.println("case3");
        save_load();
        lcd.noBlink();
        break;
    }
  }

}
