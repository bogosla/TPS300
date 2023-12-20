#ifndef TP_PAGE_API_H
#define TP_PAGE_API_H

#include <TPFonts.h>
#include "TP_ListBox.h"
#include "cJSON.h"

#define MAX_LR_BUTTON_TITLE_LEN 32 //���Ұ���������󳤶�

#ifdef TPS300
#define DISPLAY_KEY_CONFIRM TP_KEY_MENU
#define DISPLAY_KEY_CANCEL  TP_KEY_CANCEL
#define DISPLAY_KEY_POUND   TP_KEY_FUNC
#else
#define DISPLAY_KEY_CONFIRM TP_KEY_CONFIRM
#define DISPLAY_KEY_CANCEL  TP_KEY_CANCEL
#define DISPLAY_KEY_POUND   TP_KEY_POUND
#endif


//��ʾ����
typedef struct
{
    uchar* strTitle;                 //�����⣬Ϊ������ʾ
    TP_DisplayAlign  align;         //������뷽ʽ��ֻ��������⣬������Ĭ�������
    uchar* strLButtonTitle;          //����˵�����
    uchar* strRButtonTitle;          //�Ҽ��˵�����
    uint8 LKeyValue;                //���������ֵ
    uint8 RKeyValue;                //�Ҳ�������ֵ
    uint8 LF_KeyValue;              //�������ݼ���ֵ
    uint8 RF_KeyValue;              //�Ҳ�����ݼ���ֵ
    uint8 FONT;
}TP_DisplayInfo;

//�������
typedef struct
{
    uint8 defaultMethod;            //Ĭ�����뷨
    uint16 inputMode;               //���뷨ģʽ�������������֡���������ģʽ
    uint16 minLen;
    uint16 maxLen;
    uint32 timeoutInMS;
    Boolean showInputNumber;        // �Ƿ���ʾ��ǰ�����������������
    Boolean showIMIcon;             // �Ƿ���ʾ���뷨ͼ��
}TP_InputInfo;


typedef struct
{
    uint8* caption;                 //�б�������
    uint32 value;                   //�б����ֵ
    uint8* icon;
    Boolean selected;               //�Ƿ�ѡ�� TRUE:��ѡ FALSE:δѡ
}TP_ListBoxInfo;

typedef enum
{
    ALERT_OK,
    ALERT_ERROR,
    ALERT_WARNING,
    ALERT_INFO,
    ALERT_QUESTION,
    ALERT_MAX
}TP_AlertType;

/**
* ֻ����ʾһ����ʾ�����Ӧ�κβ���
* @author ���Ķ�
* @version 1.0
* @param *msg ��ʾ���ַ���
* @return ��
* @since 2011��12��31��
*/
extern
void TP_PopupMessage(uchar *msg);

/**
* ��ʾһ�����壬��ʱ�����,��ʱ����ڰ�������˳�
* @author ���Ķ�
* @version 1.0
* @param *msg ��ʾ���ַ���
* @param Ms ��ʾʱ�䣬���뵥λ
* @return ��
* @since 2011��12��31��
*/
extern
void TP_PopupMessageTimed( uchar *msg,uint32 Ms );

/**
 * Alert a window with special type icon with timeout control
 * @author linhx
 * @version 1.0
 *
 * @param alertType Special type of icon define in {@link
 *                  TP_AlertType}
 * @param msg Message to display.
 * @param Ms Display time in milliseconds
 * @date 2012-10-12
 */
extern
void TP_PopupMessageWithIcon( TP_AlertType alertType,uchar *msg,uint32 Ms);

/*
�������ƣ�TP_PopupMesWithButton
��;��    ���ھ�����ʾ�������棬����Ӧ���Ұ�������
���ߣ�    guangx
����ֵ˵����
    TRUE���������
    FALSE���Ҽ����ػ���󷵻�
���������
    displayInfo: ���Ұ�����Ϣ�ṹ��
*/
extern
Boolean TP_PopupMessageWithButton( TP_DisplayInfo *displayInfo );



void TP_ShowTextSingleLine( uint8 left, uint8 top, uint8 right, uint8 bottom,
                            uint8 FONT,TP_DisplayAlign  align,uchar* fmt,...);

//��ʾ�ı�ʱ��ע�ᰴ���ص�����
typedef void (*ShowTextKeyFunc)(uint8 keycode,uint8 *DisplayData);

