/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#define CAN_POT_VALUE_GET_ID 0x10 
#define ADC_RESOLUTION 12

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/can/can1.h"
#include "mcc_generated_files/system/pins.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
/*
    Main application
*/
struct CAN_MSG_OBJ canMsg;

static enum CAN_BUS_ERRORS{
    CAN_ERROR_NONE,
    CAN_ERROR_ACTIVE,
    CAN_ERROR_WARNING,
    CAN_ERROR_PASSIVE
}canCurrentTxState, canCurrentRxState; 

static bool rxOverflowStatus;
static bool txWriteFail;

void CAN_BusWakeUpActivityCallback(void);
void CAN_RxBufferOverFlowCallback(void);

static void PrintDemoFeaturesMessage(void);
static void PrintCanMsgObjStruct(struct CAN_MSG_OBJ *rxCanMsg);
static void CheckRxErrors(void);
static void CheckTxErrors(void);

int main(void)
{
    bool msgStatus;
    SYSTEM_Initialize();
    
    CAN_FD_Driver.RxBufferOverFlowCallbackRegister(&CAN_RxBufferOverFlowCallback);
    CAN_FD_Driver.BusWakeUpActivityCallbackRegister(&CAN_BusWakeUpActivityCallback);
    canCurrentRxState = CAN_ERROR_NONE;
    canCurrentTxState = CAN_ERROR_NONE;
    
    PrintDemoFeaturesMessage();
    
    while(1)
    {    
        msgStatus = false;
        
        /*See if there is any data in RX FIFO*/
        if(CAN_FD_Driver.ReceivedMessageCountGet() > 0) 
        {
            CAN_FD_Driver.Receive(&canMsg);
            rxOverflowStatus = false;
            LED_GREEN_SetHigh();
            printf("\r\n[*] Received Message Frame:\r\n---------\r\n");
            PrintCanMsgObjStruct(&canMsg);
            msgStatus = true;
        }
        
        if(msgStatus)
        {
            /*Sending message with different ID than that of received message*/
            canMsg.msgId += 20,
                    
            /*Transmit back the received message*/
            printf("\r\n[*] Transmitting Message Frame:\r\n---------\r\n");
            PrintCanMsgObjStruct(&canMsg);
            msgStatus = CAN_FD_Driver.Transmit(CAN1_TX_TXQ, &canMsg);
            if(msgStatus == CAN_TX_MSG_REQUEST_SUCCESS)
            {
                txWriteFail = false;
                printf("Received CAN Message fed to Transmit FIFO successfully\r\n");
            }
            else
            {
                txWriteFail = true;
                printf("CAN Message Write to FIFO Failure\r\n");
            }
        }

        /*Check if any errors while receiving*/
        CheckRxErrors();

        /*Check if any errors while transmitting*/
        CheckTxErrors();
        
        /*Check if write or read FIFO errors*/
        if(rxOverflowStatus || txWriteFail)
        {
            LED_RED_SetHigh();
        }
        LED_GREEN_SetLow();
    }
}

void CAN_BusWakeUpActivityCallback(void)
{
    printf("\r\nBus Wake-Up Callback can be used \r\nto handle wake-up activities\r\n");
}

void CAN_RxBufferOverFlowCallback(void)
{
    LED_RED_SetHigh();
    printf("\r\n\r\nCAN Receive Buffer Overflow Occurred\r\n");
    printf("CAN Receive buffer overflow occurs if receive buffer \r\nis filled faster than it is being read\r\n");   
}

static void PrintDemoFeaturesMessage(void)
{
    printf("\r\n");
    printf("*******************************************************\r\n");
    printf("dsPIC33CK Curiosity CAN FD Demo\r\n");
    printf("*******************************************************\r\n");
    
    printf("* CAN-FD communication using the on-chip CAN-FD peripheral in dsPIC33CK256MP508\r\n");
    printf("* Loopback CAN-FD data frame if received  message ID is 0x64 or 0x65 from the bus\r\n");
    printf("* Green LED will blink if data message  with 0x64 or 0x65 is received and\r\n transmitted back successfully\r\n\r\n");

}

