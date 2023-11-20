#ifndef __PWM_LIGHT_H__
#define __PWM_LIGHT_H__

#include "gpio.h"



// pwm 结构体
typedef struct pwm_t {
    PWMN_e pwm_ch;// pwm通道
    GPIO_Pin_e pin;// gpio引脚
    uint8_t val;// 当前值 15
    uint8_t total;// 总量 0-100
    uint8_t step;// 步长 5
    PWM_CLK_DIV_e div;// pwm时钟分频
} pwm_t;

// pwm结构体数组 不定长 最大5个
extern pwm_t pwm_light_list[];

/**
 * @brief 初始化pwm
 * @param pwm pwm结构体
*/
extern int pwm_light_init(pwm_t *pwm);

/**
 * @brief 设置pwm值
 * @param pwm pwm结构体
 * @param val 新值
 * @return 0 成功 -1 失败
*/
extern int pwm_light_set_val(pwm_t *pwm, uint8_t val);

/**
 * @brief 设置pwm频率
 * @param pwm pwm结构体
 * @param div 目标频率
*/
extern int pwm_light_set_div(pwm_t *pwm, PWM_CLK_DIV_e div);


#endif