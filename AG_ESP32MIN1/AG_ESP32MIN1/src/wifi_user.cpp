/********************Includes********************/
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "wifi_user.h"
#include "flash.h"
#include "uart.h"
#include "cJSON.h"
#include "main.h"

/********************Global Variables********************/
const char* p_DefaultSsid     = "Module 1";
const char* p_DefaultPassword = "123456789";
char g_Ssid[32];
char g_Password[63];

String g_HttpHeader = "";
String g_HttpMethod = "";
String g_RequestCommand = "";
String g_EspServerResponse = "";

// Define your new MAC address
uint8_t g_NewMAC[6] = {0x52, 0x47, 0x53, 0x53, 0x47, 0x52};
struct glass_parameters{ 
    uint8_t id;
    char g_name[33];
    char g_status[20];
    double g_voltage;
    double g_current;
};
struct glass_parameters g_Glass[6] = {{1,"GLASS 1","OFF",VI_VALUE,VI_VALUE},
                                        {2,"GLASS 2","OFF",VI_VALUE,VI_VALUE},
                                        {3,"GLASS 3","OFF",VI_VALUE,VI_VALUE},
                                        {4,"GLASS 4","OFF",VI_VALUE,VI_VALUE},
                                        {5,"GLASS 5","OFF",VI_VALUE,VI_VALUE},
                                        {6,"GLASS 6","OFF",VI_VALUE,VI_VALUE}};
uint8_t g_Tint = 100;

uint8_t g_ArraySize;

IPAddress g_IP;
WiFiClient g_Client;
HTTPClient g_Http;
// Set web server port number to 80
WiFiServer g_Server(80);


/********user code begin*******/
// Function to generate a random MAC address
void generate_mac_address(unsigned char mac[6]) {
    // Set the first byte to a specific value (e.g., 0x02)
    uint32_t unique_id = esp_random(); // or use another unique identifier for your device
    // Set the remaining part of the MAC address as unique identifier
    mac[3] = (unique_id >> 16) & 0xFF;
    mac[4] = (unique_id >> 8) & 0xFF;
    mac[5] = unique_id & 0xFF;
}

/*
@Function Name: Set_Mac_Addr()
@Parameters: None
@Return: None
@Functionality: Setting new MAC Address to Module
*/
void Set_Mac_Addr(void){
    // Disable WiFi
    WiFi.mode(WIFI_OFF);
    // Set the new MAC address    
    generate_mac_address(g_NewMAC);
    if (esp_base_mac_addr_set(g_NewMAC) == ESP_OK) {        
        #if DEBUG_MODE
        Serial.println("MAC address set successfully");
        #endif
    } else {       
        #if DEBUG_MODE
        Serial.println("Failed to set MAC address");
        #endif
    }    
}
/*
@Function Name: Find_Mac_Address()
@Parameters: None
@Return: None
@Functionality: Reading MAC Address
*/
void Find_Mac_Address(void){
    // Get the MAC address of the Wi-Fi station interface
    uint8_t baseMac[6];
    
    // Get the MAC address of the Wi-Fi AP interface
    esp_read_mac(baseMac, ESP_MAC_WIFI_SOFTAP);
    #if DEBUG_MODE
    Serial.print("SoftAP MAC: ");
    #endif
    for (int i = 0; i < 5; i++) {
        #if DEBUG_MODE
        Serial.printf("%02X:", baseMac[i]);
        #endif
    }
    #if DEBUG_MODE
    Serial.printf("%02X\n", baseMac[5]);    
    Serial.println();
    #endif
}
/*
@Function Name: Server_Start()
@Parameters: None
@Return: None
@Functionality: Server Start 
*/
void Server_Start(void){
    g_Server.begin();
}
/*
@Function Name: Start_AP()
@Parameters: None
@Return: None
@Functionality: Start Access Point with credentials
*/
void Start_AP(void){    
    WiFi.softAP(g_Ssid,g_Password);    //Assigning SSID and password to AP
}
/*
@Function Name: AP_StartDefault_Cred()
@Parameters: None
@Return: None
@Functionality: Start with default cred and data
*/
void AP_StartDefault_Cred(void){
    strcpy(g_Ssid,p_DefaultSsid);
    strcpy(g_Password,p_DefaultPassword);
    Save_Credentials(g_Ssid,g_Password);    //Writing New credentials in permanent memory
    Save_GlassNames();
    WiFi.softAP(g_Ssid,g_Password);    //Assigning SSID and password to AP 
}
/*
@Function Name: AP_StartSaved_cred()
@Parameters: None
@Return: None
@Functionality: Start with saved cred and data
*/
void AP_StartSaved_cred(void){
    Load_Credentials(g_Ssid, g_Password);    
    Load_GlassNames();
    WiFi.softAP(g_Ssid,g_Password);    //Assigning SSID and password to AP
}