static uint8_t CAN_DlcToDataBytesGet(const enum CAN_DLC dlc)
{
    static const uint8_t dlcByteSize[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
    return dlcByteSize[dlc];
}

static void printCanDatainHex(struct CAN_MSG_OBJ *rxCanMsg)
{
    uint8_t i=0;
    for(i=0;i<CAN_DlcToDataBytesGet(rxCanMsg->field.dlc);i++)
    {
        printf("0x%X ",rxCanMsg->data[i]);
    }
    printf("\r\n");
}

static void PrintCanMsgObjStruct(struct CAN_MSG_OBJ *rxCanMsg)
{
    printf("------------------------------------------------------------------\r\n");
    printf("[*] Msg ID: 0x%lX\r\n", rxCanMsg->msgId);
    printf("[*] Msg Data:");
    printCanDatainHex(rxCanMsg);
    printf("[*] DLC: 0x%X\r\n", CAN_DlcToDataBytesGet(rxCanMsg->field.dlc));
    printf("[*] IdType: %s\r\n", rxCanMsg->field.idType == CAN_FRAME_STD ? "CAN_FRAME_STD" : "CAN_FRAME_EXT");
    printf("[*] FrameType: %s\r\n", rxCanMsg->field.frameType == CAN_FRAME_DATA ? "CAN_FRAME_DATA" : "CAN_FRAME_RTR");
    printf("[*] BRS: %s\r\n", rxCanMsg->field.brs == CAN_NON_BRS_MODE ? "CAN_NON_BRS_MODE" : "CAN_BRS_MODE");
    printf("[*] FormateType: %s\r\n", rxCanMsg->field.formatType == CAN_2_0_FORMAT ? "CAN_2_0_FORMAT" : "CAN_FD_FORMAT");
    printf("------------------------------------------------------------------\r\n");
}

static void CheckTxErrors(void)
{
    
    /**TX Errors**/
    
    /*If node reached TX Passive Error state*/
    if(CAN_FD_Driver.IsTxErrorActive())
    {
        if(canCurrentTxState != CAN_ERROR_PASSIVE)
        {
            printf("CAN node is in TX Error Passive state, 127 < Error Count < 256 \r\n");
            canCurrentTxState = CAN_ERROR_PASSIVE;
        }
    }
    
    /*If node reached TX Warning state*/
    else if(CAN_FD_Driver.IsTxErrorWarning())
    {
        if(canCurrentTxState != CAN_ERROR_WARNING)
        {
            printf("CAN node is in TX Error Warning state, 94 < Error Count < 128  \r\n");
            canCurrentTxState = CAN_ERROR_WARNING;
        }
    }
    
    /*If node reached TX Active Error state*/
    else if(CAN_FD_Driver.IsTxErrorActive())
    {
        if(canCurrentTxState != CAN_ERROR_ACTIVE)
        {
            printf("CAN node is in TX Error Active state, 0 < Error Count < 95  \r\n");
            canCurrentTxState = CAN_ERROR_ACTIVE;
        }
    }
    
    /*Reset status if no errors*/
    else
    {
        canCurrentTxState = CAN_ERROR_NONE;
        LED_RED_SetLow();
    }
    
    /*Set status to RED if any errors*/ 
    if(canCurrentRxState != CAN_ERROR_NONE)
    {
        LED_RED_SetHigh();
    }
    
    /*CAN node reaches Bus-Off state if error count is grater than 255*/
}

static void CheckRxErrors(void)
{
    
    /**RX Errors */
    
    /*If node reached RX Passive Error state*/
    if(CAN_FD_Driver.IsRxErrorPassive())
    {
        if(canCurrentRxState != CAN_ERROR_PASSIVE)
        {
            printf("CAN node is in RX Error Passive state, 127 < Error Count < 256\r\n");
            canCurrentRxState = CAN_ERROR_PASSIVE;
        }
    }
    
    /*If node reached RX Warning state*/
    else if(CAN_FD_Driver.IsRxErrorWarning())
    {
        if(canCurrentRxState != CAN_ERROR_WARNING)
        {
            printf("CAN node is in RX Error Warning state, 94 < Error Count < 128 \r\n");
            canCurrentRxState = CAN_ERROR_WARNING;
        }
    }
    
    /*If node reached RX Active Error state*/
    else if(CAN_FD_Driver.IsRxErrorActive())
    {
        if(canCurrentRxState != CAN_ERROR_ACTIVE)
        {
            printf("CAN node is in RX Error Active state, 0 < Error Count < 95 \r\n");
            canCurrentRxState = CAN_ERROR_ACTIVE;
        }
    }
    
    /*Reset status if no errors*/
    else
    {
        LED_RED_SetLow();
        canCurrentRxState = CAN_ERROR_NONE;
    }
    
    /*Set status to RED if any errors*/ 
    if(canCurrentRxState != CAN_ERROR_NONE)
    {
        LED_RED_SetHigh();
    }
}