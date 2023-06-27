//
//  SensorView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/27/23.
//

import SwiftUI

struct SensorView: View {
    @ObservedObject var sensor: Sensor
    init(sensor: Sensor) {
        self.sensor = sensor
    }
    var body: some View {
        LabeledContent(sensor.name) {
            Text(sensor.value)
        }
    }
}

#Preview {
    SensorView(sensor: Sensor("Test Sensor"))
}
