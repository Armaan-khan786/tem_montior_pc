*** Settings ***
Library    mqtt_library.py

*** Test Cases ***

Compile ESP
    [Documentation]    Compile the ESP32 sketch
    Compile Esp

Flash ESP
    [Documentation]    Upload the sketch to ESP32
    Flash Esp

Temperature Validation
    [Documentation]    Validate real temperature from ESP
    Validate Temperature

Wi-Fi RSSI Validation
    [Documentation]    Validate Wi-Fi strength (fails if internet is off)
    Validate Wifi