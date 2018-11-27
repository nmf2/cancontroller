#include <Arduino.h>
#include <TimerOne.h>
#include "../include/interface.h"

#define SECOND 1000000 // 1 second

#define PIN_SP_TEST 12

//Defining these here FOR NOW, should be defined in BTL module
bool sp; 
bool wp;

//int volatile sp = 0;

void test_sp();
void print_array(bool*, int);
void debug();

void setup() {
    Serial.begin(9600);
    Timer1.initialize(2*SECOND);
    Timer1.attachInterrupt(test_sp);
    pinMode(PIN_SP_TEST, INPUT_PULLUP);
    state = IDLE;
}

void loop() {
    if(sp == 1){
        Rx = digitalRead(PIN_SP_TEST);
        Rx = !Rx; // Make recessive the default
        bit_stuff_monitor();
        
        frame_walker();
        
        ack_checker();
        form_checker();
        bit_monitor();
        
        err = stuff_err | ack_err | bit_err | form_err | crc_err;
        debug();
        sp = 0; // makes sure it enters in the if only once.
    }
}

void test_sp(){
    sp = 1;
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

void debug (){
    Serial.print("State: ");
    Serial.print(state_str(last_state));
    Serial.print("; Rx: ");
    Serial.print(Rx);
    Serial.print(F("; bit_index: "));
    Serial.print(bit_index);
    Serial.print(F("; DLC: "));
    Serial.print(DLC_value);
    Serial.print(F("; Rstuff_flag: "));
    Serial.print(Rstuff_flag);
    Serial.println();
    
    print_array(frame.data, bit_index);
}