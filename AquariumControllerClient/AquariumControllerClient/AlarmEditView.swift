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
    var alarmDateTime: Date = Date()
    let dateFormatter = DateFormatter()
    let step = 1
    let range = 66...93
    init(alarm: Alarm) {
        self.alarm = alarm
        //dateFormatter.dateFormat = "MM-dd HH:mm:ss"
        dateFormatter.dateFormat = "MM/dd HH:mm"
        alarmDateTime = Date(timeIntervalSince1970: Double(alarm.alarmState))
        
    }
    var body: some View {
        List {
            Section {
                Text(alarm.getName())
                HStack {
                    if (alarm.alarmState != 0) {
                        Text(String("Alarm at " + dateFormatter.string(from: alarmDateTime)))
                        Spacer()
                        Image(systemName: "exclamationmark.triangle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.red).padding(.trailing).frame(width: 22.0, height: 22.0)
                    } else {
                        Text(String("Status OK"))
                        Spacer()
                        Image(systemName: "checkmark.circle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.green).padding(.trailing).frame(width: 22.0, height: 22.0)
                    }
                }

            }
            Section {
                Toggle(isOn: Binding(
                    get:{alarm.alarmOverride},
                    set:{v in
                        alarm.setAlarmOverride(override: v)
                        Task {
                            await aqController.network.setAlarmOverride(alarm: alarm)
                        }
                    })) {
                        Text("Alarm Override")
                    }
            } footer: {
                Text("When this is enabled, the aquarium controller sends alerts during an active alarm but won't take corrective actions.").textCase(nil).bold()
            }
            Section {
                Button("Reset Alarm", action: {
                    Task{
                        alarm.setAlarmState(newState: 0)
                        await aqController.network.setAlarmState(alarm: alarm)
                    }
                })
            } header: {
                Text("Tap to Reset Alarm").textCase(nil).bold()
            }
        }
    }
}

#Preview {
    AlarmEditView(alarm: Alarm("Test Alarm"))
        .environmentObject(AqController())
}
