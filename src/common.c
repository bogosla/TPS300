/*
* Author: James DESTINE
*/
#include "common.h"
// -------------------------------------------------------------------------------


int write_to_file(const char* file, const char* contents) {
    int32 fhandle = 0;
    uint32 Len = 0;
    Boolean exists = FALSE;

    Len = strlen(contents);

    if (TP_FExist(file) == FALSE) {
        if (TP_Create(file, Len + 1) == 0) {
            return -1;
        }
    } else {
        exists = TRUE;
    }
    fhandle = TP_Open(file);

    if (fhandle == -1) {
        return -1;
    }
	else
	{
        if (exists == TRUE)
            TP_FFreeSize(fhandle); // Clean data
		TP_Write(fhandle, (uint8*)contents, Len);
		TP_Close(fhandle);
        return 0;
	}
}


int read_from_file(const char* file, char** buffer) {
    int32 fhandle = 0;
    uint8 *DataBuf = NULL;
    int32 Len = 0;
    int32 success = 0;

    fhandle = TP_Open(file);
    if (fhandle == -1)
    {
        return -1;
    }

    Len = TP_FSize(fhandle);
    
    if (TP_ReAllocMemory(Len, (void**)&DataBuf) == FALSE)
    {
        TP_Close(fhandle);
        return -1;  // Return error code indicating memory allocation failure
    }

    success = TP_Read(fhandle, DataBuf, Len);
    if (success >= 0)
    {
        if (TP_ReAllocMemory(success + 1, (void**)buffer) == FALSE) {
            TP_FreeMemory((void**)&DataBuf);
            TP_Close(fhandle);
			return -1;
        }
        
        memcpy(*buffer, DataBuf, success);
        (*buffer)[success] = '\0';  // Null-terminate the buffer
        TP_Close(fhandle);
        TP_FreeMemory((void**)&DataBuf);
        return 0;
    }
    TP_FreeMemory((void**)&DataBuf);
    TP_Close(fhandle);
    return -1;
}


void concatString(char **buffer, const char *stringToAdd) {
    size_t currLen = *buffer ? strlen(*buffer) : 0;
    size_t addLen = strlen(stringToAdd);

    if (TP_ReAllocMemory((currLen + addLen + 1) * sizeof(char), (void**)buffer) == FALSE)
        return;

    strcat(*buffer, stringToAdd);
}


void addItem(InfoTirage** array, int *size, int id, const char *name) {
    // Reallocate memory for the array of structures
    if (TP_ReAllocMemory((*size + 1) * sizeof(InfoTirage), (void**)array) == TRUE) {
        *size += 1;
        // *array = newArray;
        // Initialize the new item
        (*array)[*size - 1].id = id;
        TP_AllocMemory(strlen(name) + 1, (void**)&((*array)[*size - 1].name));
        strcpy((*array)[*size - 1].name, name);

    } else {
        TP_DbgSerialPrn("\nError allocation in addItem\n");
        return;
    }
    return;
}


void freeItems(InfoTirage* array, int size)
{
	int i = 0;
    if (array == NULL)
        return;
    while (i < size)
    {
        TP_FreeMemory((void**)&array[i].name);
		i++;
    }
    TP_FreeMemory((void**)&array);
}


