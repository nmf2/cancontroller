#include "../include/interface.h"

Frame in_frame; // The frame to be sent
bool Tx = 0;
int Sbit_count = 1;
bool Tstuff_flag = 0;
int frm_index = 0; // Equivalent to bit_index in Frame Walker
bool wait_next_frame = 0; // activated when lost arbitration
bool writing_mode = 0; // Indicates if it's currently writing a frame;
bool last_Tx;
int error_dominant_count = 0;

void stuffer(){
    if (wp == false){
        return;
    }
    if (wait_next_frame){
        Tx = 1;
    }
    else if (lost_arbitration){
        Tx = 1;
        Sbit_count = 1;
        Tstuff_flag = 0;
        frm_index = 0;
        wait_next_frame = 1;
    }
    else if (Tstuff_flag){ // If this has to be a stuff bit      
        Tx = !in_frame.data[frm_index - 1]; // Write the inverse of the last bit

        Tstuff_flag = false;
        Sbit_count = 1;
    }
    else if (state == ERROR_FLAG){
        if(error_dominant_count <= 6){
            Tx = 0;
        } 
        else {
            Tx = 1;
        }
        error_dominant_count++;
    }
    else if (state == ERROR_DELIMITER){
        error_dominant_count = 0;
        Tx = 1;
    }
    else if (writing_mode){ // if there is anything to write
        Tx = in_frame.data[frm_index];
        
        if (state < CRCd){
            if (last_Tx == Tx){
                // Last bit equals current one
                Sbit_count++;
            } else {
                Sbit_count = 1;
            }
            if(Sbit_count == 5){
                Tstuff_flag = true;
            }

        } 
        else { // After CRCd, reset;
            Sbit_count = 1;
            Tstuff_flag = false;
        }
        frm_index++;
        if (frm_index >= in_frame.frame_size){ //finished writing
            writing_mode = 0; // signaling that there is nothing to write
        }
    } 
    else { // Nothing to do, all in order and no frames to send
        Tx = 1;
    }
    last_Tx = Tx;

    if (state == IDLE){
        wait_next_frame = false;
        Sbit_count = 1;
        Tstuff_flag = 0;
        frm_index = 0;
    }
    
}

bool set_in_frame(Frame frm){
    if(writing_mode){
        return 1; // Busy
    }
    else {
        in_frame = frm;
        writing_mode = 1;
    }
    return 0; // OK
};
