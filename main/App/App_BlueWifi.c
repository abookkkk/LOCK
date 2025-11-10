#include "App_BlueWifi.h"

void App_BlueWifi_Init(void)
{
    Dri_BLUE_Init();
}

// 实现蓝牙模块的弱函数,只要蓝牙模块接收到数据,次函数执行一次！接收到蓝牙信息
void App_Communication_RecvDataCb(uint8_t *data, uint16_t dataLen)
{

    uint8_t pwd[20] = {0};
    // A+数字  A:添加命令 数字密码  A+111111
    // B+数字  B:删除命令 数字密码
    // C+数字  C:输入密码开门   数字密码
    strcpy((char *)pwd, (char *)&data[2]);

    // printf("%s\r\n",pwd);

    switch (data[0])
    {
    case 'A':
        esp_err_t res1 = Dri_NVS_Write_U8((char *)pwd, 0);
        if (res1 == ESP_OK)
        {
            sayAddSucc();
        }
        else
        {
            sayAddFail();
        }
        break;
    case 'B':
        esp_err_t res2 = Dri_NVS_Del_User((char *)pwd);
        if (res2 == ESP_OK)
        {
            sayDelSucc();
        }
        else
        {
            sayDelFail();
        }
        break;
    case 'C':
        esp_err_t res3 = Dri_NVS_Has_Key((char *)pwd);
        if (res3 == ESP_OK)
        {
            sayWithoutInt();
            sayDoorOpen();
            sayWithoutInt();
            sayWaterDrop();

            Int_BDR6120_OpenLock();
        }
        else
        {
            sayVerifyFail();
        }
        break;

    default:
        break;
    }
}