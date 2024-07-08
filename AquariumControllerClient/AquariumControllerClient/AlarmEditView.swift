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
    let step = 1
    let range = 66...93
    init(alarm: Alarm) {
        self.alarm = alarm
        self.editableAlarmValue = alarm.getAlarmState()
    }
    var body: some View {
        List {
            Section {
                HStack {
                    Text(String(alarm.name))
                    Spacer()
                    if (editableAlarmValue != 0) {
                        Image(systemName: "exclamationmark.triangle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.red).padding(.trailing).frame(width: 20.0, height: 20.0)
                    } else {
                        Image(systemName: "checkmark.circle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.green).padding(.trailing).frame(width: 20.0, height: 20.0)
                    }
                }

            }
            Section {
                Button("Reset Alarm", action: {
                    Task{
                        alarm.setAlarmState(newState: editableAlarmValue)
                        await aqController.network.setAlarmState(alarm: alarm)
                    }
                })
            } header: {
                Text("Reset " + alarm.getName()).textCase(nil).bold()
            }
        }
    }
}

#Preview {
    AlarmEditView(alarm: Alarm("Test Alarm"))
}
