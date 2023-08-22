//
//  WebSocketMessage.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/15/23.
//

import Foundation

struct AqControllerMessage: Decodable {
    enum MessageType: String, Decodable {
        case Alert, Information, StateUpdate, SettingsUpdate, Unknown
        init () {
            self = .Unknown
        }
    }
    struct Sensor: Decodable {
        var name: String
        var value: String
    }
    struct Device: Decodable {
        var name: String
        var state: Bool
    }
    
    let messageType: MessageType
    let message: String?
    let aqThermostat: Int?
    let maintenanceMode: Bool?
    let sensors: [Sensor]?
    let devices: [Device]?
    
}
