#include "Int_BDR6120.h"
//!电机模块

//待命状态
static void Int_BDR6120_ReadyStart(void)
{
    gpio_set_level(Mask_BDR6120_A,0);
    gpio_set_level(Mask_BDR6120_B,0);
}

//前进状态
static void Int_BDR6120_Forword(void)
{
    gpio_set_level(Mask_BDR6120_A,1);
    gpio_set_level(Mask_BDR6120_B,0);
}

//后退状态
static void Int_BDR6120_Back(void)
{
    gpio_set_level(Mask_BDR6120_A,0);
    gpio_set_level(Mask_BDR6120_B,1);
}

//刹车
static void Int_BDR6120_Brake(void)
{
    gpio_set_level(Mask_BDR6120_A,1);
    gpio_set_level(Mask_BDR6120_B,1);
}

void Int_BDR6120_OpenLock(void)
{
    Int_BDR6120_ReadyStart();
    vTaskDelay(1000);
    Int_BDR6120_Forword();
    vTaskDelay(1000);
    Int_BDR6120_Brake();
    vTaskDelay(2000);
    Int_BDR6120_Back();
    vTaskDelay(1000);
    Int_BDR6120_Brake();
}

//初始化
void Int_BDR6120_Init(void)
{
    // GPIO初始化,实质上是给结构体赋值
    gpio_config_t io_conf = {};
    //暂时用不到中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //设置为推挽输出
    io_conf.mode = GPIO_MODE_OUTPUT;
    //禁用下拉
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //禁用上拉
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //设置引脚
    io_conf.pin_bit_mask = ((1<<Mask_BDR6120_A) | (1<<Mask_BDR6120_B));

    gpio_config(&io_conf);

}

