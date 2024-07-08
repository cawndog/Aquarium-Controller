//
//  Sensor.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/7/24.
//

import Foundation
class Sensor: ObservableObject {
    var name: String
    @Published var value: String
    init(_ name: String) {
        self.name = name
        self.value = "NULL"
    }
    func getName() -> String {
        return self.name
    }
    func getValue() -> String {
        return self.value
    }
    func setValue(newValue: String) {
        self.value = newValue
    }
}
