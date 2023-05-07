//
//  AquariumControllerClientApp.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI

@main
struct Aquarium_ControllerApp: App {
    var network = Network()
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(network)
        }
    }
}
