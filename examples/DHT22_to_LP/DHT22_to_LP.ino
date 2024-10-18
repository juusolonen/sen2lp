// Libraries
#include <DHT.h>
#include <sen2lp.h>

// Constants
#define DHTPIN 2  
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE); 

String measurement = "test_weather";
String tags = "room=test_room";

// Variables
float hum;  // Stores humidity value
float temp; // Stores temperature value
Field* sensorHum = nullptr;
Field* sensorTemp = nullptr;
Sensor* sensor = nullptr;

void setup() {
    Serial.begin(9600);
    dht.begin();

    unsigned long customTimestamp = 1609459200;  // Example Unix timestamp
    // Create a Sensor object with space for 2 individual sensor readings
    sensor = new Sensor(2, measurement, tags, customTimestamp);

    // Allocate data for the struct at index 0 and 1
    // NOTE: the library expects the 'value' to be in string form
    sensorHum = sensor->allocateData(0, "humidity", "0", FieldType::INT);
    sensorTemp = sensor->allocateData(1, "temperature", "0", FieldType::FLT);
}

void loop() {
    // Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    String humStr = String(hum, 2);  // Convert float to String with 2 decimal places
    String tempStr = String(temp, 2); // Convert float to String with 2 decimal places

    // Store the String values into the Field objects
    humStr.toCharArray(sensorHum->value, sensorHum->value.length());
    tempStr.toCharArray(sensorTemp->value, sensorTemp->value.length());

    // Prepare a buffer to hold the formatted string (Line Protocol)
    String result = ""; 

    sensor->getLineProtocol(result);  // Get line protocol formatted data

    Serial.println(result);  // Print result to serial monitor

    delay(2000);  // Delay 2 sec.
}
