//
//  DeviceView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 6/23/23.
//

import SwiftUI

struct DeviceView: View {
    @ObservedObject var device: ControllerState.Device
    var connectedDevice: ControllerState.Device?
    @EnvironmentObject var aqController: AqController
    var disabled: Bool = false
    init(device: ControllerState.Device, connectedDevice: ControllerState.Device? = nil) {
        self.device = device
        if (connectedDevice != nil) {
            self.connectedDevice = connectedDevice
        }
        if (device.name == "Heater") {
            disabled = true
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
                    device.state = v
                    Task {
                        await aqController.network.deviceToggleChange(device: device)
                    }
                })) {
                    Text(device.name)
                }
        }
        
    }
}

#Preview {
    DeviceView(device: ControllerState.Device("Test Device"), connectedDevice: nil)
        .environmentObject(AqController())
}
