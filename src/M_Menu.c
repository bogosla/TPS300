#include "M_Menu.h"

void freeMyData(void *data) {
    TP_FreeMemory((void**)&data);
}


int GetElement(LinkedList *list, const BouleItem *item) {
    Node *current = list->head;

    while (current != NULL) {
        BouleItem *r = (BouleItem*)(current->data);
        if (strcmp("L4", item->lotto) == 0 || strcmp("L5", item->lotto) == 0) {
            if (strcmp(r->boul, item->boul) == 0 && strcmp(r->lotto, item->lotto) == 0 && strcmp(r->option, item->option) == 0)
                return 0;
        } else {
            if (strcmp(r->boul, item->boul) == 0 && strcmp(r->lotto, item->lotto) == 0)
                return 0;
        }
        current = current->next;
    }
    return -1;
}


void AddElem(LinkedList* list, BouleItem *element) 
{
    int s = GetElement(list, element);
    TP_DbgSerialPrn("Add Elem: %s:%s: %d\n", element->boul, element->lotto, s);
    
    if (s == 0)
    {
        freeMyData(element);
        return;
    } else
    {
       Push(list, element);
    }
}


static int getRevers(int boul)
{
	int r = 0;
	int _boul = boul;
	while (_boul > 0)
	{
		r = r*10 + _boul % 10;
		_boul /= 10;
	}
	return r;
}


void addBoulePaire(LinkedList *list, char *pri)
{
	int i = 0;
	while (i < 10)
	{
        BouleItem *item = NULL;
        TP_AllocMemory(sizeof(BouleItem), (void**)&item);
        if (item != NULL) {
            item->id = 1;
            sprintf(item->boul, "%d%d", i, i);
            sprintf(item->pri, "%s", pri);
            sprintf(item->option, "%s", "1");

            setOptionBouleItem(item);
            AddElem(list, item);
        }
		i++;
	}
    return;
}


void addBouleTriple(LinkedList *list, char *pri)
{
	int i = 0;
	while (i < 10)
	{
		BouleItem *item = NULL;
        TP_AllocMemory(sizeof(BouleItem), (void**)&item);
        if (item != NULL) {
            item->id = 1;
            sprintf(item->boul, "%d%d%d", i, i, i);
            sprintf(item->pri, "%s", pri);
            sprintf(item->option, "%s", "1");

            setOptionBouleItem(item);
            AddElem(list, item);
        }
		i++;
	}
    return;
}


void addBouleRevers(LinkedList *list, char *pri) {
    Node *current = list->head;

    while (current != NULL) {
        BouleItem *item = ((BouleItem*)current->data);
        TP_DbgSerialPrn("Revers: %s\n", item->boul);

        if (strcmp(item->lotto, "BO") == 0) {
            int r = getRevers(atoi(item->boul));
            
            BouleItem *nitem = NULL;
            TP_AllocMemory(sizeof(BouleItem), (void**)&nitem);
            if (nitem != NULL) {
                nitem->id = 1;
                sprintf(nitem->boul, "%02d", r);
                sprintf(nitem->option, "%s", "1");
                sprintf(nitem->pri, "%s", pri);

                setOptionBouleItem(nitem);
                AddElem(list, nitem);
            }
        }
        current = current->next;
    }
}


void addBouleL4(LinkedList *list, char *pri, const char* tip) {
    Node *current = list->head;

    while (current != NULL) {
        BouleItem *item = ((BouleItem*)current->data);
        Node *current2 = current->next;
        // if (current->next->next == NULL) return;
        while (current2 != NULL) {
            BouleItem *item2 = ((BouleItem*)current2->data);
            // Only add number of two digits
            TP_DbgSerialPrn("Boule L4: %s:%s\n", item->boul, item2->boul);

			if (strcmp(item->lotto, "BO") == 0 && strcmp(item2->lotto, "BO") == 0)
			{
				BouleItem *nitem = NULL;
                TP_AllocMemory(sizeof(BouleItem), (void**)&nitem);
                if (item != NULL) {
                    item->id = 1;
                    sprintf(nitem->boul, "%s%s", item->boul, item2->boul);
                    sprintf(nitem->pri, "%s", pri);
                    sprintf(nitem->lotto, "%s", tip);
                    sprintf(nitem->option, "%s", "1");
                    setOptionBouleItem(nitem);
                    AddElem(list, nitem);
                }

                if (strcmp(tip, "L4") == 0) {
                    BouleItem *nitem2 = NULL;
                    TP_AllocMemory(sizeof(BouleItem), (void**)&nitem2);
                    if (item != NULL) {
                        item->id = 1;
                        sprintf(nitem2->boul, "%s%s", item2->boul, item->boul);
                        sprintf(nitem2->pri, "%s", pri);
                        sprintf(nitem2->lotto, "%s", tip);
                        sprintf(nitem2->option, "%s", "1");
                        setOptionBouleItem(nitem2);
                        AddElem(list, nitem2);
                    }
                }
			}
            current2 = current2->next;
        }
        current = current->next;
    }
	return;
}


void addPwent(LinkedList *list, char *pri, unsigned int pwent) {
	int i = 0;
	while (i < 10)
	{
		BouleItem *item = NULL;
        TP_AllocMemory(sizeof(BouleItem), (void**)&item);
        if (item != NULL) {
            item->id = 1;
            sprintf(item->boul, "%d%d", i, pwent);
            sprintf(item->pri, "%s", pri);
            sprintf(item->option, "%s", "1");
            setOptionBouleItem(item);
            AddElem(list, item);
        }
		i++;
	}
    return;
}



const char menu[][MAX_CHAR_ITEMS] = {
    "1.Re-enprime",
    "2.Rejwe",
    "3.Siprime"
};


void test_network(void) {
    uint16 status_code = 0;
    char *buffer = NULL;

    LCD_Clear();
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
    TP_ScrClsDisplayArea();

    if (make_http_GET(PNULL, "http://jsonplaceholder.typicode.com", "/todos/1", NULL, &status_code, &buffer) >= 0) {
        TP_ScrGotoxyEx(20, MAX_SCREEN_HEIGHT / 2 - 8);
        TP_ScrClsDisplayArea();
        TP_LcdPrintf("Status Code = %d", status_code);
    } else {
        TP_ScrClsDisplayArea();
        TP_ScrGotoxyEx(18, MAX_SCREEN_HEIGHT / 2 - 8);
        TP_LcdPrintf("Problem Entenet!");
    }

    if (buffer != NULL)
    {
        TP_FreeMemory(&buffer);
        buffer = NULL;
    }
    while (TP_Kbhit() == 0x00);
    return;
}


void test_printer(void)
{
    TP_PrnSessionStart();
    TP_PrnClearString();
    TP_ScrSetIcon(ICON_PRINTER, OPENICON);

    TP_PrnSpaceSet(0, 0);
    TP_PrnHighlightSet(FALSE);
    TP_PrnIndentSet((uint16)142);
    TP_PrnFontSet(3);
    TP_PrnAddString("Loyallto\n");
    TP_PrnIndentSet(0);
    TP_PrnFontSet(1);
    TP_PrnAddString((uint8*)"Tel: %s", "+509 34 66 91 52\f");
    TP_PrnAddString((uint8*)"Email: %s", "info@loyalttoservices.com\f");
    TP_PrnAddString((uint8*)"Web: %s", "loyalttoservices.com\f");
    TP_PrnAddString((uint8*)"Adress: %s", "23A, Rue Faustin 1er, delams 75\f\n\n");

    TP_PrnAddString((uint8*)"Ou bezwen pwograme POS android\f");
    TP_PrnAddString((uint8*)"NEW8210, TPS300 ak sistem nou an\f");
    TP_PrnAddString((uint8*)"MGNLOTTO.\f");
    TP_PrnAddString((uint8*)"Bolet, Restoran...Rele nou!!!\f");

    TP_PrnString();
    TP_PrnClearString();
    
    TP_PrnStep(7);

    TP_PrnSessionEnd();
    TP_ScrSetIcon(ICON_PRINTER, CLOSEICON);

    return;
}


static int8 current_y = 2;


void LCD_Clear(void) {
    current_y = 2;
    TP_ScrCls();
    TP_ScrClsDisplayArea();
}


void Display_Header(const char *title) {
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, 12);
    TP_ScrFillRect(1, 1, 127, 10);
    TP_ScrClsPrint(1);
    TP_ScrGotoxyEx(4, 3);
    TP_ScrAttrSet(1);
    TP_LcdPrintf(title);
    TP_ScrAttrSet(0);
    current_y += 8;
}


