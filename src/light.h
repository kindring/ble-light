#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "types.h"
// 灯光事件
#define LIGHT_EVT_DEFAULT_MODE 0x0008 // 恢复为默认模式事件

// 保存灯光数据事件
#define LIGHT_EVT_SAVE_DATA 0x0010

// 保存数据等待时间 秒
#define SAVE_DATA_WAIT_TIME 3

// 暖光 pwm脚
#define GPIO_WARM P18
#define GPIO_WARM2 P15
#define WARM_CH 0
// 暖光最小亮度值
#define WARM_LIGHT_MIN 5
// 冷光 pwm脚
#define GPIO_COLD P20
#define GPIO_COLD2 P31
#define COLD_CH 1
// 冷光最小亮度值
#define COLD_LIGHT_MIN 5

// 风扇 pwm
#define GPIO_FAN P3
#define FAN_CH 2
// 风扇启用亮度值 (灯光亮度大于该值时启用风扇)
#define FAN_LIGHT_MIN 30

// 亮度分级 0~100 翻 100倍
#define TOTAl_LEVEL 100
// 亮度更改步长
#define LIGHT_STEP 5
// 亮度最低值
#define LIGHT_MIN 8

#define TEMP_MIN 2700
#define TEMP_MAX 6500
// 色温便宜值
#define TEMP_OFFSET 500
// 色温更改步长
#define TEMP_STEP 50



// adc 电源监测脚
#define GPIO_POWER P14
// 电压检测控制脚
#define GPIO_BATT_CH P34
// adc 电源监测通道

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
    uint8 mode;
    uint16 temp;
    uint8 light;
    uint8 fan;
} light_data_t;
#define SAVE_FILE_ID 1

// 命令头长度
#define CMD_HEADER_LEN 4
// 命令除去起始码与长度的长度
#define CMD_LEN_LEN 2
// 命令最大长度,包含起始码,长度,命令码,sn码,数据
#define CMD_MAX_LEN 128

typedef struct {
    light_cmd_start_code startCode;
    uint8_t len;
    light_cmd_code cmd;
    uint8_t sn;
    uint8 *data;
} light_cmd_t;



extern light_data_t light_data;

// 通知回调函数
typedef void (*LightCallbackFunc) (uint8 *res, uint16 len);




void getLightData(uint8_t *data);
// 计算冷暖光的亮度值,并且设置
uint8_t comLightVal();

uint16 comCmdResCode(uint8_t cmd, uint8_t sn, uint8* data, uint16 len,  uint8_t *res);


extern void light_init(uint8 taskId);
extern int light_ch_set(uint8_t ch, uint16_t val);
extern uint16 light_set(uint8_t val, uint8 sn, uint8 *res);
extern uint16 temp_set(int temp, uint8_t sn, uint8 *res);
extern uint16 open_light(uint8_t sn, uint8 *res);
extern uint16 close_light(uint8_t sn, uint8 *res);
extern uint16 change_light_mode (int mode, uint8_t sn, uint8 *res);
extern uint16 parse_light_code(const uint8* data, uint16 len, uint8 *res);


extern void light_register_notify_callback(LightCallbackFunc callback);

uint16 Light_ProcessEvent( uint8 task_id, uint16 events );




#endif