/********************Includes********************/
#include <Arduino.h>
#include <HardwareSerial.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "wifi_user.h"
#include "main.h"



/********************Global Variables********************/
HardwareSerial g_MySerial(1);
String g_RxStr = "";
String g_TxStr = "";
char g_On[] = "ON";
char  g_Off[] = "OFF";
uint8_t g_StartIndex = 0,g_EndIndex = 0;
bool g_UartAck = 0;
double g_V,g_I;
double g_V1,g_I1;
double g_V2,g_I2;
char test1[] = "GLASS,1,1.22,7.65";
char test2[] = "GLASS,2,132.23,10.50";
/********************Shared Variables********************/

/******user code begin******/
/*
@Function Name: UART1_Init()
@Parameters: None
@Return: None
@Functionality: UART1 Initialization
*/
void UART1_Init(void) {
  g_MySerial.setRxBufferSize(SER_BUFF);
  g_MySerial.begin(BAUDRATE9600,SERIAL_8N1,RXpin,TXpin);
}
/*
@Function Name: UART1_Tx()
@Parameters: String
@Return: None
@Functionality: UART1 Transmission
*/
void UART1_Tx(String str) {

  g_TxStr = str;        
  char payload[100];
  if(g_MySerial.availableForWrite()){   
      str.concat("\n");
      str.toCharArray(payload,str.length()+1);
      #if DEBUG_MODE
      Serial.println(payload); 
      #endif
      g_MySerial.write(payload);
      g_UartAck = 1;
  }
  else{
    Serial.println("UART not available");
  }
}
/*
@Function Name: UART1_Rx_Buff()
@Parameters: String
@Return: None
@Functionality: UART1 received data handling
*/
void UART1_Rx_Buff(String str) {
  String subStr;
  char payload[100], data[10], *dptr;
  
  str.toCharArray(payload,str.length()+1);
  if(strstr(payload,"MODULE")){
      Serial.println("");
    if(strstr(payload,"GLASS") && strstr(payload,"ON")){
      Get_Uart_Status(1,g_On,g_V1,g_I1);
      Get_Uart_Status(2,g_On,g_V2,g_I2);
    }
    else if(strstr(payload,"GLASS") && strstr(payload,"OFF")){
      g_V2 = g_I2 = 0;
      g_V1 = g_I1 = 0;
      Get_Uart_Status(1,g_Off,0,0);
      Get_Uart_Status(2,g_Off,0,0);        
    }
    // else{

    // }
  }
  else if(strstr(payload,"GLASS")){
    switch(payload[6]){
      case '1':{
                if(strstr(payload,"ON"))
                  Get_Uart_Status(1,g_On,g_V1,g_I1);
                else if(strstr(payload,"OFF")){
                  g_V1 = g_I1 = 0;
                  Get_Uart_Status(1,g_Off,0,0);
                }
                else{                  
                  subStr = str.substring(8,12);
                  subStr.toCharArray(data,subStr.length()+1);
                  g_V1 = strtod(data,&dptr);
                  subStr = str.substring(13,18);
                  subStr.toCharArray(data,subStr.length()+1);
                  g_I1 = strtod(data,&dptr);
                  Get_Uart_VI(1,g_V1,g_I1);
                  #if DEBUG_MODE
                  Serial.println(g_V1);
                  Serial.println(g_I1);
                  #endif
                }
                }break;
      case '2':{
                if(strstr(payload,"ON"))
                  Get_Uart_Status(2,g_On,g_V2,g_I2);
                else if(strstr(payload,"OFF")){
                  g_V2 = g_I2 = 0;
                  Get_Uart_Status(2,g_Off,0,0);
                }
                else {                 
                  subStr = str.substring(8,12);
                  subStr.toCharArray(data,subStr.length()+1);
                  g_V2 = strtod(data,&dptr);
                  subStr = str.substring(13,18);
                  subStr.toCharArray(data,subStr.length()+1);
                  g_I2 = strtod(data,&dptr);
                  Get_Uart_VI(2,g_V2,g_I2);
                  #if DEBUG_MODE
                  Serial.println(g_V2);
                  Serial.println(g_I2);
                  #endif
                }
                }break;
    }
  }
  // else{

  // }
}

/*
@Function Name: UART1_Rx_Buff()
@Parameters: None
@Return: None
@Functionality: UART1 reception
*/
void UART1_Rx(void) {    
  if(g_MySerial.available()) {      
    g_RxStr = g_MySerial.readString(); 
    if((g_RxStr == "SUCCESS\n") && (g_UartAck == 1)){
      Serial.println("Message Sent Successfully"); 
      g_UartAck = 0;
    }
    else if(g_UartAck == 1){
      Serial.println("Message Not Sent Successfully"); 
      UART1_Tx(g_TxStr);      
      g_UartAck = 0;
    }
    else{
      #if DEBUG_MODE
      Serial.println(g_RxStr); 
      #endif
      UART1_Rx_Buff(g_RxStr);
    }
  }
}

/******user code end******/