#ifndef __BTN_H__
#define __BTN_H__

#include "types.h"
#include "gpio.h"

#include "gpio_key.h"

#define Light_MODE TRUE
#define Temp_MODE FALSE
extern uint8 task_btn_id;
// 按钮调整模式  true: 亮度调节  false: 色温调节
extern bool btn_mode;

typedef void (*CallbackFunc)(void);
// 同时按下的按钮结构体
typedef struct {
    uint8_t len;
    uint8_t *btns;// 按钮数组,目前用于匹配 btn_pins 的下标
    CallbackFunc callback;
} btn_all_pressed_t;

// 同时按下的按键数组, 用于判断是否同时按下
extern btn_all_pressed_t btn_all_pressed[];

// 按钮状态
extern int btn_init();


uint16 Key_ProcessEvent( uint8 task_id, uint16 events );

// 判断按键是否同时按下
bool btn_is_all_pressed();

// 模式切换
void btn_mode_switch();
#endif