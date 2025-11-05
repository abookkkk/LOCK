#ifndef __DRI_NVS_H__
#define __DRI_NVS_H__

#include "nvs_flash.h"

void Dri_NVS_Init(void);

//向nvs写入字符串
esp_err_t Dri_NVS_Write_Str(char *key, char *value);
//向nvs读取字符串
esp_err_t Dri_NVS_Get_Str(char *key,char *buffer,size_t *buffer_MaxSize);
//向nvs写入uint8_t
esp_err_t Dri_NVS_Write_U8(char *key, uint8_t value);
//向nvs读取uint8_t
esp_err_t Dri_NVS_Get_U8(char *key,uint8_t *value);

//查看nvs中是否有key
esp_err_t Dri_NVS_Has_Key(char *key);
//删除nvs中的用户
esp_err_t Dri_NVS_Del_User(char *key);
//删除nvs中的所有用户
esp_err_t Dri_NVS_Del_All_User(void);
#endif /* __DRI_NVS_H__ */
