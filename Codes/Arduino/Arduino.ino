/*------ Arduino Fire Fighting Robot Code----- */
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 20 chars and 4 lines display
SoftwareSerial nodemcu(12, 13);//establishing the connection between arduino and nodemce
Servo myservo;
int pos = 0;    
boolean fire = false;
int sign=0;
const int setDistance = 30; // Threshold distance for stopping
String message = "Welcome to fire detection system";
/*-------defining Inputs------*/
#define Left_S 9      // left sensor
#define Right_S 10      // right sensor
#define Forward_S 8 //forward sensor
#define echo A2 // echo pin for ultrasonic sensor
#define trig A3 // Trigger pin for ultrasonic sensor
/*-------defining Outputs------*/
#define LM1 2       // left motor
#define LM2 3       // left motor
#define RM1 4       // right motor
#define RM2 5       // right motor
#define pump 6
void setup()
{
  pinMode (trig, OUTPUT);
  pinMode (echo, INPUT);
  pinMode(Left_S, INPUT);
  pinMode(Right_S, INPUT);
  pinMode(Forward_S, INPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(pump, OUTPUT);
  myservo.attach(11);
  myservo.write(90); 
  lcd.init();       // initialize the lcd
  lcd.backlight();
  lcd.setCursor(2, 0); // Set cursor to the beginning of line 0
  lcd.print("  Welcome"); // Print the message initially
  Serial.print("Welcome");
  delay(1000); // Print the message initially
  Serial.begin(9600);
  nodemcu.begin(9600);
  delay(1000);
  Serial.println("Program started");
  for (pos = 0; pos <= 180; pos ++) { 
      myservo.write(pos);
      delay(15);
  }
  digitalWrite(LM1, HIGH);
  digitalWrite(LM2, LOW);
  digitalWrite(RM1, HIGH);
  digitalWrite(RM2, LOW);

}
void put_off_fire(int sta,int end)
{
    lcd.clear();
    delay (500);
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, HIGH);
    digitalWrite(pump,HIGH); 
    delay(500);
    lcd.setCursor(1,0);
    lcd.print("Fire Detected!");
    Serial.println("Fire Detected!");
    if(sta==90){
      lcd.setCursor(5,1);
      lcd.print("Left..");
      Serial.print("Left..");
      transfer_data(1,"Left..");
    }
    else if(sta==45){
      lcd.setCursor(3,1);
      lcd.print("Straight..");
      Serial.println("Straight..");
      transfer_data(1,"Straight..");
    }
    else if(sta==0)
    {
      lcd.setCursor(5,1);
      lcd.print("Right..");
      Serial.print("Right..");
      transfer_data(1,"Right..");
    }
    for (pos = sta; pos <= end; pos ++) { 
      myservo.write(pos);
      digitalWrite(pump,HIGH);
      delay(15);
      digitalWrite(pump,LOW);
    }
    for (pos = end; pos>=sta; pos--) { 
      digitalWrite(pump,HIGH);
      myservo.write(pos);
      delay(15);
      digitalWrite(pump,LOW);
    }
    digitalWrite(pump,LOW);
    digitalWrite(pump,LOW);
    lcd.clear();
    fire=false;
}
int jk;
void loop()
{
  digitalWrite(pump,LOW);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Searching..");
  Serial.println("Searching..");
  Serial.print("Distance :");
  int distance=getDistance();
  Serial.println(distance);
  if (digitalRead(Forward_S) ==0) //If Fire is straight ahead
  {
    //Move the robot forward
    digitalWrite(LM1, HIGH);
    digitalWrite(RM1, HIGH);
    delay(1000);
    digitalWrite(LM2, LOW);
    digitalWrite(RM2, LOW);
    fire = true;
    sign=1;
  }
  else if (digitalRead(Left_S) ==0) //If Fire is to the left
  {
    //Move the robot left
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, HIGH);
    fire = true;//a=90 to 180
    sign=2;
  }
  else if (digitalRead(Right_S) ==0) //If Fire is to the right
  {
    //Move the robot right
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, LOW);
    fire = true;
    sign=3;
  }
  else if ((digitalRead(Left_S) ==1 && digitalRead(Right_S)==1 && digitalRead(Forward_S) ==1) && ((distance<30)&&(distance!=2))) //If Fire not detected all sensors are zero
  {
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, LOW);
    digitalWrite(LM1, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM2, HIGH);
    Serial.print("Distance :  ");
    Serial.println(distance);
    fire=false;
    transfer_data(0,"null");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Object Detected.");
  }
  else if(digitalRead(Left_S) ==1 && digitalRead(Right_S)==1 && digitalRead(Forward_S) ==1)
  {
    
    digitalWrite(LM1, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(LM2, LOW);
    digitalWrite(RM2, LOW);
    digitalWrite(LM1, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM2, HIGH);
    fire=false;
  }
    delay(300); //Slow down the speed of robot
     while(fire == true)
     {
         if(sign==1){
            put_off_fire(45,135);
         }
         else if(sign==2){
          //  put_off_fire(0,90);
          put_off_fire(90,180);
         }
         else if(sign==3){
          //  put_off_fire(90,180);
           put_off_fire(0,90);
         }
     }
     fire=false;
}
void transfer_data(int a,String direction){
  StaticJsonDocument<1000> jsonBuffer;
  JsonObject data = jsonBuffer.to<JsonObject>();
  data["status"]=a;
  data["direction"]=direction;
  serializeJson(data, nodemcu);
  jsonBuffer.clear();
}
int getDistance()
{
  digitalWrite (trig, LOW);
  delayMicroseconds (2);
  digitalWrite (trig, HIGH);
  delayMicroseconds (10);
  float duration= pulseIn (echo, HIGH);
  float distance;
  distance= (0.0061*duration) + 2.5923;
  return distance;
}
