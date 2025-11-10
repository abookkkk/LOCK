#include "App_Main.h"

static void App_Main_AddAdmin(void);
static void App_Main_DelAdmin(void);
static void App_Main_AddUser(void);
static void App_Main_DelUser(void);
static void App_Main_OpenDoor(char *buf);

#define ADMIN "admin"
#define Int_FPM383_IT GPIO_NUM_10

extern TaskHandle_t finger_handler;
extern uint8_t touch_flag;

size_t buf_max_size = 20;
char first_buf[20] = {};
char second_buf[20] = {};
uint8_t delete_confirmation_pending;

// 重置缓冲区
static void App_Main_resetBuffers(void)
{
    memset(first_buf, 0, sizeof(first_buf));
    memset(second_buf, 0, sizeof(second_buf));
}

void App_Main_Init(void)
{
    // 初始化电机
    Int_BDR6120_Init();
    // 初始化按键
    Int_SC12B_Init();
    // 初始化灯带
    Int_WS2812_Init();
    // 初始化语音
    Int_WTN6170_Init();
    // 初始化flash
    Dri_NVS_Init();
    // 初始化指纹
    Int_FPM383_Init();
}
// 验证输入的和管理员密码是否一致以及有无管理员
static Com_Status App_Main_Verify_Password(void)
{
    if (Dri_NVS_Has_Key(ADMIN) == ESP_OK)
    {
        sayWithoutInt();
        sayInputAdminPassword();
        Com_Status status = App_Main_Get_Key_Info(first_buf);
        switch (status)
        {
        case Com_OK:
            Dri_NVS_Get_Str(ADMIN, second_buf, &buf_max_size);
            if (strcmp(first_buf, second_buf) == 0)
            {
                App_Main_resetBuffers();
                return Com_OK;
            }
            else
            {
                sayInvalid();
                App_Main_resetBuffers();
                return Com_ERROR;
            }
            break;
        // 第二次输入的有问题
        case Com_ERROR:
            sayRetry();
            break;
        case Com_TIMEOUT:
            break;
        default:
            break;
        }
    }
    else
    {
        sayWithoutInt();
        sayIllegalOperation();
        App_Main_resetBuffers();
        return Com_NO_DATA;
        // 请添加管理员
    }
    App_Main_resetBuffers();
    return Com_OTHER;
}

// 获取按键信息
Com_Status App_Main_Get_Key_Info(char key_info[])
{
    uint8_t time_out = 0;
    uint8_t index = 0;
    while (1)
    {

        Touch_Key key = Int_SC12B_GetKey();
        if (key == key_no)
        {
            time_out++;
            if (time_out >= 100)
            {
                // 超时清除key_info[]
                memset(key_info, 0, 20);
                return Com_TIMEOUT;
            }
        }
        else
        {
            time_out = 0;
            sayWaterDrop();
            Int_WS2812_LED_OFF();
            Int_WS2812_LED_ON(key, red);
            // 处理删除确认状态
            if (delete_confirmation_pending && key == key_sharp)
            {
                delete_confirmation_pending = false;
                printf("delete succ\r\n");
                esp_err_t err = Dri_NVS_Del_User(ADMIN);
                if (err == ESP_OK)
                {
                    sayWithoutInt();
                    sayDelSucc();
                }
                else
                {
                    sayWithoutInt();
                    sayDelFail();
                }
                return Com_TIMEOUT; // 返回一个适当的值表示已处理特殊操作
            }
            else if (delete_confirmation_pending && key != key_no)
            {
                // 用户按下了除#以外的其他键，取消删除操作
                delete_confirmation_pending = false;
                sayWithoutInt();
                sayDelFail();
                return Com_TIMEOUT;
            }
            if (key == key_sharp)
            {
                // key_info[index] = '\0';
                return Com_OK;
            }
            if (key == key_m)
            {
                return Com_ERROR;
            }
            key_info[index++] = key + '0';
        }
        vTaskDelay(50);
    }
}

// 添加管理员
static void App_Main_AddAdmin(void)
{
    sayWithoutInt();
    sayAddAdmin();
    // 先判断管理员账号是否存在
    if (Dri_NVS_Has_Key(ADMIN) == ESP_OK)
    {
        sayWithoutInt();
        sayAdminFull();
    }
    else
    {
        sayWithoutInt();
        sayInputAdminPassword();
        Com_Status status = App_Main_Get_Key_Info(first_buf);
        switch (status)
        {
        case Com_ERROR:
            sayIllegalOperation();
            break;
        case Com_OK:
            sayInputAdminPasswordAgain();
            status = App_Main_Get_Key_Info(second_buf);
            switch (status)
            {
            case Com_ERROR:
                sayIllegalOperation();
                break;
            case Com_OK:
                // 在App_Main中实现
                if (strcmp(first_buf, second_buf) == 0)
                {
                    esp_err_t err = Dri_NVS_Write_Str(ADMIN, first_buf);
                    if (err == ESP_OK)
                    {
                        sayAddSucc();
                    }
                    else
                    {
                        sayAddFail();
                    }
                }
                else
                {
                    sayRetry();
                }

                break;
            case Com_TIMEOUT:
                break;
            default:
                break;
            }
            break;
        case Com_TIMEOUT:
            break;
        default:
            break;
        }
    }
    App_Main_resetBuffers();
}

