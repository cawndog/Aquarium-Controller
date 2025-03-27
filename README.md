# Aquarium Controller

A system for managing and automating aquarium settings, built to monitor and control various aspects of aquarium maintenance such as temperature, lighting, and water changes. 

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Setup and Installation](#setup-and-installation)
- [Configuration](#configuration)
- [Screenshots and Images](#screenshots-and-images)

---

## Project Overview
The Aquarium Controller automates aquarium management tasks, using sensors and actuators to maintain an optimal environment for aquarium inhabitants. The system includes an iOS application that serves as the client interface and a server running on an ESP32 to control and monitor the aquarium’s parameters.

## Features
- **Temperature Monitoring/Control**: Real-time tracking and adjustments based on temperature reading.
- **Water Quality Monitoring**: Sensors to monitor water quality (currently supports TDS, but more sensors can be added).
- **Water Leak Alerts & Response**: Email alerts when a leak is detected and the ability to automatically close a filter intake valve to prevent flooding.
- **Lighting Control**: Automated lighting schedules for day/night cycles.
- **Air Pump & CO<sub>2</sub> Control**: Automated air pump and CO<sub>2</sub> schedules for day/night cycles.
- **Maintenance Mode**: Shuts off the heater and filter during water changes.
- **User Interface**: iOS application client interface for easy control and monitoring.


## Technologies Used
- **Backend**: The ESP32 controller hosts a web server written in C++ using the ESPAsyncWebServer library. Below is the complete list of dependencies for the ESP32 project, all of which are included in this respository.
  - <a href="https://github.com/espressif/arduino-esp32/releases/tag/3.1.3" target="_blank" rel="noopener noreferrer">Arduino Core v3.1.3</a>
  - <a href="https://github.com/ESP32Async/ESPAsyncWebServer/releases/tag/v3.7.4" target="_blank" rel="noopener noreferrer">ESPAsyncWebServer v3.7.4</a>
  - <a href="https://github.com/ESP32Async/AsyncTCP/releases/tag/v3.3.8" target="_blank" rel="noopener noreferrer">AsyncTCP v3.3.8</a>
  - <a href="https://github.com/fbiego/ESP32Time/releases/tag/2.0.6" target="_blank" rel="noopener noreferrer">ESP32Time v2.0.6</a>
  - <a href="https://github.com/milesburton/Arduino-Temperature-Control-Library" target="_blank" rel="noopener noreferrer">Dallas Temperature v3.11.0</a>
  - <a href="https://github.com/PaulStoffregen/OneWire/releases/tag/v2.3.8" target="_blank" rel="noopener noreferrer">OneWire v2.3.8</a>
  - <a href="https://github.com/espressif/esp-iot-solution/tree/release/v2.0/components/motor/servo" target="_blank" rel="noopener noreferrer">Servo from esp-iot-solution v2.0</a>
- **Frontend**: SwiftUI
- **Communication Protocols**: REST API, Websockets
- **Other Tools**: ESP-IDF, ESP-IDF Extension for Visual Studio Code

## Setup and Installation
### Prerequisites
- Visual Studio Code and the ESP-IDF Extension for Visual Studio Code Installed
- ESP-IDF 5.3.2 Installed
- Xcode installed (Only required for the client application)
- Git installed

### Clone the Repository
```bash
git clone https://github.com/cawndog/Aquarium-Controller.git
cd Aquarium-Controller
```
For the iOS application, find the AquariumControllerClient.xcodeproj file inside the AquariumControllerClient directory and open it in Xcode.
For the ESP32 project, open the AquariumControllerESP32 directory in Visual Studio Code.
## Configuration
- The iOS application contains a file named MyEnvironment.h, which holds values like the API key. Modify this file as needed.
- The ESP32 project contains an Environment.h file, which will also need to be modified as needed.

## Screenshots and Images
### Client Application
<div align="center">

| Home Screen                | Settings Screen            |
|----------------------------|----------------------------|
| <img src="https://github.com/cawndog/Aquarium-Controller/blob/main/AqController%20Images/iOS%20App/Home.PNG" width="300"/> | <img src="https://github.com/cawndog/Aquarium-Controller/blob/main/AqController%20Images/iOS%20App/Settings.PNG" width="300"/> |

</div>
<div align="center">

| Email Alerts for Alarms    |
|----------------------------|
| <img src="https://github.com/cawndog/Aquarium-Controller/blob/main/AqController%20Images/iOS%20App/Email_Alerts_for_Alarms.gif" width="400"/> |

</div>

### Controller
<div align="center">

| ESP32 Microcontroller      | 120v Switch Box            | Water Valve                |
|----------------------------|----------------------------|----------------------------|
| <img src="https://github.com/cawndog/Aquarium-Controller/blob/main/AqController%20Images/Controller/Controller%20Box%20Internal.jpeg" width="300"/> | <img src="https://github.com/cawndog/Aquarium-Controller/blob/main/AqController%20Images/Controller/Switch%20Box%20Internal.jpeg" width="300"/> | <img src="https://github.com/cawndog/Aquarium-Controller/blob/main/AqController%20Images/Controller/Water%20Valve.jpg" width="300"/> |

</div>
