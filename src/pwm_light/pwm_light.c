#include "pwm.h"
#include "gpio.h"
#include "gpio.h"
#include "log.h"

#include "pwm_light.h"

const uint8_t max_pwm_light = 5;
uint8_t pwm_light_len = 0;
// 申请5个通道的内存
pwm_t pwm_light_chann_list[max_pwm_light] = {
    {PWM_CH0, GPIO_DUMMY, 0, 100, 5, PWM_CLK_DIV_16},
    {PWM_CH1, GPIO_DUMMY, 0, 100, 5, PWM_CLK_DIV_16},
    {PWM_CH2, GPIO_DUMMY, 0, 100, 5, PWM_CLK_DIV_16},
    {PWM_CH3, GPIO_DUMMY, 0, 100, 5, PWM_CLK_DIV_16},
    {PWM_CH5, GPIO_DUMMY, 0, 100, 5, PWM_CLK_DIV_16},
};

// 刷新 pwm 引脚
static void pwm_light_reflash(void)
{

    // 遍历 pwm_light_chann_list
    for (int i = 0; i < max_pwm_light; i++)
    {
        // 获取 pwm
        pwm_t *pwm = &pwm_light_chann_list[i];
        // 判断 pwm 通道是否已经初始化;
        if(pwm->pin == GPIO_DUMMY){
            continue;
        }
        LOG("pwm ch%d is reflash \n", i);
        
        hal_pwm_close_channel(pwm->pwm_ch);
        hal_pwm_destroy(pwm->pwm_ch);

        hal_pwm_stop();

        hal_gpio_pin_init(pwm->pin, IE);
        hal_gpio_pull_set(pwm->pin, WEAK_PULL_UP);

        hal_pwm_init(pwm->pwm_ch, pwm->div, PWM_CNT_UP, PWM_POLARITY_FALLING);

        hal_pwm_set_count_val(pwm->pwm_ch, pwm->val, pwm->total);

        hal_pwm_open_channel(pwm->pwm_ch, pwm->pin);
    }
    hal_pwm_start();
}

// 获取 pwm
pwm_t *pwm_light_get(uint8_t ch)
{
    // 判断 ch 是否合法
    if (ch < 0 || ch > max_pwm_light)
    {
        ch = 0;
    }
    

    // 获取 pwm
    pwm_t *pwm = &pwm_light_chann_list[ch];

    return pwm;
}

// 初始化 pwm
int pwm_light_init(uint8_t ch, GPIO_Pin_e pin, int val, uint8_t total, uint8_t step, PWM_CLK_DIV_e div)
{
    pwm_t *pwm;
    // 判断 ch 是否合法
    CHECK_PWM_CH
    // 判断 pin 是否合法
    CHECK_PWM_PIN

    // 获取 pwm
    pwm = &pwm_light_chann_list[ch];

    if(val > total){
        val = total;
    }

    // 设置 pwm
    pwm->pin = pin;
    pwm->val = val;
    pwm->total = total;
    pwm->step = step;
    pwm->div = div;

    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}

int pwm_light_set_total(uint8_t ch, int total )
{
    pwm_t *pwm;
    // 判断 ch 是否合法
    CHECK_PWM_CH
    // 获取 pwm
    pwm = &pwm_light_chann_list[ch];
    pwm->total = total;
}

int pwm_light_set_pin(uint8_t ch, GPIO_Pin_e pin)
{
    pwm_t *pwm;
    // 判断 ch 是否合法
    CHECK_PWM_CH
    // 判断 pin 是否合法
    CHECK_PWM_PIN

    // 获取 pwm
    pwm = &pwm_light_chann_list[ch];

    // 设置 pwm
    pwm->pin = pin;

    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}

int pwm_light_set_val(uint8_t ch , uint8_t val)
{
    pwm_t *pwm;
    // 判断 ch 是否合法
    CHECK_PWM_CH

    // 获取 pwm
    pwm = &pwm_light_chann_list[ch];

     if(val > pwm->total){
        val = pwm->total;
    }

    LOG("light val %d --->> %d \n", pwm->val, val);
    // 设置 pwm
    pwm->val = val;
    
    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}

int pwm_light_set_div(uint8_t ch, PWM_CLK_DIV_e div)
{
    pwm_t *pwm;
    // 判断 ch 是否合法
    CHECK_PWM_CH

    // 判断 div 是否合法
    if (div < PWM_CLK_DIV_2 || div > PWM_CLK_DIV_128)
    {
        return -2;
    }

    // 获取 pwm
    pwm = &pwm_light_chann_list[ch];
    // 设置 pwm
    pwm->div = div;

    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}
