#include "pwm.h"
#include "gpio.h"

#include "OSAL.h"
#include "log.h"

#include "pwm_light.h"

#include "light.h"

// pwm 结构体数组 不定长 最大5个
pwm_t pwm_light_list[] = {
    {PWM_CH0, GPIO_WARM, 0, 100, 5, PWM_CLK_DIV_16},
    {PWM_CH1, GPIO_COLD, 0, 100, 5, PWM_CLK_DIV_16},
    {PWM_CH2, GPIO_FAN, 0, 100, 5, PWM_CLK_DIV_16},
};

int light_init(){
    LOG("[light_init]\n");
    // 初始化pwm
    int ret = 0;
    for (int i = 0; i < sizeof(pwm_light_list) / sizeof(pwm_t); i++) {
        ret = pwm_light_init(&pwm_light_list[i]);
        if (ret != 0) {
            LOG("pwm_light_init err %d\n", ret);
            return ret;
        }
    }
    return ret;
}

int light_set(uint8_t ch, uint8_t val){
    LOG("[light_set] set ch%d val to %d\n", ch, val);
    int ret = 0;
    if (ch >= sizeof(pwm_light_list) / sizeof(pwm_t)) {
        LOG("ch%d not exist\n", ch);
        return -1;
    }
    pwm_t *pwm = &pwm_light_list[ch];
    ret = pwm_light_set_val(pwm, val);
    return ret;
}