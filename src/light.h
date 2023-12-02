#ifndef __LIGHT_H__
#define __LIGHT_H__

// 暖光 pwm脚
#define GPIO_WARM P18
// 冷光 pwm脚
#define GPIO_COLD P20

// 风扇 pwm
#define GPIO_FAN P23




// adc 电源监测脚
#define GPIO_POWER P28

extern int light_init(void);

extern int light_set(uint8_t ch, uint8_t val);

#endif