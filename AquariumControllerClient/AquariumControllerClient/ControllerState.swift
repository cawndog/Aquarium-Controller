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
    /*class Device: ObservableObject {
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
    class GeneralSetting: HashableClass, Identifiable, ObservableObject {
        var id: String
        var name: String
        @Published var value: Int
        init(_ name: String) {
            self.id = name
            self.name = name
            self.value = 0
        }
    }
    class Alarm: HashableClass, Identifiable, ObservableObject {
        var id: String
        var name: String
        @Published var alarmState: Int
        init(_ name: String) {
            self.id = name
            self.name = name
            self.alarmState = 0
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
        @Published var timeInSeconds: Int
        @Published var isDisabled: Bool
        @Published var connectedTask: Task!
        
        init(_ name: String) {
            self.id = name
            self.name = name
            self.time = Date()
            self.timeInSeconds = 1;
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
        
        func copyFromTask(taskToCopy: ControllerState.Task) {
            self.taskType = taskToCopy.taskType
            self.time = taskToCopy.time
            self.timeInSeconds = taskToCopy.timeInSeconds
            self.isDisabled = taskToCopy.isDisabled
            if taskToCopy.connectedTask != nil {
                if self.connectedTask == nil {
                    self.connectedTask = ControllerState.Task(taskToCopy.connectedTask.name)
                }
                self.connectedTask.taskType = taskToCopy.connectedTask.taskType
                self.connectedTask.time = taskToCopy.connectedTask.time
                self.connectedTask.timeInSeconds = taskToCopy.connectedTask.timeInSeconds
                self.connectedTask.isDisabled = taskToCopy.connectedTask.isDisabled
            }
            
        }
    }*/

    @Published var sensors: [Sensor]
    @Published var devices: [Device]
    @Published var settings: [GeneralSetting]
    @Published var alarms: [Alarm]
    @Published var tasks: [AqTask]

    init() {
        self.sensors = [Sensor.init("Aquarium Temperature"),
                        Sensor.init("TDS")]
        self.devices = [Device.init("Lights"),
                        Device.init("Heater"),
                        Device.init("Filter"),
                        Device.init("CO2"),
                        Device.init("Air Pump")]
                        
        self.settings = [GeneralSetting.init("Maintenance Mode"), GeneralSetting.init("Thermostat")]
        self.alarms = [Alarm.init("Water Sensor Alarm")]
        self.tasks = []
    }
    func getStateDataFromServer(network: Network) {
        Task {
            await network.determineIP()
            network.getCurrentState()
            network.getSettingsState()
        }
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
    func getGeneralSettingByName(_ name:String) -> GeneralSetting {
        for setting in settings {
            if (setting.name == name) {
                return setting
            }
        }
        self.settings.append(GeneralSetting.init(name))
        return getGeneralSettingByName(name)
    }
    func getAlarmByName(_ name:String) -> Alarm {
        for alarm in alarms {
            if (alarm.name == name) {
                return alarm
            }
        }
        self.alarms.append(Alarm.init(name))
        return getAlarmByName(name)
    }
    func getTaskByName(_ name:String) -> AqTask {
        for task in tasks {
            if (task.name == name) {
                return task
            }
        }
        self.tasks.append(AqTask.init(name))
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
    func scheduledTasksExist() -> Bool {
        for task in tasks {
            if (task.taskType == .SCHEDULED_TASK) {
                return true
            }
        }
        return false
    }
}




