#include <TimerOne.h>

#define BLUE_LED 8
#define BLUE_LED_TRIGGER 2
#define RED_LED 9

#define ON HIGH
#define OFF LOW

void blueF();
void redF();   

int r = 0;

void setup(){
    pinMode(BLUE_LED,OUTPUT);
    digitalWrite(BLUE_LED,OFF);
    pinMode(RED_LED,OUTPUT);
    digitalWrite(RED_LED,OFF);
    pinMode(BLUE_LED_TRIGGER,INPUT);
    Serial.begin(9600);
    //The BLUE_LED_TRIGGER pin calls the function blueF when the falling edge happenns at that pin
    //a debounce circuit might be needed
    attachInterrupt(digitalPinToInterrupt(BLUE_LED_TRIGGER),blueF,FALLING);
    //The 16-bit Timer1 is used to activate a time interrupt. For this example, it triggers
    //after 100ms
    Timer1.initialize(100000); //0.1s
    Timer1.attachInterrupt( redF);

}

void loop(){
    /*r = digitalRead(BLUE_LED_TRIGGER);
    Serial.println(r);
    delay(50);*/
    //Serial.println(digitalRead(BLUE_LED_TRIGGER));
}

void blueF(){
    digitalWrite(BLUE_LED,digitalRead(BLUE_LED) ^ 1);
    //Serial.println(digitalRead(BLUE_LED_TRIGGER));
}

void redF(){
    digitalWrite(RED_LED,digitalRead(RED_LED) ^ 1);
}
