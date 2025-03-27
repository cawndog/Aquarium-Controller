//
//  CurrentState.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/1/23.
//

import Foundation
import SwiftUI

class ControllerState: ObservableObject {

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




