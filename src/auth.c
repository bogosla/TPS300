#include "auth.h"
#include "TP_DemoDefine.h"


// OK
void getConfiguration(void) {
    char *bufferUser = NULL;
    char imei[16];
	cJSON *json = NULL, *user = NULL;
	cJSON *tmp = NULL;
    char chtmp[64];
    char *todisplay = NULL;

    TP_ListBoxRect listRect = {2, 17, 125, 62};

    TP_DisplayInfo displayInfo =
    {
        "",
        ALIGN_LEFT,
        "",
        "",
        TP_KEY_OK,
        TP_KEY_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        ASCII
    };

    memset(imei, 0x00, sizeof(imei));
    memset(chtmp, 0x00, sizeof(chtmp));

    LCD_Clear();
    TP_ScrCls();
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(1, 2);
    Display_Header("Konfigirasyon");
    TP_GetModalIMEI(imei, 16);
    read_from_file(INFO_USER_FILE, &bufferUser);

	json = cJSON_Parse(bufferUser);

    if (json != NULL) {
		user = cJSON_GetObjectItemCaseSensitive(json, "user");		
		tmp = cJSON_GetObjectItemCaseSensitive(user, "companyName");		
		sprintf(chtmp, "Non Konpani  : %s\n", tmp->valuestring);
        concatString(&todisplay, chtmp);
		sprintf(chtmp, "Serial NO    : %s\n", imei);
        concatString(&todisplay, chtmp);

		tmp = cJSON_GetObjectItemCaseSensitive(user, "first_name");	
		sprintf(chtmp, "Non Vande    : %s\n", tmp->valuestring);
        concatString(&todisplay, chtmp);

		tmp = cJSON_GetObjectItemCaseSensitive(user, "id");	
		sprintf(chtmp, "No Bank      : %d\n", tmp->valueint);
        concatString(&todisplay, chtmp);

        tmp = cJSON_GetObjectItemCaseSensitive(user, "mariage_gratuit");
		sprintf(chtmp, "Maryaj Gratis: %s\n", cJSON_IsTrue(tmp) ? "Wi" : "Non");
        concatString(&todisplay, chtmp);

		tmp = cJSON_GetObjectItemCaseSensitive(user, "delete_fiche_min");	
		sprintf(chtmp, "Dire siprsyon: %d\n", tmp->valueint);
        concatString(&todisplay, chtmp);

        showText(&displayInfo, listRect, todisplay, NULL, NULL, 0, NULL, NULL); // Display
    } else {
        handleJSONNULL();
    }

    if (json != NULL) {
		cJSON_Delete(json);
        json = NULL;
    }

    if (bufferUser != NULL) {
        TP_FreeMemory((void**)&bufferUser);
        bufferUser = NULL;
    }

    if (todisplay != NULL) {
        TP_FreeMemory((void**)&todisplay);
        todisplay = NULL;
    }
    return;
}



