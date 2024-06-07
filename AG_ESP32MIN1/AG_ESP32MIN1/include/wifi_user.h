#ifndef _WIFI_USER_H
#define _WIFI_USER_H

#include "cJSON.h"

/********************Macros********************/
#define NUMBER_OF_Glasses 2 //max at 6, current requirement 2
#define STATUS_KEY  "status"
#define ID_KEY   "id"
#define NAME_KEY    "name"
#define TINT_KEY    "tint"
#define V_KEY    "voltage"
#define I_KEY    "current"
#define OLD_PWD_KEY "oldPassword"
#define NEW_PWD_KEY "newPassword"
#define HTTP_SUCCESS "HTTP/1.1 200"
#define HTTP_ERROR  "HTTP/1.1 400"
#define VI_VALUE    0.0
#define DEFAULT_MAC    0x02

/********************Functions Declaration********************/
void Set_Mac_Addr(void);
void Find_Mac_Address(void);
void Server_Start(void);
void Start_AP(void);
// void Start_AP_New_Cred(char *ssid,char *password);

void AP_StartDefault_Cred(void);
void AP_StartSaved_cred(void);
bool SaveAndCheck_Cred(char *newSsid,char *newPassword);
void Load_GlassNames(void);
void Save_GlassNames(void);
void Get_Uart_Status(const uint8_t channel,const char *status, 
                                        const double voltage,
                                        const double current);
void Get_Uart_VI(const uint8_t channel,const double voltage,
                                        const double current);
String Return_SubString(String str,const uint8_t start,const uint8_t end);
char* Get_JSONStr_Item(cJSON *obj,char *item);
uint8_t Get_ID(cJSON *obj, uint8_t *idList);
uint8_t Get_JSONint(cJSON *obj,char *item);
cJSON* AddItemTo_JsonObject(cJSON *obj, uint8_t glassNumber, 
                                        const char * const name, 
                                        const char * const status,
                                        const double voltage,
                                        const double current);
// void Rename_Glass(uint8_t id, const char * const name);

void Sent_ServerResponse(String EspServerResponse);
bool UpdateStatus_ToApp(const uint8_t NoOfGlasses);
void UpdateGlass_ToUart(const char *status, const uint8_t *glassNumber, const uint8_t size);

cJSON* Get_Payload(String request);

bool Post_GlassStatus(String request);
bool Post_Tint(String request);
bool Post_GlassRename(String request);
bool Post_ModuleSsid(String request);
bool Post_ModulePwd(String request);
bool Get_Tint(String request);

void Validate_Get_Cmd(String request);
void Validate_Post_Cmd(String request);

void Check_ClientRequest_Msg(void);
#endif