/**
* TP_ShowText()�а����¼�������������
* @author ���Ķ�
* @version 1.0
* @param func_p ������������ָ��<BR>
* @since 2012��5��18��
* @note
*/
extern
void TP_ShowTextKeyFuncSet(ShowTextKeyFunc func_p);

/**
 * ��ָ�����б�������Rect��ʾ�ı�������ʾ���⣬��Ӧ���Ҽ�����Ӧ��ע��ĺ���
 * @author guangx
 * @param displayInfo
 * @param Rect
 * @param Text
 *
 * @return Boolean
 */
//modify by linhx 2012-08-24
extern
Boolean TP_ShowText( TP_DisplayInfo *displayInfo,
                     TP_ListBoxRect Rect, uchar *Text );

/**
 * ����ȫ�������ı���ʾ������ʾ���⣬��Ӧ���Ҽ�����Ӧ��ע��ĺ���
 *
 * @author guangx
 *
 * @param displayInfo ��ʾ�ṹ����Ϣ
 * @param fmt: ��ʽ������ַ���
 *
 * @return TRUE:  �������<BR>
 *          FALSE: �Ҽ����ػ���󷵻�
 * @date 2012-08-02
 */
extern
Boolean TP_ShowTextFullScr( TP_DisplayInfo *displayInfo, uchar* fmt,... );

/**
 * Display a picture in special area
 *
 * @author linhx
 *
 * @param left Left margin
 * @param top Top margin
 * @param right  Right margin
 * @param bottom  Bottom margin
 * @param FontSize ASCII-singlebyte character small size
 *                       multibyte character do not display
 *                 CFONT-singlebyte character big size
 *                       multibyte character normal size
 * @param picture Picture to display
 *
 * @date 2012-09-11
 */
extern
void TP_ShowPicture(uint8 left, uint8 top, uint8 right,
                   uint8 bottom,uint8 *picture);
/**
 * Multi-line text string input
 *
 * @author Linhx
 * @version 1.0
 *
 * @param displayInfo Display information struct pointer
 * @param inputInfo Input information struct pointer
 * @param pBuf Output buffer pointer
 * @return 0xF9 Quit by timeout<BR>
 *         0xFB Quit by pressed the left key defined in {@link
 *              TP_DisplayInfo}<BR>
 *         0xFD Input control key has not been set<BR>
 *         0xFE Error parameter<BR>
 *         0xFF Quit by pressed the right key defined in {@link
 *              TP_DisplayInfo}
 *
 * @version 1.0
 *
 * @date 2012-08-02
 *
 * @note If the pointer of title is null, the title will not be
 *       display and the input area can display three lines text
 *       height,otherwise,the input area can only display two
 *       lines text height.
 */
extern
uint8 TP_TextInput(TP_DisplayInfo *displayInfo,
                   TP_InputInfo* inputInfo, uchar *pBuf );

/**
 * Numeral string input with timeout controlled
 *
 * @author linhx
 *
 * @param displayInfo Display information struct pointer
 * @param inputInfo Input information struct pointer
 * @param pBuf Output buffer pointer
 *
 * @return 0xF9 Quit by timeout<BR>
 *         0xFA Quit by reach maxLen<BR>
 *         0xFB Quit by pressed the left key defined in {@link
 *              TP_DisplayInfo}<BR>
 *         0xFD Input control key has not been set<BR>
 *         0xFE Error parameter<BR>
 *         0xFF Quit by pressed the right key defined in {@link
 *              TP_DisplayInfo}
 *
 * @version 1.0
 *
 * @date 2012-09-10
 *
 * @note This function can be used to input float string by
 *       setting inputMode in {@link TP_InputInfo}. If the
 *       maxLen defined in {@link TP_InputInfo} is less than
 *       21,a single line input field will be
 *       create,otherwise,you will see a multi line input field.
 */
extern
uint8 TP_NumberInput(TP_DisplayInfo *displayInfo,
                     TP_InputInfo * inputInfo,uchar *pBuf );

/**
 * Password string input with timeout controlled
 *
 * @author linhx
 *
 * @param displayInfo Display information struct pointer
 * @param inputInfo Input information struct pointer
 * @param echo Echo display character to replace user input
 * @param pBuf Output buffer pointer
 *
 * @return 0xF9 Quit by timeout<BR>
 *         0xFA Quit by reach maxLen<BR>
 *         0xFB Quit by pressed the left key defined in {@link
 *              TP_DisplayInfo}<BR>
 *         0xFD Input control key has not been set<BR>
 *         0xFE Error parameter<BR>
 *         0xFF Quit by pressed the right key defined in {@link
 *              TP_DisplayInfo}
 *
 * @version 1.0
 *
 * @date  2012-09-10
 */
