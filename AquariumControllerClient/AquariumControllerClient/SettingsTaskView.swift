//
//  SettingsTaskView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/28/23.
//

import SwiftUI

struct SettingsTaskView: View {
    @ObservedObject var task: ControllerState.Task
    init(task: ControllerState.Task) {
        self.task = task
    }
    var body: some View {
        
        LabeledContent {
            Text(String(task.time))
        }
        label: {
            VStack {
                Label(task.name, systemImage: "clock")
                if (task.connectedTask != nil) {
                    Text(task.connectedTask.name)
                }
            }
            
        }
        
    }
}

#Preview {
    SettingsTaskView(task: ControllerState.Task("Test Task On"))
}
