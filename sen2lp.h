/*
  sen2lp.h - Library for converting sensor readings into Line Protocol that can be sent to InfluxDB
  Created by Juuso Solonen, November 16, 2024.
  Released into the public domain.
*/
#ifndef SEN2LP_H
#define SEN2LP_H

#include "Arduino.h"

enum FieldType {
    INT,
    FLT,
    STR,
    BOOL
};

struct Field {
    public:
        Field(const char* key, const char* value, FieldType fieldType);
        const char* key;
        char value[10]; 
        FieldType type;
        void formatValue(char* result);
};

class Sensor {
    public:
        Sensor(uint8_t vCount, const char* measurement, const char* tags, unsigned long time = 0); // vCount = amount of individual sensor readings
        ~Sensor();
        void setTimestamp(unsigned long newTimestamp); // Set optional timestamp for the line protocol
        Field* allocateData(uint8_t index, const char* key, const char* value, FieldType type); // Initialize inidivicual sensor
        void getFormattedFields(char* result);  // Method to get all Field data as a formatted string
        void getLineProtocol(char* result); // Get the sensor data as Line Protocol

    private:
        Field** data; // Array of sensor readings, or fields
        uint8_t size;
        const char* measurement;
        const char* tags; // Must be defined as comas separated list of <tagName>=<tagValue> e.g. "test1=value1,test2=value2"
        unsigned long timestamp;  // Store Unix timestamp for this field
};

#endif