//
//  AquariumControllerClientApp.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI

@main
struct Aquarium_ControllerApp: App {
    var aqController: AqController
    //var network: Network
    init() {
        aqController = AqController()
    }
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(aqController)
                
        }
    }
}
