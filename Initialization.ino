/*declaring-------------------------------------------------------------------------------------*/
//control of the motors
const int servoPin = 10;      //pin of the servo
int posTheta_initial = 0;     //the initial position of servo
//working status indicator: whether initializing or working
int led1 = 23;                //orange: adjusting stepper motor
int led2 = 25;                //blue: ready for LIDAR
int led3 = 27;                //red: adjusting servo
int t = 0;
/*-------------------------------------------------------------------------------------declaring*/

/*control functions of motors-------------------------------------------------------------------*/
void servo(int angle)
{  for(int i=0;i<50;i++)
   {  int pulsewidth = (angle * 11) + 500;
      digitalWrite(servoPin, HIGH);   
      delayMicroseconds(pulsewidth);
      digitalWrite(servoPin, LOW);
      delayMicroseconds(20000 - pulsewidth);
   }
   delay(100);
}//homemade function to replace <servo.h> to solve the colision with the LIDAR
void anticlockwise_test(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 2; i < 6; i++)
      {  digitalWrite(i, HIGH);
         delay(20);
         digitalWrite(i, LOW);}}
}//the control of the stepper motor(anti-clockwise) when initialing
void clockwise_test(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 5; i > 1; i--)
      {  digitalWrite(i, HIGH);
         delay(20);
         digitalWrite(i, LOW);}}
}//the control of the stepper motor(clockwise) when initialing
/*-------------------------------------------------------------------control functions of motors*/


/*functions run once----------------------------------------------------------------------------*/
void setup() 
{  
   pinMode(7,INPUT);          //switcher
   pinMode(led1,OUTPUT);      //orange
   pinMode(led2,OUTPUT);      //blue
   pinMode(led3,OUTPUT);      //red
   pinMode(servoPin, OUTPUT); //servoPIN
   for (int i=2;i<6;i++)      //stepper motor
   {pinMode(i,OUTPUT);}
}
/*----------------------------------------------------------------------------functions run once*/

/*main loop-------------------------------------------------------------------------------------*/
void loop()
{
   int n = digitalRead(7); //read of the switcher
   if (n == HIGH) //begin initialization
   {delay(500);t = t+1;n = 0;}
//orange light on: initializing the stepper motor
   while (t==1) 
   {
     digitalWrite(led3,LOW);
     digitalWrite(led1,HIGH);n =digitalRead(7);
     if (n == HIGH)
      {delay(500);t = t+1;digitalWrite(led1,LOW);} //exit
     if (analogRead(0) < 500)
      {anticlockwise_test((500-analogRead(0))*5/1023);}
     if (analogRead(0) > 700 )
      {clockwise_test(analogRead(0)*5/1023);}
    }    
//red light on:  initializing the servo
    while (t == 2) 
    { 
      digitalWrite(led3,LOW);
      digitalWrite(led2,HIGH);n =digitalRead(7);
      if (n == HIGH)
      {delay(500);t = 0;digitalWrite(led2,LOW);} //exit
      posTheta_initial = map(analogRead(0),0,1023,0,179);
      servo(posTheta_initial); //set the servo to exit of initialization  a long press
    }
//blue light on: ready for LIDAR
    digitalWrite(led3,HIGH);
}
/*-------------------------------------------------------------------------------------main loop*/
