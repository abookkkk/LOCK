#include "Dri_NVS.h"
nvs_handle_t my_handle;
void Dri_NVS_Init(void)
{
    // 初始化NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    err = nvs_open("PWD", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("init nvs error\r\n");
    }
    else
    {
        printf("init nvs succ\r\n");
    }
}

esp_err_t Dri_NVS_Write_Str(char *key, char *value)
{
    return nvs_set_str(my_handle,key,value);
}

esp_err_t Dri_NVS_Get_Str(char *key,char *buffer,size_t *buffer_MaxSize)
{

    return nvs_get_str(my_handle,key,buffer,&buffer_MaxSize);
}

esp_err_t Dri_NVS_Write_U8(char *key, uint8_t value)
{
    return nvs_set_u8(my_handle,key,value);
}

esp_err_t Dri_NVS_Get_U8(char *key,uint8_t *value)
{
    return nvs_get_u8(my_handle,key,value);
}

esp_err_t Dri_NVS_Has_Key(char *key)
{
    return nvs_find_key(my_handle,key,NULL);
}

esp_err_t Dri_NVS_Del_User(char *key)
{
    return nvs_erase_key(my_handle,key);
}

esp_err_t Dri_NVS_Del_All_User(void)
{
    return nvs_erase_all(my_handle);
}
