//
//  SensorView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/27/23.
//

import SwiftUI

struct SensorView: View {
    @ObservedObject var sensor: Sensor
    var units: String = ""
    init(sensor: Sensor) {
        self.sensor = sensor
        if (sensor.getName() == "TDS") {
            //Text("PPM")
            units = " PPM"
        }
        else if (sensor.getName() == "Aquarium Temperature") {
            //Text("°F")
            units = " °F"
        }
    }
    var body: some View {
        LabeledContent(sensor.getName()) {
            Text(sensor.getValue() + units)
            //\(privateIp)
        }
    }
}

#Preview {
    SensorView(sensor: Sensor("Test Sensor"))
}
