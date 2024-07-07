//
//  ParametersView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/26/23.
//

import SwiftUI

struct ParametersView: View {
    @ObservedObject var controllerState: ControllerState
    var body: some View {
        GroupBox(label: Text("Parameters")) {
            VStack {
                ForEach(controllerState.sensors, id: \.name) { sensor in
                    SensorView(sensor: sensor)
                    if sensor.name != controllerState.sensors.last?.name {
                        Divider()
                    }
                    
                }
              /*  SensorView(sensor: currentState.sensors[currentState.getSensorPosByName("Temperature")])
                Divider()
                SensorView(sensor: currentState.sensors[currentState.getSensorPosByName("TDS")])*/

            }.padding(.horizontal, 20)
                
        }.padding(/*@START_MENU_TOKEN@*/.bottom, 50.0/*@END_MENU_TOKEN@*/).padding(.horizontal, 20)
    }
}

#Preview {
    ParametersView(controllerState: ControllerState())
        .environmentObject(AqController())
}
