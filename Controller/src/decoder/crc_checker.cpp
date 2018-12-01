#include "stdbool.h"
#include "interface.h"
/*
* Whenever a CRC ERROR is detected, transmission of an ERROR FLAG starts at the 
* bit following the ACK DELIMITER, unless an ERROR FLAG for another condition 
* has already been started. That is, only do crc_err = true on the ACKd state.
*/

bool crc_err = false;
short crc = 0 ^ 0; // Already treating SOF

void crc_checker(){
    if (Rstuff_flag){
        return;
    }
    if (crc_err == true){
        crc_err = false;
        crc = 0 ^ 0; // SOF
    }
    if (last_state < CRC){
        crc = next_crc(crc, Rx);
    }
    else if (last_state == ACKd){ // Decide if there is an error
        // Get frame's CRC
        short frame_crc = (short) bits_to_int(BIT_Y_START_CRC_X, 
                                              BIT_Y_END_CRC_X, 
                                              frame.data);
        Serial.print("crc: ");
        Serial.println(crc);
        Serial.print("frame_crc: ");
        Serial.println(frame_crc);
        if(frame_crc != crc){
            crc_err = true;
        }
    }
    else if ( last_state > ACKd){
        crc = 0 ^ 0; //Already prepare for SOF
    }
}