int8 LCD_Menu(const char *title, const char menuItems[][MAX_CHAR_ITEMS], uint8 count, int8 select)
{
    uint32 charSpace = 0, lineSpace = 0;
    uint8 fontSizeFont = 0;
    uint8 running = TRUE;
    int8 istart, i, key = 0, to_ret;

    TP_ScrCls();
    TP_Kbflush();
    TP_BanIncomingCall(TRUE);

    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(1, 2);
    TP_ScrFontGet(&fontSizeFont);
    TP_StopRing();    

    if (count < 0) return -1;
    if (select > count) select = 0;

    while (running)
    {
        LCD_Clear();
        Display_Header(title);
        TP_SetDisplayArea(2, 14, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
        TP_ScrClsDisplayArea();
        TP_LcdFreeze();
        
        current_y = 14;
		istart = (select / 5) *  5; 
        for (i = 0;  i < 5; i++)
        {
            if (istart + i < count)
            {
                TP_ScrGotoxyEx(3, current_y);
                if (istart + i == select)
                {
                    TP_ScrAttrSet(1);
                    TP_LcdPrintf("%s", menuItems[istart + i]);
                    TP_ScrAttrSet(0);
                }
                else
                    TP_LcdPrintf("%s", menuItems[istart + i]);	
                current_y += 8;				
            }
            else
                break;
        }
		TP_ScrUpdate();

LOOP:
        TP_NotifyCheck();
        if (TP_Kbhit() == 0xFF) {
            key = TP_GetKey();
            switch(key)
            {
                case TP_KEY_UP:
                    select--;
                    if (select < 0) select = count - 1;
                    break;
                case TP_KEY_DOWN:
                    select++;
                    if (select >= count) select = 0;
                    break;
                case TP_KEY_OK:
                    TP_DbgSerialPrn("\r\nPress:%d\r\n", select);

                    running = FALSE;
                    break;
                case TP_KEY_CANCEL:
                    select = -1;
                    running = FALSE;
                    break;
                default:
                    goto LOOP;
            } 
        } else {
            goto LOOP;
        }
    }
    to_ret = select;
    return to_ret;
}


int8 check_connection(void)
{
    Boolean toggle = TRUE;
    int32 checkPPP, pppReturn;
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
    TP_ScrClsDisplayArea();
    TP_ScrGotoxyEx(63 - 26, 31 - 7);
    TP_LcdPrintf("Fon ti tann  ");


    TP_ScrSetIcon(ICON_DOWN, OPENICON);
    TP_ScrSetIcon(ICON_UP, OPENICON);

    checkPPP = TP_WlPPPCheck();
    if (checkPPP == WL_RET_OK) {
        TP_ScrSetIcon(ICON_DOWN, CLOSEICON);
        TP_ScrSetIcon(ICON_UP, CLOSEICON);
        return 0;
    }

    pppReturn = TP_WlPPPLogin((uint8*)"natcom", (uint8*)"card", (uint8*)"card", 0, 0, 65000);
	if (pppReturn == WL_RET_OK || pppReturn == WL_RET_ERR_DIALED) {
        TP_ScrSetIcon(ICON_DOWN, CLOSEICON);
        TP_ScrSetIcon(ICON_UP, CLOSEICON);
        return 0;
    } 

    while(1)
	{
        TP_LcdFreeze();
        TP_ScrClsDisplayArea();
        if (toggle) {
            TP_LcdPrintf("Fon ti tann. ");
            TP_ScrSetIcon(ICON_DOWN, OPENICON);
            TP_ScrSetIcon(ICON_UP, CLOSEICON);
            toggle = FALSE;
        }
        else {
            TP_LcdPrintf("Fon ti tann..");
            TP_ScrSetIcon(ICON_DOWN, CLOSEICON);
            TP_ScrSetIcon(ICON_UP, OPENICON);
            toggle = TRUE;
        }

		checkPPP = TP_WlPPPCheck();
		if (checkPPP == NET_OK) {
            TP_ScrSetIcon(ICON_DOWN, CLOSEICON);
            TP_ScrSetIcon(ICON_UP, CLOSEICON);
			return 0;
        }
        if (TP_Kbhit() == 0xFF) {
            if (TP_GetKey() == TP_KEY_CANCEL) {
                TP_ScrSetIcon(ICON_DOWN, CLOSEICON);
                TP_ScrSetIcon(ICON_UP, CLOSEICON);
                break;
            }  
        }
        // Wait for
        TP_TimerSet(1, 250);
        while (TP_TimerCheck(1) != 0);
        TP_ScrUpdate();
	}
    return -1;
}


int32 make_http_GET(const char* subdomain, const char *hostname, const char *path, const char *token, uint16 *status_code, char **buffer)
{
    uint16 port;
    uchar hostIP[MAX_HOST_IP];
    int32 socket, _result;
    TP_SockAddrT dest_addr;

    char http_request[TP_TCPIP_TCP_MAX_SEND_SIZE];
    char data[CHUNK_SIZE];
    char *response = NULL;
    char *body_start = NULL;

    int16 total_already_sent, total_size_len_request;
    uint16 realsize, size;

    check_connection(); // PPP Connection
    
    // Check protocol for HTTP or HTTPS
    if (strncmp(hostname, "http://", 7) == 0) {
        port = HTTP_DEFAULT_PORT;
        hostname = hostname + 7;
    } else if (strncmp(hostname, "https://", 8) == 0) {
        port = HTTPS_DEFAULT_PORT;
        hostname = hostname + 8;
    } else {
        return -100;
    }
    

    _result = TP_DNSResolve((uchar*)hostname, (uchar*)hostIP, MAX_HOST_IP);
    if (_result != NET_OK) return _result;

    socket = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
    if (socket < 0) return socket;
    
    _result = TP_SockAddrSet(&dest_addr, (uchar*)hostIP, port);
    if (_result < 0) {
        TP_NetCloseSocket(socket);
        return _result;
    }

    _result = TP_NetConnect(socket, &dest_addr, 0);
    if (_result < 0) {
        TP_NetCloseSocket(socket);
        return _result;
    }

    if (token == NULL) {
        snprintf(http_request, TP_TCPIP_TCP_MAX_SEND_SIZE, 
            "GET %s HTTP/1.1\r\nHost: %s%s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname
        );
    } else {
         snprintf(http_request, TP_TCPIP_TCP_MAX_SEND_SIZE, 
            "GET %s HTTP/1.1\r\nHost: %s%s\r\nAuthorization: Token %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            token
        );
    }

    total_size_len_request = strlen(http_request);
    total_already_sent = 0;

    // Send the HTTP request
    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 30000);
    while (total_already_sent < total_size_len_request) {
        _result = TP_NetSend(socket, (const uint8*)http_request + total_already_sent, (uint16)total_size_len_request - total_already_sent, 0);
        TP_DbgSerialPrn("\r\nNETSEND:%d %d\r\n", _result, total_size_len_request);
        
        if (_result < 0) {
            TP_NetCloseSocket(socket);
            return _result;
        }
        total_already_sent += _result;
    }

    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 30000);

    _result = 0;
    size = 1;
    while((_result = TP_NetRecv(socket, data, sizeof(data), 0)) > 0) {
        if (TP_ReAllocMemory(size + _result, (void**)&response) == FALSE) {
            return -1;
        }
        memcpy(response + size - 1, data, _result);
        size += _result;
        response[size - 1] = '\0';  
    }

    TP_DbgSerialPrn("(%d)Result:%s\n", _result, response);

    if (_result != -54) {
        if (response != NULL) {
            TP_FreeMemory((void**)&response);
            response = NULL;
        }
        return _result;  
    } 

    if (sscanf(response, "HTTP/1.%*[01] %d", status_code) != 1) {
        TP_DbgSerialPrn("Invalid status line format\n");
        TP_NetCloseSocket(socket);
        if (response != NULL) {
            TP_FreeMemory((void**)&response);
            response = NULL;
        }
        return -2;
    }

    body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        TP_DbgSerialPrn("Invalid HTTP response: No body found\n");
        if (response != NULL) {
            TP_FreeMemory((void**)&response);
            response = NULL;
        }
        return -3;
    }

    // Print the content/body if it exists
    size = strlen(body_start);
    if (size > 4) {
        TP_DbgSerialPrn("Body Content found(%d) = %s\n", size - 4,  body_start + 4);
		
		if (TP_ReAllocMemory(size, buffer) == FALSE) {
            if (response != NULL) {
                TP_FreeMemory((void**)&response);
                response = NULL;
            }
			return -6;
        }
		memcpy(*buffer, body_start + 4, size - 3);
    } else {
        TP_DbgSerialPrn("No body content found in the response.\n");
    }

    // Close the socket
    TP_NetCloseSocket(socket);

    // Free stuff
    if (response != NULL) {
        TP_FreeMemory((void**)&response);
        response = NULL;
    }

    return 0; // Success
}


