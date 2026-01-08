#include "Int_FPM383.h"
//!指纹模块
#define Int_FPM383_RX GPIO_NUM_20
#define Int_FPM383_TX GPIO_NUM_21
#define Int_FPM383_IT GPIO_NUM_10
static const int RX_BUF_SIZE = 1024;

uint8_t touch_flag = 0;
uint8_t recv_buf[100] = {1}; // 接收数据缓冲区
/**
 * @brief 发送指令
 *
 * @param cmd 要发送的指令
 * @param length 指令数组长度
 */
static void Int_FPM383_SendData(uint8_t *cmd, uint8_t length)
{
    uart_write_bytes(UART_NUM_1, cmd, length);
}
// 接收数据
static void Int_FPM383_RecvData(uint8_t length, TickType_t timeout)
{
    memset(recv_buf, 1, sizeof(recv_buf)); // 接收前清零
    uart_read_bytes(UART_NUM_1, recv_buf, length, timeout);
}

// 计算校验和
void Int_FPM383_ChickSum(uint8_t *cmd, uint8_t length)
{
    uint8_t sum = 0;
    for (uint8_t i = 6; i < length - 2; i++)
    {
        sum += cmd[i];
    }
    // TODO bug,当时写的<<
    cmd[length - 2] = (sum >> 8);
    cmd[length - 1] = sum;
}

// 获取最小的id
uint16_t Int_FPM383_Get_Min_ID(void)
{
    uint8_t cmd[] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x04,             // 包长度
        0x1f,                   // 指令码
        0x00,                   // 页码
        '\0', '\0'              // 校验和
    };
    Int_FPM383_ChickSum(cmd, sizeof(cmd));
    // 发送数据
    Int_FPM383_SendData(cmd, sizeof(cmd));
    // 接收数据
    Int_FPM383_RecvData(44, 2000);
    if (recv_buf[9] == 0x00)
    {
        for (uint8_t i = 0; i < 32; i++)
        {
            uint8_t byte = recv_buf[i + 10];
            // 遍历每一个字节的每一位
            for (uint8_t j = 0; j < 8; j++)
            {
                if (byte & 0x01)
                {
                    byte >>= 1;
                }
                else
                {
                    return 8 * i + j;
                }
            }
        }
        // for (uint8_t i = 0; i < 32; i++)
        // {
        //     for (uint8_t j = 0; j < 8; j++)
        //     {
        //         if (((recv_buf[10 + i] >> (j + 1)) & 0x01) != 1)
        //         {
        //             // 找到空位了，插入
        //             return 8 * i + j;
        //         }
        //     }
        // }
    }
    return 0;
}

// 取消指令(没问题了)
static void Int_FPM383_Cancel_Command()
{
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x03,             // 包长度
        0x30,                   // 指令码
        '\0', '\0'              // 校验和
    };
    Int_FPM383_ChickSum(cmd, sizeof(cmd));

    // TODO 当时写的是cmd 而不是recv_buf
    while (recv_buf[9] != 0x00)
    {
        Int_FPM383_SendData(cmd, sizeof(cmd));

        Int_FPM383_RecvData(12, 1000);
    };
    esp_rom_printf("cancel success\r\n");
}

// 中断处理函数，当指纹被触摸时，执行这里
static void Int_FPM383_ISR(void *arg)
{
    esp_rom_printf("touch key!!\r\n");
    touch_flag = 1;
}

// 初始化FPM383
void Int_FPM383_Init(void)
{
    //! 初始化uart
    uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE, // 无奇偶校验
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT};
    // 安装驱动
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    // 配置uart
    uart_param_config(UART_NUM_1, &uart_config);
    // 设置引脚
    uart_set_pin(UART_NUM_1, Int_FPM383_TX, Int_FPM383_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //! 初始化中断引脚
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1 << Int_FPM383_IT),
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE};
    // 配置中断
    gpio_config(&io_conf);
    // 配置优先级
    gpio_install_isr_service(0);
    // 注册中断
    gpio_isr_handler_add(Int_FPM383_IT, Int_FPM383_ISR, (void *)Int_FPM383_IT);

    // 关闭中断使能，在指模块休眠时再打开
    gpio_intr_disable(Int_FPM383_IT);

    // 指纹模块睡眠
    Int_FPM383_Sleep();
}

// 睡眠
void Int_FPM383_Sleep(void)
{
    // 睡眠的命令
    uint8_t cmd[] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x03,             // 包长度
        0x33,                   // 指令码
        0x00, 0x37              // 校验和
    };

    do
    {
        Int_FPM383_SendData(cmd, sizeof(cmd));
        Int_FPM383_RecvData(12, 2000);

    } while (recv_buf[9] != 0x00);

    // 打开中断使能
    gpio_intr_enable(Int_FPM383_IT);
    esp_rom_printf("sleep success\r\n");
}

// 测试
void Int_FPM383_Test(void)
{
    // Com_Status status = Int_FPM383_AutoEnroll(id);
    // if (status == Com_OK)
    //{
    //     uint16_t min_id = Int_FPM383_Get_Min_ID();
    //     esp_rom_printf("the min id is:%d\r\n", min_id);
    // }
    // else
    //{
    //     esp_rom_printf("auto enroll failed\r\n");
    // }
}

