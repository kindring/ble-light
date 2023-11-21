#include "pwm.h"
#include "gpio.h"

#include "OSAL.h"
#include "log.h"

#include "pwm_light.h"

#include "light.h"

// 配置

int light_init(){
    LOG("[light_init]\n");
    // 初始化pwm
    int ret = 0;
    if()
    ret = pwm_light_init(0, GPIO_WARM, 100, 100, 5, PWM_CLK_DIV_16);
    if(ret != 0){
        LOG("[light_init] pwm_light_init warm failed\n");
        return ret;
    }
    ret = pwm_light_init(1, GPIO_COLD, 100, 100, 5, PWM_CLK_DIV_16);
    if(ret != 0){
        LOG("[light_init] pwm_light_init cold failed\n");
        return ret;
    }

    return ret;
}

int light_set(uint8_t ch, uint8_t val){
    LOG("[light_set] set ch%d val to %d\n", ch, val);
    ret = pwm_light_set_val(ch, val);
    return ret;
}