#ifndef __INT_SC12B_H__
#define __INT_SC12B_H__

#include "driver/gpio.h"
#include "driver/i2c.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define SC_SDA_PIN GPIO_NUM_2
#define SC_SCL_PIN GPIO_NUM_1
#define SC_INT_PIN GPIO_NUM_0
#define SC_I2C_MASTER_FREQ_HZ 400000
#define SLAVE_ADDR 0x42

typedef enum{
    key_0,
    key_1,
    key_2,
    key_3,
    key_4,
    key_5,
    key_6,
    key_7,
    key_8,
    key_9,
    key_sharp,
    key_m,
    key_no
}Touch_Key;

void Int_SC12B_Init(void);

Touch_Key Int_SC12B_GetKey(void);

#endif /* __INT_SC12B_H__ */
