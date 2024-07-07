//
//  GeneralSettingEditView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/4/24.
//

import SwiftUI

struct GeneralSettingEditView: View {
    @ObservedObject var generalSetting: ControllerState.GeneralSetting
    @EnvironmentObject var aqController: AqController
    @State var editableSettingValue: Int
    let step = 1
    let range = 66...93
    init(generalSetting: ControllerState.GeneralSetting) {
        self.generalSetting = generalSetting
        self.editableSettingValue = generalSetting.value
    }
    var body: some View {
        List {
            Section {
                Stepper(value: $editableSettingValue, in: range, step: step) {
                    LabeledContent {
                        Text(String(editableSettingValue) + " °F")
                    }
                    label: {
                        Label("", systemImage: labelLookup(settingName: generalSetting.name))
                    }
                }
            } header: {
                Text("Set " + generalSetting.name).textCase(nil).bold()
            }
        }
        .toolbar {
            Button("Save", action: {
                Task{
                    generalSetting.value = editableSettingValue
                    await aqController.network.setGeneralSettingState(generalSetting: generalSetting)
                }
            })
        }
    }


    
    func labelLookup(settingName: String) -> String {
        switch (settingName) {
            case "Maintenance Mode":
                return "engine.combustion.badge.exclamationmark"
            case "Feed Mode":
                return "fish"
            case "Thermostat":
                return "thermometer"
            default:
                return ""
        }
    }
}

#Preview {
    GeneralSettingEditView(generalSetting: ControllerState.GeneralSetting("Test Setting"))
}