int32 make_http_POST(const char* subdomain, const char *hostname, const char *path, const char *body, const char *token,  uint16 *status_code, char **buffer)
{
    int32 socket, _result;
    TP_SockAddrT dest_addr;

    char data[CHUNK_SIZE];
    char http_request[TP_TCPIP_TCP_MAX_SEND_SIZE];

    uchar hostIP[MAX_HOST_IP];
    uint8 *response = NULL, *body_start = NULL, *tempBuffer = NULL;

    int32 total_already_sent, total_size_len_request, port, size;

    // Check protocol for HTTP or HTTPS
    if (strncmp(hostname, "http://", 7) == 0) {
        port = HTTP_DEFAULT_PORT;
        hostname = hostname + 7;
    } else if (strncmp(hostname, "https://", 8) == 0) {
        port = HTTPS_DEFAULT_PORT;
        hostname = hostname + 8;
    } else {
        return -100;
    }
    
    check_connection(); // PPP Connection
    _result = TP_DNSResolve((uchar*)hostname, (uchar*)hostIP, (uint16)MAX_HOST_IP);
    if (_result != NET_OK) return _result;

    socket = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
    if (socket < 0) return socket;
    
    _result = TP_SockAddrSet(&dest_addr, (uchar*)hostIP, port);
    if (_result < 0) {
        TP_NetCloseSocket(socket);
        return _result;
    }

    _result = TP_NetConnect(socket, &dest_addr, 0);
    if (_result < 0) {
        TP_NetCloseSocket(socket);
        return _result;
    }

    if (token == NULL) {
        snprintf(http_request, TP_TCPIP_TCP_MAX_SEND_SIZE, 
            "POST %s HTTP/1.1\r\nHost: %s%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n%s\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            "application/json",
            body == PNULL ? 0 : strlen(body),
            body
        );
    } else {
         snprintf(http_request, TP_TCPIP_TCP_MAX_SEND_SIZE, 
            "POST %s HTTP/1.1\r\nHost: %s%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Token %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n%s\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            "application/json",
            body == PNULL ? 0 : strlen(body),
            token,
            body
        );
    }


    total_size_len_request = strlen(http_request);
    total_already_sent = 0;

    // Send the HTTP request
    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);
    while (total_already_sent < total_size_len_request) {
        _result = TP_NetSend(socket, (const uint8*)http_request + total_already_sent, (uint16)total_size_len_request - total_already_sent, 0);
        TP_DbgSerialPrn("\nNET SEND:%d %d\r\n", _result, total_size_len_request);
        
        if (_result < 0) {
            TP_NetCloseSocket(socket);
            TP_DbgSerialPrn("\nError from NET.\r\n");
            return _result;
        }
        total_already_sent += _result;
    }
   
    _result = -1;
    size = 0;

    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);
    memset(data, 0, sizeof(data));
    do {
        _result = TP_NetRecv(socket, (uint8*)data, CHUNK_SIZE - 1, 0);
        // data[_result] = '\0'; // Null-terminate the received data
        if (_result > 0) {
            if (TP_ReAllocMemory(size + _result + 1, (void**)&response) == FALSE) {
                return -1;
            }
            memcpy(&(response[size]), data, _result);
            size += _result;
            response[size] = '\0'; // Null-terminate the response string
        } else if (_result == 0) {
            break; // No more data available
        } else {
            if (_result == -54)
                _result = 0;
            break;
        }
    } while (_result > 0);

    TP_DbgSerialPrn("\n\nResult = (%d)(%d) Res= %d\n", _result, strlen(response), size);

    if (_result != 0) {
        if (response != PNULL) {
            TP_FreeMemory(&response);
            response = PNULL;
        }
        return _result;
    }

    if (sscanf(response, "HTTP/1.%*[01] %d", status_code) != 1) {
        TP_DbgSerialPrn("Invalid status line format\n");
        TP_NetCloseSocket(socket);
         if (response != PNULL) {
            TP_FreeMemory(&response);
            response = PNULL;
        }
        return -2;
    }
    TP_DbgSerialPrn("Status Code => %d\n", *status_code);


    body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        TP_DbgSerialPrn("Invalid HTTP response: No body found\n");
        if (response != PNULL) {
            TP_FreeMemory(&response);
            response = PNULL;
        }
        return -3;
    }

    // Print the content/body if it exists
    size = strlen(body_start);
    if (size > 4) {
        TP_DbgSerialPrn("Body Content found(%d) = %s\n", size - 4,  body_start + 4);
		
		if (TP_ReAllocMemory(size, buffer) == FALSE) {
            if (response != NULL) {
                TP_FreeMemory((void**)&response);
                response = NULL;
            }
			return -6;
        }
		memcpy(*buffer, body_start + 4, size - 3);
    } else {
        TP_DbgSerialPrn("No body content found in the response.\n");
    }

    // Close the socket
    TP_NetCloseSocket(socket);

    // Free stuff
    if (response != NULL) {
        TP_FreeMemory((void**)&response);
        response = NULL;
    }
    return 0; // Success
}


int32 make_http_PUT(const char* subdomain, const char *hostname, const char *path, const char *body, const char *token,  uint16 *status_code, char **buffer)
{
    int32 socket, _result;
    TP_SockAddrT dest_addr;

    char data[CHUNK_SIZE];
    char http_request[TP_TCPIP_TCP_MAX_SEND_SIZE];

    uchar hostIP[MAX_HOST_IP];
    uint8 *response = NULL, *body_start = NULL, *tempBuffer = NULL;

    int32 total_already_sent, total_size_len_request, port, size;

    // Check protocol for HTTP or HTTPS
    if (strncmp(hostname, "http://", 7) == 0) {
        port = HTTP_DEFAULT_PORT;
        hostname = hostname + 7;
    } else if (strncmp(hostname, "https://", 8) == 0) {
        port = HTTPS_DEFAULT_PORT;
        hostname = hostname + 8;
    } else {
        return -100;
    }
    
    check_connection(); // PPP Connection
    _result = TP_DNSResolve((uchar*)hostname, (uchar*)hostIP, (uint16)MAX_HOST_IP);
    if (_result != NET_OK) return _result;

    socket = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
    if (socket < 0) return socket;
    
    _result = TP_SockAddrSet(&dest_addr, (uchar*)hostIP, port);
    if (_result < 0) {
        TP_NetCloseSocket(socket);
        return _result;
    }

    _result = TP_NetConnect(socket, &dest_addr, 0);
    if (_result < 0) {
        TP_NetCloseSocket(socket);
        return _result;
    }

    if (token == NULL) {
        snprintf(http_request, TP_TCPIP_TCP_MAX_SEND_SIZE, 
            "POST %s HTTP/1.1\r\nHost: %s%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n%s\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            "application/json",
            body == PNULL ? 0 : strlen(body),
            body
        );
    } else {
         snprintf(http_request, TP_TCPIP_TCP_MAX_SEND_SIZE, 
            "PUT %s HTTP/1.1\r\nHost: %s%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Token %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n%s\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            "application/json",
            body == PNULL ? 0 : strlen(body),
            token,
            body
        );
    }


    total_size_len_request = strlen(http_request);
    total_already_sent = 0;

    // Send the HTTP request
    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);
    while (total_already_sent < total_size_len_request) {
        _result = TP_NetSend(socket, (const uint8*)http_request + total_already_sent, (uint16)total_size_len_request - total_already_sent, 0);
        TP_DbgSerialPrn("\nNET SEND:%d %d\r\n", _result, total_size_len_request);
        
        if (_result < 0) {
            TP_NetCloseSocket(socket);
            TP_DbgSerialPrn("\nError from NET.\r\n");
            return _result;
        }
        total_already_sent += _result;
    }
   
    _result = -1;
    size = 0;

    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);
    memset(data, 0, sizeof(data));
    do {
        _result = TP_NetRecv(socket, (uint8*)data, CHUNK_SIZE - 1, 0);
        // data[_result] = '\0'; // Null-terminate the received data
        if (_result > 0) {
            if (TP_ReAllocMemory(size + _result + 1, (void**)&response) == FALSE) {
                return -1;
            }
            memcpy(&(response[size]), data, _result);
            size += _result;
            response[size] = '\0'; // Null-terminate the response string
        } else if (_result == 0) {
            break; // No more data available
        } else {
            if (_result == -54)
                _result = 0;
            break;
        }
    } while (_result > 0);

    TP_DbgSerialPrn("\n\nResult = (%d)(%d) Res= %d\n", _result, strlen(response), size);

    if (_result != 0) {
        if (response != PNULL) {
            TP_FreeMemory(&response);
            response = PNULL;
        }
        return _result;
    }

    if (sscanf(response, "HTTP/1.%*[01] %d", status_code) != 1) {
        TP_DbgSerialPrn("Invalid status line format\n");
        TP_NetCloseSocket(socket);
         if (response != PNULL) {
            TP_FreeMemory(&response);
            response = PNULL;
        }
        return -2;
    }

    body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        TP_DbgSerialPrn("Invalid HTTP response: No body found\n");
        if (response != PNULL) {
            TP_FreeMemory(&response);
            response = PNULL;
        }
        return -3;
    }

    // Print the content/body if it exists
    size = strlen(body_start);
    if (size > 4) {
        TP_DbgSerialPrn("Body Content found(%d) = %s\n", size - 4,  body_start + 4);
		
		if (TP_ReAllocMemory(size, buffer) == FALSE) {
            if (response != NULL) {
                TP_FreeMemory((void**)&response);
                response = NULL;
            }
			return -6;
        }
		memcpy(*buffer, body_start + 4, size - 3);
    } else {
        TP_DbgSerialPrn("No body content found in the response.\n");
    }

    // Close the socket
    TP_NetCloseSocket(socket);

    // Free stuff
    if (response != NULL) {
        TP_FreeMemory((void**)&response);
        response = NULL;
    }
    return 0; // Success
}


