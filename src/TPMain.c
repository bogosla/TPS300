#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPSerial.h>
#include <TPFota.h>
#include <TPSMS.h>
#include <TPFonts.h>
#include <TPKeyPad.h>

// The APP SoftwareVersion
const char SoftwareVersion[10] = {"V1.0\0"};

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

}

// Real Event Callback Function for Incoming Phone Call
void TP_OnIncomingCallNotify(void)
{

}
// Real Event Callback Function for Incoming SMS
void TP_OnNewSMSNotify(uint8 *Phone,  uint8 *Sms,  uint16 SmsLength, TP_DateTime *DateTime)
{

}

void TPMain(void)
{
	uint8 op_name[16];
	/*Setup serial debug output, Optional but recommended*/
    TP_DbgSerialPrnLevel(0);
    TP_SetDefaultPortRate("115200");

	/*Call before doing any other stuff*/
    TP_SystemInit();

    /*Setup screen font before display*/
    TP_SetCurrentCharSet(CHARSET_WEST);
	
	/*Optional, Setup Backlight Mode*/
    TP_kbLight(1);
    TP_ScrBackLight(1);

	/*TODO Application Init Stuff*/

	/*Minimum Application, We display HelloWorld here. Yes, HelloWorld Again :p*/
	memset(op_name, 0x00, sizeof(op_name));
	TP_ScrCls();
	TP_ScrGotoxyEx(4, 0);
	TP_LcdPrintf("Hello world!");

	TP_ScrGotoxyEx(4, 15);
	TP_LcdPrintf("Card status: %d", TP_GetMobileCardStatus());
	
	TP_ScrGotoxyEx(4, 30);
	TP_LcdPrintf("Active card: %d", TP_MobileCardGetActiveCardSlot());

	TP_GetMobileCardOperatorName(op_name, sizeof(op_name));
	TP_ScrGotoxyEx(4, 48);
	TP_LcdPrintf("Operator name: %s", op_name);
	
	/*If TPMain ends, terminal will reboot, we make a infinity loops here*/
	while (TRUE)
	{
		int i = 0;
		int x = 0;
		/*Must Do something here, otherwise device will treate it as an dead loop error*/
		TP_TimerSet(1, 1000);
		for (i = 0; i < 100; i++)
		{
			x++;
		}
		while (TP_TimerCheck(1) != 0);
	}
}


