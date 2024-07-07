//
//  TaskDetailedView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/29/23.
//

import SwiftUI

struct TaskDetailedView: View {
    @ObservedObject var detailedTask: ControllerState.Task
    var task: ControllerState.Task
    @EnvironmentObject var aqController: AqController
    /*var columns = [
        MultiComponentPicker.Column(label: "h", options: Array(0...23).map { MultiComponentPicker.Column.Option(text: "\($0)", tag: $0) }),
        MultiComponentPicker.Column(label: "min", options: Array(0...59).map { MultiComponentPicker.Column.Option(text: "\($0)", tag: $0) }),
        MultiComponentPicker.Column(label: "sec", options: Array(0...59).map { MultiComponentPicker.Column.Option(text: "\($0)", tag: $0) }),
    ]
    var selection1: Int = 23
    var selection2: Int = 59
    var selection3: Int = 59*/
    init(task: ControllerState.Task) {
        self.task = task
        self.detailedTask = ControllerState.Task(task.name)
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
                Text("Run Time").textCase(nil).bold()
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
           
                
            
       // }
    }
}

#Preview {
    TaskDetailedView(task: ControllerState.Task("Test Task On"))
}
