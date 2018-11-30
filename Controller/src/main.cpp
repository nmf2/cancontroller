#include <Arduino.h>
#include <TimerOne.h>
#include "../include/interface.h"

#define SECOND 1000000 // 1 second
#define MILLI 1000

#define PIN_SP_TEST 12

//Defining these here FOR NOW, should be defined in BTL module
bool volatile sp; 
bool volatile wp;
bool Rx; // Data from transceiver

bool err = false; // Error Flag

void test_sp();
void debug();
void testing();

//Frame test_frame;

void setup() {
    Serial.begin(9600);

    Timer1.initialize(SECOND/20);
    Timer1.attachInterrupt(test_sp);
    
    //pinMode(PIN_SP_TEST, INPUT_PULLUP);
    
    testing();
    
    state = IDLE;
    Rx = 1;
}
void loop() {
    if(sp == true){
        //Serial.println(F("WP"));
        stuffer();
        Rx = Tx;
        wp = false;
        
        bit_stuff_monitor(); // Reading
        
        frame_walker(); // Core State Machine
        
        // Error signaling
        // ack_checker(); 
        form_checker();
        bit_monitor();
        crc_checker();

        err = stuff_err | ack_err | bit_err | form_err | crc_err;

        debug();
        
        sp = false; // makes sure it enters in the if only once.
    }
}

void test_sp(){
    sp = true;
    wp = true;
}

void testing(){
    //Testing  
    Frame test_frame;
    uint64_t test_id = id_calc(0x0449,0x3007A);
    // Frames tested
    framer(0x0672, 0, false, DATA_FRAME, 0, &test_frame);
    // framer(0x0672, 0xAAAAAAAAAAAAAAAA, false, DATA_FRAME, 8, &test_frame);
    // framer(0x0672, 0x0, false, DATA_FRAME, 0, &test_frame);
    // framer(0x0672, 0x0, false, REMOTE_FRAME, 0, &test_frame); // CRC?
    // framer(0x0672, 0x0, false, REMOTE_FRAME, 1, &test_frame);
    // framer(test_id, 0xAAAAAAAAAAAAAAAA, true, DATA_FRAME, 8, &test_frame);
    // framer(test_id, 0x0, true, REMOTE_FRAME, 0, &test_frame);
    // print_frame(test_frame, false);

    Serial.println(F("Frame data: "));  
    print_array(in_frame.data, in_frame.frame_size - 1);
    Serial.println(F("-----------------------------------------------------"));
    // Serial.println();
    // print_array(test_frame.data, test_frame.frame_size - 1);
    // Serial.println(test_frame.frame_size);
    // End Testing
}

void debug(){
    Serial.print("State: ");
    Serial.print(state_str(last_state));
    Serial.print("; Rx: ");
    Serial.print(Rx);
    Serial.print("; Tx: ");
    Serial.print(Tx);
    Serial.print("; bit_index: ");
    Serial.print(bit_index);
    // Serial.print("; frm_index: ");
    // Serial.print(frm_index);
    // Serial.print("; DLC: ");
    // Serial.print(DLC_value);
    Serial.print("; Rstuff_flag: ");
    Serial.print(Rstuff_flag);
    Serial.print("; Tstuff_flag: ");
    Serial.print(Tstuff_flag);
    // Serial.print("; Sbit_count: ");
    // Serial.print(Sbit_count);
    // Serial.print("; writing_mode: ");
    // Serial.print(writing_mode);
    // Serial.print("; eol_recessive_count: ");
    // Serial.print(eol_recessive_count);
    // Serial.print("; eol_dominant_count: ");
    // Serial.print(eol_dominant_count);
    Serial.println();

    // Serial.print("form_err: ");
    // Serial.print(form_err);
    // Serial.print("; ack_err: ");
    // Serial.print(ack_err);
    // Serial.print("; bit_err: ");
    // Serial.print(bit_err);
    // Serial.print("; stuff_err: ");
    // Serial.print(stuff_err);
    Serial.print("; crc_err: ");
    Serial.print(crc_err);
    // Serial.print("; bsm_bit_count: ");
    // Serial.print(bsm_bit_count);
    // Serial.print("; bsm_last_bit: ");
    // Serial.print(bsm_last_bit);
    Serial.println();
    
    Serial.println(F("Frame data: "));
    print_array(frame.data, bit_index);
    Serial.println(F("Bus data: "));
    print_array(bus_data, bdi);
    // Serial.println();
    // Serial.print("test_Frame_size: ");
    // Serial.println(test_frame.frame_size);
    // print_array(test_frame.data, test_frame.frame_size - 1);
    Serial.println();
    
}