uint8 waitforKey()
{
    TP_Kbflush();
    while(1)
    {
        if(TP_Kbhit())
        {
            return TP_GetKey();
        }
    }
}


int8 waitforKeyMs(int32 ms)
{
    TP_Kbflush();
    TP_TimerSet(6, ms);
    while(TRUE)
    {
        if(TP_Kbhit())
        {
            return TP_GetKey();
        }

        if (TP_TimerCheck(6) == 0) {
            TP_Kbflush();
            return TIMEOUT_KEY;
        }
    }
}



static int GridTirageMenu(const LinkedList *menuItems, int select, int *s) {
    uint8 fontSizeFont = 0;
    uint8 running = TRUE;
    int8 istart, i, key = 0, to_ret;
    int myi = 0, total = 0, count = 0;
    const uint8 MAX_LINES = 5;
    TP_DateTime dateTime;
    Node *current = NULL;
    BouleItem *item = NULL;


    TP_ScrCls();
    TP_Kbflush();
    TP_BanIncomingCall(TRUE);
    TP_ScrFontSet(ASCII);
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(1, 2);
    TP_ScrFontGet(&fontSizeFont);
    TP_StopRing();

    while (running)
    {
        LCD_Clear();
        TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, 12);
        TP_ScrDrawRect(2, 1, 126, 11);
        TP_ScrGotoxyEx(4, 3);
        TP_LcdPrintf("%-6s => %3s", "Boul", "Pri");
        TP_SetDisplayArea(2, 14, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
        TP_ScrClsDisplayArea();
        TP_LcdFreeze();
        
        current_y = 14;
		istart = (select / MAX_LINES) *  MAX_LINES; 
        myi = 0;
        total = 0;
    
        current = menuItems->head;
        while (current != NULL) {
            TP_DbgSerialPrn("Cal TOTAL");

            total += atoi(((BouleItem*)current->data)->pri);
            current = current->next;
        }

        for (i = 0;  i < MAX_LINES; i++)
        {
            count = Length(menuItems);
            if (istart + i < count)
            {
                TP_ScrGotoxyEx(3, current_y);
                item = (BouleItem*)Get(menuItems, istart + i);
                if (istart + i == select)
                {
                    TP_ScrAttrSet(1);
                    if ((strcmp(item->lotto, "L4") == 0) || (strcmp(item->lotto, "L5") == 0) ) 
                        TP_LcdPrintf("%s:%s => %3d => %s", item->boul, item->option, atoi(item->pri), item->lotto);
                    else
                        TP_LcdPrintf("%-6s=> %3d => %s", item->boul, atoi(item->pri), item->lotto);	

                    TP_ScrAttrSet(0);
                }
                else
                    if ((strcmp(item->lotto, "L4") == 0) || (strcmp(item->lotto, "L5") == 0)) 
                        TP_LcdPrintf("%s:%s => %3d => %s", item->boul, item->option, atoi(item->pri), item->lotto);
                    else
                        TP_LcdPrintf("%-6s=> %3d => %s", item->boul, atoi(item->pri), item->lotto);	
                current_y += 8;				
            }
            else
                break;
        }
        
        TP_GetDateTime(&dateTime);
        TP_ScrGotoxyEx(2, 63 - 7);
        TP_LcdPrintf("H%02d:%02d:%02d, TT:%d HT", dateTime.Time.Hours, dateTime.Time.Minutes, dateTime.Time.Seconds, total);	

		TP_ScrUpdate();
LOOP:
        key = waitforKeyMs(1000);

        switch(key)
        {
            case TIMEOUT_KEY:
                break;
            case TP_KEY_UP:
                select--;
                if (select < 0) select = count - 1;
                break;
            case TP_KEY_DOWN:
                select++;
                if (select >= count) select = 0;
                break;
            case TP_KEY_OK:
                *s = select;
                select = 10;
                running = FALSE;
                break;
            case TP_KEY_FUNC:
                *s = select;
				select = 11;
                running = FALSE;
                break;
            case TP_KEY_CLEAR:
                *s = select;
                select = 12;
                running = FALSE;
                break;
            case TP_KEY_CANCEL:
                running = FALSE;
                select = -1;
                break;
            default:
                goto LOOP;
        }
    }
    to_ret = select;

    return to_ret;
}


static int deleteInfoTirageByIndex(InfoTirage* list, int index, int size) 
{
	int i = 0;
    if (index < 0 || index >= size) {
        // printf("Invalid index.\n");
        return -1;
    }
    i = (int)index;
    TP_FreeMemory((void**)&(list[i].name));
    while (i < size - 1) {
        list[i] = list[i + 1];
        i++;
    }
    return size - 1;// Decrement the size to reflect the deleted element
}


static int hasInfoTirageByName(InfoTirage *array, int size, const char *name)
{
    int i = 0;
    while (i < size)
    {
        if (strcmp(array[i].name, name) == 0 )
            return i;
        i++;
    }
    return -1;
}


