#include <Arduino.h>
#include <TimerOne.h>
#include "interface.h"
#include "BTL.h"

#define SECOND 1000000 // 1 second
#define MILLI 1000

#define PIN_SP_TEST 12

//Defining these here FOR NOW, should be defined in BTL module
bool Rx; // Data from transceiver

bool err = false; // Error Flag

void test_sp();
void debug();
void testing();

int counter = 10;

int i = 0;

const char input[]  =   "000000000000000x";
const char input2[] =   "000000000000000x";

//Frame test_frame;

//Transceiver IO
#define PIN_RX 2 //Rx from transceiver 
#define PIN_TX 3 //Tx from transceiver

void setup() {
    Serial.begin(9600);
    //Initializing Arduino IOs   
    pinMode(PIN_RX, INPUT);
    pinMode(PIN_TX, OUTPUT);

    //Initializing Bit Timing Logic 
    BTL_init();

    //Initializing TOGGLE
    TOGGLE_init();

    //Initializing Timer1 Interrupt
    Timer1.initialize(BTL_TIME_QUANTA);

    //Attaching Interrupts
    //call BTL_edge_detector when a falling edge is detected at PIN_RX
    //BTL_edge_detector: sets a flag called rx_falling which is used by
    //the bit timing state machine to handle synchronizations
    attachInterrupt(digitalPinToInterrupt(PIN_RX), BTL_edge_detector, FALLING);
    //call BTL_new_time_quanta after Timer1 trigger its interruption.
    //BTL_new_time_quanta: sets a flag called tq_flag which refreshes the
    //bit timing state machine
    Timer1.attachInterrupt(BTL_new_time_quanta);
    
    //pinMode(PIN_SP_TEST, INPUT_PULLUP);
    
    testing();
    
    state = IDLE;
    Rx = 1;
}
void loop() {
    cli();
    TOGGLE_write();
    //TOGGLE_write_serial(); //Ctrl+Shift+L to see in Serial Plotter.
                           //Must comment other Serial prints
                           //BTL.cpp line 54,59,86,92,197
    BTL_sm();
    if (btl_writing_point == true){
        // Serial.println(F("WP"));
        stuffer();
        digitalWrite(PIN_TX, Tx);
        btl_writing_point = false;
    }
    if (btl_sample_point == true){
        Serial.println(F("SP"));

        writing_mode = 1;
        Tx = input2[i] == '1';
        Rx = input[i] == '1';
        // Rx = digitalRead(PIN_RX);
        if (input[i] == 'x'){
            while(true);
        }
        i++;
        // Rx = digitalRead(PIN_RX);
        bit_stuff_monitor(); // Reading
        err = stuff_err;
        
        frame_walker(); // Core State Machine
        
        // Error signaling
        ack_checker(); 
        form_checker();
        bit_monitor();
        crc_checker();

        err = stuff_err | ack_err | bit_err | form_err | crc_err;

        debug();
        
        btl_sample_point = false; // makes sure it enters the if only once.
    }
    sei();
}

// void test_sp(){
//     btl_sample_point = true;
//     btl_writing_point = true;
// }

void testing(){
    
    //Testing  
    Frame test_frame;
    // uint64_t test_id = id_calc(0x0449,0x3007A);
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

    // Serial.println(F("Frame data: "));  
    // print_array(in_frame.data, in_frame.frame_size - 1);
    // Serial.println(F("-----------------------------------------------------"));
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
    if (Rstuff_flag){
        Serial.print("; Rstuff_flag: ");
        Serial.print(Rstuff_flag);
    }
    Serial.print("; Tstuff_flag: ");
    Serial.print(Tstuff_flag);
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
    if(err){
        Serial.print("; err: ");
        Serial.print(err);
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