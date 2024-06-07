#ifndef _FLASH_H
#define _FLASH_H


/********************Macros********************/
// define the number of bytes you want to access
#define EEPROM_SIZE 95
#define EEPROM_SSID_PSWD_ADDR  0
#define SSID_LEN_MAX  32
#define PSWD_LEN_MAX  63
#define EEPROM_KEY  0x42

/********************Functions Declaration********************/
bool Has_Credentials(void);
void Save_Credentials(char *ssid, char *pass);
void Load_Credentials(char *ssid, char *pass);
void Erase_Credentials(void);
void Flash_Init(void);
void Save_GlsNames(char *glass1, char *glass2);
void Load_GlsNames(char *glass1, char *glass2);
void Save_GlsNames(char *glass1, char *glass2,char *glass1status, char *glass2status);
void Load_GlsNames(char *glass1, char *glass2,char *glass1status, char *glass2status);
#endif