// 删除管理员
static void App_Main_DelAdmin(void)
{
    sayWithoutInt();
    sayDelAdmin();
    // 判断管理员账号是否存在
    Com_Status status = App_Main_Verify_Password();
    if (status == Com_OK)
    {
        sayWithoutInt();
        sayVerifySucc();

        // 是否确认删除
        // 请按#键
        sayWithoutInt();
        sayPressSharp();
        // 确认删除
        sayWithoutInt();
        sayDelConfim();
        // 设置删除确认标志，等待在主按键处理循环中处理
        delete_confirmation_pending = true;

        // 轮询检测
        //  按下#确认删除
        //  Touch_Key key;
        //  uint8_t timeout = 0;
        //  while (1)
        //  {
        //      key = Int_SC12B_GetKey();
        //      if (key == key_sharp)
        //      {
        //          printf("delete succ\r\n");
        //          esp_err_t err = Dri_NVS_Del_User(ADMIN);
        //          if (err == ESP_OK)
        //          {
        //              sayWithoutInt();
        //              sayDelSucc();
        //          }
        //          else
        //          {
        //              sayWithoutInt();
        //              sayDelFail();
        //          }
        //          break;
        //      }
        //      else if (key != key_no)
        //      {
        //          // 检测到其他按键，取消删除操作
        //          sayWithoutInt();
        //          sayDelFail();
        //          break;
        //      }
        //      else
        //      {
        //          // 没有按键，增加超时计数
        //          timeout++;
        //          if (timeout > 100)
        //          { // 超时时间约5秒(100*50ms)
        //              sayWithoutInt();
        //              //sayDelTimeout();
        //              break;
        //          }
        //          vTaskDelay(50);
        //      }
        //  }
    }
    else
    {
        sayWithoutInt();
        sayVerifyFail();
        // 不存在管理员
    }
    // 使用我们封装起来的函数后就不用清除缓冲区了
    // App_Main_resetBuffers();
}
// 添加用户
static void App_Main_AddUser(void)
{
    sayWithoutInt();
    sayAddUser();
    // 1.验证管理员
    Com_Status status = App_Main_Verify_Password();
    if (status == Com_OTHER)
    {
        // 没有管理员
        sayIllegalOperation();
    }
    if (status == Com_OK)
    {
        // 第一次输入要添加的用户密码
        sayWithoutInt();
        sayInputUserPassword();
        status = App_Main_Get_Key_Info(first_buf);
        switch (status)
        {
        // 第一次输入的密码全是数字
        case Com_OK:
            //  密码一致后，检测flash中是否有此用户
            if (Dri_NVS_Has_Key(first_buf) == ESP_OK)
            {
                // 存在，不能添加
                sayWithoutInt();
                sayAddFail();
            }
            else
            {
                sayWithoutInt();
                sayInputUserPasswordAgain();
                status = App_Main_Get_Key_Info(second_buf);
                switch (status)
                {
                case Com_OK:
                    // 两次的密码一致
                    if (strcmp(first_buf, second_buf) == 0)
                    {
                        Dri_NVS_Write_U8(first_buf, 0);
                        sayWithoutInt();
                        sayAddSucc();
                    }
                    else
                    {
                        // 密码不一致
                        sayWithoutInt();
                        sayPasswordAddFail();
                    }
                    break;
                case Com_ERROR:
                    // 第二次密码输入有特殊符号
                    sayWithoutInt();
                    sayIllegalOperation();
                    break;
                case Com_TIMEOUT:
                    break;
                default:
                    break;
                }
                break;
                // 不在，可以添加
            }
        // 第一次输入的密码有特殊符号
        case Com_ERROR:
            sayIllegalOperation();
            break;
        case Com_TIMEOUT:
            break;
        default:
            break;
        }
    }
    else
    {
        sayWithoutInt();
        sayVerifyFail();
    }
    App_Main_resetBuffers();
}

// 删除用户
static void App_Main_DelUser(void)
{
    sayWithoutInt();
    sayDelUser();
    Com_Status status = App_Main_Verify_Password();
    if (status == Com_OK)
    {
        // 有管理员了
        // 输入要删除的用户,检测flash中是否存在此用户，存在就删除，不存在就返回
        sayWithoutInt();
        sayInputUserPassword();
        status = App_Main_Get_Key_Info(first_buf);
        switch (status)
        {
        case Com_OK:
            // 输入成功，都是数字
            // 查看flash中是否有此用户
            esp_err_t err = Dri_NVS_Has_Key(first_buf);
            if (err == ESP_OK)
            {
                // 存在此用户
                // 确认删除
                sayWithoutInt();
                sayPressSharp();
                sayWithoutInt();
                sayDelConfim();
                Dri_NVS_Del_User(first_buf);
                delete_confirmation_pending = true;
            }
            else
            {
                sayWithoutInt();
                sayDelFail();
            }

            break;
        case Com_ERROR:
            sayWithoutInt();
            sayIllegalOperation();
            break;
        case Com_TIMEOUT:
            /* code */
            break;

        default:
            break;
        }
    }
    else
    {
        sayWithoutInt();
        sayIllegalOperation();
        // 无管理员
    }
    App_Main_resetBuffers();
}

