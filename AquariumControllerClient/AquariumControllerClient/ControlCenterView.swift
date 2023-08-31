//
//  ControlCenterView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/27/23.
//

import SwiftUI

struct ControlCenterView: View {
    @ObservedObject var controllerState: ControllerState
    @EnvironmentObject var aqController: AqController
    //var network: Network = aqController.network
    var body: some View {
        GroupBox(label: Text("Control Center")) {
            VStack {
                ForEach(controllerState.devices, id: \.name) { device in
                    if (device.name != "Filter" && device.name != "Heater") {
                        DeviceView(device: device)
                        Divider()
                    }   
                }
                Toggle(isOn: Binding(
                    get:{controllerState.maintenanceMode},
                    set:{v in
                        controllerState.maintenanceMode = v
                        Task {
                            await aqController.network.maintenanceToggleChange(state: v)
                        }
                        
                    })) {
                      Text("Maintenance Mode")
                    }
                }.padding(.horizontal, 20)
                
            }.padding(/*@START_MENU_TOKEN@*/.vertical, 10.0/*@END_MENU_TOKEN@*/).padding(.horizontal, 20)
    }
}


#Preview {
    
    ControlCenterView(controllerState: ControllerState())
        .environmentObject(AqController())
}
