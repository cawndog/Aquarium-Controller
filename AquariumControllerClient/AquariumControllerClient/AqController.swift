//
//  AqController.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/21/23.
//

import Foundation
import SwiftUI
class AqController: ObservableObject {
    //@Published var controllerState: ControllerState
    var controllerState: ControllerState
    var network: Network
    
    init() {
        controllerState = ControllerState()
        network = Network()
        network.controllerState = self.controllerState
    }
}
