#include<LiquidCrystal.h>
LiquidCrystal lcd(6, 7, 8, 9, 10, 11 );

int trigPin = 13;
int echoPin = 12;

int sensePin=0;
int ledPin=3;
int i=1;

long duration, distance;

void setup()
{ 

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

pinMode(ledPin,OUTPUT);
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

if (distance >= 300){
  lcd.print("Out of range...");
  delay(100);
  }
  
if (distance >= 1 && distance < 300)
{ 
lcd.print("Distance: ");
lcd.print(distance);
lcd.print("cm");                      
}

delay(40);    

}
