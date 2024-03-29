
#include "clock.h"
#include "log.h"
#include "gpio.h"


#include "btn.h"
#include "light.h"

uint8 task_btn_id;
#define KEY_DEMO_ONCE_TIMER      0x0001
#define KEY_DEMO_CYCLE_TIMER     0x0002

int last_release_btn_ind = -1;


void evt_press_release(int i){
	// 短按抬起  400ms 后检测是否还处于按下状态
	// LOG("evt_press_release %d\n", i);
	last_release_btn_ind = i;
	osal_stop_timerEx(task_btn_id, BTN_EVT_TIME_CHECK);
}

void changeTemp(int i)
{
	LOG("changeTemp \n");
	int temp = light_data.temp;
	switch (i)
	{
	case 0:
		temp = temp + TEMP_STEP;
		if (temp > TEMP_MAX){ temp = TEMP_MAX;}
		break;
	case 1:
		temp = temp - TEMP_STEP;
		if (temp < TEMP_MIN){ temp = TEMP_MIN;}
		break;
	default:
	
		break;
	}
	LOG("changeTemp temp=>>> %d\n", temp);
	temp_set(temp, 0, NULL);
}

void changeLight(int i)
{
	int light = light_data.light;
	switch (i)
	{
	case 0:
		light = light + LIGHT_STEP;
		if (light > 100){ light = 100;}
		break;
	case 1:
		light = light - LIGHT_STEP;
		if (light < 0){ light = 0;}
		break;
	default:
	
		break;
	}
	light_set(light, 0, NULL);
}

static void key_press_evt(uint8_t i,key_evt_t key_evt)
{
	// LOG("\nkey index:%d gpio:%d \n",i,key_state.key[i].pin);
	switch(key_evt)
	{
		case HAL_KEY_EVT_PRESS:
			// LOG("key(press down)\n");	
			osal_stop_timerEx(task_btn_id, BTN_EVT_TIME_CHECK);
			osal_start_timerEx(task_btn_id, BTN_EVT_TIME_CHECK, HAL_KEY_LONG_PRESS_TIME + TIME_CHECK_TEMP);
			changeLight(i);
			
#ifdef HAL_KEY_SUPPORT_LONG_PRESS
			osal_start_timerEx(task_btn_id, KEY_DEMO_LONG_PRESS_EVT, HAL_KEY_LONG_PRESS_TIME);
#endif
			break;
		
		case HAL_KEY_EVT_RELEASE:
			// LOG("key(press release)\n");
			evt_press_release(i);
			break;

#ifdef HAL_KEY_SUPPORT_LONG_PRESS		
		case HAL_KEY_EVT_LONG_RELEASE:
			// LOG("key(long press release)\n");
			break;
#endif
		
		default:
			LOG("unexpect\n");
			break;
	}
}

// key_state 引入

// 初始化按钮
void btn_init(uint8 taskId){
    LOG("[btn_init]\n");
    task_btn_id = taskId;
    // 初始化pwm
    for (int i = 0; i < HAL_KEY_NUM; i++)
    {
		hal_gpio_pin_init(btn_pins[i], IE);
        key_state.key[i].pin = btn_pins[i];
        key_state.key[i].state = HAL_KEY_EVT_PRESS;
        key_state.key[i].idle_level = HAL_HIGH_IDLE;
    }
    // 注册同时按下事件
    key_state.task_id = task_btn_id;
	key_state.key_callbank = key_press_evt;
	key_init();
}

// 按钮事件处理函数
uint16 Key_ProcessEvent( uint8 task_id, uint16 events )
{
	if(task_id != task_btn_id){
		return 0;
	}
	if( events & KEY_DEMO_ONCE_TIMER){		
		// LOG("once timer\n\n");
		osal_start_timerEx( task_btn_id, KEY_DEMO_ONCE_TIMER , 5000);
		return (events ^ KEY_DEMO_ONCE_TIMER);
	}
	if( events & KEY_DEMO_CYCLE_TIMER){		
		// LOG("recycle timer\n\n");
		return (events ^ KEY_DEMO_CYCLE_TIMER);
	}
	
	if( events & HAL_KEY_EVENT){	
		// LOG("HAL_KEY_EVENT \n");											//do not modify,key will use it
		for (uint8 i = 0; i < HAL_KEY_NUM; ++i){
			if ((key_state.temp[i].in_enable == TRUE)||
			(key_state.key[i].state == HAL_STATE_KEY_RELEASE_DEBOUNCE)){
				gpio_key_timer_handler(i);
			}
		}
		return (events ^ HAL_KEY_EVENT);
	}

#ifdef HAL_KEY_SUPPORT_LONG_PRESS	

	if( events & KEY_DEMO_LONG_PRESS_EVT){
		// LOG("KEY_DEMO_LONG_PRESS_EVT\n");
		for (int i = 0; i <= HAL_KEY_NUM; i++){
			// LOG("%d --- state [%d]",i, key_state.key[i].state);
			if(key_state.key[i].state == HAL_STATE_KEY_PRESS)
			{
				LOG("long press --- \n");
				osal_start_timerEx(task_btn_id, KEY_DEMO_LONG_PRESS_EVT, HAL_KEY_LONG_PRESS_TIME);
			}
		}
		return (events ^ KEY_DEMO_LONG_PRESS_EVT);
	}
#endif

	if( events & BTN_EVT_TIME_CHECK){
		// LOG("BTN_EVT_TIME_CHECK i: %d \n", last_release_btn_ind);
		if(last_release_btn_ind != -1)
		{
			// LOG(" io state %d", key_state.key[last_release_btn_ind].state );
			if (
				key_state.key[last_release_btn_ind].state == HAL_STATE_KEY_PRESS 
				)
			{
				// LOG("long press --- \n");
				osal_start_timerEx(task_btn_id, BTN_EVT_TIME_CHECK, TIME_CHECK_TEMP);
				changeTemp(last_release_btn_ind);
			}
		}
		
		return (events ^ BTN_EVT_TIME_CHECK);
	}
	
	return 0;

}
