#ifndef __INT_WTN6170_H__
#define __INT_WTN6170_H__

#include "driver/gpio.h"
#include "esp_task.h"
#include "sys/unistd.h"

#define WTN6170_DATA_PIN GPIO_NUM_9
#define sayWithoutInt() Int_WTN6170_SendCMD(0xf3)
#define sayNum(x) Int_WTN6170_SendCMD(x + 1)
#define sayWaterDrop() Int_WTN6170_SendCMD(11)
#define sayBuzzer() Int_WTN6170_SendCMD(12)
#define sayAlarm() Int_WTN6170_SendCMD(13)
#define sayDoorBell() Int_WTN6170_SendCMD(14)
#define sayFail() Int_WTN6170_SendCMD(16)
#define sayPassword() Int_WTN6170_SendCMD(19)
#define sayDoorOpen() Int_WTN6170_SendCMD(25)
#define sayDoorClose() Int_WTN6170_SendCMD(26)
#define sayIllegalOperation() Int_WTN6170_SendCMD(28)
#define sayVerify() Int_WTN6170_SendCMD(31)
#define sayFinish() Int_WTN6170_SendCMD(33)
#define sayPressSharp() Int_WTN6170_SendCMD(34)
#define sayDelSucc() Int_WTN6170_SendCMD(36)
#define sayDelFail() Int_WTN6170_SendCMD(37)
#define sayDelConfim() Int_WTN6170_SendCMD(38)
#define sayDelAllUser() Int_WTN6170_SendCMD(39)
#define sayVerifySucc() Int_WTN6170_SendCMD(41)
#define sayVerifyFail() Int_WTN6170_SendCMD(42)
#define saySetSucc() Int_WTN6170_SendCMD(43)
#define saySetFail() Int_WTN6170_SendCMD(44)
#define sayOperateSucc() Int_WTN6170_SendCMD(46)
#define sayOperateFail() Int_WTN6170_SendCMD(47)
#define sayInvalid() Int_WTN6170_SendCMD(48)
#define sayAddSucc() Int_WTN6170_SendCMD(49)
#define sayAddFail() Int_WTN6170_SendCMD(50)
#define sayAddUser() Int_WTN6170_SendCMD(51)
#define sayDelUser() Int_WTN6170_SendCMD(52)
#define sayUserFull() Int_WTN6170_SendCMD(53)
#define sayAddAdmin() Int_WTN6170_SendCMD(54)
#define sayDelAdmin() Int_WTN6170_SendCMD(55)
#define sayAdminFull() Int_WTN6170_SendCMD(56)
#define sayRetry() Int_WTN6170_SendCMD(58)
#define sayInputUserPassword() Int_WTN6170_SendCMD(64)
#define sayInputUserPasswordAgain() Int_WTN6170_SendCMD(65)
#define sayPasswordAddSucc() Int_WTN6170_SendCMD(66)
#define sayPasswordAddFail() Int_WTN6170_SendCMD(67)
#define sayPasswordVerifySucc() Int_WTN6170_SendCMD(68)
#define sayPasswordVerifyFail() Int_WTN6170_SendCMD(69)
#define sayInputAdminPassword() Int_WTN6170_SendCMD(70)
#define sayInputAdminPasswordAgain() Int_WTN6170_SendCMD(71)
#define sayAddUserFingerprint() Int_WTN6170_SendCMD(76)
#define sayDelUserFingerprint() Int_WTN6170_SendCMD(77)
#define sayPlaceFinger() Int_WTN6170_SendCMD(80)
#define sayPlaceFingerAgain() Int_WTN6170_SendCMD(81)
#define sayTakeAwayFinger() Int_WTN6170_SendCMD(82)
#define sayFingerprintAddSucc() Int_WTN6170_SendCMD(83)
#define sayFingerprintAddFail() Int_WTN6170_SendCMD(84)
#define sayFingerprintVerifySucc() Int_WTN6170_SendCMD(85)
#define sayFingerprintVerifyFail() Int_WTN6170_SendCMD(86)

#define WTN6170_PIN_H (gpio_set_level(WTN6170_DATA_PIN,1))
#define WTN6170_PIN_L (gpio_set_level(WTN6170_DATA_PIN,0))

void Int_WTN6170_Init(void);

void Int_WTN6170_SendCMD(uint8_t cmd);

#endif /* __INT_WTN6170_H__ */
