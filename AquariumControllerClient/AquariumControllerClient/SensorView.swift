//
//  SensorView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/27/23.
//

import SwiftUI

struct SensorView: View {
    @ObservedObject var sensor: ControllerState.Sensor
    var units: String = ""
    init(sensor: ControllerState.Sensor) {
        self.sensor = sensor
        if (sensor.name == "TDS") {
            //Text("PPM")
            units = " PPM"
        }
        else if (sensor.name == "Aquarium Temperature") {
            //Text("°F")
            units = " °F"
        }
    }
    var body: some View {
        LabeledContent(sensor.name) {
            Text(sensor.value + units)
            //\(privateIp)
        }
    }
}

#Preview {
    SensorView(sensor: ControllerState.Sensor("Test Sensor"))
}
