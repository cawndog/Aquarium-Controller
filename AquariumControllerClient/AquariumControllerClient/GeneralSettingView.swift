//
//  GeneralSettingView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/4/24.
//

import SwiftUI

struct GeneralSettingView: View {
    @ObservedObject var generalSetting: GeneralSetting
    @EnvironmentObject var aqController: AqController
    
    init(generalSetting: GeneralSetting) {
        self.generalSetting = generalSetting
    }
    var body: some View {
        if (generalSetting.name == "Thermostat") {
            LabeledContent {
                Text(String(generalSetting.value) + " °F")
            }
            label: {
                Label(generalSetting.name, systemImage: labelLookup(settingName: generalSetting.name))
            }
        } else {
            Toggle(isOn: Binding(
                get:{(generalSetting.value != 0)},
                set:{v in
                    if (v) {generalSetting.value = 1} else { generalSetting.value = 0 }
                    Task {
                        await aqController.network.setGeneralSettingState(generalSetting: generalSetting)
                    }
                })) {
                    Label(generalSetting.name, systemImage: labelLookup(settingName: generalSetting.name))
                }
        }

    }
    func labelLookup(settingName: String) -> String {
        switch (settingName) {
            case "Maintenance Mode":
                return "engine.combustion.badge.exclamationmark"
            case "Feed Mode":
                return "fork.knife.circle"
            case "Thermostat":
                return "thermometer"
            default:
                return ""
        }
    }
}

#Preview {
    GeneralSettingView(generalSetting: GeneralSetting("Test Setting"))
        .environmentObject(AqController())
}

