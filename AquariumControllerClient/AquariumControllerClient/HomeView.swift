//
//  HomeView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 8/26/23.
//

import SwiftUI

struct HomeView: View {
    @EnvironmentObject var aqController: AqController
    var body: some View {
        VStack {
            Text("Aquarium Controller")
                .font(.title)
                .fontWeight(.bold)
                .foregroundColor(Color.blue)
                .padding(.vertical, 20.0)
            ParametersView(controllerState: aqController.controllerState)
            ControlCenterView(controllerState: aqController.controllerState)
            
        }
    }
}

#Preview {
    HomeView()
        .environmentObject(AqController())
}
