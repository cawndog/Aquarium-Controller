//
//  ContentView.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI

struct ContentView: View {
    @EnvironmentObject var aqController: AqController
    @Environment(\.scenePhase) var scenePhase
    @State private var selectedTab = "One"
    var body: some View {
        //Text(/*@START_MENU_TOKEN@*/"Hello, World!"/*@END_MENU_TOKEN@*/)
        
        TabView (selection: $selectedTab) {
            HomeView()
                .tabItem {
                    Image(systemName: "house.fill")
                    Text("Home")
                }.tag("One")
            SettingsView(controllerState: aqController.controllerState)
                .tabItem {
                    Image(systemName: "gearshape.fill")
                    Text("Settings")
                    
                }.tag("Two")
        }
        /*.onAppear {
         //
         }*/
        .onChange(of: scenePhase, initial: false) { oldPhase, newPhase in
            print("onChange")
            if (newPhase == .active) {
                Task {
                    await aqController.network.determineIP()
                    aqController.network.getCurrentState()
                    aqController.network.getSettingsState()
                    aqController.network.connectWebSocket()
                }
            } else if (newPhase == .background) {
                aqController.network.disconnectWebSocket()
            }
            
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .environmentObject(AqController())
    }
}

