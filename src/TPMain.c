#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPSerial.h>
#include <TPFota.h>
#include <TPSMS.h>
#include <TPFonts.h>
#include <TPKeyPad.h>
#include <TPMobileCard.h>

// #ME

#include "TPExtPic.h"
#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "TP_SimCard.h"

#include "M_Menu.h"
#include "TP_StartPic.h"
#include "TP_TimeSet.h"


// The APP SoftwareVersion
const char SoftwareVersion[10] = {"V1.0\0"};


const char menuItems[][MAX_CHAR_ITEMS] = {
    "1.Kreye Fich",
    "2.Cheche Fich",
    "3.Jwenn Rapo",
    "4.Lo Ganyan",
    "5.Fich Ganyan",
    "6.Pwofil",
    "7.Konfigire Dat",
    "8.Teste Enprimant",
    "9.Dekonekte"
};

uint8 count = sizeof(menuItems) / MAX_CHAR_ITEMS;

int8 selected = 0;


// Fota Module Callback Function, on SIM800 Platform, Please Use New TP_Upgrade API
uint8 TP_FotaDownloadData(TP_UpgradeInfoT* upgradeInfo_p)
{
	return 0;
}


void TP_FotaProgressDisplay(TP_UpgradeStepE step, uint32 currValue, uint32 maxValue)
{

}


uint8 TP_FotaSMSDownloadData(TP_SMSUpgradeInfoT* upgradeInfo_p)
{
	return 0;
}


void TP_FotaUpgradeResultPrompt(TP_UpgradeResultE value)
{

}


// Real Event Callback Function for Incoming Serial Data
void TP_OnDefaultSerialData(const uint8* data_p, uint16 dataLen)
{
    char imeiBuf[20];
    char sendBuf[60];
    int32 cmdLen;

    cmdLen = strlen("AT+SIMEI?");

    if (dataLen >= cmdLen)
    {
        if ((memcmp(data_p, "at+simei?", cmdLen) == 0) ||
            (memcmp(data_p, "AT+SIMEI?", cmdLen) == 0))
        {
            memset(imeiBuf, 0x00, sizeof(imeiBuf));
            if (TP_GetModalIMEI(imeiBuf, sizeof(imeiBuf)) > 0)
            {
                sprintf(sendBuf,
                        "at+simei?\r\r\n%s\r\n\r\nOK\r\n",
                        imeiBuf);

                TP_DefaultPortSends(sendBuf, strlen(sendBuf));
            }

            return;
        }
    }
}


void TP_GetProductInfo(char* pInfoBuf)
{
    strcpy(pInfoBuf, TPDEMO_VERSION);
}


// Real Event Callback Function for Incoming Phone Call
void TP_OnIncomingCallNotify(void)
{

}


// Real Event Callback Function for Incoming SMS
void TP_OnNewSMSNotify(uint8 *Phone,  uint8 *Sms,  uint16 SmsLength, TP_DateTime *DateTime)
{

}


static void MgnAPP(void) {
    Boolean isLoggedIn = FALSE;
    Boolean running = TRUE;
    uint8 result = 0;

    TP_DbgSerialPrn("\r\nBuild Time:%s %s\r\n",
        __DATE__,
        __TIME__
    );

    TP_DbgSerialPrn("SIM Slot: %d\n", TP_MobileCardGetActiveCardSlot());

    TP_SetCurrentCharSet(CHARSET_ASCII);
    TP_kbLight(2);
    TP_Kbmute(1);
    TP_ScrGray(27);

    DisplayPicture();
    TP_PrnInit();

    ShowSimCardStartUPProcess();


    while (TP_TimerCheck(1) != 0);

    TP_SetVolume(SPEECH_VOLUMN, VOLUME_7);
    TP_SetVolume(MICROPHONE_VOLUMN, VOLUME_7);
    TP_BanIncomingCall(TRUE);

    running = TRUE;

    while(running) {
        // isLoggedIn = postHandleLogin();
        // if (isLoggedIn == TRUE)
        //     selected = 0;
        // else
        //     selected = -1;
        
        while (selected >= 0) {
            selected = LCD_Menu("MGNLotto", menuItems, count, selected);
            if (selected < 0) selected = 30;
            switch (selected) {
                case 0:
                    postFiches(NULL);
                    break;
                case 1:
                    getFiches();
                    break;
                case 2:
                    getRapports();
                    break;
                case 3:
                    getTirages();
                    break;
                case 4:
                    getWinningFiches();
                    break;
                case 5:
                    getConfiguration();
                    break;
                case 6:
                    TP_SetDate();
                    break;
                case 7:
                    test_printer();
                    break;
                case 8:
                    selected = -1;
                    logout();
                    break;
                default:
                    break;
            }
        }
    }

    
};


void TPMain(void)
{
    TP_DbgSerialPrnLevel(4);
    TP_SetDefaultPortRate("115200");
    TP_FactoryMode_Register_GetSoftwareVersion_Function(TP_GetProductInfo);
    TP_SetBatteryLowWarningPic(MyBatteryLowWarningPic);
    TP_SetPowerOffTipPic(MyPowerOffTipPic);

    TP_SystemInit();

    TP_SetCurrentCharSet(CHARSET_ASCII);

    TP_kbLight(1);
    TP_ScrBackLight(1);

	TP_PlayRing(0);

	// Application
	
    MgnAPP();
}


