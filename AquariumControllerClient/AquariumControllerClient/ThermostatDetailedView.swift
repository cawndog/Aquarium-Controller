//
//  ThermostatDetailedView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/30/23.
//

import SwiftUI

struct ThermostatDetailedView: View {
    @ObservedObject var controllerState: ControllerState
    let step = 1
    let range = 66...93
    var body: some View {
        List {
            Section {
                Stepper(value: $controllerState.aqThermostat, in: range, step: step) {
                    LabeledContent {
                        Text(String(controllerState.aqThermostat) + " °F")
                    }
                    label: {
                        Label("", systemImage: "thermometer")
                    }
                }
            } header: {
                Text("Set Aquarium Thermostat").textCase(nil).bold()
            }
        }
    }
}

#Preview {
    ThermostatDetailedView(controllerState: ControllerState())
}
