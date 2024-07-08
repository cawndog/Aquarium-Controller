//
//  Task.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/7/24.
//

import Foundation
import SwiftUI
class AqTask: HashableClass, Identifiable, ObservableObject {
    enum TaskType: String {
        case SCHEDULED_TASK, SCHEDULED_DEVICE_TASK, TIMED_TASK, Unknown
        init () {
            self = .Unknown
        }
        
    }
    var id: String
    var name: String
    var taskType: TaskType
    @Published var time: Date
    @Published var timeInSeconds: Int
    @Published var isDisabled: Bool
    @Published var connectedTask: AqTask!
    
    init(_ name: String) {
        self.id = name
        self.name = name
        self.time = Date()
        self.timeInSeconds = 1;
        self.isDisabled = true;
        //self.taskType = TaskType()
        //self.taskType = .Unknown
        self.taskType = .SCHEDULED_DEVICE_TASK
        self.connectedTask = nil
    }
    func setTaskTypeWithString(_ type: String) {
        switch (type) {
        case "SCHEDULED_TASK":
            self.taskType = .SCHEDULED_TASK
        case "SCHEDULED_DEVICE_TASK":
            self.taskType = .SCHEDULED_DEVICE_TASK
        case "TIMED_TASK":
            self.taskType = .TIMED_TASK
        default:
            self.taskType = .Unknown
            
        }
    }
    
    func copyFromTask(taskToCopy: AqTask) {
        self.taskType = taskToCopy.taskType
        self.time = taskToCopy.time
        self.timeInSeconds = taskToCopy.timeInSeconds
        self.isDisabled = taskToCopy.isDisabled
        if taskToCopy.connectedTask != nil {
            if self.connectedTask == nil {
                self.connectedTask = AqTask(taskToCopy.connectedTask.name)
            }
            self.connectedTask.taskType = taskToCopy.connectedTask.taskType
            self.connectedTask.time = taskToCopy.connectedTask.time
            self.connectedTask.timeInSeconds = taskToCopy.connectedTask.timeInSeconds
            self.connectedTask.isDisabled = taskToCopy.connectedTask.isDisabled
        }
        
    }
}
