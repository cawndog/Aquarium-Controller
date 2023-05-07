//
//  SettingsView.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/4/23.
//

import SwiftUI
import Foundation
struct SettingsView: View {
    @State var tempSet: Int = 82
    let tempRange = 64...94
    let tempStep = 1
    @State var currentDate = Date();
    
    var body: some View {
        NavigationView {

            Form {
                Section(header: Text("Thermostat")) {
                    Stepper(value: $tempSet, in: tempRange, step: tempStep) {
                        LabeledContent ("Set Temp") {
                            Spacer()
                            Text("\(tempSet) °F")
                            Spacer()
                        }
                    }
                }
                Section(header: Text("Timers")) {
                    DatePicker("CO2 On Time", selection: $currentDate, displayedComponents: .hourAndMinute)
                    DatePicker("CO2 Off Time", selection: $currentDate, displayedComponents: .hourAndMinute)
                }
                Section {
                    DatePicker("Air Pump On Time", selection: $currentDate, displayedComponents: .hourAndMinute)
                    DatePicker("Air Pump Off Time", selection: $currentDate, displayedComponents: .hourAndMinute)
                }
                
            }.navigationTitle("Settings")
            .toolbar {
                EditButton()
                    .padding(.trailing, 12.0)
                    
            }.onAppear {
                var comps = DateComponents()
                comps.hour = 19
                comps.minute = 30
                comps.second = 0
                currentDate = Calendar.current.date(from: comps)!
                print(currentDate)
                print(currentDate.formatted(date: .long, time: .complete))
                print(currentDate.formatted(Date.FormatStyle().hour(.defaultDigitsNoAMPM)))
                comps = Calendar.current.dateComponents([.hour, .minute], from: currentDate)
                print(comps.hour!)
                print(comps.minute!)
                      }
        }
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var previews: some View {
        SettingsView()
    }
}
