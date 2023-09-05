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
            Spacer()
                .frame(height: 10)
            Text("Aquarium Controller")
                .font(.title)
                .fontWeight(.bold)
                .foregroundColor(Color.blue)
                .padding(.vertical, 20.0)
            Spacer()
                    .frame(height: 60)
            ParametersView(controllerState: aqController.controllerState)
            ControlCenterView(controllerState: aqController.controllerState)
            Spacer()
                    .frame(height: 100)
        }
    }
}

#Preview {
    HomeView()
        .environmentObject(AqController())
}