Boolean print_rapport(const char *buffer, const char *start_date, const char *end_date)
{
	int key;
    Boolean success = FALSE;
	char *userData = NULL;
	cJSON *json = NULL, *json2 = NULL, *user = NULL, *company_name = NULL;
    cJSON *tirage = NULL, *quantite = NULL, *commission = NULL, *fGagant = NULL, *vente = NULL, *perte = NULL, *gain = NULL;

    char imei[16];

	memset(imei, 0x00, sizeof(imei));

    TP_GetModalIMEI(imei, 16);

	json = cJSON_Parse(buffer);

	if (json == NULL)
	{
        // TODO, Display error
        return FALSE;
	} else 
	{

        read_from_file(INFO_USER_FILE, &userData);
        json2 = cJSON_Parse(userData);

        if (json2 != NULL) {
            TP_PrnInit();
            TP_PrnSessionStart();
            TP_PrnLogo(logo_print, 384, 128);
            TP_PrnClearString();
            TP_ScrSetIcon(ICON_PRINTER, OPENICON);

            TP_PrnSpaceSet(0, 0);
            TP_PrnHighlightSet(FALSE);
            TP_PrnIndentSet(100);

            user = cJSON_GetObjectItemCaseSensitive(json2, "user");
            company_name = cJSON_GetObjectItemCaseSensitive(user, "companyName");
    	
            TP_PrnFontSet(3);
            // TODO, Display logo
            TP_PrnAddString((uint8*)"%s\f", company_name->valuestring);
            
            TP_PrnFontSet(1);
            // Fontsize 24
            TP_PrnIndentSet(0);

            TP_PrnAddString((uint8*)"Compagnie : %s\f", company_name->valuestring);
            TP_PrnAddString((uint8*)"POS       : %s\f", imei);
            TP_PrnAddString((uint8*)"Date Start: %s\f", start_date);
            TP_PrnAddString((uint8*)"End Start : %s\f", end_date);
            TP_PrnAddString((uint8*)"--------------------------------%s\f");

            TP_PrnString();
            TP_PrnClearString();

            tirage = cJSON_GetObjectItemCaseSensitive(json, "tirage");
			quantite = cJSON_GetObjectItemCaseSensitive(json, "quantite");
			commission = cJSON_GetObjectItemCaseSensitive(json, "commission");
			fGagant = cJSON_GetObjectItemCaseSensitive(json, "fgagnant");
			vente = cJSON_GetObjectItemCaseSensitive(json, "vente");
			perte = cJSON_GetObjectItemCaseSensitive(json, "perte");
			gain = cJSON_GetObjectItemCaseSensitive(json, "gain");
            
			if (cJSON_IsString(tirage) && cJSON_IsNumber(quantite) && cJSON_IsNumber(commission) && cJSON_IsNumber(perte) && cJSON_IsNumber(gain)) {
                TP_PrnAddString((uint8*)"Tirage         : %s\f", tirage->valuestring);
                TP_PrnAddString((uint8*)"Nombres Fiches : %d\f", quantite->valueint);
                TP_PrnAddString((uint8*)"Fiches Gagnants: %d\f", fGagant->valueint);
                TP_PrnAddString((uint8*)"Commissions    : %.2f\f", commission->valuedouble);
                TP_PrnAddString((uint8*)"Ventes         : %.2f\f", vente->valuedouble);
                TP_PrnAddString((uint8*)"A Payer        : %.2f\f", perte->valuedouble);
                TP_PrnAddString((uint8*)"Benefices      : %.2f\f", gain->valuedouble);

                // TP_PrnAddString((uint8*)"--------------------------------%s\f\n");
			}

            TP_PrnString();
            TP_PrnClearString();
            TP_PrnStep(5);
            TP_PrnSessionEnd();
            TP_ScrSetIcon(ICON_PRINTER, CLOSEICON);
        }
	}

	if (userData != NULL) {
		TP_FreeMemory((void**)&userData);
        userData = NULL;
    }

    if (json != NULL) {
		cJSON_Delete(json);
        json = NULL;
    }

    if (json2 != NULL) {
        cJSON_Delete(json2);
        json2 = NULL;
    }

	return TRUE;
}


