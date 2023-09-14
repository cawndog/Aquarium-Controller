//
//  WebSocketMessage.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/15/23.
//

import Foundation

class AqControllerMessage: Codable {
    enum MessageType: String, Codable {
        case Alert, Information, StateUpdate, SettingsUpdate, Unknown
        init () {
            self = .Unknown
        }
    }
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
        var isDisabled: Bool = true
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
    var messageType: MessageType? = .Unknown
    var message: String?
    var aqThermostat: Int?
    var maintenanceMode: Bool?
    var sensors: [Sensor]?
    var devices: [Device]?
    var tasks: [Task]?
    
    func addSensor() {
        //self.sensors.append(Sensor())
    }
    func addDevice(_ newDevice: AqControllerMessage.Device) {
        if (devices?.append(newDevice) == nil) {
            devices = [newDevice]
        }
    }
    func addTask(_ newTask: AqControllerMessage.Task) {
        if (tasks?.append(newTask) == nil) {
            tasks = [newTask]
        }
        
    }
    /*init(messageType: MessageType) {
        self.messageType = messageType
    }*/
    
}
