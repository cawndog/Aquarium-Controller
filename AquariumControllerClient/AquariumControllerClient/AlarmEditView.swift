//
//  AlarmEditView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/4/24.
//

import SwiftUI

struct AlarmEditView: View {
    @ObservedObject var alarm: Alarm
    @EnvironmentObject var aqController: AqController
    @State var editableAlarmValue: Int
    var alarmDateTime: Date = Date()
    let dateFormatter = DateFormatter()
    let step = 1
    let range = 66...93
    init(alarm: Alarm) {
        self.alarm = alarm
        self.editableAlarmValue = alarm.getAlarmState()
        //dateFormatter.dateFormat = "MM-dd HH:mm:ss"
        dateFormatter.dateFormat = "MM/dd HH:mm"
        alarmDateTime = Date(timeIntervalSince1970: Double(editableAlarmValue))
        
    }
    var body: some View {
        List {
            Section {
                HStack {
                    if (editableAlarmValue != 0) {
                        Text(String("Alarm at " + dateFormatter.string(from: alarmDateTime)))
                        Spacer()
                        Image(systemName: "exclamationmark.triangle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.red).padding(.trailing).frame(width: 20.0, height: 20.0)
                    } else {
                        Text(String("Alarm OK"))
                        Spacer()
                        Image(systemName: "checkmark.circle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.green).padding(.trailing).frame(width: 20.0, height: 20.0)
                    }
                }

            }
            Section {
                Button("Reset Alarm", action: {
                    Task{
                        editableAlarmValue = 0
                        alarm.setAlarmState(newState: editableAlarmValue)
                        await aqController.network.setAlarmState(alarm: alarm)
                    }
                })
            } header: {
                Text("Tap to Reset " + alarm.getName()).textCase(nil).bold()
            }
        }
    }
}

#Preview {
    AlarmEditView(alarm: Alarm("Test Alarm"))
        .environmentObject(AqController())
}