Boolean print_fiche(const char *buffer, int hasMultiple) { 
	cJSON *json = NULL;
	cJSON *ficheId = NULL;
	cJSON *ficheTirage = NULL;
	cJSON *total = NULL;
	cJSON *bouleList = NULL;
	cJSON *ficheDate = NULL;
	cJSON *agentId = NULL;
	cJSON *json2 = NULL;
	cJSON *user = NULL;
	cJSON *fiches = NULL;
	cJSON *company_desc = NULL;
	cJSON *company_name = NULL;

	Boolean success = FALSE;

    cJSON *agentaddressIn = NULL;
    cJSON *agentnameIn = NULL;
    cJSON *companyphoneIn = NULL;
    cJSON *companyPhone = NULL;
    cJSON *addresse_complet = NULL;

	char *userData = NULL;
	char imei[16], str[12];

	memset(imei, 0x00, sizeof(imei));
    TP_GetModalIMEI(imei, 16);

	json = cJSON_Parse(buffer);

	if (json == NULL)
	{
        TP_DbgSerialPrn("\r\nStart printing TICKET Is NULL\r\n");
	} else 
	{
        read_from_file(INFO_USER_FILE, &userData);
        TP_PrnInit();
        TP_PrnSessionStart();
        TP_PrnLogo(logo_print, 384, 128);
        
        TP_PrnClearString();
        TP_ScrSetIcon(ICON_PRINTER, OPENICON);

        TP_PrnSpaceSet(0, 0);
        TP_PrnHighlightSet(FALSE);
        TP_PrnIndentSet(100);

        if (hasMultiple == 0)
        {
            ficheId = cJSON_GetObjectItemCaseSensitive(json, "ficheId");
            ficheTirage = cJSON_GetObjectItemCaseSensitive(json, "ficheTirage");
            total = cJSON_GetObjectItemCaseSensitive(json, "total");
            bouleList = cJSON_GetObjectItemCaseSensitive(json, "bouleList");
        } else 
        {
            fiches = cJSON_GetObjectItemCaseSensitive(json, "fiches");
            total = cJSON_GetObjectItemCaseSensitive(json, "gTotal");
        }
		// end

        ficheDate = cJSON_GetObjectItemCaseSensitive(json, "ficheDate");

        json2 = cJSON_Parse(userData);
        user = cJSON_GetObjectItemCaseSensitive(json2, "user");
        agentId = cJSON_GetObjectItemCaseSensitive(user, "username");
        company_desc = cJSON_GetObjectItemCaseSensitive(user, "company_description");
        company_name = cJSON_GetObjectItemCaseSensitive(user, "companyName");

        agentaddressIn = cJSON_GetObjectItemCaseSensitive(user, "agentaddressIn");
		agentnameIn = cJSON_GetObjectItemCaseSensitive(user, "agentnameIn");
		companyphoneIn = cJSON_GetObjectItemCaseSensitive(user, "companyphoneIn");
	    companyPhone = cJSON_GetObjectItemCaseSensitive(user, "companyPhone");
		addresse_complet = cJSON_GetObjectItemCaseSensitive(user, "addresse_complet");
		
        TP_PrnFontSet(3);
        // TODO, Display logo
        TP_PrnAddString((uint8*)"%s\f", company_name->valuestring);
        TP_PrnFontSet(1);
        // Fontsize 24
        TP_PrnIndentSet(0);
        TP_PrnAddString((uint8*)"POS     : %s\f", imei);
        TP_PrnAddString((uint8*)"Date    : %s\f", ficheDate->valuestring);
    
        if (cJSON_IsBool(companyphoneIn) && cJSON_IsTrue(companyphoneIn))
        {
            TP_PrnAddString((uint8*)"Tel     : %s\f", companyPhone->valuestring);
        }

        if (hasMultiple == 0)
        {
            TP_PrnAddString((uint8*)"Tirage  : %s\f", ficheTirage->valuestring);
            TP_PrnAddString((uint8*)"#Ticket : %s\f", ficheId->valuestring);
        } else {
            if (cJSON_IsArray(fiches))
            {
                cJSON *element;
                // Iterate over the array elements
                cJSON_ArrayForEach(element, fiches) 
                {
                    ficheId = cJSON_GetObjectItemCaseSensitive(element, "ficheId");
                    ficheTirage = cJSON_GetObjectItemCaseSensitive(element, "ficheTirage");
                    bouleList = cJSON_GetObjectItemCaseSensitive(element, "bouleList");
                    TP_PrnAddString((uint8*)"Tirage  : %s\f", ficheTirage->valuestring);
                    TP_PrnAddString((uint8*)"#Ticket : %s\f", ficheId->valuestring);
                }
            }
        }
        // New Block
        {
            if (cJSON_IsBool(agentnameIn) && cJSON_IsTrue(agentnameIn))
			{
                TP_PrnAddString((uint8*)"Agent   : %s\f", agentId->valuestring);
			}

			if (cJSON_IsBool(agentaddressIn) && cJSON_IsTrue(agentaddressIn))
			{
			    TP_PrnAddString((uint8*)"Adresse : %s\f", addresse_complet->valuestring);
			}
            TP_PrnAddString((uint8*)"      **Original Fiche**       \f");


            TP_PrnAddString((uint8*)"--------------------------------\f");
            TP_PrnAddString((uint8*)"Jeux => Boule => Option => Prix\f");
            TP_PrnAddString((uint8*)"--------------------------------\f");
            TP_PrnString();
            TP_PrnClearString();

			if (cJSON_IsArray(bouleList))
			{
				cJSON *element;
				// Iterate over the array elements
				cJSON_ArrayForEach(element, bouleList) 
				{
					cJSON *boule = cJSON_GetObjectItemCaseSensitive(element, "boule");
					cJSON *lotto = cJSON_GetObjectItemCaseSensitive(element, "lotto");
					cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
					cJSON *option = cJSON_GetObjectItemCaseSensitive(element, "option");
					sprintf(str, "%.2f", montant->valuedouble);
                    TP_PrnAddString((uint8*)" %-3s => %5s => %3s    => %s\f", lotto->valuestring, boule->valuestring, option->valuestring, str);
				}
			}
			TP_PrnString();
            TP_PrnClearString();
            TP_PrnFontSet(2);

			sprintf(str, "%.2f HTG", total->valuedouble);
            TP_PrnAddString((uint8*)"\nTOTAL: %s\f", str);
            TP_PrnFontSet(1);
            TP_PrnAddString((uint8*)"--------------------------------\f\n");

            TP_PrnFontSet(2);
            TP_PrnAddString((uint8*)"%s\f", company_desc->valuestring);

            TP_PrnString();
            TP_PrnClearString();
            TP_PrnStep(5);

            TP_PrnSessionEnd();
            TP_ScrSetIcon(ICON_PRINTER, CLOSEICON);
		}
	}
    
    if (json != NULL) {
		cJSON_Delete(json);
        json = NULL;
    }

    if (json2 != NULL) {
		cJSON_Delete(json2);
        json2 = NULL;
    }
	if (userData != NULL) {
		TP_FreeMemory((void**)&userData);
	    userData = NULL;
    }

	return TRUE;
}