uint8 TP_PassWordInput( TP_DisplayInfo *displayInfo,
                          TP_InputInfo * inputInfo,
                          uint8 echo, uchar *pBuf );

/**
 * �� �� TP_CreateListBoxInterface() ���������������б�����
 * @param ListBox  �б�����
 * @return ����0 ��ʾ�ǰ�ȷ�ϼ����صģ�����0 ��ʾ�ǰ�ȡ�������ص�
 * @author ���Ķ�
 * @version 1.1
 * @date 20120211
 * @note ֻ�е�����ȷ�ϼ�����ȡ����ʱ�ŷ���
 */
extern
Boolean TP_CreateListBoxInterface(TP_ListBoxStyle listboxStyle,
                                 TP_DisplayInfo *displayInfo,
                                 uint8 *item[],
                                 uint8 itemNum,
                                 uint8 selectIndex,
                                 uint8 *chooseValue,
                                 uint8 *icon);


/**
 * Create listbox with checkbox style.
 *
 * @author linhx
 *
 * @param displayInfo   Pointer of display information
 * @param listBoxInfo   Pointer of the listBoxInfo infomation
 *                      structure list
 * @param itemNum       Number of items,it must equal with the
 *                      number of the structure list which the
 *                      listBoxInfo point to.
 * @param selectKey     Key code to seleck checkbox
 *
 * @return TRUE: Quit by pressed the confirm key<BR>
 *          FALSE: Quit by pressed the cancel key or
 *                  error parameter.
 *
 * @version 1.0
 *
 * @date 2012-09-26
 */
extern
Boolean TP_CreateCheckBoxInterface(TP_DisplayInfo *displayInfo,
                                   TP_ListBoxInfo listBoxInfo[],
                                   uint8 itemNum,uint8 selectKey);




/**
 *
 *
 * @author linhx (2012/10/12)
 *
 * @param displayInfo
 * @param listboxStyle
 * @param listBoxInfo
 * @param itemNum
 * @param optionStyle
 * @param optionInfo
 * @param optionNum
 * @param optionSelectKey
 *
 * @return Boolean
 */
extern
Boolean TP_CreateListBoxWithOp(TP_DisplayInfo *displayInfo,
                               TP_ListBoxStyle listboxStyle,
                               TP_ListBoxInfo listBoxInfo[],uint8 itemNum,
                               TP_ListBoxStyle optionStyle,
                               TP_ListBoxInfo optionInfo[],uint8 optionNum,
                               uint8 optionSelectKey);

/**
 * To show the progress bar and show the percentage if possible.
 *
 * @author linhx
 *
 * @version 1.0
 *
 * @param note Display note of progress bar
 *
 * @param current Current process value
 *
 * @param total Total process value
 *
 * @date 2012-08-31
 *
 * @note If the right margin value in parameter rect is less
 *       than {@link #MAX_SCREEN_WIDTH}-33,the percentage will
 *       be display.
 *
 */
extern
void TP_ShowProgressBar(uchar* note,uint32 current,uint32 total);

extern
Boolean TP_IsGBK(uint8 WordCode);

extern
Boolean TP_DateInput( TP_DisplayInfo *displayInfo, TP_Date *pDate );

extern
Boolean TP_TimeInput( TP_DisplayInfo *displayInfo, TP_Time *pTime );

extern
void TP_GetFontInfo( ST_FONT *AsciiFont,ST_FONT *CfontFont);

extern
uint8 TP_GetFontHeight();

extern
void TP_DrawFootMenu( uint8 left,
                      uint8 top,
                      uint8 right,
                      uint8 bottom,
                      uint8 fontSize,
                      uint8 *leftCaption,
                      uint8 *rightCaption );

extern
void TP_MenuTitleCat( char *strbuf,uint16 bufLen, uint16 dispAreaWidth,
                      char *Lstr, char *Rstr, char Fillchar );


typedef Boolean (*CallBackFUNC)(cJSON*, int, const char *start_date, const char *end_date);

Boolean showText(TP_DisplayInfo *displayInfo, TP_ListBoxRect  Rect, uint8 *pScrText, CallBackFUNC callback, const cJSON *data, int size,  const char *start_date, const char *end_date);



#endif
