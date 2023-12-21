/*
* auth.h
*
*      Author: James DESTINE
*/
#ifndef AUTH_H_
#define AUTH_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <TPTypedef.h>
#include <TPKeyPad.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPFile.h>
#include <TP_Singleime.h>

#include "M_Menu.h"
#include "cJSON.h"
#include "endpoint.h"
#include "TP_DisplayAPI.h"

#include "common.h"
#include "constants.h"


// /*
//     ----------------------------------------------------------------------------------------
//     ----------------------------------------------------------------------------------------
// */
Boolean postHandleLogin(void);

void getRapports(void);

void getTirages(void); 

void getConfiguration(void);

#endif /* AUTH_H_ */
