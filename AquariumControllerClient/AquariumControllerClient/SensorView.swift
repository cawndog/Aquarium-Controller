//
//  SensorView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/27/23.
//

import SwiftUI

struct SensorView: View {
    @ObservedObject var sensor: ControllerState.Sensor
    init(sensor: ControllerState.Sensor) {
        self.sensor = sensor
    }
    var body: some View {
        LabeledContent(sensor.name) {
            Text(sensor.value)
            if (sensor.name == "TDS") {
                Text("PPM")
            }
            else if (sensor.name == "Temperature") {
                Text("°F")
            }
        }
    }
}

#Preview {
    SensorView(sensor: ControllerState.Sensor("Test Sensor"))
}
