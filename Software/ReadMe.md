## Setup

When the processor is first powered up the setup process starts. One of the first tasks is to connect to the Wi-Fi network. The built in LED blinks rapidly while trying to connect. If the connection is successful, the LED is on solid. If the connection is not successful, the RGB LED is set per the following paragraph.

Since there is no local display, if there is a problem during setup it is difficult to know what the problem is.  To help in this situation, I used the RGB LED that is on the FREENOVE WROOM board.  The following list shows the color of the RGB LED and the problem encountered.

1. Color Red = No Temperature Sensors work

2. Color Yellow = One Temperature Sensors does not work

3. Color White = File System Corrupted or Config File Corrupted

4. Color Blue = Wifi Could Not Connect

5. Color Cyan = MDNS Responder Error

6. Color Magenta = Failed to Set Time

7. Color Green all tests Pass
