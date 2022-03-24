![image](images/microchip.jpg)

## dsPIC33CK Curiosity Development Board - CAN-FD Demo

![Board](images/board.jpg)

## Summary

This project demonstrates the CAN-FD setup and driver usage on dsPIC33CK Curiosity board.

Curiosity board will loopback the data messages if received message ID is 100 (0x64) or 101 (0x65) by changing the message IDs to 120 (0x78) and 121 (0x79) respectively. Green LED will blink if data is received and transmitted back successfully.

If messages other than above mentioned IDs are found, Curiosity will ignore it with the help of receive filter.

## Related Documentation

https://www.microchip.com/dsPIC33CK256MP508

## Software Used

- [MPLAB X v6.00](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide) or higher with [MCC Plugin](https://www.microchip.com/en-us/tools-resources/configure/mplab-code-configurator) installed
- [MPLAB Data Visualizer Plugin](https://www.microchip.com/en-us/tools-resources/debug/mplab-data-visualizer) or any other serial terminal
- [OptoLyzer Studio](https://www.microchip.com/en-us/tools-resources/develop/k2l-automotive-tools/optolyzer-studio) or any CAN-FD analyzer tool

## Hardware Used

- [dsPIC33CK Curiosity Board](https://www.microchip.com/en-us/development-tool/DM330030#)

- [MCP2542 Click Board](https://www.mikroe.com/mcp2542-click)

- Any CAN FD Analyzer with DB9 Serial port

## Setup

- Plug the [MCP2542 Click Board](https://www.mikroe.com/mcp2542-click) to microBUS B of Curiosity board
- Connect CAN-FD analyzer to [MCP2542 Click Board](https://www.mikroe.com/mcp2542-click) via DB9 serial connector
- Connect micro-USB cable provided along with curiosity board to PC for viewing debug messages.
- [Click here](images/hardware_setup.jpg) to view the hardware setup.

## Operation

- Open the `dspic33ck-curiosity-canfd.X` project in MPLAB X
- Build and program the device
- Configure CAN-FD analyzer clock to 1 Mbps nominal bit-rate and 5 Mbps data bit-rate
- Start the analyzer log and send CAN-FD data frames with message ID 101(0x64), 102(0x65) and 103(0x66)
- Sending CAN FD standard or extended data frame with message ID 100 (0x64) or 101 (0x65) on the CAN bus, the dsPIC33CK Curiosity will transmit back the same data with message ID to 120 (0x78) and 121 (0x79) respectively on the bus (Loopback). Green LED will blink acknowledging that data message is received and transmitted back successfully.
- Sending any message with ID other than 100 or 101, the dsPIC33CK Curiosity will ignore the message and no response is observed.
- Received and transmitted frames from Curiosity can also be viewed from the serial terminal for debug purposes

Following is a snip of the CAN analyzer log when messages with ID 0x64, 0x65 and 0x66 are sent from the analyzer one after the other<br>

![CAN-FD Analyzer log](images/analyzer_log.jpg)

**Summary of the above log:**

| Message Index in the log | Description                                                                                                                                   | Transmitter         | Type              | Message ID in decimal | Data Length in decimal | Data in HEX                                                                                                                                                                                     |
| ------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------- | ------------------- | ----------------- | --------------------- | ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0                        | Standard data frame sent from the analyzer with message ID 100                                                                                | CAN -FD Analyzer    | CAN FD Data Frame | 100 (Standard)        | 12                     | 4D 49 43 52 4F 43 48 49 50 00 00 00                                                                                                                                                             |
| 1                        | Loopback of the data frame with message ID 120                                                                                                | dsPIC33CK Curiosity | CAN FD Data Frame | 120 (Standard)        | 12                     | 4D 49 43 52 4F 43 48 49 50 00 00 00                                                                                                                                                             |
| 2                        | Extended data frame sent from the analyzer with message ID 101                                                                                | CAN -FD Analyzer    | CAN FD Data Frame | 101 (Extended)        | 64                     | 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 20 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 20 31 32 33 34 35 36 37 38 39 30 |
| 3                        | Loopback of the data frame with message ID 121                                                                                                | dsPIC33CK Curiosity | CAN FD Data Frame | 121 (Extended)        | 64                     | 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 20 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 20 31 32 33 34 35 36 37 38 39 30 |
| 4                        | Extended data frame sent from the analyzer with message ID 103. This message will be ignored from the curiosity board as per filter settings. | CAN -FD Analyzer    | CAN FD Data Frame | 103 (Extended)        | 12                     | 4D 49 43 52 4F 43 48 49 50 00 00 00                                                                                                                                                             |

CAN-FD frames received by the curiosity board are printed in the serial terminal for debug purposes

## Creating the project using MCC - Melody

- Create a project in MPLAB X for the device dsPIC33CK256MP508
- Load MCC plugin and select `MCC Melody` from the content manager

**CAN FD Configuration**

- Under `Device Resources` load `CAN FD` driver which auto-loads `CAN1 PLIB` dependency
- Set custom name to `CAN FD` <br>
  ![custom name](images/can_custom_name.jpg)
- Clock and Nominal Bit-Rate Settings: Set CAN clock to 20 MHz, nominal bit rate to 1Mbps and sample point to 80%<br>
  ![step1-step2](images/step1_step2.jpg)
- Data Bit-Rate Settings: Enable data bit rate, set bit rate to 5Mbps, sample point to 75%. Enable ISO CRC and enable error interrupt <br>
  ![step3-step4](images/step3_step4.jpg)
- FIFO Configuration: Enable TXQ, set depth as 32, payload bytes as 64. Enable FIFO1, set depth as 32, payload bytes as 64 and operation to RX<br>
  ![step5](images/step5.jpg)
- Receive Filter Configuration: Enable Filter0 and type message IDs `0x64, 0x65, 0x64x,0x65x`. This will filter all messages allowing only standard and extended messages with ID 0x64 and 0x65<br>
  ![step6](images/filter.jpg)

**UART Configuration**

- Load UART driver and select `UART1` as dependency
- Make following configurations in UART driver <br>
  ![uart settings](images/uart.jpg)

**PIN Configuration**

- Go to `Pin Grid View`
- CAN FD pins: Select RD9 for CAN1TX and RD12 for CAN1RX
- UART Pins: Select RD4 as U1TX and RD3 as U1RX<br>
  ![pin selection](images/pin_selection.jpg)
- LED Pins: Select RE14 and RE15 as GPIO Output<br>
  ![rgb pin selection](images/rgb_pins.jpg)
- Go to pins table view and change custom name of RE14 to `LED_GREEN` and RE15 to `LED_RED`<br>
  ![rgb custom name](images/rgb_custom_name.jpg)

**Application**

- Generate the code
- Copy and replace the entire main.c file contents from [here](dspic33ck-curiosity-canfd.X/main.c)

## OptoLyzer Studio Activity File

If the CAN-FD analyzer uses `OptoLyzer Studio`, then a pre-configured activity file can be found [here](optolyzerActivityFile/)
