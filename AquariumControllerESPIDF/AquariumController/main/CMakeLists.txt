idf_component_register(
    SRCS "main.cpp" "AqController.cpp" "AqWebServer.cpp" "Device.cpp" "Sensor.cpp" "HardwareInterface.cpp" "Switch.cpp" "Task.cpp"
    INCLUDE_DIRS ""
)

set(COMPONENT_REQUIRES
"arduino"
)