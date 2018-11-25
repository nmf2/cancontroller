#include <Arduino.h>
#include <TimerOne.h>
#include "../include/interface.h"

#define SECOND 1000000 // 1 second

#define PIN_SP_TEST 12

//Defining these here FOR NOW, should be defined in BTL module
bool sp; 
bool wp;

int volatile sample_point = 0;

void test_sample_point();
void print_array(bool*, int);
void setup() {
    Serial.begin(9600);
    Timer1.initialize(2*SECOND);
    Timer1.attachInterrupt(test_sample_point);
    pinMode(PIN_SP_TEST, INPUT);
}

void loop() {
    if(sample_point == 1){
        sample_point = 0;
        Rx = digitalRead(PIN_SP_TEST);
        Serial.print("State: ");
        Serial.print(state_str(state));
        Serial.print("; Rx: ");
        Serial.print(Rx);
        frame_walker();
        Serial.print("; bit_index: ");
        Serial.print(bit_index);
        Serial.print(" DLC: ");
        Serial.println(DLC_value);
        print_array(frame.data, bit_index);
    }
}

void test_sample_point(){
    sample_point = 1;
}

void print_array(bool *array, int max){
    Serial.print("|");
    int i = 0;
    for (; i <= max; i++){
        Serial.print(array[i]);
        Serial.print("|");
    }
    Serial.println();
}
