//
//  SettingsState.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/5/23.
//

import Foundation
import SwiftUI

struct SettingsStateJSON: Codable {
    var aquariumThermostat: Int
    var timers: Timers
    init() {
        aquariumThermostat = 0
        timers = Timers.init()
    }
    struct Timers: Codable {
        var airPump: Device
        var co2: Device
        var lights: Device
        init() {
            airPump = Device.init()
            co2 = Device.init()
            lights = Device.init()
        }
        struct Device: Codable {
            var onHr: Int
            var onMin: Int
            var offHr: Int
            var offMin: Int
            init() {
                onHr = 0
                onMin = 0
                offHr = 0
                offMin = 0
            }
        }
    
    }
        
}
struct SettingsState {
    var aquariumThermostat: Int
    var timers: Timers
    init() {
        aquariumThermostat = 0
        timers = Timers.init()
    }
    struct Timers {
        var airPump: DeviceTimes
        var co2: DeviceTimes
        var lights: DeviceTimes
        init() {
            self.airPump = DeviceTimes.init()
            self.co2 = DeviceTimes.init()
            self.lights = DeviceTimes.init()
        }
        struct DeviceTimes {
            var onTime: Date
            var offTime: Date
            init() {
                self.onTime = Date()
                self.offTime = Date()
            }
        }
    }
        
}
