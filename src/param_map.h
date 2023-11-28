#ifndef PARAM_MAP_H
#define PARAM_MAP_H
// 命令起始码 0x6c 
// 响应起始码 0xc6
// 通知响应码 0x66

#define DATA_MIN_LEN 4

#define CMD_START_CODE 0x6c
#define RESP_START_CODE 0xc6
#define NOTIFY_RESP_CODE 0x66


// 解析命令
extern int parse_package(uint8* data, uint16 len);





#endif
