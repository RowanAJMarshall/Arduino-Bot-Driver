#define IR_IN 2
#define IR_OUT 3
#define L_SERVO_STOP 113
#define R_SERVO_STOP 86
#define R_LDR A0
#define C_LDR A1
#define L_LDR A2
#define ITERATOR 10

/**
L_SERVO_STOP + 30 and R_SERVO_STOP - 30 to make robot go forward
**/


#include <EEPROM.h>
#include <Servo.h>

Servo lServo;
Servo rServo;
int leftSpeed = L_SERVO_STOP;
int rightSpeed = R_SERVO_STOP;

/**
  Average values when left and right are light and centre is dark
**/
int leftLDR = 0;
int centreLDR = 0;
int rightLDR = 0;

int left_LDR_range;
int right_LDR_range;


void setup() {
  Serial.begin(9600);
   pinMode(L_LDR, INPUT);
   pinMode(R_LDR, INPUT);
   pinMode(C_LDR, INPUT);
   
   lServo.attach(6);
   rServo.attach(5);
   lServo.write(L_SERVO_STOP);
   rServo.write(R_SERVO_STOP);
      
   pinMode(IR_IN, INPUT);
   pinMode(IR_OUT, OUTPUT);
   
   calibration();


}

void loop() {
  
  while(within_bounds(rightLDR, analogRead(R_LDR), right_LDR_range) == false && within_bounds(leftLDR, analogRead(L_LDR), left_LDR_range) == true)
  {
    turn('r');
    obstruction();
  }
  while(within_bounds(leftLDR, analogRead(L_LDR), left_LDR_range) == false && within_bounds(rightLDR, analogRead(R_LDR), right_LDR_range) == true)
  {
    turn('l');
    obstruction();
  }
  obstruction();
  forward();
  
}

void calibration()
{
  int i;
  
  int l_max = 0;
  int l_min = 10000;
  
  int r_max = 0;
  int r_min = 10000;
  
  int c_max = 0;
  int c_min = 10000;
  
  int l_temp;
  int r_temp;
  int c_temp;
  
  for(i=0;i<ITERATOR;i++)
  {
    l_temp = analogRead(L_LDR);
    r_temp = analogRead(R_LDR);
    c_temp = analogRead(C_LDR);
    
    if(l_temp > l_max) l_max = l_temp;
    if(r_temp > r_max) r_max = r_temp;
    if(c_temp > c_max) c_max = c_temp;
    
    if(l_temp < l_min) l_min = l_temp;
    if(r_temp < r_min) r_min = r_temp;
    if(c_temp < c_min) c_min = c_temp;
    
    leftLDR = leftLDR + l_temp;
    centreLDR = centreLDR + c_temp;
    rightLDR = rightLDR + r_temp;

    delay(50);
  }
  
  
  left_LDR_range = l_min/10;
  right_LDR_range = r_min/10;
  
  leftLDR = leftLDR/ITERATOR;
  centreLDR = centreLDR/ITERATOR;
  rightLDR = rightLDR/ITERATOR;
  
  left_LDR_range = (leftLDR - centreLDR)/2; //50 also works
  right_LDR_range = (rightLDR - centreLDR)/2;//50 also works
}

bool within_bounds(int centre, int num, int bound)
{
  if(num < centre - bound) return false;
  else{return true;}
}

void forward()
{
  lServo.write(L_SERVO_STOP + 30);
  rServo.write(R_SERVO_STOP - 30);
}

void stop()  //HAMMERTIME
{
  lServo.write(L_SERVO_STOP);
  rServo.write(R_SERVO_STOP);
}

void turn(char c)
{
  int L_go_forward = L_SERVO_STOP; //+ 30;
  int R_go_forward = R_SERVO_STOP;//- 30;
  if(c == 'l')
  {
    lServo.write(L_go_forward - 30);
    rServo.write(R_go_forward - 30);
  }
  else if(c == 'r')
  {
    lServo.write(L_go_forward + 30);
    rServo.write(R_go_forward + 30);
  }
}

void obstruction()
{
  tone(IR_OUT, 38000);
  
  Serial.println(digitalRead(IR_IN));
  
  while(digitalRead(IR_IN) == 0)
  {
    
    stop();
  }
  forward();
  
}
