#ifndef __INT_FPM383_H__
#define __INT_FPM383_H__

#include "driver/gpio.h"
#include "driver/uart.h"
#include "Com_Config.h"
#include "esp_log.h"
#include <string.h>



void Int_FPM383_Init(void);

void Int_FPM383_Sleep(void);

void Int_FPM383_Test(void);

// 获取最小ID
uint16_t Int_FPM383_Get_Min_ID(void);
// 自动注册用户指纹
Com_Status Int_FPM383_AutoEnroll(uint16_t id);

//删除所有用户指纹
Com_Status Int_FPM383_Clear_DB(void);

// 验证用户指纹
Com_Status Int_FPM383_Verify_Fingerprint(void);

// 获取删除ID
int16_t Int_FPM383_Get_Del_ID(void);

/**
 * @brief 删除用户指纹
 * 
 * @param page_num 指纹id
 * @return Com_Status Com_OK 删除成功
 * @return Com_Status Com_ERROR 删除失败
 */
Com_Status Int_FPM383_Delete_Template(uint16_t page_num);


#endif /* __INT_FPM383_H__ */
