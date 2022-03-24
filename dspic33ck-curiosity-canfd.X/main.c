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
#include "mcc_generated_files/adc/adc1.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
/*
    Main application
*/
struct CAN_MSG_OBJ gMsg;

bool busWakeUpOccured = false;
bool rxOverFlowIntOccured = false;

bool txErrorActiveOccured = false;
bool txErrorWarningOccured = false;
bool txErrorPassiveOccured = false;

bool rxErrorActiveOccured = false;
bool rxErrorWarningOccured = false;
bool rxErrorPassiveOccured = false;

static uint8_t CAN_DlcToDataBytesGet(const enum CAN_DLC dlc)
{
    static const uint8_t dlcByteSize[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
    return dlcByteSize[dlc];
}

void printCanMsgObjStruct(struct CAN_MSG_OBJ *rxCanMsg)
{
    printf("------------------------------------------------------------------\r\n");
    printf("[*] Msg ID: %lu\r\n", rxCanMsg->msgId);
    printf("[*] Msg Data: %s\r\n", rxCanMsg->data);
    printf("[*] DLC: %d\r\n", CAN_DlcToDataBytesGet(rxCanMsg->field.dlc));
    printf("[*] IdType: %s\r\n", rxCanMsg->field.idType == CAN_FRAME_STD ? "CAN_FRAME_STD" : "CAN_FRAME_EXT");
    printf("[*] FrameType: %s\r\n", rxCanMsg->field.frameType == CAN_FRAME_DATA ? "CAN_FRAME_DATA" : "CAN_FRAME_RTR");
    printf("[*] BRS: %s\r\n", rxCanMsg->field.brs == CAN_NON_BRS_MODE ? "CAN_NON_BRS_MODE" : "CAN_BRS_MODE");
    printf("[*] FormateType: %s\r\n", rxCanMsg->field.formatType == CAN_2_0_FORMAT ? "CAN_2_0_FORMAT" : "CAN_FD_FORMAT");
    printf("------------------------------------------------------------------\r\n");
}

static enum CAN_TX_MSG_REQUEST_STATUS canMessageWrite(
    int fifoNum,
    uint32_t msgIde, 
    bool idType, 
    bool formatType,  
    bool frameType,
    uint8_t len, 
    bool brs, 
    uint8_t *data
) 
{   
    gMsg.msgId = msgIde;
    gMsg.field.formatType = formatType;
    gMsg.field.frameType = frameType;
    gMsg.field.idType = idType;
    gMsg.field.dlc = len;
    gMsg.field.brs = brs;
    gMsg.data = data;
    return CAN_FD.Transmit(fifoNum, &gMsg);
}

bool readCanMsgObjTasks(struct CAN_MSG_OBJ *rxMsg)
{
    bool status = false;
    if(CAN_FD.ReceivedMessageCountGet() > 0) 
    {
        CAN_FD.Receive(rxMsg);
        printf("[*] Following Message received:\r\n---------\r\n");
        printCanMsgObjStruct(rxMsg);
        status = true;
    }
    
    return status;
}

void CAN_BusWakeUpActivityHandler(void)
{
    printf("\r\nBus Wake-Up Callback can be used \r\nto handle wake-up activities\r\n");
    busWakeUpOccured = true;
}

void CAN_RxBufferOverFlowHandler()
{
    printf("\r\n\r\nCAN Receive Buffer Overflow Occurred\r\n");
    printf("CAN Receive buffer overflow occurs if receive buffer \r\nis filled faster than it is being read\r\n");
    rxOverFlowIntOccured = true;    
}

void PrintIfAnyRxErrors(void)
{
    /**RX Errors */
    
    //Print if node reached RX Active Error state
    if(CAN_FD.IsRxErrorActive()&&(!rxErrorActiveOccured))
    {
        printf("RX Active Error Occurred, 0 < Error Count < 95 \r\n");
        rxErrorActiveOccured = true;
    }
    
    //Print if node reached RX Warning state
    if(CAN_FD.IsRxErrorWarning()&&(!rxErrorWarningOccured))
    {
        printf("RX Warning Error Occurred, 94 < Error Count < 128 \r\n");
        rxErrorWarningOccured = true;
    }
    
    //Print if node reached RX Passive Error state
    if(CAN_FD.IsRxErrorPassive()&&(!rxErrorPassiveOccured))
    {
        printf("RX Passive Error Occurred, 127 < Error Count < 256\r\n");
        rxErrorPassiveOccured = true;
    }
}

void PrintIfAnyTxErrors(void)
{
    /**TX Errors */
    
    //Print if node reached TX Active Error state
    if(CAN_FD.IsTxErrorActive()&&(!txErrorActiveOccured))
    {
        printf("TX Active Error Occurred, 0 < Error Count < 95  \r\n");
        txErrorActiveOccured = true;
    }
    
    //Print if node reached TX Warning state
    if(CAN_FD.IsTxErrorWarning()&&(!txErrorWarningOccured))
    {
        printf("TX Warning Error Occurred, 94 < Error Count < 128  \r\n");
        txErrorWarningOccured = true;
    }
    
    //Print if node reached TX Passive Error state
    if(CAN_FD.IsTxErrorPassive()&&(!txErrorPassiveOccured))
    {
        printf("TX Passive Error Occurred, 127 < Error Count < 256 \r\n");
        txErrorPassiveOccured = true;
    }
    
    //CAN node reaches Bus-Off state if error count is grater than 255
}

void ADC1_ChannelCallback (enum ADC_CHANNEL channel, uint16_t adcVal)
{
    bool msgStatus;
    enum ADC_DATA_FRAME {
        ADC_RESOLUTION_BYTE,
        ADC_VALUE_HIGH_BYTE,
        ADC_VALUE_LOW_BYTE
    };
    uint8_t data[3] = {ADC_RESOLUTION, adcVal >> 8 , adcVal & 0xFF}; 
    if(channel == POT_Channel)
    {
        gMsg.field.dlc = DLC_3;
        gMsg.field.frameType = CAN_FRAME_DATA;
        gMsg.field.formatType = CAN_FD_FORMAT;
        gMsg.data = &data[0];
        /*Transmit back the received message*/
        msgStatus = canMessageWrite(
                CAN1_TX_TXQ                 //Transmit FIFO 
                ,gMsg.msgId                 //CAN Message ID
                ,gMsg.field.idType          //Standard or Extended ID
                ,gMsg.field.formatType      //CAN_FD_FORMAT for CAN-FD frames
                ,gMsg.field.frameType       //Data frame or RTR frame
                ,gMsg.field.dlc             //Data length
                ,gMsg.field.brs             //Specifies if bit-rate switching is enabled 
                ,(uint8_t *)gMsg.data       //Data 
                );
        if(msgStatus == CAN_TX_MSG_REQUEST_SUCCESS)
        {
            printf("ADC value fed to CAN FIFO successfully\r\n");
        }
        else
        {
            printf("CAN Message Write to FIFO Failure\r\n");
        }
    }
}

int main(void)
{
    bool msgStatus;
    SYSTEM_Initialize();
    CAN_FD.RxBufferOverFlowCallbackRegister(&CAN_RxBufferOverFlowHandler);
    CAN_FD.BusWakeUpActivityCallbackRegister(&CAN_BusWakeUpActivityHandler);
    while(1)
    {    
        /*Check if any errors while receiving*/
        PrintIfAnyRxErrors();
        
        /*See if there is any data in RX FIFO*/
        msgStatus = readCanMsgObjTasks(&gMsg);
        if(msgStatus)
        {
            printf("CAN Message Received Successfully\r\n");
            printf("-----------------------------\r\n");
            printf("\r\n\r\n****** CAN Transmit ******\r\n");
            
            if(gMsg.field.frameType == CAN_FRAME_RTR)
            {
                if(gMsg.msgId == CAN_POT_VALUE_GET_ID)
                {
                    ADC1.SoftwareTriggerEnable();
                }
            }
            else
            {
                /*Transmit back the received message*/
                msgStatus = canMessageWrite(
                        CAN1_TX_TXQ,                 //Transmit FIFO 
                        gMsg.msgId,                 //CAN Message ID
                        gMsg.field.idType,          //Standard or Extended ID
                        gMsg.field.formatType,      //CAN_FD_FORMAT for CAN-FD frames
                        gMsg.field.frameType,       //Data frame or RTR frame
                        gMsg.field.dlc,             //Data length
                        gMsg.field.brs,             //Specifies if bit-rate switching is enabled 
                        (uint8_t *)gMsg.data        //Data 
                        );
                if(msgStatus == CAN_TX_MSG_REQUEST_SUCCESS)
                {
                    printf("Received CAN Message fed to Transmit FIFO successfully\r\n");
                }
                else
                {
                    printf("CAN Message Write to FIFO Failure\r\n");
                }
            }
        }
        
        /*Check if any errors while transmitting*/
        PrintIfAnyTxErrors();
           
    }
}