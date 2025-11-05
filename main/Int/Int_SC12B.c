#include "Int_SC12B.h"
//!按键模块
uint8_t flag = 0;
static void Int_SC12B_Write_Byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    i2c_master_write_to_device(I2C_MASTER_NUM, SLAVE_ADDR, write_buf, sizeof(write_buf), 1000);
}

static uint8_t Int_SC12B_Read_Byte(uint8_t reg_addr)
{
    uint8_t data;
    i2c_master_write_read_device(I2C_MASTER_NUM, SLAVE_ADDR, &reg_addr, 1, &data, 1, 1000);
    return data;
}

static void gpio_isr_handler(void *arg)
{
    flag = 1;
}

void Int_SC12B_Init(void)
{
    //! 1.i2c初始化
    // int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SC_SDA_PIN,
        .scl_io_num = SC_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = SC_I2C_MASTER_FREQ_HZ};
    i2c_param_config(I2C_MASTER_NUM, &i2c_conf);

    i2c_driver_install(I2C_MASTER_NUM, i2c_conf.mode, 0, 0, 0);
    //! 2.设置灵敏度
    Int_SC12B_Write_Byte(0x00, 0x25);
    Int_SC12B_Write_Byte(0x01, 0x25);

    //! 配置引脚
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1 << SC_INT_PIN),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    gpio_config(&io_conf);

    // 安装中断服务，优先级
    gpio_install_isr_service(0);
    gpio_isr_handler_add(I2C_MASTER_NUM, gpio_isr_handler, (void *)I2C_MASTER_NUM);
}

Touch_Key Int_SC12B_GetKey(void)
{
    uint16_t key_num = key_no;
    // 通过i2c读取数据
    if (flag == 1)
    {
        uint16_t result = (Int_SC12B_Read_Byte(0x08) << 8 | Int_SC12B_Read_Byte(0x09));
        switch (result)
        {
        case 0x8000:
            key_num = key_0;
            break;
        case 0x4000:
            key_num = key_1;
            break;
        case 0x2000:
            key_num = key_2;
            break;
        case 0x1000:
            key_num = key_3;
            break;
        case 0x0800:
            key_num = key_7;
            break;
        case 0x0400:
            key_num = key_5;
            break;
        case 0x0200:
            key_num = key_6;
            break;
        case 0x0100:
            key_num = key_4;
            break;
        case 0x0080:
            key_num = key_m;
            break;
        case 0x0040:
            key_num = key_8;
            break;
        case 0x0020:
            key_num = key_9;
            break;
        case 0x0010:
            key_num = key_sharp;
            break;
        default:
            break;
        }

        flag = 0;
    }

    return key_num;
}
