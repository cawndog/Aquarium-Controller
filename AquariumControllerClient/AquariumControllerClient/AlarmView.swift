//
//  AlarmView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/4/24.
//

import SwiftUI

struct AlarmView: View {
    @ObservedObject var alarm: Alarm
    @EnvironmentObject var aqController: AqController
    init(alarm: Alarm) {
        self.alarm = alarm
    }
    var body: some View {
        HStack {
            Text(String(alarm.getName()))
            Spacer()
            if (alarm.getAlarmState() != 0) {
                Image(systemName: "exclamationmark.triangle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.red).padding(.trailing).frame(width: 20.0, height: 20.0)
            } else {
                Image(systemName: "checkmark.circle.fill").resizable(resizingMode: .stretch).aspectRatio(contentMode: .fill).foregroundColor(.green).padding(.trailing).frame(width: 20.0, height: 20.0)
            }
        }
        /*
        LabeledContent {
            Image(systemName: "exclamationmark.triangle.fill").foregroundColor(.red)
        }
        label: {
            Text(String(alarm.name))
        }
         */

    }
    func labelLookup(alarmName: String) -> String {
        if (alarmName.contains("Water Sensor")) {return "drop"}
        if (alarmName.contains("Temperature")) {return "thermometer.transmission"}
        return ""
    }
}

#Preview {
    AlarmView(alarm: Alarm("Test Alarm"))
        .environmentObject(AqController())
}
