#include "stdbool.h"
#include "../include/interface.h"
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
    if (last_state < CRC){
        crc = next_crc(crc, Rx);
    }
    else if (last_state == ACKd){ // Decide if there is an error
        // Get frame's CRC
        short frame_crc = (short) bits_to_int(BIT_START_DLC_X, 
                                              BIT_END_DLC_X, 
                                              frame.data);

        if(frame_crc != crc){
            crc_err = true;
        }
    }
    else if ( state > ACKd){
        crc = 0 ^ 0; //Already prepare for SOF
    }
}