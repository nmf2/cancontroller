#include "stdbool.h"
#include "../include/interface.h"
/*
* Whenever a CRC ERROR is detected, transmission of an ERROR FLAG starts at the 
* bit following the ACK DELIMITER, unless an ERROR FLAG for another condition 
* has already been started. That is, only do crc_err = true on the ACKd state.
*/

bool crc_err = false;