Boolean reprint_fiche(const Tirage tirage)
{
	cJSON *bouleList = NULL;
	cJSON *json = NULL;
	cJSON *user = NULL;
	cJSON *agentId = NULL;
	cJSON *company_desc = NULL;
	cJSON *company_name = NULL;

	char *userData = NULL;
	char imei[16], str[12];

    const char *ficheId = tirage.id;
    const char *ficheTirage = tirage.tirage_name;
    const char *total = tirage.montant;
	const char *ficheDate = tirage.created;
	
	memset(imei, 0x00, sizeof(imei));
	TP_GetModalIMEI(imei, 16);
    
    read_from_file(INFO_USER_FILE, &userData);
	
	bouleList = cJSON_Parse(tirage.boules);
	json = cJSON_Parse(userData);

    if (json != NULL && bouleList != NULL) {
        TP_PrnInit();
        TP_PrnSessionStart();
        TP_PrnLogo(logo_print, 384, 128);

        TP_ScrSetIcon(ICON_PRINTER, OPENICON);
        TP_PrnSpaceSet(0, 0);
        TP_PrnHighlightSet(FALSE);
        TP_PrnIndentSet(100);

		user = cJSON_GetObjectItemCaseSensitive(json, "user");
        company_desc = cJSON_GetObjectItemCaseSensitive(user, "company_description");
        company_name = cJSON_GetObjectItemCaseSensitive(user, "companyName");
        agentId = cJSON_GetObjectItemCaseSensitive(user, "ref_code");

        TP_PrnFontSet(3);
        // TODO, Display logo
        TP_PrnAddString((uint8*)"%s\f", company_name->valuestring);
        
        TP_PrnFontSet(1); // Fontsize 24
        TP_PrnIndentSet(0);

        {
            cJSON *agentaddressIn = cJSON_GetObjectItemCaseSensitive(user, "agentaddressIn");
			cJSON *agentnameIn = cJSON_GetObjectItemCaseSensitive(user, "agentnameIn");
			cJSON *companyphoneIn = cJSON_GetObjectItemCaseSensitive(user, "companyphoneIn");

			cJSON *addresse_complet = cJSON_GetObjectItemCaseSensitive(user, "addresse_complet");
			cJSON *username = cJSON_GetObjectItemCaseSensitive(user, "username");
			cJSON *companyPhone = cJSON_GetObjectItemCaseSensitive(user, "companyPhone");
            TP_PrnAddString((uint8*)"POS     : %s\f", imei);
            TP_PrnAddString((uint8*)"Date    : %s\f", ficheDate);

            if (cJSON_IsBool(companyphoneIn) && cJSON_IsTrue(companyphoneIn))
			{
                TP_PrnAddString((uint8*)"Tel     : %s\f", companyPhone->valuestring);
			}

            TP_PrnAddString((uint8*)"Tirage  : %s\f", ficheTirage);
            TP_PrnAddString((uint8*)"#Ticket : %s\f", ficheId);

            if (cJSON_IsBool(agentnameIn) && cJSON_IsTrue(agentnameIn))
			{
                TP_PrnAddString((uint8*)"Agent   : %s\f", username->valuestring);
			}

			if (cJSON_IsBool(agentaddressIn) && cJSON_IsTrue(agentaddressIn))
			{
			    TP_PrnAddString((uint8*)"Adresse : %s\f", addresse_complet->valuestring);
			}
        }
        TP_PrnAddString((uint8*)"       **Copie Fiche**       \f");
        
        TP_PrnAddString((uint8*)"Jeux => Boule => Option => Prix\f");
        TP_PrnAddString((uint8*)"--------------------------------\f");
        TP_PrnString();
        TP_PrnClearString();
        
        if (cJSON_IsArray(bouleList))
        {
            cJSON *element;
            // Iterate over the array elements
            cJSON_ArrayForEach(element, bouleList) 
            {
                cJSON *boule = cJSON_GetObjectItemCaseSensitive(element, "boule");
                cJSON *lotto = cJSON_GetObjectItemCaseSensitive(element, "lotto");
                cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
                cJSON *option = cJSON_GetObjectItemCaseSensitive(element, "option");
                sprintf(str, "%.2f", montant->valuedouble);
                TP_PrnAddString((uint8*)" %-3s => %5s => %3s    => %s\f", lotto->valuestring, boule->valuestring, option->valuestring, str);
				
            }
        }

        TP_PrnString();
        TP_PrnClearString();
        TP_PrnFontSet(2);

        // sprintf(str, "%s HTG", total);
        TP_PrnAddString((uint8*)"\nTOTAL: %s\f", total);
        TP_PrnFontSet(1);
        TP_PrnAddString((uint8*)"--------------------------------\f\n");

        TP_PrnFontSet(2);
        TP_PrnAddString((uint8*)"%s\f", company_desc->valuestring);

        TP_PrnString();
        TP_PrnClearString();
        
        TP_PrnStep(5);

        TP_PrnSessionEnd();
        TP_ScrSetIcon(ICON_PRINTER, CLOSEICON);

    }


	if (userData != NULL) {
        TP_FreeMemory((void**)&userData);
    	userData = NULL;
    }

    if (json != NULL) {
        cJSON_Delete(json);
        json = NULL;
    }

     if (bouleList != NULL) {
        cJSON_Delete(bouleList);
        bouleList = NULL;
    }
	return TRUE;
}


