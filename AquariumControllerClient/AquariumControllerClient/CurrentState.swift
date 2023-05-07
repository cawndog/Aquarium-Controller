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
struct CurrentState {
    var temp: String
    var tds: String
    var lights: Device
    var filter: Device
    var co2: Device
    var air: Device
    var heater: Device
    var maint: Device
    
    struct Device {
        var name: String
        var deviceState: Bool
        var stateUpdatedByController: Bool
        init(devName: String) {
            self.name = devName
            self.deviceState = false
            self.stateUpdatedByController = false
        }
    }
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



