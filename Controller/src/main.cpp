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

int counter = 10;

int i = 0;

const char input[]  =   "0110011100100000100110101010110011100001010101111111100000011111111x";
const char input2[] =   "0110011100100000100110101010110011100001010101111111100000011111111x";

//Frame test_frame;

void setup() {
    Serial.begin(9600);

    Timer1.initialize(SECOND/100);
    Timer1.attachInterrupt(test_sp);
    
    //pinMode(PIN_SP_TEST, INPUT_PULLUP);
    
    testing();
    
    state = IDLE;
    Rx = 1;
}
void loop() {
    // cli();
    // if (wp == true){
    //     Serial.println(F("WP"));
        
    //     wp = false;
    // }
    if (sp == true){
        // Serial.println(F("SP"));
        // stuffer();
        
        
        writing_mode = 1;
        Tx = input2[i] == '1';
        Rx = input[i] == '1';
        if (input[i] == 'x'){
            while(true);
        }

        i++;
        
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
    //sei();
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
    // framer(0x0672, 0xAAAAAAAAAAAAAAAA, false, DATA_FRAME, 8, &test_frame);
    // framer(0x0672, 0x0, false, DATA_FRAME, 0, &test_frame);
    // framer(0x0672, 0x0, false, REMOTE_FRAME, 0, &test_frame); // CRC?
    // framer(0x0672, 0x0, false, REMOTE_FRAME, 1, &test_frame); //11
    // framer(test_id, 0xAAAAAAAAAAAAAAAA, true, DATA_FRAME, 8, &test_frame); // 12
    // framer(test_id, 0x0, true, REMOTE_FRAME, 0, &test_frame);
    // print_frame(test_frame, false);
    
    // framer(0x0672, 0xAAAAAAAAAAAAAA, false, DATA_FRAME, 7, &test_frame); 

    framer(0x0672, 0x0, false, REMOTE_FRAME, 0, &test_frame); // CRC

    

    Serial.println(F("Frame data: "));  
    print_array(in_frame.data, in_frame.frame_size - 1);
    Serial.println(F("-----------------------------------------------------"));
    // Serial.println();
    // print_array(test_frame.data, test_frame.frame_size - 1);
    // Serial.println(test_frame.frame_size);
    // End Testing
}

void debug(){
    // if(last_state >= INTERMISSION1){
    //     return;
    // }
    Serial.print("State: ");
    if(overload_flag){
        Serial.print("OVERLOAD_FLAG");
    }
    else{
        Serial.print(state_str(last_state));
    }

    Serial.print("; Rx: ");
    Serial.print(Rx);
    // Serial.print("; Tx: ");
    // Serial.print(Tx);
    // Serial.print("; bit_index: ");
    // Serial.print(bit_index);
    // Serial.print("; frm_index: ");
    // Serial.print(frm_index);
    // Serial.print("; DLC: ");
    // Serial.print(DLC_value);
    if (Rstuff_flag){
        Serial.print("; Rstuff_flag: ");
        Serial.print(Rstuff_flag);
    }
    // Serial.print("; Tstuff_flag: ");
    // Serial.print(Tstuff_flag);
    // Serial.print("; Sbit_count: ");
    // Serial.print(Sbit_count);
    // Serial.print("; writing_mode: ");
    // Serial.print(writing_mode);
    if(last_state >= ERROR_FLAG){
        Serial.print("; eol_recessive_count: ");
        Serial.print(eol_recessive_count);
        Serial.print("; eol_dominant_count: ");
        Serial.print(eol_dominant_count);
    }
    // Serial.print("; lost_arbitration: ");
    // Serial.print(lost_arbitration);

    Serial.println();
    if(form_err){
        Serial.print("form_err: ");
        Serial.print(form_err);
    }
    if (ack_err){
        Serial.print("; ack_err: ");
        Serial.print(ack_err);
    }
    if(bit_err){
        Serial.print("; bit_err: ");
        Serial.print(bit_err);
    }
    if(stuff_err){
        Serial.print("; stuff_err: ");
        Serial.print(stuff_err);
    }
    if(crc_err){
        Serial.print("; crc_err: ");
        Serial.print(crc_err);
    }
    Serial.print("; bsm_bit_count: ");
    Serial.print(bsm_bit_count);
    Serial.print("; bsm_last_bit: ");
    Serial.print(bsm_last_bit);
    Serial.println();
    
    // Serial.println(F("Frame data: "));
    // print_array(frame.data, bit_index);
    // Serial.println(F("Bus data: "));
    // Serial.print("0");
    // print_array(bus_data, bdi - 1);
    // Serial.println();
    // Serial.print("test_Frame_size: ");
    // Serial.println(test_frame.frame_size);
    // print_array(test_frame.data, test_frame.frame_size - 1);
    Serial.println();
    
}