/*
@Function Name: SaveAndCheck_Cred()
@Parameters: char*, char*
@Return: bool
@Functionality: save the credentials into permanent memory, 
*/
bool SaveAndCheck_Cred(char *newSsid,char *newPassword){
    
    char storedSsid[32], storedPassword[63];
    Save_Credentials(newSsid,newPassword);    //Writing New credentials in permanent memory   
    Load_Credentials(storedSsid,storedPassword);
    #if DEBUG_MODE
    Serial.println(storedSsid);
    Serial.println(storedPassword);
    #endif
    if((!strcmp(newSsid,storedSsid)) && (!strcmp(newPassword,storedPassword))){
        return SUCCESS;        
    }
    else{
        return ERROR;
    }    
}
/*
@Function Name: Load_GlassNames()
@Parameters: None
@Return: None
@Functionality: load glass names from permanent memory to variables
*/
void Load_GlassNames(void){
    Load_GlsNames(g_Glass[0].g_name,g_Glass[1].g_name);
    // Save_GlsNames(g_Glass[0].g_name,g_Glass[1].g_name,g_Glass[0].g_status,g_Glass[1].g_status);
}
/*
@Function Name: Save_GlassNames()
@Parameters: None
@Return: None
@Functionality: save glass names from permanent memory
*/
void Save_GlassNames(void){
    Save_GlsNames(g_Glass[0].g_name,g_Glass[1].g_name);
    // Save_GlsNames(g_Glass[0].g_name,g_Glass[1].g_name,g_Glass[0].g_status,g_Glass[1].g_status);
}
/*
@Function Name: Get_Uart_Status()
@Parameters: uint8_t,char*
@Return: None
@Functionality: get glass status from hardware switches
*/
void Get_Uart_Status(const uint8_t channel,const char *status, 
                                        const double voltage,
                                        const double current){

    sprintf(g_Glass[channel-1].g_status,status);
    g_Glass[channel-1].g_voltage = voltage;
    g_Glass[channel-1].g_current = current;
    UpdateStatus_ToApp(NUMBER_OF_Glasses);
}
void Get_Uart_VI(const uint8_t channel,const double voltage,
                                        const double current){
    g_Glass[channel-1].g_voltage = voltage;
    g_Glass[channel-1].g_current = current;
    //UpdateStatus_ToApp(NUMBER_OF_Glasses);
}
/*
@Function Name: Return_SubString()
@Parameters: String,uint8_t,uint8_t
@Return: String
@Functionality: Return substring
*/
String Return_SubString(String str,const uint8_t start,const uint8_t end){
    str = str.substring(start,end);
    return str;
}
/*
@Function Name: Get_JSONStr_Item()
@Parameters: cJSON, char*
@Return: char*
@Functionality: get the string element from json object
*/
char* Get_JSONStr_Item(cJSON *obj, const char *item){

    cJSON *status = cJSON_GetObjectItem(obj, item);
    if (status && status->type == cJSON_String)
        return status->valuestring;
    else
        return NULL;
}
/*
@Function Name: Get_ID()
@Parameters: cJSON, uint8_t
@Return: uint8_t
@Functionality: get the id from json object
*/
uint8_t Get_ID(cJSON *obj, uint8_t *idList){
    
    cJSON *id = cJSON_GetObjectItem(obj, "id"); 
    g_ArraySize = cJSON_GetArraySize(id);

    for (int i = 0; i < g_ArraySize; ++i) {
        cJSON *jsonItem = cJSON_GetArrayItem(id, i);
        if (jsonItem && jsonItem->type == cJSON_Number)
            idList[i] = jsonItem->valueint;
    }
    return g_ArraySize;
}
/*
@Function Name: Get_Tint()
@Parameters: cJSON
@Return: uint8_t
@Functionality: get the tint from json object
*/
uint8_t Get_JSONint(cJSON *obj, const char *item){

    cJSON *tint = cJSON_GetObjectItem(obj, item);
    if (tint && tint->type == cJSON_Number)
        return tint->valueint;
    return 0;
}
/*
@Function Name: AddItemTo_JsonObject()
@Parameters: cJSON,uint8_t,const char*,const char*
@Return: cJSON
@Functionality: Add elements to JSON object
*/
cJSON* AddItemTo_JsonObject(cJSON *obj, uint8_t glassNumber, 
                                        const char * const name, 
                                        const char * const status,
                                        const double voltage,
                                        const double current){

    cJSON_AddNumberToObject(obj, ID_KEY, glassNumber);
    cJSON_AddStringToObject(obj, NAME_KEY, name);
    cJSON_AddStringToObject(obj, STATUS_KEY, status);    
    cJSON_AddNumberToObject(obj, V_KEY, voltage);    
    cJSON_AddNumberToObject(obj, I_KEY, current);    
    return obj;    
}
/*
@Function Name: Sent_ServerResponse()
@Parameters: String
@Return: None
@Functionality: Response to Mobile APP
*/
void Sent_ServerResponse(String espServerResponse){

    g_Client.println(HTTP_SUCCESS);
    g_Client.println("Content-Type: application/json");
    g_Client.println(""); //  Comillas importantes.
    g_Client.println(espServerResponse); //  Return status
    g_Client.flush();      
    #if DEBUG_MODE
    Serial.println(espServerResponse); //  Return status
    Serial.println("response sent");
    #endif
    g_Client.stop();
}
/*
@Function Name: Sent_ServerError()
@Parameters: uint8_t
@Return: None
@Functionality: Response to Mobile APP
*/
void Sent_ServerError(void){ 
    g_Client.println(HTTP_ERROR);
    g_Client.println("Content-Type: application/json");
    g_Client.println(""); //  Comillas importantes.
    g_Client.flush();      
    #if DEBUG_MODE
    Serial.println("error response sent");
    #endif
    g_Client.stop();
}
/*
@Function Name: UpdateStatusToApp()
@Parameters: uint8_t
@Return: None
@Functionality: Update Glass status to Mobile APP
*/
bool UpdateStatus_ToApp(const uint8_t noOfGlasses){
    
    g_EspServerResponse = '[';
    for(int i = 0;i < noOfGlasses;i++){
        cJSON *configGlass = cJSON_CreateObject();
        if (configGlass == NULL) {
            printf("Error: Memory allocation failed for cJSON object.\n");
            // Handle the error appropriately
            return ERROR;
        }
        else{
            configGlass = AddItemTo_JsonObject(configGlass,g_Glass[i].id,
                                                            g_Glass[i].g_name,
                                                            g_Glass[i].g_status,
                                                            g_Glass[i].g_voltage,
                                                            g_Glass[i].g_current);
            g_EspServerResponse += cJSON_Print(configGlass);
            if(i < (noOfGlasses-1))
                g_EspServerResponse += ',';          
            cJSON_Delete(configGlass); 
        }
    }
    g_EspServerResponse += ']';
    Sent_ServerResponse(g_EspServerResponse);
    return SUCCESS;
}
/*
@Function Name: UpdateGlassToUart()
@Parameters: char*,uint8_t*,uint8_t
@Return: None
@Functionality: Update Glass status via UART to Main Controller
*/
void UpdateGlass_ToUart(const char *status, const uint8_t *glassNumber, const uint8_t size){

    String txBuffer;
    // for(int i = 0;i < size;i++){
    //     TxBuffer = "GLASS";
    //     TxBuffer += glassNumber[i];        
    //     sprintf(glass[glassNumber[i]-1].g_status,status);
    //     TxBuffer += status;  
    //     Serial.println(TxBuffer);
    //     uart_tx(TxBuffer);
    // }
    if (size > 1)
    {
        txBuffer = "MODULE";  
        txBuffer += ",";
        txBuffer += "GLASS";    
        for(int i = 0;i < size;i++){      
            sprintf(g_Glass[glassNumber[i]-1].g_status,status);
        }
    }else
    {
        txBuffer = "GLASS"; 
        txBuffer += ",";   
        txBuffer += glassNumber[0];   
        sprintf(g_Glass[glassNumber[0]-1].g_status,status);
    }
    txBuffer += ",";
    txBuffer += status;    
    #if DEBUG_MODE
    Serial.println(txBuffer);
    #endif
    delay(5);
    UART1_Tx(txBuffer);
    UpdateStatus_ToApp(NUMBER_OF_Glasses);
}
/*
@Function Name: GetPayload()
@Parameters: String
@Return: cJSON
@Functionality: return HTTP POST Command Payload
*/
cJSON* Get_Payload(String request){

    char payload[500];
    String postRequest;
    
    postRequest = Return_SubString(request, request.indexOf("{"), request.indexOf("}")+1);
    postRequest.toCharArray(payload,postRequest.length()+1);
    #if DEBUG_MODE
    Serial.println(payload);    
    #endif
    cJSON *jsonPayload = cJSON_Parse(payload);

    return jsonPayload;
}
/*
@Function Name: PostGlassStatus()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST module Glass Status Command
*/
bool Post_GlassStatus(String request){    
    uint8_t id[6],numOfGlass = 0;
    char *glassStatus;

    cJSON *json_payload = Get_Payload(request); 
    if (json_payload == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        // Handle the error, possibly by exiting the function or program
        return ERROR;
    }     
    // access the JSON data
    glassStatus = Get_JSONStr_Item(json_payload,STATUS_KEY);
    if(!strcmp(glassStatus,"ON") || !strcmp(glassStatus,"OFF")){
        numOfGlass = Get_ID(json_payload,id);        
        UpdateGlass_ToUart(glassStatus,id,numOfGlass);
        cJSON_Delete(json_payload); 
    }
    else {
        #if DEBUG_MODE
        Serial.println(glassStatus);
        #endif
        cJSON_Delete(json_payload); 
        return ERROR;
    }
    return SUCCESS;
}

