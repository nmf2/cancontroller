#include "stdbool.h"
#include "interface.h"

bool form_err = false;


bool overload_flag = false;

void form_checker(){
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
        case IDE:                           //frame.data[BIT_IDE]     (Checked)
            if (frame.data[BIT_SRR_B] == 0 && Rx == 1){
                form_err = true;
            }
            break;

        // If the CRC delimiter is ‘dominant’
        case CRCd:                                                  //(Checked)
            //  frame.data[BIT_Y_CRC_DELIMITER_X]
            if (Rx == 0){
                form_err = true;
            }
            break;

        // If the ACK delimiter is ‘dominant’
        case ACKd:                                                  //(Checked)
              //frame.data[BIT_Y_ACK_DELMITER_X]
            if (Rx == 0){
                form_err = true;
            }
            break;

        /* 
        If there is a dominant bit anywhere in the EOF field except for the 
        last (7th) bit.
        */
        case EOFR:                                                  //(Checked)
            // frame.data[eof_index]
            if(Rx == 0 && bit_index != BIT_Y_END_EOF_X){
                form_err = true;
            }
            break;
        
        // Dominant bit in any of the first two bits of Intermission
        case INTERMISSION1:                               //(Checked)
            if(Rx == 0){
                // form_err = true;
                state = OVERLOAD_FLAG;
            }
            break;

        default:
            break;
    }
}