
#include "pwm.h"
#include "gpio.h"

#include "OSAL.h"
#include "log.h"
#include "error.h"

#include "pwm_light.h"

#include "light.h"
#include "fs.h"
uint8 task_light_id;

uint32_t _light_total = TOTAl_LEVEL;
// 配置 
light_data_t light_data = {true, TEMP_MAX, 100, LIGHT_MODE_DEFAULT, 100};

// 通知回调函数
LightCallbackFunc notify_callback = NULL;



int light_ch_set(uint8_t ch, uint16_t val){
    // LOG("[light_set] set ch%d val to %d \n", ch, val);
    int ret = pwm_light_set_val(ch, val);
    return ret;
}




// 计算 冷暖光的亮度值
uint8_t comLightVal(){
    int light_val = light_data.light, temp_val = light_data.temp;
    if(light_val > 100){
        light_val = 100;
    }
    if(temp_val > TEMP_MAX){
        temp_val = TEMP_MAX;
    }
    if(temp_val < TEMP_MIN){
        temp_val = TEMP_MIN;
    }
    // 防止数据异常
    light_data.light = light_val;
    light_data.temp = temp_val;

    // 根据色温与亮度来计算冷暖灯光对应的亮度值
    // 亮度值范围 0~100 翻 100倍
    // 色温范围 2500~6500
    // 暖色温最大亮度值 = 亮度值 * (色温 - 2500) / (6500 - 2500)
    // 亮度调整偏移后的暖色温亮度值 = 暖色温最大亮度值 + 亮度值 * 2500 / (6500 - 2500)
    int tmp_warm_val = _light_total * (temp_val - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
    int tmp_cold_val = _light_total - tmp_warm_val;
    int warm_val = light_val * tmp_warm_val / 100;
    int cold_val = light_val * tmp_cold_val / 100;
    LOG("[comLightVal] light: %d temp: %d warm_val %d, cold_val %d \n", light_val, temp_val, warm_val, cold_val);
    
    if(light_data.mode == LIGHT_MODE_FULL){
        // 全亮模式
        warm_val = light_val * _light_total / 100;
        cold_val = light_val * _light_total / 100;
    }


    if(!light_data.open){
        // 设备已经关闭, 设置为 0
        warm_val = 0;
        cold_val = 0;
    }

    light_ch_set(WARM_CH, warm_val);
    light_ch_set(COLD_CH, cold_val);


    return 0;
}


int save_file(uint16_t id,uint8_t* buf,uint16_t len){
    uint32_t free_size;
    int ret;
    if(hal_fs_initialized() == FALSE){
		ret = hal_fs_init(0x11005000,4);
		if(PPlus_SUCCESS != ret)
			LOG("error:%d\n",ret);
	}
    // 尝试移除文件
    if(hal_fs_item_del(id) == PPlus_SUCCESS)
    {
        LOG("del file ok\n"); 
        // 清理内存
        ret = hal_fs_garbage_collect();
        if(PPlus_SUCCESS != ret)
            LOG("error:%d\n",ret);
    }
    free_size = hal_fs_get_free_size();
	LOG("free_size:%d\n",free_size);
    if(len < free_size)
    {
        ret = hal_fs_item_write(id, buf, len);
    }else{
        LOG("free_size is not enough\n");
        ret = PPlus_ERR_DATA_SIZE;
    }

    if(PPlus_SUCCESS != ret)
        LOG("error:%d\n",ret);
    else
        LOG("write ok\n");
    return ret;
}
int read_file(uint16_t id,uint8_t* buf,uint16_t len){
    int ret;
    uint16_t file_len;
    ret = hal_fs_item_read(id, buf, len, &file_len);
    if(PPlus_SUCCESS != ret)
        LOG("error:%d\n",ret);
    else
        LOG("read ok file_len:%d\n", file_len);
    return ret;
}

int save_light_data(){
    LOG("Start save light data \n");
    // 保存数据到文件中
    uint8_t data[5];
    data[0] = light_data.open;
    data[1] = light_data.mode;
    data[2] = light_data.light;
    data[3] = light_data.temp >> 8;
    data[4] = light_data.temp & 0xff;
    LOG("[read_ligsave_light_dataht_data] data: ");
        for (int i = 0; i < 5; i++)
        {
            LOG("%02x ", data[i]);
        }
        LOG("\n");
    int ret = save_file(SAVE_FILE_ID, data, 5);
    return ret;
}

int read_light_data(){
    LOG("Start read light data read len:%d\n", sizeof(light_data_t));
    // 从文件中读取数据
    uint8_t data[5] = {0};
    int ret = PPlus_SUCCESS;
    if(hal_fs_initialized() == FALSE){
		ret = hal_fs_init(0x11005000,4);
		if(PPlus_SUCCESS != ret)
			LOG("fs init error:%d\n",ret);
	}
    ret = read_file(SAVE_FILE_ID, data, 5);
    if(ret == PPlus_SUCCESS){
        // 输出hex数据
        LOG("[read_light_data] data: ");
        for (int i = 0; i < 5; i++)
        {
            LOG("%02x ", data[i]);
        }
        LOG("\n");
        light_data.open = true;
        light_data.mode = data[1];
        light_data.light = data[2];
        light_data.temp = data[3] << 8 | data[4];
        
        LOG("read light data success light:%d temp:%d\n", light_data.light, light_data.temp);
        comLightVal();
    }
    return ret;
}



// [DEBUG] js code
// function comLightVal(light_val, temp_val){
//     // 根据色温与亮度来计算冷暖灯光对应的亮度值
//     // 亮度值范围 0~100
//     // 色温范围 2500~6500
//     // 暖色温最大亮度值 = 亮度值 * (色温 - 2500) / (6500 - 2500)
//     // 亮度调整偏移后的暖色温亮度值 = 暖色温最大亮度值 + 亮度值 * 2500 / (6500 - 2500)
//     let tmp_warm_val = 100 * (temp_val - 2500) / (6500 - 2500);
//     let tmp_cold_val = 100 - tmp_warm_val;
//     let warm_val = light_val * tmp_warm_val / 100;
//     let cold_val = light_val * tmp_cold_val / 100;
//     console.log(`[comLightVal] warm_val: ${warm_val} , cold_val: ${cold_val}\n`);
// }

// 计算命令响应码
uint16 comCmdResCode(uint8_t cmd, uint8_t sn, uint8* data, uint16 len, uint8_t *res){
    // 响应码 `起始码` `长度` `命令码` `sn码` `数据1` `数据2`
    // 0x6c 0x06 0x01 0x64 
    // 判断是否有sn码,有则为响应,没有则为通知
    // 申请内存
    if(sn == 0)
    {
        res[0] = START_CODE_NOTIFY;
    }else
    {
        res[0] = START_CODE_RES;
    }
    // 数据长度
    res[1] = len + CMD_LEN_LEN;
    // 命令码
    res[2] = cmd;
    // sn码
    res[3] = sn;
    // 数据可能为空
    if(len > 0){
        // 数据拷贝
        memcpy(res + CMD_HEADER_LEN, data, len);
    }
    // 打印对应的hex数据体
    LOG("[comCmdResCode] data: ");
    for (int i = 0; i < len + CMD_HEADER_LEN; i++)
    {
        LOG("%02x ", res[i]);
    }
    LOG("\n");

    // 构建一个设置码,将light_data_t写入到文件中
    
    // 取消上一个定时器
    osal_stop_timerEx( task_light_id, LIGHT_EVT_SAVE_DATA);
    // 启动定时器, 准备保存数据
    osal_start_timerEx( task_light_id, LIGHT_EVT_SAVE_DATA, SAVE_DATA_WAIT_TIME * 1000);
    
    return len + 4;
}


/**
 * 开灯
 */
uint16 open_light(uint8_t sn , uint8 *res){
    light_data.open = true;
    comLightVal();
    uint16 resLen = 0;
    if(res == NULL && notify_callback != NULL)
    {
        uint8 *_res = osal_mem_alloc(CMD_MAX_LEN);
        resLen = comCmdResCode(CMD_OPEN, sn, NULL, 0, _res);
        notify_callback(res, resLen);
        osal_mem_free(_res);
    }else{
        resLen =  comCmdResCode(CMD_OPEN, sn, NULL, 0, res);
    }
    return resLen;
}

// 关闭灯光
uint16 close_light(uint8_t sn , uint8 *res){
    light_data.open = false;
    comLightVal();
    uint16 resLen = 0;
    if(res == NULL && notify_callback != NULL)
    {
        uint8 *_res = osal_mem_alloc(CMD_MAX_LEN);
        resLen = comCmdResCode(CMD_CLOSE, sn, NULL, 0, _res);
        notify_callback(res, resLen);
        osal_mem_free(_res);
    }else{
        resLen = comCmdResCode(CMD_CLOSE, sn, NULL, 0, res);
    }
    return resLen;
}

// 灯光状态查询
uint16 query_light(uint8_t sn , uint8 *res){
    uint8_t data[4] = {
        light_data.open, 
        light_data.temp, 
        light_data.light, 
        light_data.mode
    };
    return comCmdResCode(CMD_QUERY, sn, data, 4, res);
}

// 亮度调节
uint16 light_set(uint8 val, uint8_t sn , uint8 *res){
    // LOG("[light_set] set light val to %d \n", val);
    // 亮度值为 0~100
    if(val > 100){
        val = 100;
    }
    light_data.light = val;
    comLightVal();
    uint8 data[1] = {val};
    uint16 resLen = 0;
    if(res == NULL && notify_callback != NULL)
    {
        uint8 *_res = osal_mem_alloc(CMD_MAX_LEN);
        resLen = comCmdResCode(CMD_LIGTH, sn, data, 1, _res);
        notify_callback(_res, resLen);
        osal_mem_free(_res);
    }else{
        resLen = comCmdResCode(CMD_LIGTH, sn, data, 1, res);
    }
    // 输出hex数据
    LOG("[light_set] res: ");
    for (int i = 0; i < resLen; i++)
    {
        LOG("%02x ", res[i]);
    }
    LOG("\n");
    
    return resLen;
}

// 色温调节
uint16 temp_set(int temp, uint8_t sn , uint8 *res){
    // LOG("[temp_set] set temp val to %d \n", temp);
    if(temp > TEMP_MAX){
        temp = TEMP_MAX;
    }
    if(temp < TEMP_MIN){
        temp = TEMP_MIN;
    }
    light_data.temp = temp;
    comLightVal();
    // 如果res是空,则尝试调用通知回调函数
    uint8_t data[2] = {temp >> 8, temp & 0xff};
    uint16 resLen = 0;
    if(res == NULL && notify_callback != NULL)
    {
        uint8 *_res = osal_mem_alloc(CMD_MAX_LEN);
        resLen = comCmdResCode(CMD_TEMP, sn, data, 2, _res);
        notify_callback(res, resLen);
        osal_mem_free(_res);
    }else{
        resLen = comCmdResCode(CMD_TEMP, sn, data, 2, res);
    }
    return resLen;
}

/**
 * led 模式切换
*/
uint16 change_light_mode (int mode, uint8_t sn, uint8 *res)
{
    uint16 resLen = 0;
    if (mode == light_data.mode)
    {
        // 模式相同, 不做处理
        return resLen;
    }
    light_data.mode = mode;
    if (mode == LIGHT_MODE_FULL)
    {
        // 全亮模式, 切换至最大模式
        light_data.light = 100;
        light_data.fan = 100;
        // 添加定时器, 1分钟后切换至默认模式 
        osal_start_timerEx( task_light_id, LIGHT_EVT_DEFAULT_MODE, FULL_MODE_WAIT_TIME * 1000);
    }
    comLightVal();
    uint8_t data[1] = {mode};
    
    if(res == NULL && notify_callback != NULL)
    {
        uint8 *_res = osal_mem_alloc(CMD_MAX_LEN);
        resLen = comCmdResCode(CMD_MODE, sn, data, 1, _res);
        notify_callback(res, resLen);
        osal_mem_free(_res);
    }else{
        resLen = comCmdResCode(CMD_MODE, sn, data, 1, res);
    }
    return resLen;
}

// 指令匹配
// 指令码 `起始码` `长度` `命令码` `sn码` `数据1` `数据2`
// 最低长度要为 4 个字节, 部分命令没有数据
// 0x6c 0x06 0x01 0x64 
/**
 * 指令解析
 * @param {uint8*} data 数据
 * @param {uint16} len 数据长度
 * @param {uint8*} res 返回数据 
*/
uint16 parse_light_code(const uint8* data, uint16 len, uint8 *res)
{
    light_cmd_t light_cmd;
    uint16 resLen = 0;
    // 判断是否为灯光指令 
    if (len < 4 || data[0] != START_CODE_CMD)
    {
        LOG("[parse_light_code] not light cmd \n");
        return resLen;
    }
    // 假设 len 是数据的长度
    memcpy(&light_cmd, data, len);
    LOG("[parse_light_code] cmd: %d, sn: %d, len: %d \n", light_cmd.cmd, light_cmd.sn, light_cmd.len);
    // 为data分配内存
    light_cmd.data = (uint8_t *)osal_mem_alloc(light_cmd.len - 2);
    memcpy(light_cmd.data, data + 4, light_cmd.len - 2);
    // 打印对应的hex数据体
    LOG("[parse_light_code] data: ");
    for (int i = 0; i < light_cmd.len - 2; i++)
    {
        LOG("%02x ", light_cmd.data[i]);
    }
    LOG("\n");
    // 现在你可以使用 light_cmd 结构体中的数据了
    // 如果需要访问数据部分，可以使用 light_cmd.data 指针
    switch (light_cmd.cmd)
    {
    case CMD_OPEN:
        LOG("[parse_light_code] CMD_OPEN \n");
        resLen = open_light(light_cmd.sn, res);
        break;
    case CMD_CLOSE:
        LOG("[parse_light_code] CMD_CLOSE \n");
         resLen = close_light(light_cmd.sn, res);
        break;
    case CMD_QUERY:
        LOG("[parse_light_code] CMD_QUERY \n");
         resLen = query_light(light_cmd.sn, res);
        break;
    case CMD_LIGTH:
        LOG("[parse_light_code] CMD_LIGTH %d\n", light_cmd.data[0]);
         resLen = light_set(light_cmd.data[0], light_cmd.sn, res);
        break;
    case CMD_TEMP:
        LOG("[parse_light_code] CMD_TEMP %d\n", light_cmd.data[0] << 8 | light_cmd.data[1]);
         resLen = temp_set(light_cmd.data[0] << 8 | light_cmd.data[1], light_cmd.sn, res);
        break;
    case CMD_MODE:
        LOG("[parse_light_code] CMD_MODE %d\n", light_cmd.data[0]);
         resLen = change_light_mode(light_cmd.data[0], light_cmd.sn, res);
        break;
    }
    LOG("[parse_light_code] res: ");
    for (int i = 0; i < resLen; i++)
    {
        LOG("%02x ", res[i]);
    }
    LOG("\n");
    // 释放内存
    osal_mem_free(light_cmd.data);
    return resLen;
}

// 注册通知回调函数,用于再某些情况下通知上位机
void light_register_notify_callback(LightCallbackFunc callback)
{
    LOG("init light notify callback \n");
    notify_callback = callback;
}


// 定时器事件处理
uint16 Light_ProcessEvent( uint8 task_id, uint16 events )
{
    if(task_id != task_light_id){
		return 0;
	}
    if( events & LIGHT_EVT_DEFAULT_MODE){
        // 切换至默认模式
        uint8 *res = osal_mem_alloc(CMD_MAX_LEN);
        change_light_mode(LIGHT_MODE_DEFAULT, 0, res);
        osal_mem_free(res);
        return (events ^ LIGHT_EVT_DEFAULT_MODE);
    }
    if( events & LIGHT_EVT_SAVE_DATA){
        // 保存数据
        save_light_data();
        return (events ^ LIGHT_EVT_SAVE_DATA);
    }
}




void light_init(uint8 taskId){
    LOG("[light_init]\n");
    task_light_id = taskId;
    // 初始化pwm
    int ret = 0;
    ret = pwm_light_init(WARM_CH, GPIO_WARM, 0, _light_total, 5, PWM_CLK_DIV_16);
    ret = pwm_light_init(WARM_CH, GPIO_WARM2, 0, _light_total, 5, PWM_CLK_DIV_16);
    if(ret != 0){
        LOG("[light_init] pwm_light_init warm failed %d \n", ret);
        return ;
    }

    ret = pwm_light_init(COLD_CH, GPIO_COLD, 0, _light_total, 5, PWM_CLK_DIV_16);
    ret = pwm_light_init(COLD_CH, GPIO_COLD2, 0, _light_total, 5, PWM_CLK_DIV_16);
    if(ret != 0){
        LOG("[light_init] pwm_light_init cold failed  %d \n", ret);
        return ;
    }

    // 读取保存的数据
    read_light_data();
    // 拉低 warm2 与 cold2 的电平

    // hal_gpio_pull_set(GPIO_WARM2, PULL_DOWN);
    // hal_gpio_pull_set(GPIO_COLD2, PULL_DOWN);
}