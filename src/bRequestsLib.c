
#include "bRequestsLib.h"


static int8 check(void)
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



static void concat_String(char **buffer, const char *format, ...) {
    size_t currLen = *buffer ? strlen(*buffer) : 0;
    size_t sizeNeeded = 0;
    va_list args;

    va_start(args, format);
    sizeNeeded = vsnprintf(NULL, 0, format, args);
    va_end(args);

    // Reallocate memory
    if (TP_ReAllocMemory((currLen + sizeNeeded + 1) * sizeof(char), (void**)buffer) == FALSE)
        return;

    va_start(args, format);
    vsnprintf(*buffer + currLen, sizeNeeded + 1, format, args);
    va_end(args);
}


static int32 send_large_data(int32 sockfd, const char *data, size_t data_length) {
    size_t total_sent = 0;
    int32 _res = 0;
    
    while (total_sent < data_length) {
        size_t remaining = data_length - total_sent;
        size_t chunk_size = remaining < TP_TCPIP_TCP_MAX_SEND_SIZE - 1 ? remaining : TP_TCPIP_TCP_MAX_SEND_SIZE - 1;
        const char *chunk = data + total_sent;

        _res  = TP_NetSend(sockfd, chunk, chunk_size, 0);
        
        if (_res < 0) {
            TP_NetCloseSocket(sockfd);
            return _res;
        }
        total_sent += _res;
    }
    return total_sent;
}


int32 bmake_http_GET(const char* subdomain, const char *hostname, const char *path, const char *token, uint16 *status_code, char **buffer)
{
    uint16 port;
    uchar hostIP[MAX_HOST_IP];
    int32 socket, _result;
    TP_SockAddrT dest_addr;

    char *http_request = NULL;
    char data[CHUNK_SIZE];
    char *response = NULL;
    char *body_start = NULL;
    int16 total_already_sent, total_size_len_request;
    uint16 realsize, size;

    check(); // PPP Connection
    
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
        concat_String(&http_request, "GET %s HTTP/1.1\r\nHost: %s%s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname
        );
    } else {
        concat_String(&http_request, "GET %s HTTP/1.1\r\nHost: %s%s\r\nAuthorization: Token %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            token
        );
    }

    total_size_len_request = strlen(http_request);
    total_already_sent = 0;
    TP_DbgSerialPrn("\r\nREQ LEN:%d => %s\r\n", total_size_len_request, http_request);

    // Send the HTTP request
    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);
    send_large_data(socket, http_request, total_size_len_request);

    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);

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

    if (http_request != NULL) {
        TP_FreeMemory((void**)&http_request);
        http_request = NULL;
    }

    // if (body_start != NULL) {
    //     TP_FreeMemory((void**)&body_start);
    //     body_start = NULL;
    // }

    return 0; // Success
}


int32 bmake_http_POST(const char* subdomain, const char *hostname, const char *path, const char *body, const char *token,  uint16 *status_code, char **buffer)
{
    int32 socket, _result;
    TP_SockAddrT dest_addr;

    char data[CHUNK_SIZE];
    char *http_request = NULL;

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
        concat_String(&http_request, "POST %s HTTP/1.1\r\nHost: %s%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n%s\r\n",
            path, 
            subdomain == PNULL ? "" : subdomain,
            hostname,
            "application/json",
            body == PNULL ? 0 : strlen(body),
            body
        );
    } else {
        concat_String(&http_request, "POST %s HTTP/1.1\r\nHost: %s%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Token %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\nConnection: close\r\n\r\n%s\r\n",
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
    send_large_data(socket, http_request, total_size_len_request);
    _result = -1;
    size = 0;

    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, 60000);
    memset(data, 0, sizeof(data));
    do {
        _result = TP_NetRecv(socket, (uint8*)data, CHUNK_SIZE - 1, 0);
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

    if (http_request != NULL) {
        TP_FreeMemory((void**)&http_request);
        http_request = NULL;
    }

    // if (body_start != NULL) {
    //     TP_FreeMemory((void**)&body_start);
    //     body_start = NULL;
    // }
    return 0; // Success
}


// Create a new linked list
LinkedList* Array() {
    LinkedList *list = NULL;

    TP_AllocMemory(sizeof(LinkedList), (void**)&list);
    if (list == NULL) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}


int Push(LinkedList *list, void *data) {
    Node *newNode = NULL;
    TP_AllocMemory(sizeof(Node), (void**)&newNode);
    if (newNode == NULL) {
        return 0;
    }

    newNode->data = data;
    newNode->next = NULL;

    if (list->tail == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    list->size++;

    return 1;  // Successfully inserted
}


void Map(LinkedList *list, void (*displayFunc)(void *, size_t index)) {
    Node *current = NULL;
    int _i = 0;
    if (list == NULL) return;
    current = list->head;
    while (current != NULL) {
        displayFunc(current->data, _i);
        current = current->next;
        _i++;
    }
}


void FreeArray(LinkedList *list, void (*freeFunc)(void *)) {
    Node *current = NULL;
    if (list == NULL) return;
    current = list->head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        if (freeFunc != NULL)
            freeFunc(temp->data);
        TP_FreeMemory((void**)&temp);
    }
    TP_FreeMemory((void**)&list);
}


int FreeItem(LinkedList *list, void *data, void (*freeFunc)(void *)) {
    Node *current = list->head;
    Node *prev = NULL;

    while (current != NULL && current->data != data) {
        prev = current;
        current = current->next;
    }

    // If the node with the specified data is found, delete it
    if (current != NULL) {
        if (prev == NULL) {
            // If the node to be deleted is the head, update the head pointer
            list->head = current->next;
            if (list->head == NULL) {
                // If the list becomes empty, update the tail pointer
                list->tail = NULL;
            }
        } else {
            // Update the next pointer of the previous node
            prev->next = current->next;
            // If the node to be deleted is the tail, update the tail pointer
            if (current == list->tail) {
                list->tail = prev;
            }
        }

        // Free the memory occupied by the node and its data
        freeFunc(current->data);

        list->size--;
        return 1;  // Successfully deleted
    }
    return 0;
}


void* Get(LinkedList *list, size_t index) {
    if (index >= 0 && index < list->size) {
        Node *current = list->head;
        size_t currentIndex = 0;

        // Traverse the list to find the node at the specified index
        while (currentIndex < index) {
            current = current->next;
            currentIndex++;
        }

        return current->data;
    }
    return NULL;  // Index out of bounds
}


size_t Length(LinkedList *list) {
    return list->size;
}