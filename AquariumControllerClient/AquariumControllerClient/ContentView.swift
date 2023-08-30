//
//  ContentView.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 4/29/23.
//

import SwiftUI
struct ContentView: View {
    @EnvironmentObject var aqController: AqController
    @State private var selectedTab = "One"
    
    //let timer = Timer.publish(every: 5, on: .main, in: .common).autoconnect()
    var body: some View {
        //Text(/*@START_MENU_TOKEN@*/"Hello, World!"/*@END_MENU_TOKEN@*/)

        TabView (selection: $selectedTab) {
            HomeView()
                //.environmentObject(aqController)
                .tabItem {
                    Image(systemName: "house.fill")
                    Text("Home")
                }.tag("One")
            SettingsView(controllerState: aqController.controllerState)
                //.environmentObject(aqController)
                .tabItem {
                    Image(systemName: "gearshape.fill")
                    Text("Settings")
                    
                }.tag("Two")
        }
        /*VStack {
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
                            .environmentObject(network)
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
                  /*  SensorView(sensor: currentState.sensors[currentState.getSensorPosByName("Temperature")])
                    Divider()
                    SensorView(sensor: currentState.sensors[currentState.getSensorPosByName("TDS")])*/

                }.padding(.horizontal, 20)
                
            }.padding(/*@START_MENU_TOKEN@*/.bottom, 50.0/*@END_MENU_TOKEN@*/).padding(.horizontal, 20)
            
            GroupBox(label: Text("Control Center")) {
                VStack {
                   /* DeviceView(device: currentState.devices[currentState.getDevicePosByName("Lights")], connectedDevice: nil).environmentObject(network)
                    Divider()*/
 
                    
                }.padding(/*@START_MENU_TOKEN@*/.vertical, 10.0/*@END_MENU_TOKEN@*/).padding(.horizontal, 20)
            }.padding(.horizontal, 20)
            
        }.onAppear {
            aqController.network.attachControllerState(controllerState: aqController.controllerState)
            //network.connectWebSocket()
            //network.getCurrentState(currentState: currentState)
        }//.onReceive(timer) { time in
            //network.getCurrentState()
        //}
        Spacer()
         */
    }
    
    
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .environmentObject(AqController())
    }
}

