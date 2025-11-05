#include "Int_WTN6170.h"
//!语音模块
void Int_WTN6170_Init(void)
{
    gpio_config_t io_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask =(1<<WTN6170_DATA_PIN),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE
    };

    gpio_config(&io_config);
    WTN6170_PIN_H;
}

void Int_WTN6170_SendCMD(uint8_t cmd)
{
    WTN6170_PIN_L;
    vTaskDelay(10);
    for (uint8_t i = 0; i < 8; i++)
    {
        if (cmd & 0x01)
        {
            WTN6170_PIN_H;
            usleep(600);
            WTN6170_PIN_L;
            usleep(200);
        }
        else
        {
            WTN6170_PIN_H;
            usleep(200);
            WTN6170_PIN_L;
            usleep(600);
        }
        cmd >>= 1;

    }
    WTN6170_PIN_H;
    vTaskDelay(5);
}
