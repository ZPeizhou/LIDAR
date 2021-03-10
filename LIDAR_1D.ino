int dist;      //LiDAR actually measured distance value
int strength;  //LiDAR signal strength
int check;     //check numerical value storage 
int i;
int uart[9];   //store data measured by LiDAR
String clearReceived; //used to clear old data
const int HEADER=0x59;  //data package frame header
float posPhiGain = 5.625;   //the rotation of stepper motor                           
                            //! make sure it's an int after *256/180 !
float PhiRANGE = 112.5;     //the detecting range of Phi HORIZONTAL(stepper mootor)
float posPhi = 0;
bool ojbk = 1;


void anticlockwise(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 2; i < 6; i++)
      {  digitalWrite(i, HIGH);
         delay(3);
         digitalWrite(i, LOW);}}
}
void clockwise(int num)
{  for (int count = 0; count < num; count++)
   {  for (int i = 5; i > 1; i--)
      {  digitalWrite(i, HIGH);
         delay(3);
         digitalWrite(i, LOW);}}
}

void LIDAR_read(float posPhi)
{
   while(ojbk)
   {
      if (Serial1.available())//check whether the serial port has data input
      {
         for (int k=1; k<=10; k++)
         {
          clearReceived = String(clearReceived) + String(char(Serial1.read()));
          delay(1);
         }
         clearReceived = "";
         if(Serial1.read()==HEADER)//determine data package frame header 0x59
         {
            delay(10);
            uart[0]=HEADER;
            if(Serial1.read()==HEADER)//determine datapackage frame header 0x59
            {
               uart[1]=HEADER;
               for(i=2;i<9;i++)//store data to array
               {
                  uart[i]=Serial1.read();
               }
               check=uart[0]+uart[1]+uart[2]+uart[3]+uart[4]+uart[5]+uart[6]+uart[7];
               if(uart[8]==(check&0xff))//check the received data as per protocols 
               {
                  dist=uart[2]+uart[3]*256;//calculate distance value
                  strength=uart[4]+uart[5]*256;// calculate signal strength value 
                  Serial.print("dist = ");
                  Serial.print(dist);//output LiDAR testsdistance value 
                  Serial.print('\t');
                  Serial.print("strength = ");
                  Serial.print(strength);//output signal strength value
                  Serial.print("\t");
                  Serial.print("phi = ");
                  Serial.print(posPhi*180/256); //real Phi angle
                  Serial.print('\n');
                  delay(10);
                  ojbk = 0;
                }
             }
           }
        }
     }
 }

void setup() 
{
   for (int i=2;i<6;i++)      //stepper motor
   {pinMode(i,OUTPUT);}
   Serial.begin(9600);//set the Baud rate of Arduino and computer serial port
   Serial1.begin(115200);//set the Baud rate of LiDAR and Arduino serial port
   PhiRANGE = PhiRANGE*256/180; //convert the Phi angle to stepper motor STEPs
   posPhiGain = posPhiGain*256/180;
   for (posPhi = 0; posPhi < PhiRANGE; posPhi += posPhiGain)          
   {
      LIDAR_read(posPhi);
      delay(200);
      clockwise(posPhiGain);
      delay(2000);
      ojbk = 1;
   }
   LIDAR_read(posPhi);
   ojbk = 1;
   delay(200);
   anticlockwise(PhiRANGE);
   delay (200);
   PhiRANGE = PhiRANGE*180/256; //convert back
   posPhiGain = posPhiGain*180/256; //convert back 
}


void loop() 
{
    
}  
