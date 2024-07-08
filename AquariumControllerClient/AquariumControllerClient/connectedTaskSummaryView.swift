//
//  connectedTaskSummaryView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/29/23.
//

import SwiftUI

struct connectedTaskSummaryView: View {
    @ObservedObject var task: AqTask
    let dateFormatter = DateFormatter()
    
    init(task: AqTask) {
        self.task = task
        if (task.taskType == .SCHEDULED_DEVICE_TASK || task.taskType == .SCHEDULED_TASK) {
            dateFormatter.dateFormat = "HH:mm"
        } else {
            dateFormatter.dateFormat = "HH:mm:ss"
        }
    }
    var body: some View {
        Text(dateFormatter.string(from: task.time))
        
    }
}

#Preview {
    connectedTaskSummaryView(task: AqTask("Test Task On"))
}
