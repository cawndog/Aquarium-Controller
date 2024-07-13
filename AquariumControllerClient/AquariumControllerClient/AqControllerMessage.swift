//
//  WebSocketMessage.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/15/23.
//

import Foundation

class AqControllerMessage: Codable {

    class Sensor: Codable {
        var name: String = ""
        var value: String = "0"
        init() {
            name = ""
            value = "0"
        }
    }
    class Device: Codable {
        var name: String = ""
        var state: Bool = false
    }
    class Settings: Codable {
        class GeneralSetting: Codable  {
            var name: String = ""
            var value: Int = 0
        }
        class Alarm: Codable  {
            var name: String = ""
            var alarmState: Int = 0
        }
        class Task: Codable {
            enum TaskType: String, Codable {
                case SCHEDULED_TASK, SCHEDULED_DEVICE_TASK, TIMED_TASK, Unknown
                init () {
                    self = .Unknown
                }
            }
            var name: String = ""
            var taskType: TaskType = .Unknown
            var time: Int = 0
            var enabled: Bool = true
            var connectedTask: Task?
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
        var generalSettings: [GeneralSetting]?
        var alarms: [Alarm]?
        var tasks: [Task]?
        func addGeneralSetting(_ newSetting: AqControllerMessage.Settings.GeneralSetting) {
            if (generalSettings?.append(newSetting) == nil) {
                generalSettings = [newSetting]
            }
        }
        func addAlarm(_ newAlarm: AqControllerMessage.Settings.Alarm) {
            if (alarms?.append(newAlarm) == nil) {
                alarms = [newAlarm]
            }
        }
        func addTask(_ newTask: AqControllerMessage.Settings.Task) {
            if (tasks?.append(newTask) == nil) {
                tasks = [newTask]
            }
        }
    }
    
    var sensors: [Sensor]?
    var devices: [Device]?
    var settings: Settings?

    func addSensor(_ newSensor: AqControllerMessage.Sensor) {
        if (sensors?.append(newSensor) == nil) {
            sensors = [newSensor]
        }
    }
    func addDevice(_ newDevice: AqControllerMessage.Device) {
        if (devices?.append(newDevice) == nil) {
            devices = [newDevice]
        }
    }
    func addSettings(_ newSettings: AqControllerMessage.Settings) {
        settings = newSettings
    }
}
