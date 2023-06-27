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
    @EnvironmentObject var network: Network
    var disabled: Bool = false
    init(device: Device, connectedDevice: Device?) {
        self.device = device
        if (connectedDevice != nil) {
            self.connectedDevice = connectedDevice
        }
        if (device.name == "Heater") {
            disabled = true
        }
    }

    
    var body: some View {
        Toggle(isOn: $device.state) {
            Text(device.name)
        }.toggleStyle(.switch).disabled(disabled).onChange(of: device.state) {
            print ("in onChange for " + device.name)
            
            if (!device.stateUpdatedByController){
                if (connectedDevice != nil){
                    if (device.state == true && connectedDevice?.state == true) {
                        connectedDevice!.stateUpdatedByController = true
                        connectedDevice!.state = false
                    }
                    
                }
                    
                
                
                Task {
                    await network.checkToggleChange(device: device)
                }
            } else {
                device.stateUpdatedByController = false
            }
        }
    }
}

#Preview {
    DeviceView(device: Device("Test Device"), connectedDevice: nil)
}
