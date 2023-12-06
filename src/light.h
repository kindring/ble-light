#ifndef __LIGHT_H__
#define __LIGHT_H__

// 暖光 pwm脚
#define GPIO_WARM P18
#define WARM_CH 0
// 冷光 pwm脚
#define GPIO_COLD P20
#define COLD_CH 1

// 风扇 pwm
#define GPIO_FAN P23

// 亮度分级 0~100 翻 100倍

#define TEMP_MIN 2500
#define TEMP_MAX 6500

// adc 电源监测脚
#define GPIO_POWER P28

extern int light_init(void);

extern int light_ch_set(uint8_t ch, uint16_t val);

// 计算冷暖光的亮度值,并且设置
int comLightVal();

extern int light_set(uint8_t val);
extern int temp_set(int temp);



// 灯光数据 色温, 亮度
typedef struct {
    int temp;
    int light;
} light_data_t;

#endif