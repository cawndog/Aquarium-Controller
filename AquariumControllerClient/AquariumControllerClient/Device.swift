//
//  Device.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/7/24.
//

import Foundation
class Device: ObservableObject {
    var name: String
    @Published var state: Bool
    var stateUpdatedByController: Bool
    
    init(_ name: String) {
        self.name = name
        self.state = false
        self.stateUpdatedByController = false
    }
    func getName() -> String {
        return self.name
    }
    func getState() -> Bool {
        return self.state
    }
    func setState(newState: Bool) {
        self.state = newState
    }
}
