#include "App_BlueWifi.h"

// 创建OTA任务句柄
TaskHandle_t ota_task_handle = NULL;
void App_OTA_Task(void *pvParameters);

void App_BlueWifi_Init(void)
{
    Dri_BLUE_Init();

    // 创建OTA任务，挂起状态
    xTaskCreate(App_OTA_Task, "ota_task", 4096, NULL, 5, &ota_task_handle);
    vTaskSuspend(ota_task_handle); // 暂停任务，等待通知
}
// OTA任务函数，使用任务通知等待执行
void App_OTA_Task(void *pvParameters)
{
    uint32_t notification_value;
    // const uint32_t ota_notification_bit = 0x01; // OTA通知位

    while (1)
    {
        // 等待任务通知
        notification_value = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (notification_value > 0)
        {
            // 收到通知，执行OTA升级
            printf("Starting OTA upgrade from task notification\r\n");
            App_OTA_Init();
            vTaskSuspend(NULL); // 升级完成后再次挂起任务，等待下一次通知
        }
    }
}

// 实现蓝牙模块的弱函数,只要蓝牙模块接收到数据,此函数执行一次！接收到蓝牙信息
void App_Communication_RecvDataCb(uint8_t *data, uint16_t dataLen)
{

    uint8_t pwd[20] = {0};
    // A+数字  A:添加命令 数字密码  A+111111
    // B+数字  B:删除命令 数字密码
    // C+数字  C:输入密码开门   数字密码
    // O       O:OTA升级命令
    strcpy((char *)pwd, (char *)&data[2]);

    // printf("%s\r\n",pwd);

    switch (data[0])
    {
    case 'A':
        esp_err_t res1 = Dri_NVS_Write_U8((char *)pwd, 0); // value并不重要设置为零
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
    case 'O': // OTA升级命令
        printf("OTA command received, sending notification to OTA task\r\n");
        // 通过任务通知唤醒OTA任务
        if (ota_task_handle != NULL)
        {
            xTaskNotifyGive(ota_task_handle);
            // 把挂起的任务重新启动
            vTaskResume(ota_task_handle);
        }
        break;
    default:
        break;
    }
}

static const char *TAG = "simple_ota";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

static void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i)
    {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s %s", label, hash_print);
}

static void get_sha256_of_partitions(void)
{
    uint8_t sha_256[HASH_LEN] = {0};
    esp_partition_t partition;

    // get sha256 digest for bootloader
    partition.address = ESP_BOOTLOADER_OFFSET;
    partition.size = ESP_PARTITION_TABLE_OFFSET;
    partition.type = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");
}

void App_OTA_Init(void)
{
    // TODO 初始化flash  在蓝牙中已经初始化了,在这里验证一下
    //  esp_err_t err = nvs_flash_init();
    //  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    //  {
    //      ESP_ERROR_CHECK(nvs_flash_erase());
    //      err = nvs_flash_init();
    //  }
    const esp_partition_t *running = esp_ota_get_running_partition();
    printf("Current running partition: %s\r\n", running->label);
    get_sha256_of_partitions();

    // 初始化wifi
    Dri_WIFI_Init();
    // HTTP客户端配置
    esp_http_client_config_t config = {
        .url = "http://10.13.208.15:8080/Hello.bin", // 本地服务器地址，需要先进入到服务器所在目录即执行cd D:\Embed_Studay\projects\intelligent_door_lock\ota，
        // 再执行python -m http.server 8080命令启动服务器
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = NULL,
        .keep_alive_enable = true,
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    // 执行OTA升级
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        printf("OTA upgrade success\r\n");
        esp_restart();
    }
    else
    {
        printf("OTA upgrade failed\r\n");
    }
    // OTA完成后，可以挂起任务等待下一次通知
    // if (ota_task_handle != NULL)
    // {
    //     vTaskSuspend(ota_task_handle);
    // }
}