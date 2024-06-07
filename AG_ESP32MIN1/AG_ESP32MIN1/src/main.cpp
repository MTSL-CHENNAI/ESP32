
/********************Includes********************/
#include <Arduino.h>
#include "uart.h"
#include "wifi_user.h"
#include "flash.h"
#include "main.h"
#include "esp_http_server.h"

/********************Macros********************/
/********************Global Variables********************/
/******user code begin******/
/*
@Function Name: setup()
@Parameters: None
@Return: None
@Functionality: Initialization
*/
void setup ()
{
  Serial.begin(9600);// initialize EEPROM with predefined size
  UART1_Init();
  //Erase_Credentials();   //for production should remove
  if(Has_Credentials() == ERROR) {
    AP_StartDefault_Cred();
  }
  else{  
    AP_StartSaved_cred();
  }  
  Find_Mac_Address();
  Set_Mac_Addr();
  Find_Mac_Address();
  Start_AP();
  Server_Start();
}
/*
@Function Name: loop()
@Parameters: None
@Return: None
@Functionality: Infinite loop
*/
void loop() {  
  UART1_Rx();
  Check_ClientRequest_Msg();    
  delay(50);
}

/******user code end******/
