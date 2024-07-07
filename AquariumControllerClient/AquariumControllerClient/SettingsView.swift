//
//  SettingsView.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/4/23.
//

import SwiftUI
import Foundation
struct SettingsView: View {
    //@State var editMode: Bool = false;
    @EnvironmentObject var aqController: AqController
    @Environment(\.scenePhase) var scenePhase
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
                    ForEach(controllerState.settings) { setting in
                        if (setting.name == "Thermostat") {
                            NavigationLink(value: setting) {
                                GeneralSettingView(generalSetting: setting)
                            }
                        } else {
                            GeneralSettingView(generalSetting: setting)
                        }
                    }
                } header: {
                    Text("General").textCase(nil).bold()
                }
                Section {
                    ForEach(controllerState.alarms) { alarm in
                        NavigationLink(value: alarm) {
                            AlarmView(alarm: alarm)
                        }
                    }
                } header: {
                    Text("Alarms").textCase(nil).bold()
                }
                Section {
                    ForEach(controllerState.tasks) { task in
                        if (task.taskType == .SCHEDULED_DEVICE_TASK || task.taskType == .SCHEDULED_TASK ) {
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
            .navigationDestination(for: ControllerState.GeneralSetting.self) { generalSetting in
                GeneralSettingEditView(generalSetting: generalSetting)
                    .navigationTitle(generalSetting.name)
            }
            .navigationDestination(for: ControllerState.Alarm.self) { alarm in
                AlarmEditView(alarm: alarm)
                    .navigationTitle(alarm.name)
            }
            .navigationDestination(for: ControllerState.Task.self) { task in
                TaskDetailedView(task: task)
                    .navigationTitle(task.name)
                    /*.toolbar {
                        Button("Save", action: {
                            Task{
                                await aqController.network.setSettingsState(task: task)
                            }

                        })
                    }*/
            }
            /*.navigationDestination(for: String.self) { string in
                ThermostatDetailedView(controllerState: controllerState)
                    .navigationTitle(string)
                    .toolbar {
                        Button("Save", action: {
                            Task{
                                await aqController.network.setSettingsState(aqThermostat: controllerState.aqThermostat)
                            }
                            
                        })
                    }
            }*/
            
            //.listStyle(.insetGrouped)
            .navigationTitle("Settings")
        }
        .onAppear {
            //aqController.network.getSettingsState()
        }
        .onChange(of: scenePhase, initial: false) { oldPhase, newPhase in
            if (newPhase == .active) {
                print("SettingsView is now active")
                //aqController.network.getSettingsState()
            }
        }
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var previews: some View {
        SettingsView(controllerState: ControllerState())
            .environmentObject(AqController())
    }
}
