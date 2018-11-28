#include "stdbool.h"
#include "../include/interface.h"

bool ack_err;

void ack_checker(){
    if (sp == 0){
        return;
    }

    if (ack_err == true) {
        ack_err = false;
    }
    /*
    Ack Error happens when the actual bit value Rx signal is ‘recessive’ when 
    the STATE of the Frame Walker’s FSM is ACK.
    */
    if (last_state == ACK && Rx == 1){
        ack_err = true;
    }

}