/*
  sen2lp.h - Library for converting sensor readings into Line Protocol that can be sent to InfluxDB
  Created by Juuso Solonen, November 16, 2024.
  Released into the public domain.
*/
#include "Arduino.h"
#include "sen2lp.h"

Field::Field(const String& fkey, const String& fvalue, FieldType fieldType)
    : key(fkey), value(fvalue), type(fieldType) {}

void Field::formatValue(String &result) {
    switch (this->type) {
        case INT: {
            float floatVal = atof(this->value.c_str());
            int intVal = (int)floatVal;
            String intStr = String(intVal);
            result += intStr + "i";  // Concatenate integer as string
            break;
        }
        case STR: {
            result += "\"";
            result += this->value;
            result += "\"";
            break;
        }
        case BOOL: {
            // Simplify the boolean comparison by using String comparison
            if (this->value == "true" || this->value == "false" || this->value == "T" || this->value == "F" 
                || this->value == "True" || this->value == "False" || this->value == "t" || this->value == "f"
                || this->value == "TRUE" || this->value == "FALSE") {
                result += this->value;
            } else {
                int val = atoi(this->value.c_str());
                result += (val > 0 ? "true" : "false");
            }
            break;
        }
        default: {
            float fVal = atof(this->value.c_str());
            String fStr = String(fVal, 2);  // 2 decimal places
            result += fStr;
            break;
        }
    }
}


Sensor::Sensor(uint8_t vCount, const String& measurement, const String& tags, unsigned long time)
    : measurement(measurement), tags(tags), timestamp(time), size(vCount) {
    data = new Field*[size];  // Allocate space for the sensor readings

    for (uint8_t i = 0; i < size; ++i) {
        data[i] = nullptr;  // Initialize each element to nullptr
    }
}


Sensor::~Sensor() {
    for (uint8_t i = 0; i < size; ++i) {
        delete data[i];
    }
    delete[] data;
}

void Sensor::setTimestamp(unsigned long newTimestamp) {
    this->timestamp = newTimestamp;
}

Field* Sensor::allocateData(uint8_t index, const String key, const String value, FieldType type) {
    if (index < size) {
        if (data[index] == nullptr) {
            data[index] = new Field(key, value, type);
            return data[index];
        } else {
            Serial.println("Data already allocated at index");
        }
    } else {
        Serial.println("Index out of bounds.");
    }
    return nullptr;
}

void Sensor::getFormattedFields(String &result) {
    result = "";  // Reset the result string
    for (uint8_t i = 0; i < size; ++i) {
        if (data[i] != nullptr) {
            result += data[i]->key;
            result += "=";
            result += data[i]->value;
            if (i < size - 1) {
                result += ",";
            }
        }
    }
}

void Sensor::getLineProtocol(String &result) {
    result = "";  // Reset the result string
    result += measurement;
    result += ",";
    result += tags;

    for (uint8_t i = 0; i < size; ++i) {
        if (data[i] != nullptr) {
            if (i == 0) {
                result += " ";
            }
            result += data[i]->key;
            result += "=";
            if (data[i]->type != FieldType::FLT) {
                data[i]->formatValue(result);  // Append formatted value
            } else {  // Hack-ish fix for FLT type formatting
                result += data[i]->value;
            }
            if (i < size - 1) {
                result += ",";
            }
        }
    }

    if (this->timestamp > 0) {
        String timestampStr = String(this->timestamp);
        result += " ";
        result += timestampStr;
    }
}
