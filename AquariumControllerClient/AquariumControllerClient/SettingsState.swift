//
//  SettingsState.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/5/23.
//

import Foundation
import SwiftUI

struct SettingsStateJSON: Codable {
    var aquariumThermostat: UInt8
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
            var onHr: UInt8
            var onMin: UInt8
            var offHr: UInt8
            var offMin: UInt8
            init() {
                onHr = 0
                onMin = 0
                offHr = 0
                offMin = 0
            }
        }
    
    }
        
}
