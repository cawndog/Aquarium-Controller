//
//  SettingsView.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/4/23.
//

import SwiftUI
import Foundation
struct SettingsView: View {
    @State var editMode: Bool = false;
    @EnvironmentObject var aqController: AqController
    @ObservedObject var controllerState: ControllerState
    
    @State var tempSet: Int = 82
    let tempRange = 64...94
    let tempStep = 1
    init(controllerState: ControllerState) {
        self.controllerState = controllerState
        /*self.controllerState.getTaskByName("Test Task On").connectedTask = ControllerState.Task("Test Task Off")
        self.controllerState.getTaskByName("Test Task 2 On").connectedTask = ControllerState.Task("Test Task 2 Off")
        */
    }
    //@State var currentDate = Date();
    //@ObservedObject var controllerState: ControllerState!
    var body: some View {
        //Text("Hello from settings!")
        NavigationStack {
            
            List {
                Section {
                    NavigationLink (value: "Thermostat") {
                        LabeledContent {
                            Text(String(controllerState.aqThermostat) + " °F")
                        }
                        label: {
                            Label("Thermostat", systemImage: "thermometer")
                        }
                    }
                } header: {
                    Text("")
                }
                Section {
                    ForEach(controllerState.tasks) { task in
                        if (task.taskType == .SCHEDULED_DEVICE_TASK) {
                            NavigationLink(value: task) {
                                TaskSummaryView(task: task)
                            }
                        }
                    }
                    
                } header: {
                    Text("Device Tasks").textCase(nil).bold()
                }
                Section {
                    ForEach(controllerState.tasks) { task in
                        if (task.taskType == .SCHEDULED_TASK) {
                            NavigationLink(value: task) {
                                TaskSummaryView(task: task)
                            }
                        }
                    }
                } header: {
                    Text("Scheduled Tasks").textCase(nil).bold()
                }
                Section {
                    ForEach(controllerState.tasks) { task in
                        if (task.taskType == .TIMED_TASK) {
                            NavigationLink(value: task) {
                                TaskSummaryView(task: task)
                            }
                        }
                    }
                } header: {
                    Text("Timed Tasks").textCase(nil).bold()
                }
            }
            .navigationDestination(for: ControllerState.Task.self) { task in
                TaskDetailedView(task: task)
                    .navigationTitle(task.name)
                    .toolbar {
                        Button("Save", action: {
                            Task{
                                await aqController.network.setSettingsState()
                            }

                        })
                    }
            }
            .navigationDestination(for: String.self) { string in
                ThermostatDetailedView(controllerState: aqController.controllerState)
                    .navigationTitle(string)
                    .toolbar {
                        Button("Save", action: {
                            Task{
                                await aqController.network.setSettingsState()
                            }
                            
                        })
                    }
            }
            
            .listStyle(.insetGrouped)
            .navigationTitle("Settings")
        }
        .onAppear {
            //aqController.network.getSettingsState()
        }
        
        /*
         case "SCHEDULED_TASK":
             self.taskType = .SCHEDULED_TASK
         case "SCHEDULED_DEVICE_TASK":
             self.taskType = .SCHEDULED_DEVICE_TASK
         case "TIMED_TASK":
         */
        
        /*NavigationView {

            Form {
                Section(header: Text("Thermostat")) {
                    Stepper(value: $network.settingsState.aquariumThermostat, in: tempRange, step: tempStep) {
                        LabeledContent ("Set Temp") {
                            Spacer()
                            Text("\(network.settingsState.aquariumThermostat) °F")
                            Spacer()
                        }
                    }
                }
                Section(header: Text("Timers")) {
                    DatePicker("CO2 On Time", selection: $network.settingsState.timers.co2.onTime, displayedComponents: .hourAndMinute)
                    DatePicker("CO2 Off Time", selection: $network.settingsState.timers.co2.offTime, displayedComponents: .hourAndMinute)
                }
                Section {
                    DatePicker("Air Pump On Time", selection: $network.settingsState.timers.airPump.onTime, displayedComponents: .hourAndMinute)
                    DatePicker("Air Pump Off Time", selection: $network.settingsState.timers.airPump.offTime, displayedComponents: .hourAndMinute)
                }
                Section {
                    DatePicker("Lights On Time", selection: $network.settingsState.timers.lights.onTime, displayedComponents: .hourAndMinute)
                    DatePicker("Lights Off Time", selection: $network.settingsState.timers.lights.offTime, displayedComponents: .hourAndMinute)
                }
                
            }.navigationTitle("Settings")
            .toolbar {
                Button("Save", action: {
                    Task {
                        network.settingsStateJSON.aquariumThermostat = network.settingsState.aquariumThermostat
                        //------------airPump
                        network.comps = Calendar.current.dateComponents([.hour, .minute], from: network.settingsState.timers.airPump.onTime)
                        network.settingsStateJSON.timers.airPump.onHr = network.comps.hour!
                        network.settingsStateJSON.timers.airPump.onMin = network.comps.minute!
                        network.comps = Calendar.current.dateComponents([.hour, .minute], from: network.settingsState.timers.airPump.offTime)
                        network.settingsStateJSON.timers.airPump.offHr = network.comps.hour!
                        network.settingsStateJSON.timers.airPump.offMin = network.comps.minute!
                        
                        //----------------co2
                        network.comps = Calendar.current.dateComponents([.hour, .minute], from: network.settingsState.timers.co2.onTime)
                        network.settingsStateJSON.timers.co2.onHr = network.comps.hour!
                        network.settingsStateJSON.timers.co2.onMin = network.comps.minute!
                        network.comps = Calendar.current.dateComponents([.hour, .minute], from: network.settingsState.timers.co2.offTime)
                        network.settingsStateJSON.timers.co2.offHr = network.comps.hour!
                        network.settingsStateJSON.timers.co2.offMin = network.comps.minute!
                        
                        //-------------lights
                        network.comps = Calendar.current.dateComponents([.hour, .minute], from: network.settingsState.timers.lights.onTime)
                        network.settingsStateJSON.timers.lights.onHr = network.comps.hour!
                        network.settingsStateJSON.timers.lights.onMin = network.comps.minute!
                        network.comps = Calendar.current.dateComponents([.hour, .minute], from: network.settingsState.timers.lights.offTime)
                        network.settingsStateJSON.timers.lights.offHr = network.comps.hour!
                        network.settingsStateJSON.timers.lights.offMin = network.comps.minute!
                        await network.setSettingsState()
                    }
                })
                    .buttonStyle(.bordered)
                    //.padding(.trailing, 12.0)
                    
            }.onAppear {
                network.getSettingsState()
                /*var comps = DateComponents()
                comps.hour = 19
                comps.minute = 30
                comps.second = 0
                currentDate = Calendar.current.date(from: comps)!
                print(currentDate)
                print(currentDate.formatted(date: .long, time: .complete))
                print(currentDate.formatted(Date.FormatStyle().hour(.defaultDigitsNoAMPM)))
                comps = Calendar.current.dateComponents([.hour, .minute], from: currentDate)
                print(comps.hour!)
                print(comps.minute!)*/
            }
        }
         */
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var previews: some View {
        SettingsView(controllerState: ControllerState())
            .environmentObject(AqController())
    }
}
