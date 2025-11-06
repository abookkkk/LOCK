#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

//枚举状态,在其他文件中会使用
typedef enum
{
    Com_OK,//0
    Com_ERROR,
    Com_TIMEOUT,
    Com_OTHER,
    Com_NO_DATA
}Com_Status;

#endif /* __COM_CONFIG_H__ */
