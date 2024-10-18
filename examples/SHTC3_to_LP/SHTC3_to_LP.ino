// Libraries
#include <sen2lp.h>
#include "Adafruit_SHTC3.h"

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
const int PwrSht = 12;

float hum;  // Stores humidity value
float temp; // Stores temperature value
Field* sensorHum = nullptr;
Field* sensorTemp = nullptr;
Sensor* sensor = nullptr;

const String measurement = "test_weather2";
const String tags = "room=test_room_shtc";

void setup() {
    Serial.begin(9600);
    pinMode(PwrSht, OUTPUT);
    digitalWrite(PwrSht, HIGH);

    // Initialize the sensor
    if (!shtc3.begin()) {
        Serial.println("Couldn't find SHTC3");
        while (1) delay(1);
    }

    // Create a Sensor object with space for 2 individual sensor readings
    unsigned long customTimestamp = 1609459200;  // Example Unix timestamp
    sensor = new Sensor(2, measurement, tags, customTimestamp);

    // Allocate data for the struct at index 0 and 1
    // NOTE: the library expects the 'value' to be in string form
    sensorHum = sensor->allocateData(0, "humidity", "0", FieldType::FLT);
    sensorTemp = sensor->allocateData(1, "temperature", "0", FieldType::FLT);
}

void loop() {
    // Read data and store it to variables hum and temp
    sensors_event_t humidity, tempEvent;
    shtc3.getEvent(&humidity, &tempEvent);

    // Test prints to compare values with the Line Protocol result
    Serial.print("Temperature is ");
    Serial.print(tempEvent.temperature);
    Serial.print(" °C and relative humidity is ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" %");

    // Convert the float values into String and assign them to the sensor values
    String humidityStr = String(humidity.relative_humidity, 2);  // 2 decimal places
    String tempStr = String(tempEvent.temperature, 2);  // 2 decimal places

    // Update the sensor values
    sensorHum->value = humidityStr;  // Set the humidity value (as String)
    sensorTemp->value = tempStr;      // Set the temperature value (as String)

    // Prepare a buffer to hold the formatted string (Line Protocol)
    String result;

    // Get the line protocol formatted data
    sensor->getLineProtocol(result);  // This updates the 'result' string with the formatted data
    Serial.println(result);  // Print result to serial monitor

    delay(2000);  // Delay 2 seconds
}
