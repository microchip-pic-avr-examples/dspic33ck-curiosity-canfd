# dspic33ck-curiosity-canfd
### Release Highlights

* Initial release - Refer README.md for summary and execution procedure


### Features Added\Updated

* CAN-FD communication using the on-chip CAN-FD peripheral in dsPIC33CK256MP508
* Loopback CAN-FD data frame if received message ID is 0x64 or 0x65 from the bus and ignore any other message frames 
* Green LED will blink if data message with 0x64 or 0x65 is received  
