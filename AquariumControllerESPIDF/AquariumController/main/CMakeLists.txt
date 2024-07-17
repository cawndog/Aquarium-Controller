#idf_component_register(
#    SRCS "main.cpp" "AqController.cpp" "AqWebServer.cpp" "Device.cpp" "Sensor.cpp" "HardwareInterface.cpp" "Switch.cpp" "Task.cpp"
#    INCLUDE_DIRS ""
#)
set(COMPONENT_SRCDIRS
"./" "./Peripherals/Device" "./Peripherals/Sensor" "./Peripherals/HardwareInterface" "./Server" "./Task" "./Settings"
)

set(COMPONENT_ADD_INCLUDEDIRS
"./" "./Peripherals/Device" "./Peripherals/Sensor" "./Peripherals/HardwareInterface" "./Server" "./Task" "./Settings"
)
set(COMPONENT_REQUIRES
"arduino" "ArduinoJson" "AsyncTCP" "DallasTemperature" "OneWire" "ESP32Servo" "ESP32Time" "ESPAsyncWebServer" "unity" "servo"
)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC -DESP32)
target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)  