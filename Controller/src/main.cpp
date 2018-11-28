#include <Arduino.h>
#include <TimerOne.h>
#include "../include/interface.h"

#define SECOND 1000000 // 1 second

#define PIN_SP_TEST 12

//Defining these here FOR NOW, should be defined in BTL module
bool sp; 
bool wp;
bool Rx = 1; // Data from transceiver


bool err = false; // Error Flag 

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
        stuffer(); // Responsible for writing.
        bit_stuff_monitor(); // Reading
        err = stuff_err;
        
        frame_walker(); // Core State Machine
        
        // Error signaling
        ack_checker(); 
        form_checker();
        bit_monitor();
        
        err = ack_err | bit_err | form_err | crc_err;

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
    Serial.print(F("State: "));
    Serial.print(state_str(last_state));
    Serial.print(F("; Rx: "));
    Serial.print(Rx);
    Serial.print(F("; bit_index: "));
    Serial.print(bit_index);
    Serial.print(F("; DLC: "));
    Serial.print(DLC_value);
    Serial.print(F("; Rstuff_flag: "));
    Serial.print(Rstuff_flag);
    Serial.print(F("; eol_recessive_count: "));
    Serial.print(eol_recessive_count);
    Serial.print(F("; eol_dominant_count: "));
    Serial.print(eol_dominant_count);
    Serial.println();
    Serial.print(F("; stuff_err: "));
    Serial.print(stuff_err);
    Serial.print(F("; bsm_bit_count: "));
    Serial.print(bsm_bit_count);
    Serial.print(F("; bsm_last_bit: "));
    Serial.print(bsm_last_bit);
    
    Serial.println();
    print_array(frame.data, bit_index);
    Serial.println();
    
}