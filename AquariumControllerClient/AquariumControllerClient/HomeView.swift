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
            /*Text("Aquarium Controller")
                .font(.title)
                .fontWeight(.bold)
                .foregroundColor(Color.blue)
                .padding(.top, 20.0)
             */
            //Image(systemName: "fish.circle.fill")
            Image("GreenFishNoBackground")
                .resizable(resizingMode: .stretch)
                .aspectRatio(contentMode: .fill)
                .padding(.vertical, -5.0)
                .frame(width: 90.0, height: 90.0)
            //Spacer().frame(height: 60)
            //Spacer()
            ParametersView(controllerState: aqController.controllerState)
                .padding(.top, 5.0)
            ControlCenterView(controllerState: aqController.controllerState)
            //Spacer()
            //Spacer()
            //Spacer().frame(height: 100)
        }.frame(maxWidth: .infinity, // Full Screen Width
                maxHeight: .infinity, // Full Screen Height
                alignment: .topLeading) // Align To top
    }
}

#Preview {
    HomeView()
        .environmentObject(AqController())
}
