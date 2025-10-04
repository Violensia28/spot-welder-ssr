# Android Flashing Guide

## Required Apps
- ESP32 Flash Download Tool (Play Store)
- File Manager

## Steps
1. Download firmware.bin and spiffs.bin
2. Open ESP Flash app
3. Select files:
   - firmware.bin @ 0x10000
   - spiffs.bin @ 0x110000
4. Set Flash Speed: 115200
5. Click FLASH button
6. Wait for completion

## Connection
- WiFi: SpotWelder
- Password: 12345678
- Browser: 192.168.4.1
