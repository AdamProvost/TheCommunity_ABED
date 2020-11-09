#include<LiquidCrystal.h>
LiquidCrystal lcd(6, 7, 8, 9, 10, 11 );

int trigPin = 13;
int echoPin = 12;
int BJTPin = 4;
int ledPin=3;
int i=1;

int delayStart = 10; // Used to delay starting back up

long duration, distance;

void setup()
{ 

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

pinMode(ledPin,OUTPUT);

digitalWrite(BJTPin, HIGH); // Turn on BJT/MOSFET (current can flow)
}

void loop() 
{ 

while(i){
  lcd.print("   ABED   ");
  delay(2000);
  i = 0;
}

lcd.clear();

digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = (duration/2)/29.1;

if (distance >= 200){
  lcd.print("Out of range...");
  delayStart++; // One good out of range read.  Precautionary.
  delay(100);
  }
  
if (distance >= 1 && distance < 200)
{ 
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print("cm");
  delayStart++; // One good safe distance read.  Precautionary.                      
}


// RB choice math for 2N2222 BJT...
/*
 * IB=3IL/hfe=3*100mA/100 =3.0mA
 * 
 * RB = (5V -1.2V)/3.0mA =1.266kOhm 
 * 
 * 
 */
// Braking logic + precautionary delay
if (distance >= 1 && distance < 10)
{ 
  delayStart = 0;   // Something was close, delay starting...
  lcd.clear();
  digitalWrite(BJTPin, LOW);
  lcd.print("STOP!!!");
  delay(100);                    
} else if (delayStart < 10){
  digitalWrite(BJTPin, LOW);  // Keep brake on until 5 readings that are far enough away
} else {
  digitalWrite(BJTPin, HIGH); //Nothing close and no precaution so pin high
}

delay(40);    

}
