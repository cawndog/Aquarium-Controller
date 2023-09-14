//
//  AquariumControllerClientApp.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI

@main
struct Aquarium_ControllerApp: App {
    
    @StateObject var aqController: AqController = AqController()
    //var network: Network
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(aqController)
                
        }
    }
}
