
#ifndef BREQUESTSLIB_H_
#define BREQUESTSLIB_H_


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPPPP.h>
#include <TPKeyPad.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include <TPNET.h>
#include <TPPrint.h>
#include <TP_Singleime.h>

#include "cJSON.h"
#include "common.h"

#define MAX_CHAR_ITEMS 25
#define CHUNK_SIZE 510
#define HTTP_DEFAULT_PORT 80
#define HTTPS_DEFAULT_PORT 443

#define MAX_HOST_IP 16


typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    size_t size;
} LinkedList;


int32 bmake_http_GET(const char* subdomain, const char *hostname, const char *path, const char *token, uint16 *status_code, char **buffer);

int32 bmake_http_POST(const char* subdomain, const char *hostname, const char *path, const char *body, const char *token,  uint16 *status_code, char **buffer);



// List 
LinkedList* Array();
int Push(LinkedList *list, void *data);
void Map(LinkedList *list, void (*displayFunc)(void *, size_t index));
void FreeArray(LinkedList *list, void (*freeFunc)(void *));
int FreeItem(LinkedList *list, void *data, void (*freeFunc)(void *));
void* Get(LinkedList *list, size_t index);
size_t Length(LinkedList *list);

#endif /* ENDPOINT_H_ */
