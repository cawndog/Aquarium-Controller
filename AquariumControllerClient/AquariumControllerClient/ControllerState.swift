//
//  CurrentState.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/1/23.
//

import Foundation
import SwiftUI


/*class CurrentState: ObservableObject {
    @Published var temp: String
    @Published var tds: String
    @Published var lights: Device
    @Published var filter: Device
    @Published var co2: Device
    @Published var air: Device
    @Published var heater: Device
    @Published var maint: Device
    init() {
        temp = ""
        tds = ""
        self.lights = Device.init(devName: "lights")
        self.filter = Device.init(devName: "filter")
        self.co2 = Device.init(devName: "co2")
        self.air = Device.init(devName: "air")
        self.heater = Device.init(devName: "heater")
        self.maint = Device.init(devName: "maint")
    }
}*/


class ControllerState: ObservableObject {
    class Device: ObservableObject {
        var name: String
        @Published var state: Bool
        var stateUpdatedByController: Bool
        
        init(_ name: String) {
            self.name = name
            self.state = false
            self.stateUpdatedByController = false
        }
    }
    class Sensor: ObservableObject {
        var name: String
        @Published var value: String
        init(_ name: String) {
            self.name = name
            self.value = "NULL"
        }
    }
    class Task: HashableClass, Identifiable, ObservableObject {
       
        enum TaskType: String {
            case SCHEDULED_TASK, SCHEDULED_DEVICE_TASK, TIMED_TASK, Unknown
            init () {
                self = .Unknown
            }

        }
        var id: String
        var name: String
        var taskType: TaskType
        @Published var time: Date
        @Published var isDisabled: Bool
        @Published var connectedTask: Task!
        
        init(_ name: String) {
            self.id = name
            self.name = name
            self.time = Date()
            self.isDisabled = true;
            //self.taskType = TaskType()
            //self.taskType = .Unknown
            self.taskType = .SCHEDULED_DEVICE_TASK
            self.connectedTask = nil
        }
        func setTaskTypeWithString(_ type: String) {
            switch (type) {
                case "SCHEDULED_TASK":
                    self.taskType = .SCHEDULED_TASK
                case "SCHEDULED_DEVICE_TASK":
                    self.taskType = .SCHEDULED_DEVICE_TASK
                case "TIMED_TASK":
                    self.taskType = .TIMED_TASK
                default:
                    self.taskType = .Unknown
                   
            }
        }
        
    }
    //@Published var temp: String
    //@Published var tds: String
    @Published var sensors: [Sensor]
    @Published var devices: [Device]
    @Published var tasks: [Task]
    @Published var maintenanceMode: Bool
    @Published var aqThermostat: Int
    init() {
        self.sensors = [Sensor.init("Temperature"),
                        Sensor.init("TDS")]
        self.devices = [Device.init("Lights"),
                        Device.init("Filter"),
                        Device.init("CO2"),
                        Device.init("Air Pump"),
                        Device.init("Heater")]
        self.tasks = []
        maintenanceMode = false
        aqThermostat = 0
        
        
    }
    func getDeviceByName(_ name:String) -> Device {
        for device in devices {
            if (device.name == name) {
                return device
            }
        }
        self.devices.append(Device.init(name))
        return getDeviceByName(name)
    }
    func getSensorByName(_ name:String) -> Sensor {
        for sensor in sensors {
            if (sensor.name == name) {
                return sensor
            }
        }
        self.sensors.append(Sensor.init(name))
        return getSensorByName(name)
    }
    func getTaskByName(_ name:String) -> Task {
        for task in tasks {
            if (task.name == name) {
                return task
            }
        }
        self.tasks.append(Task.init(name))
        return getTaskByName(name)
    }
    func getDevicePosByName(_ name:String) -> Int {
        var i = 0
        for device in self.devices {
            if (device.name == name) {
                return i
            }
            i+=1
        }
        return -1
    }
    func getSensorPosByName(_ name:String) -> Int {
        var i = 0
        for sensor in self.sensors {
            if (sensor.name == name) {
                return i
            }
            i+=1
        }
        return -1
    }
}
/*class Device: ObservableObject {
    var name: String
    @Published var deviceState: Bool
    var stateUpdatedByController: Bool
    init(devName: String) {
        self.name = devName
        self.deviceState = false
        self.stateUpdatedByController = false
    }
}*/



