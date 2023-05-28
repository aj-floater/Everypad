# Everypad Arduino Code

![IMG_1499](https://github.com/aj-floater/Everypad/assets/80073564/9805f9a3-6e93-44c4-b0c6-3185f18d3d7c)


This repository contains Arduino code for the Everypad project. The code is designed to run on an Arduino Nano 33 BLE board and control the Everypad device.

For more details, please message me, below is an AI generated overview for basic understanding.

## Dependencies
The code relies on the following libraries:

- **LiquidCrystal**: This library is used for interfacing with the LCD display.
- **ArduinoBLE**: This library provides Bluetooth Low Energy (BLE) functionality.
- **ArduinoSTL**: This library provides the vector container implementation.

Please make sure to install these libraries before compiling and uploading the code to your Arduino board.

## Hardware Setup
The code assumes the following hardware setup:

- LCD Display: The display is connected to the Arduino pins `rs`, `en`, `d4`, `d5`, `d6`, and `d7`.
- LEDs: An LED is connected to pin 9 for visual indication.
- Analog Inputs: Four analog inputs (`A0`, `A1`, `A2`, and `A3`) are used for reading sensor values.
- Buttons: Two buttons are connected to pins 10 and 11 for user input.

Make sure to wire your Arduino board according to the specifications mentioned above.

## Functionality
The code provides the following functionality:

1. **Main Menu**: Displays a main menu on the LCD, with the option to initiate a BLE scan.
2. **Scanning**: Scans for nearby BLE devices and displays their names on the LCD. Allows navigation through the list of discovered devices using buttons.
3. **Movement**: Establishes a connection with a selected BLE device and sends sensor data to it. Displays the connected device's name on the LCD.

The code uses the LiquidCrystal library for interfacing with the LCD, ArduinoBLE library for BLE communication, and ArduinoSTL library for vector operations.

## Usage
1. Connect your Arduino board to your computer.
2. Compile and upload the code to the Arduino board.
3. Make sure the hardware setup is correctly connected.
4. Power on the Arduino board.
5. The LCD will display the main menu. Use the buttons to navigate and select options.
6. Select the BLE scan option to discover nearby devices.
7. Navigate through the list of discovered devices and select the desired device.
8. If the connection is successful, the LCD will display a success message.
9. The code will then enter the movement mode and start sending sensor data to the connected device.
10. If the connection is lost, the code will return to the scanning mode.
11. Repeat steps 6-10 to connect to a different device.

Make sure to refer to the hardware documentation and adjust the code accordingly if your hardware setup differs from the assumed setup.

**Note**: This README provides a brief overview of the Arduino code for the Everypad project. For more detailed information, please refer to the comments in the code itself.

Enjoy using Everypad!
