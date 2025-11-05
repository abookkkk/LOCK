#ifndef __INT_WS2812_H__
#define __INT_WS2812_H__

#include "driver/gpio.h"
#include "led_strip.h"
#include "Int_SC12B.h"

#define LED_PIN GPIO_NUM_6
#define LED_MAX_NUM 12
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)
#define LED_STRIP_MEMORY_BLOCK_WORDS 0
#define LED_STRIP_USE_DMA 0

extern uint8_t black[3];
extern uint8_t white[3];
extern uint8_t red[3];
extern uint8_t green[3];
extern uint8_t blue[3];
extern uint8_t cyan[3];
extern uint8_t purple[3];
extern uint8_t pink[3];

void Int_WS2812_Init(void);

void Int_WS2812_LED_ON(Touch_Key key_num,uint8_t clolor[]);

void Int_WS2812_LED_OFF(void);

#endif /* __INT_WS2812_H__ */
