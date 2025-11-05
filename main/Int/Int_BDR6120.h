#ifndef __INT_BDR6120_H__
#define __INT_BDR6120_H__

#define Mask_BDR6120_A GPIO_NUM_4
#define Mask_BDR6120_B GPIO_NUM_5

#include "esp_task.h"
#include "driver/gpio.h"


void Int_BDR6120_Init(void);

void Int_BDR6120_OpenLock(void);
#endif /* __INT_BDR6120_H__ */
