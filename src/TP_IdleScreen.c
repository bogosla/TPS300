#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeypad.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include <TPSMS.h>
#include <TP_Singleime.h>
#include <TPSerial.h>
#include <TPPhone.h>
#include <TPMobileCard.h>

#include "TP_DemoDefine.h"

#include "TP_IdleScreen.h"
#include "TP_ListBox.h"
#include "TP_DisplayAPI.h"

// #ifdef INCLUDE_PARAM_MANAGER
// #include "TP_ParamsManager.h"
// #endif
// #ifdef INCLUDE_PHONE_SETTING
// #include "TP_PhoneSetting.h"
// #include "TP_CallRecord.h"
// #endif

// #include "TP_Secretcode.h"

// #include "TP_SmsStore.h"
// #include "TP_MsgMenu.h"

// #include "TP_Phone.h"



TP_DisplayInfo displayInfo=
                            {
                                0,
                                ALIGN_LEFT,
                                (uint8*)"Confirm",
                                (uint8*)"Cancel",
                                KEY_DEMO_CONFIRM,
                                KEY_DEMO_CANCEL,
                                TP_KEY_OK,
                                TP_KEY_NONE,
                                CFONT
                            };
TP_InputInfo inputInfo =
                            {
                                INPUTMODE_UPERCASE,
                                0xF4,
                                1,
                                3,
                                0,
                                TRUE,
                                TRUE
                            };


//ST_FONT  GoblaFont[32];
static uint8 currlevel= 0xFF;



uint16 TP_HexToStr(uint8* Src,uint16 Size,uint8* des)
{
    uint16 Index,Pos;
    Pos =0;
    for (Index=0;Index < Size;Index++)
    {
            if (((Src[Index] >> 4)>=0) && ((Src[Index] >> 4)<=9))
            {
                des[Pos++] = (Src[Index] >> 4) + 0x30;
            }
            else
            {
                des[Pos++] = (Src[Index] >> 4) + 0x37;
            }

            if (((Src[Index] & 0x0F )>=0) && ((Src[Index] & 0x0F)<=9))
            {
                des[Pos++] = (Src[Index] & 0x0F) + 0x30;
            }
            else
            {
                des[Pos++] = (Src[Index] & 0x0F) + 0x37;
            }

    }
    return Pos;
}


uint16 TP_StrToHex(uint8* Src,uint16 Size,uint8* des)
{
    uint16 Index,Pos;
    uint8 Byte;
    if (Size %2 >0)
    {
        return 0;
    }
    Pos =0;
    for (Index=0;Index < Size;Index++)
    {
        Byte =0;
        if ((Src[Index] >= 0x30) && (Src[Index] <= 0x39))
        {
            Byte= Src[Index] - 0x30;

        }
        else if((Src[Index] >= 0x41) && (Src[Index] <= 0x46))
        {
            Byte= Src[Index] - 0x37;
        }
        else if((Src[Index] >= 0x61) && (Src[Index] <= 0x66))
        {
            Byte= Src[Index] - 0x50;
        }

        Byte <<= 4;

        Index ++;

        if ((Src[Index] >= 0x30) && (Src[Index] <= 0x39))
        {
            Byte |= (Src[Index] - 0x30);

        }
        else if((Src[Index] >= 0x41) && (Src[Index] <= 0x46))
        {
            Byte |= (Src[Index] - 0x37);
        }
        else if((Src[Index] >= 0x61) && (Src[Index] <= 0x66))
        {
            Byte |= (Src[Index] - 0x50);
        }

        *des = Byte;
        des++;


    }
    return Size/2;
}


static void TP_SetDefaultParam()
{
    TP_SetVolume(KEYPAD_VOLUMN, VOLUME_3);
    TP_SetVolume(RING_VOLUMN, VOLUME_3);
    TP_SetVolume(SPEECH_VOLUMN, VOLUME_3);
    TP_SetVolume(MICROPHONE_VOLUMN, VOLUME_7);
    TP_ScrBackLight(1);
    TP_kbLight(1);
}


static void ConvertKeyCodeToStr(uint8 keyCode,uint8* keyCodeStr)
{
    switch (keyCode)
    {
    case TP_KEY_0:
        strcpy((char*)keyCodeStr,"0");
        break;
    case TP_KEY_1:
        strcpy((char*)keyCodeStr,"1");
        break;
    case TP_KEY_2:
        strcpy((char*)keyCodeStr,"2");
        break;
    case TP_KEY_3:
        strcpy((char*)keyCodeStr,"3");
        break;
    case TP_KEY_4:
        strcpy((char*)keyCodeStr,"4");
        break;
    case TP_KEY_5:
        strcpy((char*)keyCodeStr,"5");
        break;
    case TP_KEY_6:
        strcpy((char*)keyCodeStr,"6");
        break;
    case TP_KEY_7:
        strcpy((char*)keyCodeStr,"7");
        break;
    case TP_KEY_8:
        strcpy((char*)keyCodeStr,"8");
        break;
    case TP_KEY_9:
        strcpy((char*)keyCodeStr,"9");
        break;
    case TP_KEY_POUND:
        strcpy((char*)keyCodeStr,"#");
        break;
    case TP_KEY_STAR:
        strcpy((char*)keyCodeStr,"*");
        break;
    default:
        break;
    }
}