int8 LCD_Menu_InfoTirage(const InfoTirage *menuItems, uint8 count, int8 select, int *id, const InfoTirage *selectedTirage, uint16 sizeSelected)
{
    uint32 charSpace = 0, lineSpace = 0;
    uint8 fontSizeFont = 0;
    uint8 running = TRUE;
    int8 istart, i, key = 0, to_ret;

    TP_ScrCls();
    TP_Kbflush();
    TP_BanIncomingCall(TRUE);
    TP_ScrFontSet(ASCII);
    TP_ScrAttrSet(0);
    // TP_ScrSpaceSet(0, 2);
    TP_ScrSpaceSet(1, 2);
    TP_ScrSpaceGet(&charSpace,&lineSpace);
    TP_ScrFontGet(&fontSizeFont);
    TP_StopRing();
    
    if (count < 0) return -1;

    while (running == TRUE)
    {
        int beenSelect = -1;
        LCD_Clear();
        Display_Header("Chwazi Tiraj");
        TP_SetDisplayArea(2, 14, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
        TP_ScrClsDisplayArea();
        TP_LcdFreeze();
        
        current_y = 14;
		istart = (select / 5) *  5; 
        for (i = 0;  i < 5; i++)
        {
            if (istart + i < count)
            {
                beenSelect = hasInfoTirageByName(selectedTirage, sizeSelected, menuItems[istart + i].name);
                if (beenSelect >= 0)
                    TP_ScrAttrSet(1);
                else
                    TP_ScrAttrSet(0);

                TP_ScrGotoxyEx(3, current_y);
                if (istart + i == select)
                {
                    TP_ScrAttrSet(1);
                    TP_LcdPrintf("%d.%s", istart + i + 1, (char*)menuItems[istart + i].name);	
                    TP_ScrAttrSet(0);
				    *id = istart + i;
                }
                else
                    TP_LcdPrintf("%d.%s", istart + i + 1, (char*)menuItems[istart + i].name);	
                current_y += 8;		
            }
            else
                break;
        }
		TP_ScrUpdate();
LOOP:
        TP_ScrSetIcon(ICON_UP, CLOSEICON);
        TP_ScrSetIcon(ICON_DOWN, CLOSEICON);

        if (TP_Kbhit() == 0xFF) {
            key = TP_GetKey();
            TP_DbgSerialPrn("\r\nKEY:%d\r\n", key);
            
            switch(key)
            {
                case TP_KEY_UP:
                    select--;
                    if (select < 0) select = count - 1;
                    break;
                case TP_KEY_DOWN:
                    select++;
                    if (select >= count) select = 0;
                    break;
                case TP_KEY_OK:
                    running = FALSE;
                    select = 125;
                    break;
                case TP_KEY_FUNC:
                    running = FALSE;
                    select = 100;
                    break;
                case TP_KEY_CANCEL:
                    running = FALSE;
                    select = -1;
                    break;
                default:
                    goto LOOP;
            } 
        } else {
            goto LOOP;
        }
    }
    return select;
}


int LCD_MenuTirage(const char *title, const Tirage *menuItems, unsigned int count, int select, int *id)
{	
	uint8 i = 0, key = 0;
	unsigned int istart = 0;
    Boolean running = TRUE;
    const uint8 SIZE_MAX = 5;
	
    TP_ScrCls();
    TP_Kbflush();
    TP_BanIncomingCall(TRUE);

    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    // TP_ScrSpaceSet(0, 2);
    TP_ScrSpaceSet(1, 2);

	if (select < 0)
		select = 0;
	if (count <= 0)
		select = -1;
	else
	{
		running = TRUE;
		while (running == TRUE)
		{
			LCD_Clear();
			Display_Header(title);
            TP_SetDisplayArea(2, 14, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
            TP_ScrClsDisplayArea();
            TP_LcdFreeze();
        
            current_y = 14;
			istart = (select / SIZE_MAX) * SIZE_MAX; 

			for (i = 0;  i < SIZE_MAX; i++)
			{
				if (istart + i < count)
				{
                    TP_ScrGotoxyEx(3, current_y);
                    if (istart + i == select)
                    {
                        TP_ScrAttrSet(1);
                        TP_LcdPrintf("%d.%s", istart + i + 1, menuItems[istart + i].id);
                        TP_ScrAttrSet(0);
                        *id = istart + i;			
                    }
                    else
                        TP_LcdPrintf("%d.%s", istart + i + 1, menuItems[istart + i].id);	
                    current_y += 8;
				}
				else
					break;
			}
			TP_ScrUpdate();
	LOOP:
			key = waitforKeyMs(3 * 1000);
            TP_DbgSerialPrn("\r\nKEY=%d\r\n", key);

			switch(key)
			{
				case TP_KEY_UP:
					select--;
					if (select < 0) select = count - 1;
					break;
				case TP_KEY_DOWN:
					select++;
					if (select >= count) select = 0;
					break;
                case TP_KEY_FUNC:
					running = FALSE;
                    break;
				case TP_KEY_CANCEL:
					select = -1;
					running = FALSE;
					break;
				case TP_KEY_OK:
					running = FALSE;
					break;
				default:
					goto LOOP;
			}
		}
	}
	LCD_Clear();
	return select;
}


// OK
int getLottoType(InfoTirage **selectedTirage, int *sizeTirage, char *name, int *idName)
{
	InfoTirage *menu = NULL;
	char *bufferTirages = NULL;
	cJSON *json = NULL;
	int32 size = 0, toret = 0, selected = 0;
	int32 id = 0, idToDelete = 0;
		
    read_from_file(INFO_TIRAGE_FILE, &bufferTirages);
    TP_DbgSerialPrn("Tirages DATA: %s", bufferTirages);
	json = cJSON_Parse(bufferTirages);
	
	if (json != NULL)
	{
		if (cJSON_IsArray(json))
		{
			cJSON *element;
			cJSON_ArrayForEach(element, json) 
			{
				cJSON *id = cJSON_GetObjectItemCaseSensitive(element, "id");
				cJSON *name = cJSON_GetObjectItemCaseSensitive(element, "name");
				addItem(&menu, &size, id->valueint, name->valuestring);
			}
			toret = 0;
		}
	} else {
		toret = -1;
        handleJSONNULL();
    }

	if (bufferTirages != NULL) {
		TP_FreeMemory((void**)&bufferTirages);
		bufferTirages = NULL;
	}

    if (json != NULL) {
		cJSON_Delete(json);
        json = NULL;
    }

	if (size == 0)
	{
		handleEmptyList();
	}
	selected = 0;
	while (selected >= 0 && size > 0)
	{
		selected = LCD_Menu_InfoTirage(menu, size, selected, &id, *selectedTirage, *sizeTirage);

		if (selected < 0) {
			toret = -1;
        }
		else if (selected == 100)
		{
			idToDelete = hasInfoTirageByName(*selectedTirage, *sizeTirage, menu[id].name);
			if (idToDelete >= 0)
			{
				*sizeTirage = deleteInfoTirageByIndex(*selectedTirage, idToDelete, *sizeTirage);
			} else
				addItem(selectedTirage, sizeTirage, menu[id].id, menu[id].name);
			selected = id;
		} else if (selected == 125) {
			toret = 0;
			strcpy(name, menu[id].name);
			*idName = menu[id].id;
			selected = -1;
		}
	}
    // Free stuff 
	freeItems(menu, size);
	return toret;
}


// OK
void postFiches(const char *buffBoules) {
    cJSON *json = NULL;
    char name[96], boul[6], pri[7], option[2] = "1", pwent[2];
	int id = 0, tip = 0;
	InfoTirage *tirages = NULL;
	int16 sizeTirage = 0, sselected = 0;
	List *list = NULL;
    int selected = 0, selected2 = 0;
    Boolean stopped = FALSE, running = FALSE;
    uint8 key_ret, sWiNon = 1;

    Boolean fromRePost = FALSE;

    LinkedList *array = Array();
    int totest = 0;

    const char menu[][MAX_CHAR_ITEMS] = {
		"1. Enprime",
		"2. Boul Pe",
		"3. L3 Oto",
		"4. Reve",
		"5. Maryaj Oto",
		"6. L4 Oto",
		"7. Pwent"
	};

    Boolean isMA = FALSE;
    Boolean isOK = FALSE;

	memset(name, 0x00, sizeof(name));
	memset(pwent, 0x00, sizeof(pwent));
	memset(boul, 0x00, sizeof(boul));
	memset(pri, 0x00, sizeof(pri));

    selected = 0;

    tip = getLottoType(&tirages, &sizeTirage, name, &id);

    list = createList();
	memset(pri, 0x00, sizeof(pri));


    if (tip == -1) 
        selected = -1;
    else 
        if (sizeTirage == 0)
            addItem(&tirages, &sizeTirage, id, name);


    if (buffBoules != NULL)
	{
		cJSON *tempJson = cJSON_Parse(buffBoules);
		if (tempJson != NULL)
		{
			if (cJSON_IsArray(tempJson))
			{
				cJSON *element;
                fromRePost = TRUE;
				// Iterate over the array elements
				cJSON_ArrayForEach(element, tempJson) 
				{
					cJSON *lotto = cJSON_GetObjectItemCaseSensitive(element, "lotto");
					cJSON *boule = cJSON_GetObjectItemCaseSensitive(element, "boule");
					cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
					cJSON *option = cJSON_GetObjectItemCaseSensitive(element, "option");
					
                    if (strcmp(lotto->valuestring, "MA") == 0 && (uint32)(montant->valuedouble) == 0)
                        "// do nothing";
                    else {
                        BouleItem *_new = NULL;
                        TP_AllocMemory(sizeof(BouleItem), (void**)&_new);
                        if (_new != NULL) {
                            sprintf(_new->pri, "%.2f", montant->valuedouble);
                            strcpy(_new->boul, boule->valuestring);
                            strcpy(_new->lotto, lotto->valuestring);
                            strcpy(_new->option, option->valuestring);
                            AddElem(array, _new);
                        }
                        
                        // addElement(list, _new); // add boule in list boules
                    }
				}
			}
		}

        if (tempJson != NULL) {
            cJSON_Delete(tempJson);
            tempJson = NULL;
        }
	}

	while (selected >= 0)
	{
        BouleItem *newItem = NULL;
		selected = GridTirageMenu(array, selected, &sselected);
		switch(selected) 
		{
            case 10:
                isMA = FALSE;
                isOK = TRUE;
                memset(boul, 0x00, sizeof(boul));

                LCD_Clear();
                Display_Header("Mete Boul");
                TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);

                TP_ScrGotoxyEx(3, 14);
                TP_LcdPrintf("Boul:");
                TP_SetDisplayArea(6, 23, MAX_SCREEN_WIDTH - 1, 31);

                TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                TP_ShowInputNum(0, 0, 0);
                TP_SetInputMode(INPUTMODE_LOWCASE);

                key_ret = TP_GetString((char*)boul, 0x80|0x04|0x10, 2, 5);
                if (key_ret == 0xFF) 
                    break;

                TP_SetDisplayArea(1, 31, MAX_SCREEN_WIDTH / 2, 40);
                TP_ScrGotoxyEx(3, 31);
                TP_LcdPrintf("Pri: ");
                TP_SetDisplayArea(27, 32, MAX_SCREEN_WIDTH - 1, 49);
                TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                TP_ShowInputNum(0, 0, 0);
                TP_SetInputMode(INPUTMODE_MAX);

                key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                if (key_ret == 0xFF) 
                    break;
  
                TP_AllocMemory(sizeof(BouleItem), (void**)&newItem);

                if (newItem != NULL){
                    newItem->id = 1;
                    strcpy(newItem->boul, boul);
                    strcpy(newItem->pri, pri);
                    strcpy(newItem->option, "1");
                }

                setOptionBouleItem(newItem);

				if (strlen(newItem->boul) == 4 && isOK == TRUE)
				{
                    uint8 kkey = 0, sop = 1;
                    TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
                    TP_ScrClsDisplayArea();

                    TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);
                    TP_ScrGotoxyEx(3, 15);

                    TP_LcdPrintf("%s X %s", newItem->boul, newItem->pri);

                    running = TRUE;
                    while (running)
                    {
                        TP_SetDisplayArea(6, 23, MAX_SCREEN_WIDTH - 1, 31);
                        TP_ScrClsDisplayArea();
                        TP_ScrGotoxyEx(2, 27);
                        TP_LcdPrintf("Maryaj? ");

                        TP_ScrGotoxyEx(43, 27);
                        if (sWiNon == 1) {
                            TP_ScrAttrSet(1);
                            TP_LcdPrintf("WI");
                        } else {
                            TP_ScrAttrSet(0);
                            TP_LcdPrintf("WI");
                        }
                        TP_ScrAttrSet(0);
                        TP_ScrGotoxyEx(57, 27);
                         if (sWiNon == 2) {
                            TP_ScrAttrSet(1);
                            TP_LcdPrintf("NON");
                        } else {
                            TP_ScrAttrSet(0);
                            TP_LcdPrintf("NON");
                        }
                        TP_ScrAttrSet(0);

                        kkey = waitforKey();

                        switch (kkey)
                        {
                            case TP_KEY_UP:
                                sWiNon = 1;
                                break;
                            case TP_KEY_DOWN:
                                sWiNon = 2;
                                break;
                            case TP_KEY_OK:
                                if (sWiNon == 1)
								{
									isMA = TRUE;
									sprintf(newItem->lotto, "%s", "MA");
								}
								else
								{
									isMA = FALSE;
									sprintf(newItem->lotto, "%s", "L4");
								}
                                isOK = TRUE;
                                running = FALSE;
                                break;
                            
                            case TP_KEY_CANCEL:
                                isOK = FALSE;
                                running = FALSE;
                                break;
                            default:
                                break;
                        }
                    }
                }

				if (strlen(newItem->boul) >= 4 && isOK == TRUE && isMA == FALSE) {
                    do
                    {
                        TP_SetDisplayArea(1, 32, MAX_SCREEN_WIDTH / 2, 48);
                        TP_ScrClsDisplayArea();                        

                        TP_ScrGotoxyEx(3, 39);
                        TP_LcdPrintf("Opsyon: ");
                        TP_SetDisplayArea(48, 39, MAX_SCREEN_WIDTH - 1, 49);
                        TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                        TP_ShowInputNum(0, 0, 0);
                        TP_SetInputMode(INPUTMODE_MAX);

                        key_ret = TP_GetString((char*)option, 0x80|0x04|0x10, 1, 1);
                        if (key_ret == 0xFF) {
                            break;
                            isOK = FALSE;
                        } else {
                            sprintf(newItem->option, option);
                            isOK = TRUE;                
                        }
                    } while (atoi(option) < 1 || atoi(option) > 3);
                }

                if (isOK == TRUE) {
                    AddElem(array, newItem);
                    selected = Length(array) - 1;
                }
                break;
            case 11:
                selected2 = 0;
                stopped = FALSE;
                while (selected2 >= 0)
				{
					selected2 = LCD_Menu("Lot Opsyon", menu, sizeof(menu) / MAX_CHAR_ITEMS, selected2);
					switch(selected2) 
					{
                        case 0:
                        if (make_post_fiches(array, id, tirages, sizeTirage) == 0)
							{
								// canSelectBoulePaire = 1;
								// canSelectBouleTriple = 1;
								FreeArray(array, freeMyData);
								array = Array();
								selected2 = -1;
								if (buffBoules != NULL)
								{
									selected2 = -1;
                                    // TODO, return outter
									// stop = 0;
								}

                                if (fromRePost) {
                                    selected2 = -1;
                                    selected = -1; // Stop to goto list tickets
                                }
							}
                            break;
                        case 1:
                            LCD_Clear();
                            Display_Header("Mete Pri");
                            TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);

                            TP_ScrGotoxyEx(3, 15);
                            TP_LcdPrintf("Pri: ");
                            TP_SetDisplayArea(27, 15, MAX_SCREEN_WIDTH - 1, 31);

                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_LOWCASE);
                            key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                            if (key_ret == 0xFF) 
                                break;
							addBoulePaire(array, pri);
                            selected2 = -1;
                            break;
                        case 2:
                            LCD_Clear();
                            Display_Header("Mete Pri");
                            TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);

                            TP_ScrGotoxyEx(3, 15);
                            TP_LcdPrintf("Pri: ");
                            TP_SetDisplayArea(27, 15, MAX_SCREEN_WIDTH - 1, 31);

                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_LOWCASE);
                            key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                            if (key_ret == 0xFF) 
                                break;
							addBouleTriple(array, pri);
                            selected2 = -1;
                            break;
                        case 3:
                            LCD_Clear();
                            Display_Header("Mete Pri");
                            TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);

                            TP_ScrGotoxyEx(3, 15);
                            TP_LcdPrintf("Pri: ");
                            TP_SetDisplayArea(27, 15, MAX_SCREEN_WIDTH - 1, 31);

                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_LOWCASE);
                            key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                            if (key_ret == 0xFF) 
                                break;
							addBouleRevers(array, pri);
                            selected2 = -1;
                            break;
                        case 4:
                            LCD_Clear();
                            Display_Header("Mete Pri");
                            TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);

                            TP_ScrGotoxyEx(3, 15);
                            TP_LcdPrintf("Pri: ");
                            TP_SetDisplayArea(27, 15, MAX_SCREEN_WIDTH - 1, 31);

                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_LOWCASE);
                            key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                            if (key_ret == 0xFF) 
                                break;
							addBouleL4(array, pri, "MA");
                            selected2 = -1;
                            break;
                        case 5:
                            LCD_Clear();
                            Display_Header("Mete Pri");
                            TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);

                            TP_ScrGotoxyEx(3, 15);
                            TP_LcdPrintf("Pri: ");
                            TP_SetDisplayArea(27, 15, MAX_SCREEN_WIDTH - 1, 31);

                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_LOWCASE);
                            key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                            if (key_ret == 0xFF) 
                                break;
							addBouleL4(array, pri, "L4");
                            selected2 = -1;
                            break;
                        case 6:
                            LCD_Clear();
                            Display_Header("Pwent");
                            TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);
                            memset(pwent, 0x00, sizeof(pwent));

                            TP_ScrGotoxyEx(3, 15);
                            TP_LcdPrintf("Pwent:");
                            TP_SetDisplayArea(6, 23, MAX_SCREEN_WIDTH - 1, 31);

                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_LOWCASE);
                            key_ret = TP_GetString((char*)pwent, 0x80|0x04|0x10, 1, 1);
                            if (key_ret == 0xFF) 
                                break;
                            TP_SetDisplayArea(1, 31, MAX_SCREEN_WIDTH / 2, 40);

                            TP_ScrGotoxyEx(3, 32);
                            TP_LcdPrintf("Pri:  ");
                            TP_SetDisplayArea(27, 32, MAX_SCREEN_WIDTH - 1, 49);
                            TP_SetInputModeControl(TP_KEY_OK, TP_KEY_CANCEL, KEY_DEMO_POUND);
                            TP_ShowInputNum(0, 0, 0);
                            TP_SetInputMode(INPUTMODE_MAX);
                            key_ret = TP_GetString((char*)pri, 0x80|0x04|0x10, 1, 7);
                            if (key_ret == 0xFF) 
                                break;

							addPwent(array, pri, atoi(pwent));
                            selected2 = -1;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case 12:
                FreeItem(array, Get(array, sselected), freeMyData);
				selected = Length(array);
                break;
            default:
                break;
        }
    }
    
    // Free Stuff
    FreeArray(array, freeMyData);
    // destroyList(list);
    freeItems(tirages, sizeTirage);
	sizeTirage = 0;
    return;
}


