
// 最大按钮数量
#define HAL_KEY_NUM   2 

// 启用长按
#define HAL_KEY_SUPPORT_LONG_PRESS
// 是否启用按钮长按. 启用则设定长按时间
#ifdef HAL_KEY_SUPPORT_LONG_PRESS
#define HAL_KEY_LONG_PRESS_TIME      700    //2s
#define KEY_DEMO_LONG_PRESS_EVT   0x0200 
#endif

// 按钮消抖时间
#define HAL_KEY_DEBOUNCD              20      //20ms


// 按钮定义
// btn +
#define GPIO_BTN_PLUS GPIO_P01
// btn -
#define GPIO_BTN_MINUS GPIO_P02


// 防止重复定义
#ifndef BTN_PINS_DEFINED
#define BTN_PINS_DEFINED
static const GPIO_Pin_e btn_pins[HAL_KEY_NUM] = {
    GPIO_BTN_PLUS, 
    GPIO_BTN_MINUS
};
#endif

