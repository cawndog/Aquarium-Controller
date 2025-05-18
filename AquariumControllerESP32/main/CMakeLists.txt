#idf_component_register(
#    SRCS "main.cpp" "AqController.cpp" "AqWebServer.cpp" "Device.cpp" "Sensor.cpp" "HardwareInterface.cpp" "Switch.cpp" "Task.cpp"
#    INCLUDE_DIRS ""
#)

# Add the ESP-IDF project's source files
file(GLOB COMPONENT_SRCS
  "./*.cpp"
  "./Peripherals/Device/*.cpp"
  "./Peripherals/Sensor/*.cpp"
  "./Peripherals/HardwareInterface/*.cpp"
  "./Server/*.cpp"
  "./Task/*.cpp"
  "./Settings/*.cpp"
)
set(COMPONENT_SRCS ${COMPONENT_SRCS})

# Add the ESP-IDF project's include directories
set(COMPONENT_ADD_INCLUDEDIRS
  "./"
  "./Peripherals/Device"
  "./Peripherals/Sensor"
  "./Peripherals/HardwareInterface"
  "./Server"
  "./Task"
  "./Settings"
)

set(COMPONENT_REQUIRES
#"arduino" "ArduinoJson" "AsyncTCP" "DallasTemperature" "OneWire" "ESP32Time" "ESPAsyncWebServer" "unity" "servo" "nvs_flash" "protocol_examples_common" "ElegantOTA"
"arduino" "ArduinoJson" "AsyncTCP" "DallasTemperature" "OneWire" "ESP32Time" "ESPAsyncWebServer" "unity" "ESP32Servo" "nvs_flash" "protocol_examples_common" "ElegantOTA"
)
set(COMPONENT_EMBED_TXTFILES
"./server_root_cert.pem" "./green_fish.png"
)

register_component()

#target_compile_definitions(${COMPONENT_TARGET} PUBLIC -DESP32)
#target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)