// OK
static void getFicheById(void) {
    uint8 keycode = 0;
    char temp[30], path[60];
    char *buffer = NULL, *bufferToken = NULL;
    int16 status_code = 0, size = 0, selected2 = 0;
    cJSON *json = NULL;
    Tirage *items = NULL;
    int32 ret = 0;

    LCD_Clear();
    Display_Header("Fich Pa ID");
    TP_SetDisplayArea(1, 14, MAX_SCREEN_WIDTH - 1, 22);
    TP_ScrGotoxyEx(3, 15);
    TP_LcdPrintf("ID: ");

    TP_SetDisplayArea(6, 23, 124, 45);
    TP_ScrGotoxyEx(6, 23);
    TP_SetInputModeControl(TP_KEY_OK, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
    TP_ShowInputNum(0, 1, 1);
    memset(path, 0x00, sizeof(path));
    memset(temp, 0x00, sizeof(temp));
    strcpy(temp, "MGN-");
    keycode = TP_GetHzString(temp, 0, sizeof(temp) - 1);
    if (keycode == 0xFF) return;
    sprintf(path, "/api/games/find-fiche?barcode=%s", temp);

    read_from_file(ACCESS_TOKEN_FILE, &bufferToken);
    if ((ret = make_http_GET(SUBDOMAIN_URL, BASE_URL, path, bufferToken, &status_code, &buffer)) >= 0)
    {
        if (status_code >= 200 && status_code <= 299) {
            json = cJSON_Parse(buffer);		
			if (json != NULL)
			{
                cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "ref_code");
                cJSON *_id = cJSON_GetObjectItemCaseSensitive(json, "id");
                cJSON *created_on = cJSON_GetObjectItemCaseSensitive(json, "created_on");
                cJSON *boules = cJSON_GetObjectItemCaseSensitive(json, "boules");
                cJSON *tirage_name = cJSON_GetObjectItemCaseSensitive(json, "tirage_name");
                cJSON *montant = cJSON_GetObjectItemCaseSensitive(json, "montant");
                cJSON *is_delete = cJSON_GetObjectItemCaseSensitive(json, "is_delete");
                if (!cJSON_IsTrue(is_delete))
                {
                    char *bStr = cJSON_Print(boules);
                    addTirageItem(&items, &size, id->valuestring, bStr, tirage_name->valuestring, montant->valuestring, _id->valueint, created_on->valuestring);
                    
                    if (json != NULL) {
                        cJSON_Delete(json);
                        json = NULL;
                    }
                    if (buffer != NULL) {
                        TP_FreeMemory((void**)&buffer);
                        buffer = NULL;
                    }
                    selected2 = 0;
                    while (selected2 >= 0) {
                        selected2 = LCD_Menu(items[0].id, menu, sizeof(menu) / MAX_CHAR_ITEMS, selected2);
                        switch (selected2)
                        {
                            case 0:
                                reprint_fiche(items[0]);
                                break;
                            case 1:
                                postFiches(items[0].boules);
                                break;
                            case 2:
                                if (yesNoModal("Siprime?") == TRUE) {
                                    char *json_string = NULL;
                                    cJSON *data = cJSON_CreateObject();

                                    cJSON_AddNumberToObject(data, "ficheId", items[0]._id);
                                    cJSON_AddStringToObject(data, "delete", "True");
                                    json_string = cJSON_Print(data);
                                    TP_DbgSerialPrn("\r\nSiprime data =%s\r\n", json_string);


                                    if (make_http_PUT(SUBDOMAIN_URL, BASE_URL, "/api/games/fiche", json_string, bufferToken, &status_code, &buffer) >= 0) {
                                        if (status_code >= 200 && status_code <= 299) {
                                            selected2 = -1;
                                        } else {
                                            // TODO
                                        }
                                    } else {
                                        // TODO
                                    }

                                    if (json_string != NULL) {
                                        TP_FreeMemory((void**)&json_string);
                                        json_string = NULL;
                                    }

                                    if (data != NULL) {
                                        cJSON_Delete(data);
                                        data = NULL;
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (json != NULL) {
                    cJSON_Delete(json);
                    json = NULL;
                }
			} else
			{
				handleJSONNULL();
			}
        } else {
           Handle404Error(status_code, buffer);
        }
    } else {
        handleSocketError(ret);
    }

    if (json != NULL) {
        cJSON_Delete(json);
        json = NULL;
    }

    if (buffer != NULL) {
        TP_FreeMemory((void**)&buffer);
        buffer = NULL;
    }

    if (bufferToken != NULL) {
        TP_FreeMemory((void**)&bufferToken);
        bufferToken = NULL;
    }
    freeTirageItems(items, size);
    size = 0;
    return;
}

// AL OK
static void make_get_fiches(char *start_date, char *end_date) {
    uint16 status_code = 0, size = 0;
    char *buffer = NULL, *bufferToken = NULL;
    char path[96];
    cJSON *json = NULL;
    Tirage *items = NULL;
    int selected = 0, id = 0, selected2 = 0;
    int32 ret = 0;

    memset(path, 0x00, sizeof(path));
    read_from_file(ACCESS_TOKEN_FILE, &bufferToken);

    sprintf(path, "/api/games/v2/fiche?start_date=%s&end_date=%s", start_date, end_date);

    if ((ret = make_http_GET(SUBDOMAIN_URL, BASE_URL, path, bufferToken, &status_code, &buffer)) >= 0)
    {
        if (status_code >= 200 && status_code <= 299) {
            json = cJSON_Parse(buffer);		
			if (json != NULL)
			{
                if (cJSON_IsArray(json))
				{
					cJSON *element;
					cJSON_ArrayForEach(element, json) 
					{
						cJSON *id = cJSON_GetObjectItemCaseSensitive(element, "ref_code");
						cJSON *_id = cJSON_GetObjectItemCaseSensitive(element, "id");
						cJSON *created_on = cJSON_GetObjectItemCaseSensitive(element, "created_on");
						cJSON *boules = cJSON_GetObjectItemCaseSensitive(element, "boules");
						cJSON *tirage_name = cJSON_GetObjectItemCaseSensitive(element, "tirage_name");
						cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
						cJSON *is_delete = cJSON_GetObjectItemCaseSensitive(element, "is_delete");

						if (!cJSON_IsTrue(is_delete))
						{
							char *bStr = cJSON_Print(boules);
							addTirageItem(&items, &size, id->valuestring, bStr, tirage_name->valuestring, montant->valuestring, _id->valueint, created_on->valuestring);
						}
					}
				}
                cJSON_Delete(json);
                json = NULL;
			} else
			{
				handleJSONNULL();
			}
        } else {
            // TODO
        }    
    } else {
        handleSocketError(ret);
    }

    if (json != NULL) {
        cJSON_Delete(json);
        json = NULL;
    }

    if (buffer != NULL) {
        TP_FreeMemory((void**)&buffer);
        buffer = NULL;
    }

    // Display Menu
    if (size == 0)
    {
       handleEmptyList();
    } else {
        while (selected >= 0 && size > 0)
        {
            // size2 = 0;
            selected = LCD_MenuTirage("Lis Fich", items, size, selected, &id);
            selected2 = 0;
            
            if (selected >= 0)
            {
                // Lot Opsyon
                while (selected2 >= 0)
                {
                    selected2 = LCD_Menu(items[id].id, menu, sizeof(menu) / MAX_CHAR_ITEMS, selected2);
                    switch (selected2)
                    {
                    case 0:
                        reprint_fiche(items[selected]);
                        break;
                    case 1:
                        postFiches(items[selected].boules);
                        selected2 = -1;
                        break;
                    case 2:
                        if (yesNoModal("Siprime?") == TRUE) {
                            char *json_string = NULL;
                            cJSON *data = cJSON_CreateObject();

                            cJSON_AddNumberToObject(data, "ficheId", items[id]._id);
                            cJSON_AddStringToObject(data, "delete", "True");
                            json_string = cJSON_Print(data);

                            if ((ret = make_http_PUT(SUBDOMAIN_URL, BASE_URL, "/api/games/fiche", json_string, bufferToken, &status_code, &buffer)) >= 0) {
                                if (status_code >= 200 && status_code <= 299) {
									size = deleteTirageByIndex(items, id, size);
                                    if (size > 0) selected = size;
                                    selected2 = -1;
                                } else {
                                    Handle404Error(status_code, "Paka Efase Fich Sa!");
                                }
                            } else {
                                handleSocketError(ret);
                            }

                            if (buffer != NULL) {
                                TP_FreeMemory((void**)&buffer);
                                buffer = NULL;
                            }

                            if (json_string != NULL) {
                                TP_FreeMemory((void**)&json_string);
                                json_string = NULL;
                            }

                            if (data != NULL) {
                                cJSON_Delete(data);
                                data = NULL;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    if (bufferToken != NULL) {
        TP_FreeMemory((void**)&bufferToken);
        bufferToken = NULL;
    }

    freeTirageItems(items, size);

    return;
}

// OK
void getFiches(void)
{
    TP_DateTime dateTime;
    int8 selected = 0;
	char start_date[11];
	
	const char menu[][MAX_CHAR_ITEMS] = {
		"1. Pa ID",
		"2. Pou 1 Jou"
	};
    
	memset(start_date, 0x00, sizeof(start_date));
    TP_GetDateTime(&dateTime);

    while (selected >= 0)
	{
		selected = LCD_Menu("Cheche Fich", menu, sizeof(menu) / MAX_CHAR_ITEMS, selected);
		switch (selected)
		{
			case 0:
				getFicheById();
				break;
			case 1:
				sprintf(start_date, "%4d-%02d-%02d", dateTime.Date.Year, dateTime.Date.Month, dateTime.Date.Day);
				make_get_fiches(start_date, start_date);
				break;
			default:
				break;
		}
	}
    return;
}


// OK
int make_post_fiches(const LinkedList *list, int id_tirage, const InfoTirage *tirages, int sizeTirage)
{
	uint16 status_code = 0;
	char *buffer = NULL;
	char *bufferToken = NULL;	
    char *jsonStr = NULL;
	cJSON *root = NULL;
	cJSON *arrayBoul = NULL;
	cJSON *ficheData = NULL;
	int state = 0, i = 0, x = 0;
    int32 ret = 0;

    Node *current = NULL;
	read_from_file(ACCESS_TOKEN_FILE, &bufferToken);

	root = cJSON_CreateObject();
	arrayBoul = cJSON_CreateArray();
	ficheData = cJSON_CreateArray();
    
    current = list->head;
	i = 0;
    while (current != NULL) {
        cJSON *tirage = cJSON_CreateObject();
		cJSON_AddStringToObject(tirage, "boule", ((BouleItem*)current->data)->boul);
		cJSON_AddStringToObject(tirage, "option", ((BouleItem*)current->data)->option);
		cJSON_AddStringToObject(tirage, "lotto", ((BouleItem*)current->data)->lotto);
		cJSON_AddNumberToObject(tirage, "montant", atoi(((BouleItem*)current->data)->pri));
		cJSON_AddNumberToObject(tirage, "id", i + 1);
		cJSON_AddItemToArray(arrayBoul, tirage);
        current = current->next;
		i++;
    }

	if (sizeTirage == 0)
	{
		cJSON_AddNumberToObject(root, "tirage", id_tirage);
		cJSON_AddItemToObject(root, "bouleList", arrayBoul);
	} else 
	{
		x = 0;
		while (x < sizeTirage)
		{
			cJSON *tir = cJSON_CreateObject();
			cJSON_AddNumberToObject(tir, "tirage", tirages[x].id);
            cJSON_AddItemReferenceToObject(tir, "bouleLists", arrayBoul);
			cJSON_AddItemToArray(ficheData, tir);
			x++;
		}

		cJSON_AddItemToObject(root, "ficheData", ficheData);
	}
	
	jsonStr = cJSON_Print(root);
    TP_DbgSerialPrn("JSON TO SEND: %s\n", jsonStr);

	if (sizeTirage == 0)
	{
		if ((ret = bmake_http_POST(SUBDOMAIN_URL, BASE_URL, "/api/games/fiche", jsonStr, bufferToken, &status_code, &buffer)) < 0)
		{
			handleSocketError(ret);
			state = -1;
		} else
		{
			if (status_code >= 200 && status_code <= 299) 
			{
				state = 0;
				print_fiche(buffer, 0);
			} else 
			{
				state = -1;
                Handle404Error(status_code, buffer);
			}
		}
	} else 
	{
		if ((ret = bmake_http_POST(SUBDOMAIN_URL, BASE_URL, "/api/games/v2/fiche", jsonStr, bufferToken, &status_code, &buffer)) < 0)
		{
			handleSocketError(ret);
			state = -1;
		} else
		{
			if (status_code >= 200 && status_code <= 299) 
			{
				state = 0;
				print_fiche(buffer, 1);
			} else 
			{
				state = -1;
                Handle404Error(status_code, buffer);
			}
		}
	}
	
	if (jsonStr != NULL)
	{
		TP_FreeMemory((void**)&jsonStr);
		jsonStr = NULL;
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
    if (root != NULL)
	{
		cJSON_Delete(root);
		root = NULL;
	}
	return state;
}


// OK
void getWinningFiches(void)
{	TP_DateTime dateTime1;
    TP_DateTime dateTime2;
    char path[115], str[90];
    char *bufferToken = NULL, *buffer = NULL;
    Boolean result = FALSE;
    uint16 status_code = 0;
    cJSON *root = NULL, *data = NULL, *count = NULL;
    int32 count_items = 0, index = 0, ret = 0;
    char start_date[11], end_date[11];
    uint8 key = 0;
    TP_ListBoxRect listRect = {2, 17, 125, 62};
    char *todisplay = NULL;

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

    TP_ScrCls();
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(0, 2);

    memset(str, 0x00, sizeof(str));
    memset(path, 0x00, sizeof(path));

    while (TRUE) {
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
        sprintf(path, "/api/games/fiche-win?start_date=%s&end_date=%s", start_date, end_date);

        read_from_file(ACCESS_TOKEN_FILE, &bufferToken);
        
        if ((ret = make_http_GET(SUBDOMAIN_URL, BASE_URL, path, bufferToken, &status_code, &buffer)) >= 0) {
            LCD_Clear();
            if (status_code >= 200 && status_code <= 299) {
                Display_Header("Fich Ganyan");
                root = cJSON_Parse(buffer);
                if (root != NULL)
                {
                    count = cJSON_GetObjectItemCaseSensitive(root, "count");
                    data = cJSON_GetObjectItemCaseSensitive(root, "data");
                    
                    if (buffer != NULL) {
                        TP_FreeMemory((void**)&buffer);
                        buffer = NULL;
                    }
                    count_items = (int32)(count->valueint);
                    
                    if (cJSON_IsArray(data)) {
                        index = 0;
                        while (index < count_items)
                        {
                            cJSON *element = cJSON_GetArrayItem(data, index);
                            cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
                            cJSON *ref_code = cJSON_GetObjectItemCaseSensitive(element, "ref_code");
                            cJSON *perte = cJSON_GetObjectItemCaseSensitive(element, "perte");

                            sprintf(str, "ID: %s\nMontan: %s\nKob: %s\n\n", ref_code->valuestring, montant->valuestring, perte->valuestring);

                            concatString(&todisplay, (char *)str);
                            index ++;	
                        }
                    }
                    
                    showText(&displayInfo, listRect, todisplay, print_winnings_fiche, data, count_items, start_date, end_date);
                    break;
                } 
                else 
                {
                    handleJSONNULL();
                }   
            } else {
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
                break;;
        }
    }

    if (todisplay != NULL) {
        TP_FreeMemory((void**)&todisplay);
        todisplay = NULL;
    }
    if (bufferToken != NULL) {
        TP_FreeMemory((void**)&bufferToken);
        bufferToken = NULL;
    }
    if (buffer != NULL) {
        TP_FreeMemory((void**)&buffer);
        buffer = NULL;
    }
    if (root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    return;
}


// OK
void logout(void) {
    write_to_file(INFO_TIRAGE_FILE, " ");
    write_to_file(INFO_USER_FILE, " ");
    write_to_file(ACCESS_TOKEN_FILE, " ");
    return;
}
