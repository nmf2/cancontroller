#include "stdbool.h"
#include "../include/interface.h"

bool form_err;
int eof_index;

void form_checker(){
    // Make sure it only runs on sample point
    if (sp == 0){ 
        return;
    }
    // Error has been treated, reset the flag and abort.
    if(form_err == true){
        form_err = false;
        return;
    }
    switch(last_state){
        /*
            If the SRR (12th bit) bit is ‘dominant’ but the IDE (13th bit) is 
            ‘recessive’ there is a form error, that is, the SRR bit must be one
            for extended frames.
        */
        case IDE:                           //frame.data[BIT_IDE]  
            if (frame.data[BIT_SRR_B] == 0 && Rx == 1){
                form_err = true;
            }
            break;

        // If the CRC delimiter is ‘dominant’
        case CRCd:
            //  frame.data[BIT_Y_CRC_DELIMITER_X]
            if (Rx == 0){
                form_err = true;
            }
            break;

        // If the ACK delimiter is ‘dominant’
        case ACKd:
              //frame.data[BIT_Y_ACK_DELMITER_X]
            if (Rx == 0){
                form_err = true;
            }
            break;

        /* 
        If there is a dominant bit anywhere in the EOF field except for the 
        last (7th) bit.
        */
        case EOFR:
            // frame.data[eof_index]
            if(Rx == 0 && (bit_index + 1) != BIT_Y_END_EOF_X){
                // bit_index will only be incremented when frame_walker runs
                // and it didn't happen yet.
                form_err = true;
            }
            break;
        
        // Dominant bit in any of the first two bits of Intermission
        case INTERMISSION1:
            if(Rx == 0){
                state = ERROR_FLAG;
            }
            break;

        default:
            break;
    }
}