Boolean postHandleLogin(void) {

    Boolean isRunning = TRUE;
    uint8 key, key_ret;
    cJSON *json = NULL, *imei = NULL, *role = NULL, *accessToken = NULL, *user = NULL;
    char name[30], password[30];
    uint16 status_code = 0;
    char *buffer = NULL;
    char json_string[128];
    int32 ret;
    Boolean success = FALSE;
    uint8 imeiStr[16];

    TP_ScrCls();
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(1, 2);

    memset(json_string, 0x00, sizeof(json_string));
    memset(name, 0x00, sizeof(name));
    memset(imeiStr, 0x00, sizeof(imeiStr));
    

    TP_GetModalIMEI(imeiStr, 16);
    isRunning = TRUE;
    
    while (isRunning) {
        memset(password, 0x00, sizeof(password));
        LCD_Clear();
        Display_Header("Koneksyon");

        TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);
        TP_ScrGotoxyEx(3, 15);
        TP_LcdPrintf("Itilizate: ");
        TP_SetDisplayArea(6, 23, 124, 45);
        TP_ScrGotoxyEx(6, 23);
        TP_SetInputModePosition(110, 4);
        TP_SetInputModeControl(TP_KEY_OK, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_ShowInputNum(0, 1, 1);
        key_ret = TP_GetHzString(name, 0, sizeof(name) - 1);
        if (key_ret == 0xFF) return;


        TP_ScrSpaceSet(0, 2);
        TP_SetDisplayArea(1, MAX_SCREEN_HEIGHT - 16, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
        TP_ScrGotoxyEx(1, MAX_SCREEN_HEIGHT - 16);
        TP_LcdPrintf("SN:%s", imeiStr);

        TP_ScrAttrSet(0);
        TP_ScrFontSet(ASCII);
        TP_ScrSpaceSet(1, 2);
        TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);
        TP_ScrClsDisplayArea();
        TP_ScrGotoxyEx(3, 15);
        TP_LcdPrintf("Modpas: ");
        TP_SetDisplayArea(6, 23, 124, 45);
        TP_ScrGotoxyEx(6, 23);
        TP_SetInputModePosition(110, 4);
        TP_SetInputModeControl(TP_KEY_OK, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_ShowInputNum(0, 1, 1);
        key_ret = TP_GetPasswordTimed(password, 4, sizeof(password) - 1, 0x0001|0x0002|0x0003|0x0004|0x0400|0x1000, '*', 0);
        if (key_ret == 0xFF) return;
        
        sprintf(json_string, "{\"username\": %s, \"password\": %s}", name, password);
        
        LCD_Clear();
        if ((ret = make_http_POST(SUBDOMAIN_URL, BASE_URL, AUTH_PATH, json_string, NULL, &status_code, &buffer)) >= 0) {
            json = cJSON_Parse((char*)buffer);
            if (json == NULL) {
                success = FALSE;
                handleJSONNULL();
            }
            else {
                if (status_code >= 400 && status_code <= 599) {
                    // Handle errors(>=400)
                    success = FALSE;
                    Handle404Error(status_code, buffer);
                } else if (status_code >= 200 && status_code <= 299)
                {
                    role = cJSON_GetObjectItemCaseSensitive(json, "role");
                    accessToken = cJSON_GetObjectItemCaseSensitive(json, "accessToken");
                    user = cJSON_GetObjectItemCaseSensitive(json, "user");
					imei = cJSON_GetObjectItemCaseSensitive(user, "imei");
                    

                    // Check If agent and IMEI
                    if ((strcmp(role->valuestring, "agent") == 0 && strcmp(imei->valuestring, (char*)imeiStr) == 0))
                    {   
                        success = TRUE;
                        write_to_file(ACCESS_TOKEN_FILE, accessToken->valuestring);
                        write_to_file(INFO_USER_FILE, buffer);

                        if (buffer != NULL)
                            TP_FreeMemory((void**)&buffer);
                        buffer = NULL;
                        
                        if ((ret = make_http_GET(SUBDOMAIN_URL, BASE_URL, "/api/games/info-tirage", accessToken->valuestring, &status_code, &buffer)) >= 0)
                        {
                            if (status_code >= 200 && status_code <= 299) {
                                int32 ret = write_to_file(INFO_TIRAGE_FILE, buffer);
                                TP_DbgSerialPrn("Tirages: %d", ret);
                            } else {
                                TP_DbgSerialPrn("Tirages %s: %d", buffer, ret);
                            }
                            
                        } else {
                            handleSocketError(ret);
                        }
                        isRunning = FALSE;
                    } else
                    {
                        success = FALSE;
                        // Display OU pa otorize 
                        HandleIMEI_NOT_OK();  
                    }
                }
            }
            if (buffer != NULL)
            {
                TP_FreeMemory(&buffer);
                buffer = NULL;
            }
        } else {
            success = FALSE;
            LCD_Clear();
            TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
            TP_ScrClsDisplayArea();
            TP_ScrGotoxyEx(63 - 28, 32 - 7);
            TP_LcdPrintf("Ere! Reeseye!");
            TP_ScrGotoxyEx(3, 55);
            TP_LcdPrintf("Wi=OK");

            key = waitforKey();
            switch (key) {
                case TP_KEY_OK:
                    if (buffer != NULL)
                    {
                        TP_FreeMemory(&buffer);
                        buffer = NULL;
                    }
                    break;
                case TP_KEY_CANCEL:
                    isRunning = 0;
                    TP_Kbflush();
                    break;
                default: 
                    break;
            }
        }
    }
    if (json != NULL)
    {
        cJSON_Delete(json);
        json = NULL;
    }
    if (buffer != NULL)
    {
        TP_FreeMemory(&buffer);
        buffer = NULL;
    }
    return success;
}



void getRapports(void) {
    TP_DateTime dateTime1;
    TP_DateTime dateTime2;
    char path[90];
    char *bufferToken = NULL, *buffer = NULL;
    Boolean result = FALSE, isRunning = TRUE;
    uint16 status_code = 0;

    char start_date[11], end_date[11];
    uint8 key = 0;

    TP_DisplayInfo displayInfo =
    {
        "Dat Komanse:",
        ALIGN_LEFT,
        "OK",
        "Bak",
        TP_KEY_OK,
        TP_KEY_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        ASCII
    };

    isRunning = TRUE;
    while (isRunning) {
        LCD_Clear();
        TP_GetDateTime(&dateTime1);
        displayInfo.strTitle = "Dat Komanse";
        result = TP_DateInput(&displayInfo, &dateTime1.Date);
        if (result == FALSE)
            return;

        displayInfo.strTitle = "Dat Fen";
        TP_GetDateTime(&dateTime2);
        result = TP_DateInput(&displayInfo, &dateTime2.Date);
        if (result == FALSE)
            return;

        memset(path, 0, sizeof(path));
        memset(start_date, 0, sizeof(start_date));
        memset(end_date, 0, sizeof(end_date));

        sprintf(start_date, "%d-%02d-%02d", dateTime1.Date.Year, dateTime1.Date.Month, dateTime1.Date.Day);
        sprintf(end_date, "%d-%02d-%02d", dateTime2.Date.Year, dateTime2.Date.Month, dateTime2.Date.Day);
        sprintf(path, "/api/games/rapports?start_date=%s&end_date=%s", start_date, end_date);

        read_from_file(ACCESS_TOKEN_FILE, &bufferToken);   
        if (make_http_GET(SUBDOMAIN_URL, BASE_URL, path, bufferToken, &status_code, &buffer) >= 0) {
                LCD_Clear();
                TP_SetDisplayArea(2, 2, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
                TP_ScrClsDisplayArea();
                TP_ScrGotoxyEx(63 - 20, 31 - 7);
                if (status_code >= 200 && status_code <= 299) {
                    TP_LcdPrintf("Enprime...");
                    if (print_rapport(buffer, start_date, end_date) == TRUE) {
                        TP_LcdPrintf("Enprime!  ");
                        waitforKey();
                        // isRunning = FALSE;
                        break;
                    }
                } else {
                    // ere
                    Handle404Error(status_code, buffer);
                }
        } else {
            LCD_Clear();
            TP_SetDisplayArea(2, 2, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
            TP_ScrClsDisplayArea();
            TP_ScrGotoxyEx(63 - 28, 31 - 7);
            TP_LcdPrintf("Ere! Reeseye!");

            TP_ScrGotoxyEx(2, 63 - 7);
            TP_LcdPrintf("Wi=OK!");
            key = waitforKey();
            if (key == TP_KEY_OK)
                continue;
            else
                break;
        }
    }

    if (buffer != NULL)
    {
        TP_FreeMemory((void**)&buffer);
        buffer = NULL;
    }

    if (bufferToken != NULL)
    {
        TP_FreeMemory((void**)&bufferToken);
        bufferToken = NULL;
    }
    return;
}



// OK
void getTirages(void) {
    char *bufferToken = NULL, *buffer = NULL;
	char start_date[30], path[43];
    uint16 status_code = 0;
	cJSON *json = NULL;
    uint8 size = 0;
    char tir[32];
    TP_ListBoxRect listRect = {0};
    TP_DateTime dateTime;
    int32 ret = 0;
    Boolean isRunning = FALSE, result = FALSE;

    TP_DisplayInfo displayInfo =
    {
        "",
        ALIGN_LEFT,
        "",
        "",
        TP_KEY_OK,
        TP_KEY_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        ASCII
    };

    TP_ScrCls();
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(0, 2);

    isRunning = TRUE;

    TP_GetDateTime(&dateTime);
    memset(start_date, 0x00, sizeof(start_date));
    while (isRunning) {
        LCD_Clear();
        displayInfo.strTitle = "Dat";
        result = TP_DateInput(&displayInfo, &dateTime.Date);
        if (result == FALSE)
            return;
        // LCD_Clear();
        memset(path, 0x00, sizeof(path));
        memset(tir, 0x00, sizeof(tir));
        
        // TP_GetDateTime(&dateTime);

        sprintf(path, "/api/games/list-tirage?date=%d-%02d-%02d", dateTime.Date.Year, dateTime.Date.Month, dateTime.Date.Day);
        
        // Get bufferToken
        read_from_file(ACCESS_TOKEN_FILE, &bufferToken);
        if ((ret = make_http_GET(SUBDOMAIN_URL, BASE_URL, path, bufferToken, &status_code, &buffer)) >= 0) {
            if (status_code >= 200 && status_code <= 299) {
                json = cJSON_Parse(buffer);
                if (json != NULL)
                {
                    if (buffer != NULL) {
                        TP_FreeMemory((void**)&buffer);
                        buffer = NULL;
                    }
                    if (cJSON_IsArray(json))
                    {
                        cJSON *element;
                        cJSON_ArrayForEach(element, json) 
                        {
                            cJSON *list = cJSON_GetObjectItemCaseSensitive(element, "list");

                            if (cJSON_IsArray(list)) 
                            {
                                cJSON *itemList;
                                cJSON_ArrayForEach(itemList, list) 
                                {
                                    cJSON *name = cJSON_GetObjectItemCaseSensitive(itemList, "id");
                                    cJSON *t1 = cJSON_GetObjectItemCaseSensitive(itemList, "tirage_1");
                                    cJSON *t2 = cJSON_GetObjectItemCaseSensitive(itemList, "tirage_2");
                                    cJSON *t3 = cJSON_GetObjectItemCaseSensitive(itemList, "tirage_3");
                                    // TP_DbgSerialPrn("\r\nTirage:%s\r\n", name->valuestring);

                                    sprintf(tir, "%s: %s-%s-%s\n", name->valuestring, t1->valuestring, t2->valuestring, t3->valuestring);  
                                    
                                    concatString(&buffer, tir);              
                                    size++;
                                }
                            }
                        }
                    }
                    if (json != NULL) {
                        cJSON_Delete(json);
                        json = NULL;
                    }
                } else {
                    handleJSONNULL();
                }
            }
        } else {
        handleSocketError(ret);
        }

        LCD_Clear();
        TP_ScrFontSet(ASCII);
        TP_ScrSpaceSet(0, 2);
        sprintf(start_date, "Lo Ganyan / %d-%02d-%02d", dateTime.Date.Year, dateTime.Date.Month, dateTime.Date.Day);
        Display_Header(start_date);
        
        if (size > 0) {
            listRect.left = 2;
            listRect.top = 17;
            listRect.right = 125;
            listRect.bottom = 62;
            showText(&displayInfo, listRect, buffer, NULL, NULL, 0, NULL, NULL);
        } else {
            handleEmptyList();
        }

        if (buffer != NULL) {
            TP_FreeMemory((void**)&buffer);
            buffer = NULL;
        }
        if (bufferToken != NULL) {
            TP_FreeMemory((void**)&bufferToken);
            bufferToken = NULL;
        }
    }
    return;
}


