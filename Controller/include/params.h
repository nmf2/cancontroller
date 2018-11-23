// Data Frame (up to the start of the data frame)
#ifndef CAN_PARAMS // Prevents double includes
#define CAN_PARAMS // 
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
// Struct that represents a CAN Frame.
typedef
struct Frame {
    bool data[128];
    int size;
    bool extended;
    bool type;
}
Frame;

Frame frame;

#endif