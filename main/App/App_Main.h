#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "Int_BDR6120.h"
#include "Int_WTN6170.h"
#include "Int_SC12B.h"
#include "Int_WS2812.h"
#include "Dri_NVS.h"
#include "Com_Config.h"
#include "Int_FPM383.h"
#include "string.h"

void App_Main_Init(void);

Com_Status App_Main_Get_Key_Info(char key_info[]);
//处理Com_OK时候的逻辑
void App_Main_handler(char *buf);

//处理Fingerprint逻辑
void App_Main_Finger_handler();

#endif /* __APP_MAIN_H__ */
