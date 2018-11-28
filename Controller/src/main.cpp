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

Frame test_frame;

void setup() {
    Serial.begin(9600);
    Timer1.initialize(2*SECOND);
    Timer1.attachInterrupt(test_sp);
    pinMode(PIN_SP_TEST, INPUT_PULLUP);
    
    // Testing
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Frame test_frame;
    bool id[11] = { 0 };
    test_frame.type = DATA_FRAME;
    test_frame.extended = false;
    test_frame.payload_size = 0;
    Serial.println();
    //delay(SECOND);
    framer(id, nullptr, &test_frame);
    // Serial.print("test_Frame_size: ");
    // Serial.println(test_frame.frame_size);
    // print_array(test_frame.data, test_frame.frame_size - 1);
    // Serial.print("in_frame: ");
    // Serial.println(test_frame.frame_size);
    // print_array(test_frame.data, test_frame.frame_size - 1);
    //End Testing

}

void loop() {
    if(wp == 1){
        stuffer(); // Responsible for writing.
        wp = false;
        Rx = Tx;
    }
    if(sp == 1){
        //Rx = digitalRead(PIN_SP_TEST);
        //Rx = !Rx; // Make recessive the default
        bit_stuff_monitor(); // Reading
        
        frame_walker(); // Core State Machine
        
        // Error signaling
        // ack_checker(); 
        // form_checker();
        // bit_monitor();
        
        err = stuff_err; //| ack_err | bit_err | form_err | crc_err;

        debug();
        sp = false; // makes sure it enters in the if only once.
    }
}

void test_sp(){
    sp = true;
    wp = true;
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

void debug(){
    Serial.print(F("State: "));
    Serial.print(state_str(last_state));
    Serial.print(F("; Rx: "));
    Serial.print(Rx);
    Serial.print(F("; Tx: "));
    Serial.print(Tx);
    Serial.print(F("; bit_index: "));
    Serial.print(bit_index);
    Serial.print(F("; DLC: "));
    Serial.print(DLC_value);
    Serial.print(F("; Rstuff_flag: "));
    Serial.print(Rstuff_flag);
    Serial.print(F("; Tstuff_flag: "));
    Serial.print(Tstuff_flag);
    Serial.print(F("; Sbit_count: "));
    Serial.print(Sbit_count);
    // Serial.print(F("; eol_recessive_count: "));
    // Serial.print(eol_recessive_count);
    // Serial.print(F("; eol_dominant_count: "));
    // Serial.print(eol_dominant_count);
    Serial.println();

    // Serial.print(F("; form_err: "));
    // Serial.print(form_err);
    // Serial.print(F("; ack_err: "));
    // Serial.print(ack_err);
    // Serial.print(F("; bit_err: "));
    // Serial.print(bit_err);
    Serial.print(F("; stuff_err: "));
    Serial.print(stuff_err);
    Serial.print(F("; bsm_bit_count: "));
    Serial.print(bsm_bit_count);
    Serial.print(F("; bsm_last_bit: "));
    Serial.print(bsm_last_bit);
    
    Serial.println();

    print_array(frame.data, bit_index);
    // Serial.println();
    // Serial.print("test_Frame_size: ");
    // Serial.println(test_frame.frame_size);
    // print_array(test_frame.data, test_frame.frame_size - 1);
    Serial.println();
    
}