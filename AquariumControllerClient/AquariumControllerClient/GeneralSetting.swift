//
//  GeneralSetting.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 7/7/24.
//

import Foundation
import SwiftUI

class GeneralSetting: HashableClass, Identifiable, ObservableObject {
    var id: String
    var name: String
    @Published var value: Int
    init(_ name: String) {
        self.id = name
        self.name = name
        self.value = 0
    }
    func getName() -> String {
        return self.name
    }
    func getValue() -> Int {
        return self.value
    }
    func setValue(newValue: Int) {
        self.value = newValue
    }
}
