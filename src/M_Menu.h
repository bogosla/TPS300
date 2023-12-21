#ifndef M_MENU
#define M_MENU
// --------------------------------------------------------------------------------------
#define MAX_CHAR_ITEMS 25
#define CHUNK_SIZE 510
#define HTTP_DEFAULT_PORT 80
#define HTTPS_DEFAULT_PORT 443

#define MAX_HOST_IP 16

#define START_SCREEN_Y  2
#define START_SCREEN_X  2

// --------------------------------------------------------------------------------------
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
#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#include "endpoint.h"

// // --------------------------------------------------------------------------------------
// // ---------------------------------------------------------------------------------------

#define TIMEOUT_KEY - 100

typedef struct _HTTPResponse
{
    int32 status_code;
    int32 content_length;
    char* body;
} HTTPResponse;


// // --------------------------------------------------------------------------------------
// // ---------------------------------------------------------------------------------------
int8 LCD_Menu(const char *title, const char menuItems[][MAX_CHAR_ITEMS], uint8 count, int8 select);

int32 make_http_GET(const char* subdomain, const char *hostname, const char *path, const char *token, uint16 *status_code, char **buffer);

int32 make_http_POST(const char* subdomain, const char *hostname, const char *path, const char *body, const char *token,  uint16 *status_code, char **buffer);

int32 make_http_PUT(const char* subdomain, const char *hostname, const char *path, const char *body, const char *token,  uint16 *status_code, char **buffer);

int8 check_connection(void);


void test_printer(void);

void test_network(void);

void Display_Header(const char *title);

void LCD_Clear(void);

uint8 waitforKey();

int8 waitforKeyMs(int32 ms);


int8 LCD_Menu_InfoTirage(const InfoTirage *menuItems, uint8 count, int8 select, int *id, const InfoTirage *selectedTirage, uint16 sizeSelected);


int getLottoType(InfoTirage **selectedTirage, int *sizeTirage, char *name, int *idName);


void postFiches(const char *buffBoules);
int make_post_fiches(const List *list, int id_tirage, InfoTirage *tirages, int sizeTirage);

void getFiches(void);
void getWinningFiches(void);

int LCD_MenuTirage(const char *title, const Tirage *menuItems, unsigned int count, int select, int *id);
void logout(void);
#endif