// 自动注册指纹
Com_Status Int_FPM383_AutoEnroll(uint16_t id)
{
    uint8_t cmd[] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x08,             // 包长度
        0x31,                   // 指令码
        '\0', '\0',             // id号
        0x01,                   // 录入次数
        0x00, 0x33,             // 参数
        '\0', '\0'              // 校验和
    };
    cmd[10] = (id >> 8);
    cmd[11] = id;
    Int_FPM383_ChickSum(cmd, sizeof(cmd));

    // 取消指令（4次）
    Int_FPM383_Cancel_Command();
    Int_FPM383_Cancel_Command();
    Int_FPM383_Cancel_Command();
    Int_FPM383_Cancel_Command();

    Int_FPM383_SendData(cmd, sizeof(cmd));

    do
    {
        Int_FPM383_RecvData(14, 2000);
        if (recv_buf[9] != 0x00)
        {
            return Com_ERROR;
        }

    } while (recv_buf[10] != 0x06);

    // return Com_OK;
    //  最终判断结果
    if (recv_buf[9] == 0x00)
    {
        return Com_OK;
    }
    else
    {
        return Com_ERROR;
    }
}

// TODO 清空指纹库
Com_Status Int_FPM383_Clear_DB(void)
{
    uint8_t cmd[] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x03,             // 包长度
        0x0d,                   // 指令码
        0x00, 0x11              // 校验和
    };
    Int_FPM383_SendData(cmd, sizeof(cmd));
    Int_FPM383_RecvData(12, 1000);
    if (recv_buf[9] == 0x00)
    {
        return Com_OK;
    }
    else
    {
        return Com_ERROR;
    }
    return Com_OTHER;
}

// TODO 自动验证指纹
Com_Status Int_FPM383_Verify_Fingerprint(void)
{
    uint8_t cmd[17] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x08,             // 包长度
        0x32,                   // 指令码
        0x03,                   // 分数等级,就是安全等级
        0xff, 0xff,             // ID号
        0x00, 0x07,             // 参数
        '\0', '\0'              // 校验和
    };
    Int_FPM383_ChickSum(cmd, sizeof(cmd));
    Int_FPM383_SendData(cmd, sizeof(cmd));
    //?为什么随便一个指纹就能开锁,原因：没写下面的recv
    //Int_FPM383_RecvData(17, 2000);
    //esp_rom_printf("recv buf:%d\r\n",recv_buf[0]);

    // if (recv_buf[9] == 0x00)
    // {
    //     return Com_OK;
    // }
    // else
    // {
    //     return Com_ERROR;
    // }

    //do
    //{
    //    //TODO 解决这里的问题,接收不到数据
    //    Int_FPM383_RecvData(17, 2000);
    //    esp_rom_printf("recv buf:%d,%d\r\n",recv_buf[0],recv_buf[1]);
    //    if (recv_buf[9] == 0x00)
    //    {
    //        return Com_OK;
    //    }
//
    //} while (recv_buf[10] != 0x05);
//
    //return Com_ERROR;
        // 添加重试机制和更清晰的错误处理
    int retry_count = 3;
    while (retry_count > 0) {
        Int_FPM383_RecvData(17, 2000);
        
        // 打印接收到的数据以便调试
        esp_rom_printf("recv buf:");
        for(int i = 0; i < 17; i++) {
            esp_rom_printf("%02X ", recv_buf[i]);
        }
        esp_rom_printf("\r\n");
        
        // 检查数据包头是否正确
        if (recv_buf[0] == 0xEF && recv_buf[1] == 0x01) {
            if (recv_buf[9] == 0x00) {
                return Com_OK;
            } else if (recv_buf[9] == 0x01) {
                // 接收失败，重试
                retry_count--;
                vTaskDelay(100 / portTICK_PERIOD_MS);
                continue;
            } else {
                return Com_ERROR;
            }
        } else {
            // 数据包头错误，可能是数据未完整接收
            retry_count--;
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    
    return Com_ERROR;
}

// TODO 获取用户要删除指纹的id
int16_t Int_FPM383_Get_Del_ID(void)
{
    uint8_t cmd[] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x08,             // 包长度
        0x32,                   // 指令码
        0x04,                   // 分数等级
        0xff, 0xff,             // ID号
        0x00, 0x07,             // 参数
        '\0', '\0'              // 校验和
    };
    Int_FPM383_ChickSum(cmd, sizeof(cmd));
    Int_FPM383_SendData(cmd, sizeof(cmd));

    do
    {
        Int_FPM383_RecvData(17, 2000);
        if (recv_buf[9] != 0x00)
        {
            return -1;
        }

    } while (recv_buf[10] != 0x05);
    return recv_buf[12];
}

// TODO 删除模版
Com_Status Int_FPM383_Delete_Template(uint16_t page_num)
{
    uint8_t cmd[] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 设备地址
        0x01,                   // 包标记，就是命令标记
        0x00, 0x07,             // 包长度
        0x0c,                   // 指令码
        '\0', '\0',             // 从哪个id开始删除
        0x00, 0x01,             // 删除个数
        '\0', '\0'              // 校验和
    };
    cmd[10] = (page_num >> 8);
    cmd[11] = page_num;

    Int_FPM383_ChickSum(cmd, sizeof(cmd));

    Int_FPM383_SendData(cmd, sizeof(cmd));

    Int_FPM383_RecvData(12, 1000);
    if (recv_buf[9] == 0x00)
    {
        return Com_OK;
    }
    else
    {
        return Com_ERROR;
    }
    return Com_OTHER;
}
