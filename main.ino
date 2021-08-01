#include <LiquidCrystal.h>
#include </Applications/Arduino.app/Contents/Java/libraries/LiquidCrystal/src/LiquidCrystal.h>
#include "/Users/archiejames/Documents/Arduino/libraries/ArduinoBLE/src/ArduinoBLE.h"
#include "/Users/archiejames/Documents/Arduino/libraries/ArduinoBLE/src/BLEDevice.h"
#include "/Users/archiejames/Documents/Arduino/libraries/ArduinoSTL/src/vector.cpp"
#include "/Users/archiejames/Desktop/Coding/Everypad/position.h"

#define MAIN_MENU 0
#define SCANNING 1
#define MOVEMENT 2

using namespace std;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte Character[8] = {
	0b00000,
	0b00000,
	0b10101,
	0b00100,
	0b11111,
	0b00100,
	0b10101,
	0b00000
};

vector<BLEDevice> existing_peripherals;

void setup() {
	Serial.begin(9600);
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	lcd.createChar(0, Character);

	pinMode(9, OUTPUT);

	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);

	pinMode(10, INPUT_PULLUP);
	pinMode(11, INPUT_PULLUP);

	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting BLE failed!");

	while (1);
	}

	Serial.println("BLE Central scan");
}

int activity = MAIN_MENU;
int peripheral_pointer = 0;
float previousMillis = 0;
int cursor_pos = 0;
bool connected = false;

float previousPulseMillis;
float time_since_last_pulse;

BLECharacteristic write_characteristic;

void customDelay(float millis_input){
	float initialMillis = millis();
	while (millis() < initialMillis + millis_input){
		// keep refreshing screen
		float currentMillis = millis();
		time_since_last_pulse = currentMillis - previousPulseMillis;
		if (time_since_last_pulse > 50){
			previousPulseMillis = currentMillis;
			if (digitalRead(9) == LOW) digitalWrite(9, HIGH);
			else if (digitalRead(9) == HIGH) digitalWrite(9, LOW);
		}
	}
}

void convertToFloat(float* input, float midpoint, float boundary){
	*input -= midpoint;
	*input /= boundary;
}

