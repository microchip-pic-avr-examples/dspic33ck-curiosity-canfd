# dspic33ck-curiosity-can v1.0.1

### Release Highlights

- Versions upgraded to latest public release :: Core - 5.4.11 and MCC Melody Core - 2.3.1

# dspic33ck-curiosity-canfd v1.0.0

### Release Highlights

- Initial release - Refer README.md for summary and execution procedure

### Features Added\Updated

- CAN-FD communication using the on-chip CAN-FD peripheral in dsPIC33CK256MP508
- Loopback CAN-FD data frame if received message ID is 0x64 or 0x65 from the bus and ignore any other message frames
- Green LED will blink if data message with 0x64 or 0x65 is received
