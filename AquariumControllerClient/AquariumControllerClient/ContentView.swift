//
//  ContentView.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI
struct ContentView: View {
    @EnvironmentObject var network: Network
    
    @State private var showingPopover = false
    let timer = Timer.publish(every: 5, on: .main, in: .common).autoconnect()
    var body: some View {
        VStack {
            HStack {
                Spacer()
                Button {
                    showingPopover = true
                }label: {Image(systemName: "gear")
                        .foregroundColor(Color.black)
                }.padding(.trailing, 20.0)
                    .popover(isPresented: $showingPopover) {
                        /*Text("Settings")
                            .font(.headline)
                            .padding()*/
                        SettingsView()
                    }
            }
            .padding(.top, 20.0)
            Text("Aquarium Controller")
                .font(.title)
                .fontWeight(.bold)
                .foregroundColor(Color.blue)
                .padding(.vertical, 20.0)
            
            GroupBox(label: Text("Sensors")) {
                VStack {
                    LabeledContent("Temperature") {
                        Text(network.currentState.temp)
                    }
                    Divider()
                    LabeledContent("TDS") {
                        Text(network.currentState.tds)
                    }
                }.padding(.horizontal, 20)
                
            }.padding(/*@START_MENU_TOKEN@*/.bottom, 50.0/*@END_MENU_TOKEN@*/).padding(.horizontal, 20)
            
            GroupBox(label: Text("Control Center")) {
                VStack {
                    Toggle(isOn: $network.currentState.lights.deviceState) {
                        Text("Lights")
                    }.toggleStyle(.switch).onChange(of: network.currentState.lights.deviceState) { value in
                        if (!network.currentState.lights.stateUpdatedByController){
                            Task {
                                await network.checkToggleChange(device: network.currentState.lights)
                            }
                        } else {
                            network.currentState.lights.stateUpdatedByController = false
                        }
                        
                        
                    }
                    Divider()
                    Toggle(isOn: $network.currentState.air.deviceState) {
                        Text("Air Pump")
                    }.toggleStyle(.switch).onChange(of: network.currentState.air.deviceState) { value in
                        if (!network.currentState.air.stateUpdatedByController){
                            if (value == true){
                                if (network.currentState.co2.deviceState == true){
                                    network.currentState.co2.stateUpdatedByController = true
                                    network.currentState.co2.deviceState = false
                                }
                            }
                            Task {
                                await network.checkToggleChange(device: network.currentState.air)
                            }
                        } else {
                            network.currentState.air.stateUpdatedByController = false
                        }
                        
                    }
                    Divider()
                    Toggle(isOn: $network.currentState.co2.deviceState) {
                        Text("CO2")
                    }.toggleStyle(.switch).onChange(of: network.currentState.co2.deviceState) { value in
                        if (!network.currentState.co2.stateUpdatedByController){
                            if (value == true){
                                if (network.currentState.air.deviceState == true) {
                                    network.currentState.air.stateUpdatedByController = true
                                    network.currentState.air.deviceState = false
                                }
                            }
                            Task {
                                await network.checkToggleChange(device: network.currentState.co2)
                            }
                        } else {
                            network.currentState.co2.stateUpdatedByController = false
                        }
                        
                    }
                    Divider()
                    Toggle(isOn: $network.currentState.heater.deviceState) {
                        Text("Heater")
                    }.toggleStyle(.switch).disabled(true)
                    Divider()
                    Toggle(isOn: $network.currentState.maint.deviceState) {
                        Text("Maintenance Mode")
                    }.toggleStyle(.switch).onChange(of: network.currentState.maint.deviceState) { value in
                        Task {
                            await network.checkToggleChange(device: network.currentState.maint)
                        }
                        
                    }
                }.padding(/*@START_MENU_TOKEN@*/.vertical, 10.0/*@END_MENU_TOKEN@*/).padding(.horizontal, 20)
            }.padding(.horizontal, 20)
            
        }.onAppear {
            network.getCurrentState()
        }.onReceive(timer) { time in
            network.getCurrentState()
        }
        Spacer()
    }
    
    
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .environmentObject(Network())
    }
}

