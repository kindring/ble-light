#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "types.h"
// 灯光事件
#define LIGHT_EVT_DEFAULT_MODE 0x0008 // 恢复为默认模式事件

// 暖光 pwm脚
#define GPIO_WARM P18
#define GPIO_WARM2 P15
#define WARM_CH 0
// 冷光 pwm脚
#define GPIO_COLD P20
#define GPIO_COLD2 P31
#define COLD_CH 1

// 风扇 pwm
#define GPIO_FAN P23

// 亮度分级 0~100 翻 100倍
#define TOTAl_LEVEL 100
// 亮度更改步长
#define LIGHT_STEP 5
#define TEMP_MIN 2700
#define TEMP_MAX 6500
// 色温更改步长
#define TEMP_STEP 50


// adc 电源监测脚
#define GPIO_POWER P28

// 工作模式
// 默认工作模式
#define LIGHT_MODE_DEFAULT 0

// 全亮模式(色温无效)
#define LIGHT_MODE_FULL 1
// 全亮模式下自动关闭时间 秒
#define FULL_MODE_WAIT_TIME 60

// 灯光控制命令起始码
typedef enum {
    // 下发命令
    START_CODE_CMD = 0x6c,
    // 响应命令
    START_CODE_RES = 0xc6,
    // 通知命令
    START_CODE_NOTIFY = 0x66
}light_cmd_start_code;

// 灯光控制命令码
typedef enum{
    CMD_OPEN = 0x01,
    CMD_CLOSE = 0x02,
    CMD_QUERY = 0x03,
    CMD_LIGTH = 0x04,
    CMD_TEMP = 0x05,
    CMD_FAN = 0x06,
    CMD_MODE = 0x07,
    CMD_CH = 0x08,
    CMD_HZ = 0x09,
}light_cmd_code;

// 灯光数据 色温, 亮度, 工作模式, 风扇
typedef struct {
    bool open;
    int temp;
    int light;
    int mode;
    int fan;
} light_data_t;

typedef struct {
    light_cmd_start_code startCode;
    uint8_t len;
    light_cmd_code cmd;
    uint8_t sn;
    uint8_t *data;
} light_cmd_t;

extern light_data_t light_data;

// 通知回调函数
typedef void (*LightCallbackFunc) (uint8 *res, uint16 len);


extern int light_init(int taskId);

extern int light_ch_set(uint8_t ch, uint16_t val);

// 计算冷暖光的亮度值,并且设置
int comLightVal();

uint16 comCmdResCode(uint8_t cmd, uint8_t sn, uint8* data, uint16 len,  uint8_t *res);

extern uint16 light_set(uint8_t val, uint8_t sn, uint8 *res);
extern uint16 temp_set(int temp, uint8_t sn, uint8 *res);
extern uint16 open_light(uint8_t sn, uint8 *res);
extern uint16 close_light(uint8_t sn, uint8 *res);
extern uint16 change_light_mode (light_cmd_start_code mode, uint8_t sn, uint8 *res);
extern uint16 parse_light_code(uint8* data, uint16 len, uint8 *res);

extern void light_register_notify_callback(LightCallbackFunc callback);







#endif