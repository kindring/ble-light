
#ifndef __LIGHT_H__
#define __LIGHT_H__

// 暖光 pwm脚
#define GPIO_WARM P24
// 冷光 pwm脚
#define GPIO_COLD P25

// 风扇 pwm
#define GPIO_FAN P23
// btn +
#define GPIO_BTN_PLUS P26
// btn -
#define GPIO_BTN_MINUS P27


// adc 电源监测脚
#define GPIO_POWER P28

extern int light_init(void);

extern int light_set(uint8_t ch, uint8_t val);

#endif