Boolean print_winnings_fiche(const cJSON *data, int size, const char *start_date, const char *end_date)
{
	cJSON *bouleList = NULL;
	cJSON *json2 = NULL;
	cJSON *user = NULL;
	cJSON *company_desc = NULL;
	cJSON *company_name = NULL;

    Boolean success = TRUE;

	char *userData = NULL;
	char imei[16], str[27];
    int index = 0;
	
	memset(imei, 0x00, sizeof(imei));

	TP_GetModalIMEI(imei, 16);	
	read_from_file(INFO_USER_FILE, &userData);

    json2 = cJSON_Parse(userData);
    user = cJSON_GetObjectItemCaseSensitive(json2, "user");
    company_name = cJSON_GetObjectItemCaseSensitive(user, "companyName");

    TP_PrnInit();
    TP_PrnSessionStart();
    TP_PrnLogo(logo_print, 384, 128);

    TP_PrnClearString();
    TP_ScrSetIcon(ICON_PRINTER, OPENICON);

    TP_PrnSpaceSet(0, 0);
    TP_PrnHighlightSet(FALSE);
    TP_PrnIndentSet(100);

    TP_PrnFontSet(3);
    // TODO, Display logo
    TP_PrnAddString((uint8*)"%s\f", company_name->valuestring);
            
    TP_PrnFontSet(1);
    // Fontsize 24
    TP_PrnIndentSet(0);

    {
        cJSON *agentaddressIn = cJSON_GetObjectItemCaseSensitive(user, "agentaddressIn");
        cJSON *agentnameIn = cJSON_GetObjectItemCaseSensitive(user, "agentnameIn");
        cJSON *companyphoneIn = cJSON_GetObjectItemCaseSensitive(user, "companyphoneIn");

        cJSON *addresse_complet = cJSON_GetObjectItemCaseSensitive(user, "addresse_complet");
        cJSON *username = cJSON_GetObjectItemCaseSensitive(user, "username");
        cJSON *companyPhone = cJSON_GetObjectItemCaseSensitive(user, "companyPhone");

        TP_PrnAddString((uint8*)"POS : %s\f", imei);
       
        // if (cJSON_IsBool(companyphoneIn) && cJSON_IsTrue(companyphoneIn))
        // {
        //     TP_PrnAddString((uint8*)"Tel : %s\f", companyPhone->valuestring);
        // }

        if (cJSON_IsBool(agentnameIn) && cJSON_IsTrue(agentnameIn))
        {
            TP_PrnAddString((uint8*)"Agent : %s\f", username->valuestring);
        }

        // if (cJSON_IsBool(agentaddressIn) && cJSON_IsTrue(agentaddressIn))
        // {
        //     TP_PrnAddString((uint8*)"Adresse : %s\f", addresse_complet->valuestring);
        // }
    }

    TP_PrnAddString((uint8*)"Date Start : %s\f", start_date);
    TP_PrnAddString((uint8*)"End Date : %s\f", end_date);

    TP_PrnAddString((uint8*)"--------------------------------\f");
    TP_PrnAddString((uint8*)"%s => %12s => %s\f", "ID", "montant", "Gains");
    TP_PrnAddString((uint8*)"--------------------------------\f");
    TP_PrnString();
    TP_PrnClearString();

    if (cJSON_IsArray(data)) {
        index = 0;
        while (index < size)
        {
            cJSON *element = cJSON_GetArrayItem(data, index);
            cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
            cJSON *ref_code = cJSON_GetObjectItemCaseSensitive(element, "ref_code");
            cJSON *perte = cJSON_GetObjectItemCaseSensitive(element, "perte");
            sprintf(str, "%.2f", montant->valuedouble);
            TP_PrnAddString((uint8*)"%13s => %7s => %s\f", ref_code->valuestring, montant->valuestring, perte->valuestring);
                
            // display at each to limit buffer to be fulled
            if ((index % 10) == 0) {
                TP_PrnString();
                TP_PrnClearString();
            }
            index ++;	
        }
    }	
    
    TP_PrnString();
    TP_PrnClearString();
    TP_PrnStep(5);
    TP_PrnSessionEnd();
    TP_ScrSetIcon(ICON_PRINTER, CLOSEICON);
    TP_ScrRestore(0);


	if (userData != NULL) {
		TP_FreeMemory((void**)&userData);
        userData = NULL;
    }

    if (json2 != NULL) {
        cJSON_Delete(json2);
        json2 = NULL;
    }

	return success;
}



