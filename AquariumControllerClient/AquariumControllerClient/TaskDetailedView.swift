//
//  TaskDetailedView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/29/23.
//

import SwiftUI

struct TaskDetailedView: View {
    @ObservedObject var detailedTask: AqTask
    var task: AqTask
    @EnvironmentObject var aqController: AqController
    init(task: AqTask) {
        self.task = task
        self.detailedTask = AqTask(task.name)
        self.detailedTask.copyFromTask(taskToCopy: task)
        //self.task.taskType = .TIMED_TASK
    }
    var body: some View {
        //NavigationStack {
        List {
            //VStack {
            if (detailedTask.taskType == .SCHEDULED_DEVICE_TASK || detailedTask.taskType == .SCHEDULED_TASK) {
                Section {
                    //VStack {
                    DatePicker("Start Time", selection: $detailedTask.time, displayedComponents: .hourAndMinute)
                    if (detailedTask.connectedTask != nil) {
                        DatePicker("End Time", selection: $detailedTask.connectedTask.time, displayedComponents: .hourAndMinute)
                    }
                } header: {
                    Text("Scheduled Time").textCase(nil).bold()
                }
                //}
                
            } else {
                Section {
                    LabeledContent {
                        DatePickerView(seconds: $detailedTask.timeInSeconds)
                            .onChange(of: detailedTask.timeInSeconds) {
                                var dateComps = DateComponents()
                                var timeInSecondsLocal = detailedTask.timeInSeconds
                                var hours: Int
                                var minutes: Int
                                var seconds: Int
                                seconds = timeInSecondsLocal%60
                                timeInSecondsLocal -= seconds
                                minutes = (timeInSecondsLocal%3600)/60
                                timeInSecondsLocal -= (minutes*60)
                                hours = timeInSecondsLocal/3600
                                dateComps.hour = hours
                                dateComps.minute = minutes
                                dateComps.second = seconds
                                detailedTask.time = Calendar.current.date(from: dateComps)!
                            }
                    }
                label: {
                    Label("", systemImage: "timer")
                }
                } header: {
                    Text("Run Time Interval").textCase(nil).bold()
                }
            }
            
            Section {
                Toggle(isOn: $detailedTask.isDisabled) {
                    Text("Task Disabled")
                }
            }
            
        }
        // }//.listStyle(.insetGrouped)
        .toolbar {
            Button("Save", action: {
                Task{
                    task.copyFromTask(taskToCopy: detailedTask)
                    await aqController.network.setTaskState(task: task)
                }
            })
        }
    }
}

#Preview {
    TaskDetailedView(task: AqTask("Test Task On"))
}
