
#include "param_map.h"
int parse_package(uint8* data, uint16 len){
    // 判断长度是否正确
    if(len < data_min_len){
        return -1;
    }
    uint8_t cmd = data[0];
    uint8_t sn = data[1];

}