List* createList() 
{
    List* list = NULL;
    TP_AllocMemory(sizeof(List), (void**)&list);
    if (list == NULL) {
        // printf("Failed to allocate memory for the list.\n");
        return NULL;
    }

    list->capacity = 7; // Initial capacity
    list->size = 0;
    TP_AllocMemory(list->capacity * sizeof(BouleItem), (void**)&list->items);
    if (list->items == NULL) {
        // printf("Failed to allocate memory for the list elements.\n");
        TP_FreeMemory((void**)&list);
        return NULL;
    }
    return list;
}


void destroyList(List* list) 
{
    if (list != NULL) {
        TP_FreeMemory((void**)&list->items);
        TP_FreeMemory((void**)&list);
    }
}


int getElement(List *list, BouleItem item)
{
    int i = 0;
    while (i < list->size)
    {
        if (strcmp(list->items[i].boul, item.boul) == 0 && strcmp(list->items[i].lotto, item.lotto) == 0)
            return 0;
        i++;
    }
    return -1;
}


void addElement(List* list, BouleItem element) 
{
    int s = getElement(list, element);
    // BouleItem* newElements = NULL;

    if (s == 0)
    {
        return;
    } else
    {
        if (list->size >= list->capacity) {
            list->capacity *= 2;
            if (TP_ReAllocMemory(list->capacity * sizeof(BouleItem), (void**)&list->items)) {
                // printf("Failed to reallocate memory for the list elements.\n");
                return;
            }
            // list->items = newElements;
        }

        list->items[list->size] = element;
        list->size++;
    }
}


