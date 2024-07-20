//
//  DeviceView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/23/23.
//

import SwiftUI

struct DeviceView: View {
    @ObservedObject var device: Device
    var connectedDevice: Device?
    @EnvironmentObject var aqController: AqController
    var disabled: Bool = false
    init(device: Device, connectedDevice: Device? = nil) {
        self.device = device
        if (connectedDevice != nil) {
            self.connectedDevice = connectedDevice
        }
        if (device.getName() == "Heater") {
            disabled = true
        }
    }

    func labelLookup(deviceName: String) -> String {
        switch (deviceName) {
            case "Lights":
                return "lightbulb"
            case "CO2":
                return "carbon.dioxide.cloud"
            case "Air Pump":
                return "bubbles.and.sparkles"
            case "Water Valve":
                return "water.waves"
            default:
                return ""
        }
    }
    var body: some View {
        if (device.name == "Heater") {
            LabeledContent(device.name) {
                if (device.state == true) {
                    Text("ON")
                }
                else {
                    Text("OFF")
                }
                
            }
        }
        else {
            Toggle(isOn: Binding(
                get:{device.state},
                set:{v in
                    device.setState(newState: v)
                    Task {
                        await aqController.network.deviceToggleChange(device: device)
                    }
                })) {
                    Label(device.getName(), systemImage: labelLookup(deviceName: device.getName()))
                }
        }
        
    }
}

#Preview {
    DeviceView(device: Device("Test Device"), connectedDevice: nil)
        .environmentObject(AqController())
}