/*
@Function Name: PostTint()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST module Tint Adjustment Command
*/
bool Post_Tint(String request){
    String txBuffer;

    cJSON *jsonPayload = Get_Payload(request); 
    if (jsonPayload == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        // Handle the error, possibly by exiting the function or program
        return ERROR;
    }           
    // access the JSON data
    g_Tint = Get_JSONint(jsonPayload, TINT_KEY);         
    g_EspServerResponse = cJSON_Print(jsonPayload);   
    #if DEBUG_MODE
    Serial.println(g_Tint);
    #endif
    txBuffer = "MODULE";
    txBuffer += ",";
    txBuffer += "TINT";
    txBuffer += ",";
    txBuffer += g_Tint;  
    UART1_Tx(txBuffer);
    cJSON_Delete(jsonPayload);    
    Sent_ServerResponse(g_EspServerResponse);
    return SUCCESS;
}
/*
@Function Name: PostGlassRename()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST Glass Renaming Command
*/
bool Post_GlassRename(String request){
    char *newGlassName;
    char payload[500];
    String postRequest = "";
    uint8_t id;
    
    postRequest = Return_SubString(request, request.indexOf("["), request.indexOf("]")+1);
    postRequest.toCharArray(payload,postRequest.length()+1);
    #if DEBUG_MODE
    Serial.println(payload);  
    #endif  
    cJSON *root = cJSON_Parse(payload);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        } else {
            fprintf(stderr, "Error: Unable to parse JSON.\n");
        }
        return ERROR;
    }
    else{
        // Check if it's an array
        if (!cJSON_IsArray(root)) {
            fprintf(stderr, "Error: JSON is not an array.\n");
            cJSON_Delete(root);
            return ERROR;
        }
        else{   
            // Parse the JSON string  
            // Get the size of the array
            int arraySize = cJSON_GetArraySize(root);        
                // Iterate over the array elements
            g_EspServerResponse = '[';
            for (int i = 0; i < arraySize; ++i) {
                cJSON *item = cJSON_GetArrayItem(root, i);         
                if (item != NULL && cJSON_IsObject(item)){
                    id = Get_JSONint(item,ID_KEY);
                    newGlassName = Get_JSONStr_Item(item,NAME_KEY);
                    sprintf(g_Glass[id-1].g_name,newGlassName);
                    cJSON_AddStringToObject(item, "status", g_Glass[id-1].g_status);    
                    g_EspServerResponse += cJSON_Print(item);
                    if(i < (arraySize-1))
                        g_EspServerResponse += ',';         
                }
                else{
                    return ERROR;
                }
            }
            cJSON_Delete(root);
            g_EspServerResponse += ']';
            Save_GlassNames();
            Sent_ServerResponse(g_EspServerResponse);
            return SUCCESS;
        }    
    }
}
/*
@Function Name: PostModuleSsid()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST Module ssid Change Command
*/
bool Post_ModuleSsid(String request){    
    char *newSsid;
    cJSON *jsonPayload = Get_Payload(request);    
    if (jsonPayload == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        // Handle the error, possibly by exiting the function or program
        return ERROR;
    }  
    // access the JSON data
    newSsid = Get_JSONStr_Item(jsonPayload,NAME_KEY);
    #if DEBUG_MODE
    Serial.println(newSsid); 
    #endif                   

    if(!SaveAndCheck_Cred(newSsid,g_Password)){   
        g_EspServerResponse = cJSON_Print(jsonPayload);   
        Sent_ServerResponse(g_EspServerResponse); 
        #if DEBUG_MODE
        Serial.println("saved successfully");
        #endif
        delay(1000);
        strcpy(g_Ssid,newSsid);   
        cJSON_Delete(jsonPayload);             
        WiFi.softAP(g_Ssid,g_Password);    //Assigning SSID and password to AP
        return SUCCESS; 
    }
    else{        
        cJSON_Delete(jsonPayload);  
        #if DEBUG_MODE
        Serial.println("not saved");
        #endif
        return ERROR;
    }    
}
/*
@Function Name: PostModulePwd()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST Module Password Change Command
*/
bool Post_ModulePwd(String request){
    char *newPwd;
    char *oldPwd;
    cJSON *jsonPayload = Get_Payload(request);    
    if (jsonPayload == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        // Handle the error, possibly by exiting the function or program
        return ERROR;
    }            
    // access the JSON data
    oldPwd = Get_JSONStr_Item(jsonPayload,OLD_PWD_KEY);
    newPwd = Get_JSONStr_Item(jsonPayload,NEW_PWD_KEY); 
    #if DEBUG_MODE       
    Serial.println(oldPwd);                          
    Serial.println(newPwd);   
    #endif    
    if(!strcmp(oldPwd,g_Password)){
        if(!SaveAndCheck_Cred(g_Ssid,newPwd)){ 
            g_EspServerResponse = "";            
            Sent_ServerResponse(g_EspServerResponse);
            #if DEBUG_MODE     
            Serial.println("saved successfully");
            #endif
            delay(1000);
            strcpy(g_Password,newPwd);   
            cJSON_Delete(jsonPayload);   
            WiFi.softAP(g_Ssid,g_Password);    //Assigning SSID and password to AP 
            return SUCCESS; 
        }
        else{     
            cJSON_Delete(jsonPayload);          
            #if DEBUG_MODE
            Serial.println("not saved");
            #endif
            return ERROR;
        }
    }
    else{        
        cJSON_Delete(jsonPayload);   
        #if DEBUG_MODE
        Serial.println("old password is wrong");
        #endif
        return ERROR;
    }     
}
/*
@Function Name: Get_Tint()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST Module Password Change Command
*/
bool Get_Tint(String request){    
    cJSON *module = cJSON_CreateObject();
    if (module == NULL) {
        printf("Error: Memory allocation failed for cJSON object.\n");
        return ERROR;
    }
    cJSON_AddNumberToObject(module, "tint", g_Tint);         
    g_EspServerResponse = cJSON_Print(module);
    cJSON_Delete(module); 
    #if DEBUG_MODE
    Serial.println(g_EspServerResponse); 
    #endif
    Sent_ServerResponse(g_EspServerResponse);
    return SUCCESS;
}
/*
@Function Name: ValidatePostCommand()
@Parameters: String
@Return: None
@Functionality: Validating HTTP POST Command
*/
void Validate_Post_Cmd(String request){

    if((request.indexOf("/glass/status ") > 0)){
        if(Post_GlassStatus(request) == ERROR){          
            Sent_ServerError();
        }  
    }
    else if((request.indexOf("/tint ") > 0)){
        if(Post_Tint(request) == ERROR){         
            Sent_ServerError();
        }
    }
    else if((request.indexOf("/glass/rename ") > 0)){
        if(Post_GlassRename(request) == ERROR){    
            Sent_ServerError();
        }
    }
    else if((request.indexOf("/module/ssid ") > 0)){
        if(Post_ModuleSsid(request) == ERROR){   
            Sent_ServerError();
        }
    }
    else if((request.indexOf("/module/pwd ") > 0)){        
        if(Post_ModulePwd(request) == ERROR){
            Sent_ServerError();
        }
    }
}
/*
@Function Name: ValidateGetCommand()
@Parameters: String
@Return: None
@Functionality: Validating HTTP GET Commands
*/
void Validate_Get_Cmd(String request){ 

    if((request.indexOf("/glass ") > 0)){
        if(UpdateStatus_ToApp(NUMBER_OF_Glasses) == ERROR){            
            Sent_ServerError();
        }
    }
    else if((request.indexOf("/tint ") > 0)){     
        if(Get_Tint(request) == ERROR){        
            Sent_ServerError();
        }
    }
}
/*
@Function Name: check_client_request_msg()
@Parameters: None
@Return: None
@Functionality: checking client req messages
*/
void Check_ClientRequest_Msg(void){
    g_Client = g_Server.available();
    if (g_Client)  {                             // If a new client connects,
        // Serial.println("New Client.");          // print a message out in the serial port
        g_RequestCommand = "";
        while(g_Client.available()){
            char c = g_Client.read();
            g_RequestCommand += c;      

        }
        #if DEBUG_MODE
        Serial.println(g_RequestCommand);
        #endif
        g_HttpMethod = Return_SubString(g_RequestCommand,0,4);
        if(g_HttpMethod == "GET "){        
            Validate_Get_Cmd(g_RequestCommand);
        }
        else if(g_HttpMethod == "POST"){       
            Validate_Post_Cmd(g_RequestCommand);            
        }
    }
}

/******user code end******/