void deleteByIndex(List* list, int index) 
{
	int i = 0;
    if (index < 0 || index >= list->size) {
        printf("Invalid index.\n");
        return;
    }
    i = (int)index;
    // Shift elements to the left to overwrite the element to be deleted
    while (i < list->size - 1) {
        list->items[i] = list->items[i + 1];
        i++;
    }
    list->size--; // Decrement the size to reflect the deleted element
}


void setOptionBouleItem(BouleItem *item)
{
	int size = strlen(item->boul);
	if (size == 3)
		strcpy(&item->lotto, "L3");
	else if (size == 2)
		strcpy(&item->lotto, "BO");
	else if (size == 5)
	{
		strcpy(&item->lotto, "L5");
	}
	return;
}


void addTirageItem(Tirage** array, int* size, char *id, const char* boules, const char *tirage_name, const char *montant, int _id, const char* created) {
    // Reallocate memory for the array of structures
    if (TP_ReAllocMemory((*size + 1) * sizeof(Tirage), (void**)array) == TRUE) {
        *size += 1;
		sprintf((*array)[*size - 1].id, id);
        (*array)[*size - 1]._id =  _id;
		sprintf((*array)[*size - 1].montant, montant);

        TP_AllocMemory(strlen(boules) + 1, (void**)&((*array)[*size - 1].boules));
        TP_AllocMemory(strlen(boules) + 1, (void**)&((*array)[*size - 1].created));
        TP_AllocMemory(strlen(boules) + 1, (void**)&((*array)[*size - 1].tirage_name));

        strcpy((*array)[*size - 1].boules, boules);
        strcpy((*array)[*size - 1].created, created);
        strcpy((*array)[*size - 1].tirage_name, tirage_name);
    }
    else
        return;
}


