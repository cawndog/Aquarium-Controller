//
//  SettingsTaskView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/28/23.
//

import SwiftUI

struct TaskSummaryView: View {
    @ObservedObject var task: ControllerState.Task
    let dateFormatter = DateFormatter()
    
    init(task: ControllerState.Task) {
        self.task = task
        if (task.taskType == .SCHEDULED_DEVICE_TASK || task.taskType == .SCHEDULED_TASK) {
            dateFormatter.dateFormat = "HH:mm"
        } else {
            dateFormatter.dateFormat = "HH:mm:ss"
        }
    }
    var body: some View {
        
        if (task.taskType == .SCHEDULED_DEVICE_TASK || task.taskType == .SCHEDULED_TASK) {
            LabeledContent {
                VStack {
                    Text(dateFormatter.string(from: task.time))
                    if (task.connectedTask != nil) {
                        connectedTaskSummaryView(task: task.connectedTask)
                    }
                }
                
            }
            label: {
                VStack {
                    Label(task.name, systemImage: "clock")
                    
                }
            
            }
        } else {
            LabeledContent {
                VStack {
                    Text(dateFormatter.string(from: task.time))
                }
            }
            label: {
                VStack {
                    Label(task.name, systemImage: "timer")
                    
                }
                
            }
        }
    }
}

#Preview {
    TaskSummaryView(task: ControllerState.Task("Test Task On"))
}