void loop() {
	float currentMillis = millis();
	time_since_last_pulse = currentMillis - previousPulseMillis;
	if (time_since_last_pulse > 50){
		previousPulseMillis = currentMillis;
		if (digitalRead(9) == LOW) digitalWrite(9, HIGH);
		else if (digitalRead(9) == HIGH) digitalWrite(9, LOW);
	}

	if (activity == MAIN_MENU) {
		lcd.setCursor(2, 0);
		lcd.print("BLE Scan");
		lcd.setCursor(0, 0);
		lcd.write(byte(0));

		if (digitalRead(11) == LOW){
			lcd.setCursor(0, 1);
			activity = SCANNING;
			lcd.clear();
			BLE.scan();
		}
	}
	if (activity == SCANNING) {
		// check if a peripheral has been discovered
		BLEDevice peripheral = BLE.available();

		// if a new peripheral is found and has a local name
		if (peripheral && peripheral.hasLocalName()) {
			// check if existing_peripherals has any peripherals in it already
			if (existing_peripherals.size() == 0)
				existing_peripherals.push_back(peripheral);
			else { // if existing_peripherals has no peripherals in it already then ...
				bool already_discovered = false;
				for (int i = 0; i < existing_peripherals.size() && !already_discovered; i++){
					if (existing_peripherals[i].address() == peripheral.address())
					already_discovered = true;
				} // go through each peripheral to check if the new peripheral has already been discovered
				if (!already_discovered) // if the peripheral hasn't already been discovered then add it to the vector
					existing_peripherals.push_back(peripheral);
			}
		}

		lcd.setCursor(0, cursor_pos);
		lcd.write(byte(0));

		int ep_size = existing_peripherals.size();
		int final_item_in_ep = ep_size - 1;
		if (ep_size > 0){
			if (ep_size == 1){
				lcd.setCursor(2, 0);
				lcd.write(existing_peripherals[0].localName().c_str());
			}
			else{			
				float time_since_last_analog_input = millis() - previousMillis;
				if (time_since_last_analog_input > 500){
					if (analogRead(A3) > 700){
						if (cursor_pos == 0)
							peripheral_pointer -= 1;
						else cursor_pos = 0;
						lcd.clear();
						previousMillis = millis();
					}
					if (analogRead(A3) < 400){
						if (cursor_pos == 1 && peripheral_pointer+cursor_pos != final_item_in_ep)
							peripheral_pointer += 1;
						else cursor_pos = 1;
						lcd.clear();
						previousMillis = millis();
					}
				}

				// clamps peripheral pointer to prevent vector access errors
				if (peripheral_pointer > final_item_in_ep) peripheral_pointer = final_item_in_ep;
				if (peripheral_pointer < 0) peripheral_pointer = 0;
				lcd.setCursor(2, 0);
				lcd.write(existing_peripherals[peripheral_pointer].localName().c_str());
				lcd.setCursor(2, 1);
				lcd.write(existing_peripherals[peripheral_pointer+1].localName().c_str());
			}
			
			if (digitalRead(11) == LOW) {
				// attempt connection to device
				lcd.clear();
				BLE.stopScan();
				bool connected = false;

				BLEDevice PotentialBluetoothReceiver = existing_peripherals[peripheral_pointer+cursor_pos];
				Serial.println(PotentialBluetoothReceiver.localName());
				if (PotentialBluetoothReceiver.connect()) {
					if (PotentialBluetoothReceiver.discoverAttributes()){
						BLEService service = PotentialBluetoothReceiver.service("FFE0");
						if (service) {
							// loop the characteristics of the service and explore each
							BLECharacteristic characteristic = service.characteristic("FFE1");
							if (characteristic){
								connected = true;
								write_characteristic = characteristic;
							}
							else {Serial.println("error with characteristic"); PotentialBluetoothReceiver.disconnect();}
						}
						else {Serial.println("error at service query"); PotentialBluetoothReceiver.disconnect();}
					}
					else {Serial.println("error when discovering attributes"); PotentialBluetoothReceiver.disconnect();}
				}
				else {Serial.println("error at connection stage"); PotentialBluetoothReceiver.disconnect();}

				if (connected){
					lcd.setCursor(0, 0);
					lcd.write("Connection");
					lcd.setCursor(0, 1);
					lcd.write("Success!");
					customDelay(2000);
					lcd.clear();
					activity = MOVEMENT; // exit condition
				} else {
					lcd.setCursor(0, 0);
					lcd.write("Connection");
					lcd.setCursor(0, 1);
					lcd.write("Failure");
					customDelay(2000);
					lcd.clear();
					BLE.scan();
					return;
				}
			}
		}
		else {
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.write("Scanning for BLE");
			lcd.setCursor(0,1);
			lcd.write("devices...");
			lcd.clear();
		}
	}
	if (activity == MOVEMENT){
		BLEDevice* BluetoothReciever = &existing_peripherals[peripheral_pointer+cursor_pos];
		lcd.setCursor(0, 0);
		lcd.write("Sending Data to");
		lcd.setCursor(0, 1);
		lcd.write(BluetoothReciever->localName().c_str());

		float axes[4];
		axes[0] = analogRead(A3);
		axes[1] = analogRead(A2);
		axes[2] = analogRead(A0);
		axes[3] = analogRead(A1);

		convertToFloat(&axes[0], 530, 300);
		convertToFloat(&axes[1], 630, 250);
		convertToFloat(&axes[2], 400, 400);
		convertToFloat(&axes[3], 660, 400);
		
		String axes_s[4];
		for (int i = 0; i < 4; i++)
			axes_s[i] = String(axes[i]);
		String send_data = "<" + 
			axes_s[0] + ":" + 
			axes_s[1] + ":" + 
			axes_s[2] + ":" + 
			axes_s[3] + ":>";
		// Serial.println(send_data);

		if (BluetoothReciever->connected()){
			write_characteristic.writeValue((byte)0x01, false);
			Serial.println("connected");
		} else {
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.write("Disconnected");
			customDelay(2000);
			lcd.clear();
			activity = SCANNING;
			return;
		}

	}
}