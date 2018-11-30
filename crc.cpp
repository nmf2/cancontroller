#include <iostream>
#include <string.h>
using namespace std;

//sof + 0x0672 = 011001110010
//A = 1010
//sof.ida.rtr.ide.r0.dlc.data
//sof.ida.srr.idb.rtr.ide.r0.dlc.data

//0x0672 rtr 0 ide 0 dlc 8
//stuffed "0.11001110010.0.0.0.1000.1010101010101010101010101010101010101010101010101010101010101010";
//char *strr = "01100111001000010001010101010101010101010101010101010101010101010101010101010101010";
//david: "000000001010001"
//eu:    "000000001010001"

//0x0672 rtr 0 ide 0 dlc 7
//stuffed "0.11001110010.0.0.0.0*1*111.10101010101010101010101010101010101010101010101010101010"
//char *strr = "011001110010000011110101010101010101010101010101010101010101010101010101010";
//david: "101100110011101"
//eu:    "101100110011101"

//0x0672 rtr 0 ide 0 dlc 6
//stuffed  "0.11001110010.0.0.0.0*1*110.101010101010101010101010101010101010101010101010"
//char *strr = "0110011100100000110101010101010101010101010101010101010101010101010";
//david: "000100100101110"
//eu:    "000100100101110"

//0x0672 rtr 0 ide 0 dlc 0
//stuffed  "0.11001110010.0.0.0.0*1*000."
//char *strr = "0110011100100000000";
//david: "011001011010101"
//eu:    "011001011010101"

//char *strr = "010001001001111100000000011110100000011101010101010101010101010";
//expected: 0001111100101110 (stuffed) 
//          000111110101110 (destuffed)
//res:      000111110101110

bool exor(bool x,  bool y){
    return (x && !y) || (!x && y);
}

bool frame[128];
bool CRC[15];
bool resCRC[15];
int cur_index;

bool ReadyCRC(){
    if(cur_index == 0){
        for(int i = 0; i < 15; i++){
            CRC[i] = false;
        }
    }

    if(cur_index <= strlen(strr)){
        bool DoInvert = (true == frame[cur_index]) ^ CRC[14]; 


        CRC[14] = CRC[13] ^ DoInvert;
        CRC[13] = CRC[12];
        CRC[12] = CRC[11];
        CRC[11] = CRC[10];
        CRC[10] = CRC[9] ^ DoInvert;
        CRC[9] = CRC[8];
        CRC[8] = CRC[7] ^ DoInvert;
        CRC[7] = CRC[6] ^ DoInvert;
        CRC[6] = CRC[5];
        CRC[5] = CRC[4];
        CRC[4] = CRC[3] ^ DoInvert;
        CRC[3] = CRC[2] ^ DoInvert;
        CRC[2] = CRC[1];
        CRC[1] = CRC[0];
        CRC[0] = DoInvert;
    }

    if(cur_index < strlen(strr) - 1)
        return false;
    else{
        for(int i = 0; i < 15; i++){
            resCRC[14-i] = CRC[i]; 
        }
        return true;
    }
    
}

int main(){
    cur_index = 0; 
    for(int i = 0; i < 128; i++){
        frame[i] = false;
    }
    for(int i = 0; i < strlen(strr); i++){
        frame[i] = strr[i] == '1';
    }
    cout << "Char Frame: \t\t";
    for(int i = 0; i < strlen(strr); i++){
        cout << strr[i];
    }
    cout << endl;
    cout << "Bool Frame: \t\t";
    for(int i = 0; i < strlen(strr); i++){
        cout << frame[i];
    }
    cout << endl;

    for(cur_index = 0; cur_index < strlen(strr); cur_index++){
        ReadyCRC();
    }

    cout << "CRC: \t\t\t";
    for(int i = 0; i < 15; i++){
        cout << CRC[i];
    }
    cout << endl; 
    cout << "resCRC: \t\t";

    for(int i = 0; i < 15; i++){
        cout << resCRC[i];
    }
    cout << endl; 

    
    return 0;
    
}

