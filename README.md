# Aquarium Controller

A system for managing and automating aquarium settings, built to monitor and control various aspects of aquarium maintenance such as temperature, lighting, and water changes. 

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Setup and Installation](#setup-and-installation)
- [Configuration](#configuration)

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
- **Backend**: Server written in C++ using the ESPAsyncWebServer library for ESP32.
- **Frontend**: SwiftUI
- **Communication Protocols**: REST API, Websockets
- **Other Tools**: ESP-IDF, ESP-IDF Extension for Visual Studio Code

## Setup and Installation
### Prerequisites
- ESP-IDF 5.1.4 Installed
- Visual Studio Code and the ESP-IDF Extension for Visual Studio Code Installed
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
