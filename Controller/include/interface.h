// Data Frame (up to the start of the data frame)
#ifndef _CAN_PARAMS // Prevents double include errors
#define _CAN_PARAMS
/************************* CAN Bit Indexes *************************/          
#define BIT_START_OF_FRAME  0
#define BIT_START_ID_A  1
#define BIT_END_ID_A  11
#define BIT_RTR_A  12
#define BIT_SRR_B  12
#define BIT_IDE  13
#define BIT_R0_A  14
#define BIT_START_ID_B  14
#define BIT_END_ID_B  31
#define BIT_RTR_B  32
#define BIT_R1_B  33
#define BIT_R0_B  34
#define BIT_START_DLC_A  15
#define BIT_START_DLC_B  35
#define BIT_END_DLC_A  18
#define BIT_END_DLC_B  38
#define BIT_START_DATA_A  19
#define BIT_START_DATA_B  39

// Remote Frame
#define BIT_REMOTE_START_CRC_A  19
#define BIT_REMOTE_START_CRC_B  39 
#define BIT_REMOTE_END_CRC_A  33
#define BIT_REMOTE_END_CRC_B  53
#define BIT_REMOTE_CRC_DELIMITER_A  34
#define BIT_REMOTE_CRC_DELIMITER_B  54
#define BIT_REMOTE_ACK_A  35
#define BIT_REMOTE_ACK_B 55
#define BIT_REMOTE_ACK_DELIMITER_A 36
#define BIT_REMOTE_ACK_DELIMITER_B 56
#define BIT_REMOTE_START_EOF_A 37
#define BIT_REMOTE_START_EOF_B 57
#define BIT_REMOTE_END_EOF_A 43
#define BIT_REMOTE_END_EOF_B 63
#define IFS_START_INTERMISSION1 0
#define IFS_END_INTERMISSION1 1
#define IFS_INTERMISSION2 2
#define EOL_START_FLAG 0
#define EOL_MINIMUM_END_FLAG 5

#define DOMINANT 0
#define RECESSIVE 1

#define DATA_FRAME 0
#define REMOTE_FRAME 1

// Max frame size: 128 (64 bits of data)
/***************** Struct that represents a CAN Frame **********************/
typedef
struct Frame {
    bool data[128];
    int payload_size; // in bytes
    int frame_size; // in bits
    bool extended; 
    bool type; // data (0) or remote (1)?
}
Frame;

/****************** Frame Walker States Variable *********************/
typedef
enum State {IDA, RTRA_SRR, IDE, r0, IDB, RTRB, r1_r0, DLC, PAYLOAD,
            CRC, CRCd, ACK, ACKd, EOFR, INTERMISSION1, INTERMISSION2, 
            ERROR_FLAG, ERROR_DELIMITER, IDLE}
State;

/*************** Mapping function from States to strings **************/
static inline const char *state_str(State s)
{
    static const char *strings[] = {
        "IDA", "RTRA_SRR", "IDE", "r0", "IDB", "RTRB", "r1_r0", "DLC", 
        "PAYLOAD", "CRC", "CRCd", "ACK", "ACKd", "EOFR", "INTERMISSION1", 
        "INTERMISSION2", "ERROR_FLAG", "ERROR_DELIMITER", "IDLE"
    };

    return strings[s];
}

/********************* Functions *********************/
//frame_walker.cpp
void frame_walker(); 

//framer.cpp
int framer(bool *id, bool *payload, Frame *frm); 

// util.cpp
int bits_to_int(int start, int end, bool * data);
int int_to_bits(int value, bool * array);

// stuffer.cpp
void stuffer(); // Takes care of bit stuffing
bool set_in_frame(Frame frm); // Sets frame to be written

// bit_monitor.cpp
void bit_monitor();

// bit_stuff_monitor.cpp
void bit_stuff_monitor();

// form_checker.cpp
void form_checker();

// ack_checker.cpp
void ack_checker();

/**************** Global Variables ****************/
extern Frame frame;
extern State state;
extern State last_state;
extern bool wp;
extern bool sp;
extern bool Rx;


/**************** Frame Walker Vars ****************/
extern int bit_index;
extern int DLC_value;

extern int BIT_START_DLC_X,
            BIT_END_DLC_X,
            BIT_START_DATA_X,
            BIT_END_DATA_X,
            BIT_Y_START_CRC_X,
            BIT_Y_END_CRC_X,
            BIT_Y_CRC_DELIMITER_X,
            BIT_Y_ACK_X,
            BIT_Y_ACK_DELMITER_X,
            BIT_Y_START_EOF_X,
            BIT_Y_END_EOF_X;

/**************** Stuffer Vars ****************/
extern bool writing_mode;
extern bool Tx;

/**************** Bit Monitor Vars ***************/
extern bool bit_err;
extern bool lost_arbitration;

/**************** Bit Stuff Monitor Vars ***************/
extern bool Rstuff_flag;
extern bool stuff_err;

/**************** Form Checker Vars ***************/
extern bool form_err;

/**************** Acknoledgement Checker Vars ***************/
extern bool ack_err;

/**************** Form Checker Vars ***************/
extern bool crc_err;

#endif