// void IdleKeyProcess(uint8 firstKeyCode)
// {
//     uint8 userKey ;
//     uint8 str[20] ={0};
//     uint8 cancellist[]={TP_KEY_ONHOOK};

//     #if 0
//     TP_DisplayInfo displayInfo =
//     {
//         "Please Input Phone:",
//         ALIGN_MIDDLE,
//         "Comfirm",
//         "Cancel",
//         TP_KEY_CONFIRM,
//         TP_KEY_CANCEL,
//         #if defined (TPS320)
//         TP_KEY_HANDFREE,
//         #else
//         TP_KEY_OK,
//         #endif
//         0,
//         CFONT
//     };
//     TP_InputInfo inputInfo =
//     {
//         0,0,1,sizeof(str)-1,0,FALSE,FALSE
//     };
//     TP_Kbflush();
//     TP_SetKey(firstKeyCode);
//     memset(str, 0, sizeof(str));
//     userKey = TP_NumberInput(&displayInfo,&inputInfo,str);
//     #endif
//     TP_Kbflush();
//     TP_ScrCls();

//     TP_ScrGotoxyEx(3,3);
//     TP_SetDisplayArea(3,3,126,15);
//     TP_LcdPrintf((uint8*)"Please Input Phone:");

//     TP_ScrDrawRect(3, 17, 125, 48);//Draw the input content's window
//     TP_ScrAttrSet(0);
//     TP_ScrGotoxyEx(3,49);
//     TP_ScrFontSet(CFONT);
//     TP_SetDisplayArea(1,48,126,63);
//     TP_LcdPrintf((uint8*)"Confirm       Cancel");

//     TP_ScrGotoxyEx(4,19);
//     TP_SetDisplayArea(4,19,124,47);
//     TP_SetInputModePosition(128, 64);
//     TP_ShowInputNum(0, 0, 0);


//     memset(str,0x00,sizeof(str));
//     //ConvertKeyCodeToStr(firstKeyCode,str);
//     TP_SetKey(firstKeyCode);

//     TP_SetInputModeControlEx(
//                              KEY_DEMO_CONFIRM,
//                              #if defined (TPS320)
//                              TP_KEY_HANDFREE,
//                              #else
//                              TP_KEY_OK,
//                              #endif
//                              KEY_DEMO_CANCEL,
//                              TP_KEY_CLEAR,
//                              TP_KEY_NONE);
//     TP_SetInputModeControlList(0, 0, cancellist, sizeof(cancellist));
//     while(1)
//     {
//         userKey = TP_GetStringTimedEx((char*)str,0xF4,1,(sizeof(str)-1),0,NULL);

//         if(userKey == 0xFF)
//         {
//            break;
//         }
//         else if (userKey == 0xFB ||
//                  userKey == 0xF9)
//         {
//             if (strlen((char*)str) > 0)
//             {
// #ifdef INCLUDE_SECRET_CODE
//                 if(TRUE == TP_SecretCodeParse((char*)str))
//                 {
//                     break;
//                 }
//                 if (strcmp((char*)str, "*66#0") == 0)
//                 {
//                     TP_DbgSerialPrnLevel(5);
//                     break;
//                 }
//                 else if (strcmp((char*)str, "*66#1") == 0)
//                 {
//                     TP_DbgSerialPrnLevel(0);
//                     break;
//                 }

// 				#ifdef TPS320
//                 TP_HookUp();
//                 TP_ShowDialDilog(str);
// 				#endif

//                 break;
// #else
//                 if (memcmp(str,"*#55#",5)== 0)
//                 {
//                     TP_SystemTest();
//                     return;
//                 }
//                 if (strcmp(str, "*66#0") == 0)
//                 {
//                     TP_DbgSerialPrnLevel(5);
//                 }
//                 else if (strcmp(str, "*66#1") == 0)
//                 {
//                     TP_DbgSerialPrnLevel(0);
//                 }
//                 else
//                 {
// 					#ifdef TPS320
// 					TP_HookUp();
// 					TP_ShowDialDilog(str);
// 					#endif
//                 }
// #endif

//             }
//         }
//     }
//     TP_ClearInputModeControlList();
//     TP_Kbflush();
// }



