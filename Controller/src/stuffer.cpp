#include "../include/interface.h"

Frame in_frame; // The frame to be sent
bool Tx = 0;
int bit_count = 0;
bool stuff_flag = 0;
int frm_index = 0; // Equivalent to bit_index in Frame Walker
bool wait_next_frame = 1; // activated when lost arbitration
bool buffer_full; // Indicates if there a frame send write

void stuffer(){
    if (wait_next_frame){
        Tx = 1;
    }
    else if (lost_arbitration){
        Tx = 1;
        bit_count = 0;
        stuff_flag = 0;
        frm_index = 0;
        wait_next_frame = 1;
    }
    else if (stuff_flag){ // If this has to be a stuff bit      
        Tx = !in_frame.data[frm_index - 1]; // Write the inverse of the last bit

        stuff_flag = 0;
        bit_count = 0;
    }
    else if (state == ERROR_FLAG){
        wait_next_frame = 1;
        Tx = 0;
    }
    else if (state == ERROR_DELIMITER){
        wait_next_frame = 1;
        Tx = 1; 
    }
    else if (buffer_full){ // if there is anything to write
        Tx = in_frame.data[frm_index];
        if (state < CRCd){
            if (frm_index == 0){
                bit_count = 1;
            }
            else if (in_frame.data[frm_index] == in_frame.data[frm_index - 1]){
                // Last bit equals current one
                bit_count++;
            }
            if (bit_count >= 5){
                stuff_flag = true;
                bit_count = 0;
            }
        }
        frm_index++;
        if (frm_index >= in_frame.frame_size){ //finished writing
            buffer_full = 0; // signaling that there is nothing to write
        }
    } 
    else { // Nothing to do, all in order and no frames to send
        Tx = 1;
    }

    if (state == IDLE){
        wait_next_frame = false;
        bit_count = 0;
        stuff_flag = 0;
        frm_index = 0;
    }
    
}

bool set_in_frame(Frame frm){
    if(buffer_full){
        return 0; // Busy
    }
    else {
        in_frame = frm;
        buffer_full = 1;
    }
    return 1; // OK
};