// 开门
static void App_Main_OpenDoor(char *buf)
{

    // 获取密码成功
    // 验证密码
    //! 这里要从App_Main_handler获取buf不然会没有数据
    if (Dri_NVS_Has_Key(buf) == ESP_OK)
    {
        // 存在,添加音效，开门
        sayWithoutInt();
        sayDoorOpen();
        // 开启电机
        Int_BDR6120_OpenLock();
        sayWithoutInt();
        sayDoorClose();
    }
    else
    {
        sayAlarm();
    }
}

// 处理用户输入
void App_Main_handler(char *buf)
{
    if (strlen(buf) < 2)
    {
        sayIllegalOperation();
    }
    else if (strlen(buf) == 2)
    { // 添加管理员
        if (buf[0] == '0' && buf[1] == '0')
        {
            App_Main_AddAdmin();
        } // 删除管理员
        else if (buf[0] == '0' && buf[1] == '1')
        {
            App_Main_DelAdmin();
        } // 添加用户
        else if (buf[0] == '1' && buf[1] == '0')
        {

            App_Main_AddUser();
        } // 删除用户
        else if (buf[0] == '1' && buf[1] == '1')
        {
            App_Main_DelUser();
        } // 删除全部
        else if (buf[0] == '9' && buf[1] == '9')
        {
            esp_err_t err = Dri_NVS_Del_All_User();
            if (err == ESP_OK)
            {
                sayDelAllUser();
            }
            else
            {
                sayDelFail();
            }
        }
        else if (buf[0] == '0' && buf[1] == '2')
        {
            // TODO 通知指纹任务添加指纹
            xTaskNotify(finger_handler, 1, eSetValueWithOverwrite);
        }
        else if (buf[0] == '2' && buf[1] == '0')
        {
            // TODO 通知指纹任务删除指纹
            xTaskNotify(finger_handler, 2, eSetValueWithOverwrite);
        }
        else if (buf[0] == '8' && buf[1] == '8')
        {
            // TODO 删除所有指纹
            Com_Status status = Int_FPM383_Clear_DB();
            if (status == Com_OK)
            {
                sayDelSucc();
            }
            else
            {
                sayDelFail();
            }
        }
    }
    else
    {
        App_Main_OpenDoor(buf);
    }
}
// 指纹处理
void App_Main_Finger_handler()
{
    uint8_t value = 0;
    xTaskNotifyWait(UINT32_MAX, UINT32_MAX, &value, 0);
    if (value != 0)
    {
        gpio_intr_disable(Int_FPM383_IT);

        
        if (value == 1)
        {
            // 添加指纹
            sayWithoutInt();
            sayAddUserFingerprint();
            sayWithoutInt();
            sayPlaceFinger();

            vTaskDelay(2000);

            uint16_t id = Int_FPM383_Get_Min_ID();
            Com_Status status = Int_FPM383_AutoEnroll(id);
            esp_rom_printf("add fingerprint status:%d\r\n", status);
            esp_rom_printf("the min id is:%d\r\n", Int_FPM383_Get_Min_ID());
            if (status == Com_OK)
            {
                sayWithoutInt();
                sayAddSucc();
            }
            else
            {
                sayWithoutInt();
                sayAddFail();
            }
        }
        else if (value == 2)
        {
            // TODO 删除指纹
            sayWithoutInt();
            sayDelUserFingerprint();
            sayWithoutInt();
            sayPlaceFinger();
            int16_t del_id = Int_FPM383_Get_Del_ID();
            if (del_id != -1)
            {
                Com_Status status = Int_FPM383_Delete_Template(del_id);
                if (status == Com_OK)
                {
                    sayWithoutInt();
                    sayDelSucc();
                }
            }
            else
            {
                sayWithoutInt();
                sayDelFail();
            }
        }
        esp_restart();
    }
    else
    {
        // 验证指纹，成功了就开门

        if (touch_flag)
        {
            touch_flag = 0;
            Com_Status status = Int_FPM383_Verify_Fingerprint();
            if (status == Com_OK)
            {
                // 存在,添加音效，开门
                sayWithoutInt();
                sayDoorOpen();
                // 开启电机
                Int_BDR6120_OpenLock();
                sayWithoutInt();
                sayDoorClose();
            }
            else
            {
                sayWithoutInt();
                sayVerifyFail();
            }
            esp_restart();
        }
    }
}