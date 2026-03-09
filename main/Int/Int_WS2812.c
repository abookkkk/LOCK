#include "Int_WS2812.h"
//! 灯带模块
uint8_t black[3] = {0, 0, 0};
uint8_t white[3] = {255, 255, 255};
uint8_t red[3] = {0, 255, 0};
uint8_t green[3] = {255, 0, 0};
uint8_t blue[3] = {0, 0, 255};
uint8_t cyan[3] = {255, 0, 255};   /* 青色 */
uint8_t purple[3] = {0, 255, 255}; /* 紫色 */
uint8_t pink[3] = {255, 204, 255};

led_strip_handle_t strip_handle;
void Int_WS2812_Init(void)
{
    // 配置灯带
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,
        .max_leds = LED_MAX_NUM,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
        .flags.invert_out = false};

    // 配置RMT(红外，不是遥控器)
    /*红外通信协议（NEC、Sony、RC5等）
    其他需要精确时序的单线协议
    编码器信号解码
    脉冲宽度调制（PWM）
    自定义协议的信号生成和接收  */
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_BASECLK_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS,
        .flags.with_dma = LED_STRIP_USE_DMA // 0,不用DMA
    };
    // 真正的初始化

    led_strip_new_rmt_device(&strip_config, &rmt_config, &strip_handle);
}

void Int_WS2812_LED_ON(Touch_Key key_num, uint8_t clolor[])
{

    led_strip_set_pixel(strip_handle, key_num, clolor[0], clolor[1], clolor[2]);
    led_strip_refresh(strip_handle);
}

void Int_WS2812_LED_OFF(void)
{
    led_strip_clear(strip_handle);
}
