#ifndef __WRIST_H
#define __WRIST_H

#include "types.h"
// 启动事件
#define START_DEVICE_EVT                        0x0001

// 事件定义

// 该事件用于同步时间, 组件内依赖该事件. 必须要定义该事件
#define TIMER_UI_EVT                            0x0002  //for UI timer event
#define TIMER_DT_EVT                            0x0004  //for datetime sync
#define TOUCH_PRESS_EVT                         0x0008  //for touch key event
#define TIMER_HR_EVT                            0x0010  //for heartrate detect
#define TIMER_BATT_EVT                          0x0020  //for battery detect
#define BATT_VALUE_EVT                          0x0040  //event for battery voltage value update
#define BATT_CHARGE_EVT                         0x0080  //event for battery charge status change
#define ACC_DATA_EVT                            0x0100  //event for accelerator data change
#define TIMER_LIGHT_EVT                         0x0200  //for led light timeout
#define TIMER_KSCAN_DEBOUNCE_EVT                0x0400  //for keyscan debounce
#define TIMER_KSCAN_REPEAT_EVT                  0x0800  //for keyscan repeat
#define TIMER_KSCAN_LONG_EVT                    0x1000  //for keyscan long press
#define TIMER_KSCAN_LONG_REPEAT_EVT             0x2000  //for keyscan long press repeat
#define TIMER_KSCAN_LONG_REPEAT_STOP_EVT        0x4000  //for keyscan long press repeat stop


// 电池电量监测
#define BATTERY_MONITOR_EVT                     0x0004







// 从 pwm 案例中获取的宏定义

// Fast advertising interval in 625us units
#define DEFAULT_FAST_ADV_INTERVAL             32

// Duration of fast advertising duration in ms
#define DEFAULT_FAST_ADV_DURATION             30000

// Slow advertising interval in 625us units
#define DEFAULT_SLOW_ADV_INTERVAL             1600

// Duration of slow advertising duration in ms (set to 0 for continuous advertising)
#define DEFAULT_SLOW_ADV_DURATION             0

// How often to perform heart rate periodic event
#define DEFAULT_HEARTRATE_PERIOD              2000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         FALSE

// Minimum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     200

// Maximum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     1600

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         1

// Supervision timeout value (units of 10ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000


//  蓝牙名称
#define BleName "bleLight"

// 事件处理Id
extern uint8 AppWrist_TaskID;


// GAP connection handle
static uint16 gapConnHandle;




// 初始化 蓝牙应用
extern void appWristInit( uint8 task_id );

// 应用事件处理器
extern uint16 appWristProcEvt( uint8 task_id, uint16 events );

static void wristCB(uint8 event, uint8 param_size, uint8* param);


#endif /* __WRIST_H */