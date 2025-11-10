#include <stdio.h>
#include "esp_task.h"
#include "App_Main.h"
#include "Int_FPM383.h"
#include "App_BlueWifi.h"

#define buf_MaxSize 20

void Touch_Task(void *par);
TaskHandle_t touch_handler;

void Finger_Task(void *par);
TaskHandle_t finger_handler;

void app_main(void)
{
    App_Main_Init();
    App_BlueWifi_Init();
    // Int_FPM383_Test();

    xTaskCreate(Touch_Task, "Touch_Task", 4096, NULL, 4, &touch_handler);
    xTaskCreate(Finger_Task, "Finger_Task", 2048, NULL, 4, &finger_handler);
}

// 按键任务回调
void Touch_Task(void *par)
{

    char buf[20] = {};
    while (1)
    {
        Com_Status status = App_Main_Get_Key_Info(buf);
        switch (status)
        {
        case Com_ERROR:
            sayIllegalOperation();
            break;
        case Com_OK:
            // 在App_Main中实现
            App_Main_handler(buf);
            break;
        case Com_TIMEOUT:
            Int_WS2812_LED_OFF();
            break;
        default:
            break;
        }
        vTaskDelay(50);
    }
}

// 指纹任务回调
void Finger_Task(void *par)
{
    while (1)
    {
        App_Main_Finger_handler();
        vTaskDelay(50);
    }
}