void freeTirageItems(Tirage* array, int size) {
	int i = 0;
    if (array == NULL)
        return;
    while (i < size)
    {
        TP_FreeMemory((void**)&array[i].boules);
		i++;
    }
    TP_FreeMemory((void**)&array);
	array = NULL;
}


int deleteTirageByIndex(Tirage* list, int index, int size) {
	int i = 0;
    if (index < 0 || index >= size) {
        return -1;
    }
    i = (int)index;
    TP_FreeMemory((void**)&list[i].boules);
	TP_FreeMemory((void**)&list[i].tirage_name);
	TP_FreeMemory((void**)&list[i].created);
    while (i < size - 1) {
        list[i] = list[i + 1];
        i++;
    }
    return size - 1; // Decrement the size to reflect the deleted element
}


Boolean yesNoModal(const char *title) {
    uint8 fontHeight = 0;
    uint8 KeyCode;

    TP_Kbflush();
    TP_ScrRestore(0);
    TP_ScrSpaceSet(0, 2);
    TP_ScrClsPrint(0);
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    fontHeight = TP_GetFontHeight();

    TP_SetDisplayArea(12, 14, MAX_SCREEN_WIDTH -  12, 50);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(12, 14, MAX_SCREEN_WIDTH - 12, 50);

    TP_ScrGotoxyEx(15, 17);
    TP_LcdPrintf(title);

    TP_ScrGotoxyEx(13, 50 - fontHeight - 3);
    TP_LcdPrintf("Wi=CLEAR, Non=CANCEL");

    while (TRUE)
    {
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();
            if ( KeyCode == TP_KEY_CLEAR)
            {
                TP_ScrRestore(1);
                return TRUE;
            }
            else if( KeyCode == TP_KEY_CANCEL || KeyCode == TP_KEY_OK)
            {
                TP_ScrRestore(1);
                return FALSE;
            }
        }
    }
}


void handleJSONNULL(void) {
    LCD_Clear();
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
    TP_ScrGotoxyEx(63 - 30, 32 - 7);
    TP_LcdPrintf("Ere JSON NULL!");
    while (waitforKey() != TP_KEY_CANCEL);
    return;
}


void handleSocketError(int32 code) {
    LCD_Clear();
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
    TP_ScrGotoxyEx(63 - 34, 32 - 7);
    TP_LcdPrintf("Socket ere(%3d)!", code);
    while (waitforKey() != TP_KEY_CANCEL);
    return;
}


void handleEmptyList(void) {
    LCD_Clear();
    TP_SetDisplayArea(1, 15, MAX_SCREEN_WIDTH - 1, 62);
    TP_ScrGotoxyEx(63 - 28, 32 - 7);
    TP_LcdPrintf("Lis Fich Vid!");
    while (waitforKey() != TP_KEY_CANCEL);
    return;
}


void HandleIMEI_NOT_OK() {
    LCD_Clear();
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
    TP_ScrGotoxyEx(63 - 30, 32 - 7);
    TP_LcdPrintf("Ou Pa Otorize!");
    while (waitforKey() != TP_KEY_CANCEL);
    return;
}


void Handle404Error(uint16 code, const char *str) {
    LCD_Clear();
    TP_SetDisplayArea(1, 1, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1);
    TP_ScrGotoxyEx(63 - 20, 7);
    TP_LcdPrintf("Ere(%d)!", code);
    TP_ScrGotoxyEx(6, 18);
    TP_LcdPrintf(str);
    while (waitforKey() != TP_KEY_CANCEL);
    return;
}




