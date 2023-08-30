//
//  TaskDetailedView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/29/23.
//

import SwiftUI

struct TaskDetailedView: View {
    @ObservedObject var task: ControllerState.Task
    
    
    init(task: ControllerState.Task) {
        self.task = task
      
    }
    var body: some View {
        NavigationStack {
            List {
                Section {
                    if (task.taskType == .SCHEDULED_DEVICE_TASK || task.taskType == .SCHEDULED_TASK) {
                        //VStack {
                            DatePicker("Start Time", selection: $task.time, displayedComponents: .hourAndMinute)
                            if (task.connectedTask != nil) {
                                DatePicker("End Time", selection: $task.connectedTask.time, displayedComponents: .hourAndMinute)
                            }
                        //}
                        
                    } else {
                        //VStack {
                            DatePicker("Run Time Interval", selection: $task.time, displayedComponents: .hourAndMinute)
                        //}
                    }
                }
                Section {
                    Toggle(isOn: $task.isDisabled) {
                        Text("Task Disabled")
                    }
                }
                
            }.listStyle(.insetGrouped)
                .navigationTitle(task.name)
                .toolbar {
                    Button("Save", action: {
                        
                    })
                }
            
        }
    }
}

#Preview {
    TaskDetailedView(task: ControllerState.Task("Test Task On"))
}
