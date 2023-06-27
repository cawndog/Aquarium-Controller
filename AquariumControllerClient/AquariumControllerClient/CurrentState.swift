//
//  CurrentState.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/1/23.
//

import Foundation
import SwiftUI

struct CurrentStateJSON: Codable {
    var temp: String
    var tds: String
    var lights: Bool
    var filter: Bool
    var co2: Bool
    var air: Bool
    var heater: Bool
    var maint: Bool
    init() {
        self.temp = ""
        self.tds = ""
        self.lights = false
        self.filter = false
        self.co2 = false
        self.air = false
        self.heater = false
        self.maint = false
    }
}
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
        self.value = ""
    }
}
class CurrentState: ObservableObject {
    @Published var temp: String
    @Published var tds: String
    var sensors: [Sensor]
    var devices: [Device]
    
    var lights: Device
    var filter: Device
    var co2: Device
    var air: Device
    var heater: Device
    var maint: Device
    

    init() {
        temp = ""
        tds = ""
        self.sensors = [Sensor.init("Temperature"),
                        Sensor.init("TDS")]
        
        self.devices = [Device.init("Lights"),
                        Device.init("Filter"),
                        Device.init("CO2"),
                        Device.init("Air Pump"),
                        Device.init("Heater"),
                        Device.init("Maintenance Mode")]
        
        self.lights = Device.init("lights")
        self.filter = Device.init("filter")
        self.co2 = Device.init("co2")
        self.air = Device.init("air")
        self.heater = Device.init("heater")
        self.maint = Device.init("maint")
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



