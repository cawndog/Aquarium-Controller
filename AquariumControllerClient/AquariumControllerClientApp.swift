//
//  AquariumControllerClientApp.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI

@main
struct Aquarium_ControllerApp: App {
    var currentState: CurrentState
    var network: Network
    init() {
        currentState = CurrentState()
        network = Network(currentState: currentState)
    }
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(network)
                .environmentObject(currentState)
        }
    }
}
