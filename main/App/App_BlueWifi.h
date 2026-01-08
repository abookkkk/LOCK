#ifndef __APP_BLUEWIFI_H__
#define __APP_BLUEWIFI_H__

#include "Dri_BLUE.h"
#include "Dri_NVS.h"
#include "Int_BDR6120.h"
#include "Int_WTN6170.h"
#include <string.h>

//OTA头文件
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"
#include "string.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>
#include "Dri_WIFI.h"
#define HASH_LEN 32
#define OTA_URL_SIZE 256

void App_BlueWifi_Init(void);

//初始化OTA功能
void App_OTA_Init(void);
#endif /* __APP_BLUEWIFI_H__ */
