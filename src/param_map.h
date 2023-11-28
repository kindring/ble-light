#ifndef PARAM_MAP_H
#define PARAM_MAP_H
// 命令起始码 0x6c 
// 响应起始码 0xc6
// 通知响应码 0x66

#define CMD_MIN_LEN 4
// 数据长度 
#define DATA_LEN 4

#define CMD_START_CODE 0x6c
#define RESP_START_CODE 0xc6
#define NOTIFY_RESP_CODE 0x66


// 解析命令
extern int parse_package(uint8* data, uint16 len);


typedef struct {
    uint8 start_code;// 起始码
    uint8 len;// 包长度
    uint8 cmd;// 命令码
    uint8 sn;// 序列号
    uint8 *data;// 数据 可变
} package_t;



#endif
