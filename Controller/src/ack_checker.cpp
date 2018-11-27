#include "stdbool.h"
#include "../include/interface.h"

bool ack_err;

void ack_checker(){
    if(sp == 0){
        return;
    }
    /*
    Ack Error happens when the actual bit value Rx signal is ‘recessive’ when 
    the STATE of the Frame Walker’s FSM is ACK.
    */
    if (last_state == ACK && Rx == 0){
        ack_err = 1;
    }

}