//
//  Alarm.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/7/24.
//

import Foundation
import SwiftUI

class Alarm: HashableClass, Identifiable, ObservableObject {
    var id: String
    var name: String
    @Published var alarmState: Int
    @Published var alarmOverride: Bool
    init(_ name: String) {
        self.id = name
        self.name = name
        self.alarmState = 0
        self.alarmOverride = false
    }
    func getName() -> String {
        return self.name
    }
    func getAlarmState() -> Int {
        return self.alarmState
    }
    func setAlarmState(newState: Int) {
        self.alarmState = newState
    }
    func getAlarmOverride() -> Bool {
        return self.alarmOverride
    }
    func setAlarmOverride(override: Bool) {
        self.alarmOverride = override
    }
}
