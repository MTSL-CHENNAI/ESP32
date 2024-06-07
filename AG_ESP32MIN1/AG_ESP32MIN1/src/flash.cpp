/********************Includes********************/
#include <EEPROM.h> // include library to read and write from flash memory
#include "flash.h"
#include "main.h"
#include "Preferences.h"

Preferences preferences;

/********************Macros********************/

/********************Global Variables********************/

/******user code begin******/

/*
@Function Name: Has_Credentials()
@Parameters: None
@Return: bool
@Functionality: Checking any credentials stored
*/
bool Has_Credentials(void){
  preferences.begin("Credentials", false); 
  String ssid = preferences.getString("SSID", ""); 
  String password = preferences.getString("Password", "");
  preferences.end();
  if (ssid == "" || password == ""){   
    #if DEBUG_MODE 
    Serial.println("No credentials stored previously");
    #endif
    return ERROR;
  }
  else{
    return SUCCESS;
  }
}
// bool Has_Credentials1(void){
//   return EEPROM.read(0) == EEPROM_KEY /* credentials marker */;
// }

/*
@Function Name: Save_GlsNames()
@Parameters: char*,char*
@Return: None
@Functionality: Save keys with glass names in permanent memory
*/
void Save_GlsNames(char *glass1, char *glass2) {
  preferences.begin("Credentials", false); 
  preferences.putString("glass1", glass1); 
  preferences.putString("glass2", glass2); 
  preferences.end();
}
void Save_GlsNames(char *glass1, char *glass2,char *glass1status, char *glass2status) {
  preferences.begin("Credentials", false); 
  preferences.putString("glass1", glass1); 
  preferences.putString("glass2", glass2); 
  preferences.putString("glass1status", glass1status); 
  preferences.putString("glass2status", glass2status);
  preferences.end();
}
/*
@Function Name: Save_Credentials()
@Parameters: char*,char*
@Return: None
@Functionality: Save keys with cred in permanent memory
*/
void Save_Credentials(char *ssid, char *pass) {
  preferences.begin("Credentials", false); 
  preferences.putString("SSID", ssid); 
  preferences.putString("Password", pass);
  preferences.end();
}
// void Save_Credentials1(char *ssid, char *pass) {
//   char buf[EEPROM_SIZE];
//   sprintf(buf, "%c%s:%s", EEPROM_KEY, ssid, pass);
//   EEPROM.writeString(0, buf);
//   EEPROM.commit();
// }

/*
@Function Name: Load_GlsNames()
@Parameters: char*,char*
@Return: None
@Functionality: load glass names from permanent memory
*/
void Load_GlsNames(char *glass1, char *glass2) {
  preferences.begin("Credentials", false); 
  String glass1_str = preferences.getString("glass1", ""); 
  String glass2_str = preferences.getString("glass2", "");
  preferences.end();
  glass1_str.toCharArray(glass1,glass1_str.length()+1);
  glass2_str.toCharArray(glass2,glass2_str.length()+1);
}
void Load_GlsNames(char *glass1, char *glass2,char *glass1status, char *glass2status) {
  preferences.begin("Credentials", false); 
  String glass1_str = preferences.getString("glass1", ""); 
  String glass2_str = preferences.getString("glass2", "");
  String glass1_status = preferences.getString("glass1status", ""); 
  String glass2_status = preferences.getString("glass2status", "");
  preferences.end();
  glass1_str.toCharArray(glass1,glass1_str.length()+1);
  glass2_str.toCharArray(glass2,glass2_str.length()+1);
  glass1_str.toCharArray(glass1status,glass1_status.length()+1);
  glass2_str.toCharArray(glass2status,glass2_status.length()+1);
}
/*
@Function Name: Load_Credentials()
@Parameters: char*,char*
@Return: None
@Functionality: load cred from permanent memory
*/
void Load_Credentials(char *ssid, char *pass) {
  preferences.begin("Credentials", false); 
  String ssid_str = preferences.getString("SSID", ""); 
  String password_str = preferences.getString("Password", "");
  preferences.end();
  ssid_str.toCharArray(ssid,ssid_str.length()+1);
  password_str.toCharArray(pass,password_str.length()+1);
}
// void Load_Credentials1(char *ssid, char *pass) {
//   if (!Has_Credentials()) {
//     return;
//   }
//   char buf[EEPROM_SIZE];
//   EEPROM.readString(1, buf, EEPROM_SIZE - 1);
//   int i = 0;
//   while ( i < EEPROM_SIZE && *(buf + i) != ':') {
//     *ssid = *(buf + i);
//     ssid++;
//     i++;
//   }
//   *ssid = '\0';
//   if (i == EEPROM_SIZE) {
//     return;
//   }
//   i++;

//   while ( i < EEPROM_SIZE && *(buf + i) != '\0') {
//     *pass = *(buf + i);
//     pass++;
//     i++;
//   }
//   *pass = '\0';
// }

/*
@Function Name: Erase_Credentials()
@Parameters: None
@Return: None
@Functionality: Erase date in permanent memory
*/
void Erase_Credentials(void) {
  String zero = "";
  preferences.begin("Credentials", false); 
  preferences.putString("SSID", zero); 
  preferences.putString("Password", zero);
  preferences.putString("glass1", zero); 
  preferences.putString("glass2", zero);
  preferences.putString("glass1status", zero); 
  preferences.putString("glass2status", zero);
}
// void Erase_Credentials1(void) {
//   EEPROM.write(0, 0);
//   EEPROM.commit();
// }
/******user code end******/