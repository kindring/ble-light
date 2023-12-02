#ifndef __BTN_H__
#define __BTN_H__

#include "types.h"
#include "gpio.h"

#include "gpio_key.h"

extern uint8 task_btn_id;




// 按钮状态
extern int btn_init();


uint16 Key_ProcessEvent( uint8 task_id, uint16 events );

// 按钮表格
// extern GPIO_Pin_e btn_pins[];

#endif