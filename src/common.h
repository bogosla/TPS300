#ifndef COMMON_H_
#define COMMON_H_
/*
 ------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TPPrint.h>
#include <TPFile.h>
#include "cJSON.h"
#include "constants.h"
/*------------------------------------------------------------------------------*/

typedef struct
{
	char *name;
	int id;
} InfoTirage;


typedef struct
{
	char *boules;
	char id[25];
	int _id;
	char *tirage_name;
	char montant[12];
	char *created;
} Tirage;

typedef struct
{
	int id;
	char boul[6];
	char pri[7];
	char lotto[5];
	char option[2];
} BouleItem;


typedef struct {
    BouleItem* items;
    int capacity;
    int size;
} List;



List* createList();
void destroyList(List* list);
void addElement(List* list, BouleItem element);
void deleteByIndex(List* list, int index);
int getElement(List *list, BouleItem item);


int write_to_file(const char* file, const char* contents);
int read_from_file(const char* file, char** buffer);


void concatString(char **buffer, const char *stringToAdd);


void addItem(InfoTirage** array, int* size, int id, const char* name);

void freeItems(InfoTirage* array, int size);

void setOptionBouleItem(BouleItem *item);


Boolean print_rapport(const char *buffer, const char *start_date, const char *end_date);
Boolean print_fiche(const char *buffer, int hasMultiple);
Boolean reprint_fiche(const Tirage tirage);
Boolean print_winnings_fiche(const cJSON *data, int size, const char *start_date, const char *end_date);

void addBoulePaire(List *list, char *pri);
void addBouleTriple(List *list, char *pri);
void addBouleRevers(List *list, char *pri);
void addBouleMarriage(List *list, char *pri);
void addBouleL4(List *list, char *pri);
void addPwent(List *list, char *pri, unsigned int pwent);


Boolean yesNoModal(const char *title);

void freeTirageItems(Tirage* array, int size);
void addTirageItem(Tirage** array, int* size, char *id, const char* boules, const char *tirage_name, const char *montant, int _id, const char* created);

int deleteTirageByIndex(Tirage* list, int index, int size);


void handleJSONNULL(void);
void handleSocketError(int32 code);
void handleEmptyList(void);
void Handle404Error(uint16 code, const char *str);

#endif /* COMMON_H_ */
