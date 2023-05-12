//
//  Network.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/1/23.
//

import Foundation
import SwiftUI

class Network: ObservableObject {
    @Published var currentState: CurrentState
    @Published var settingsState: SettingsState
    //@State var currentState: CurrentState
    //var currentState: CurrentState
    var currentStateJSON: CurrentStateJSON
    var settingsStateJSON: SettingsStateJSON
    var comps: DateComponents
    let bearerToken: String = "31f18cfbab58825aedebf9d0e14057dc"
    var AqControllerIP: String = "AquariumController.freeddns.org"
    init() {
        currentStateJSON = CurrentStateJSON.init()
        currentState = CurrentState.init()
        settingsStateJSON = SettingsStateJSON.init()
        settingsState = SettingsState.init()
        comps = DateComponents()
    }
    func getSettingsState() {
        guard let url = URL(string: "http://\(AqControllerIP)/getSettingsState") else { fatalError("Missing URL") }
        
        var urlRequest = URLRequest(url: url)
        urlRequest.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        let dataTask = URLSession.shared.dataTask(with: urlRequest) { (data, response, error) in
            if let error = error {
                print("Request error: ", error)
                return
            }
            
            guard let response = response as? HTTPURLResponse else { return }
            if response.statusCode == 200 {
                guard let data = data else { return }
                DispatchQueue.main.async {
                    do {
                        let decodedState = try JSONDecoder().decode(SettingsStateJSON.self, from: data)
                        self.settingsStateJSON = decodedState
                        self.settingsState.aquariumThermostat = self.settingsStateJSON.aquariumThermostat
                        //------------airPump
                        self.comps.hour = self.settingsStateJSON.timers.airPump.onHr
                        self.comps.minute = self.settingsStateJSON.timers.airPump.onMin
                        self.settingsState.timers.airPump.onTime = Calendar.current.date(from: self.comps)!
                        self.comps.hour = self.settingsStateJSON.timers.airPump.offHr
                        self.comps.minute = self.settingsStateJSON.timers.airPump.offMin
                        self.settingsState.timers.airPump.offTime = Calendar.current.date(from: self.comps)!
                        //----------------co2
                        self.comps.hour = self.settingsStateJSON.timers.co2.onHr
                        self.comps.minute = self.settingsStateJSON.timers.co2.onMin
                        self.settingsState.timers.co2.onTime = Calendar.current.date(from: self.comps)!
                        self.comps.hour = self.settingsStateJSON.timers.co2.offHr
                        self.comps.minute = self.settingsStateJSON.timers.co2.offMin
                        self.settingsState.timers.co2.offTime = Calendar.current.date(from: self.comps)!
                        //-------------lights
                        self.comps.hour = self.settingsStateJSON.timers.lights.onHr
                        self.comps.minute = self.settingsStateJSON.timers.lights.onMin
                        self.settingsState.timers.lights.onTime = Calendar.current.date(from: self.comps)!
                        self.comps.hour = self.settingsStateJSON.timers.lights.offHr
                        self.comps.minute = self.settingsStateJSON.timers.lights.offMin
                        self.settingsState.timers.lights.offTime = Calendar.current.date(from: self.comps)!
                        //print(self.settingsStateJSON)
                        
                    } catch let error {
                        print("Error decoding: ", error)
                    }
                }
            }
        }
        
        dataTask.resume()
         
    }
    func setSettingsState() async {
        
        guard let encoded = try? JSONEncoder().encode(self.settingsStateJSON) else {
            print("Failed to encode JSON")
            return
        }
        print(encoded)
        var urlString: String = "http://\(AqControllerIP)/setSettingsState"
        let url = URL(string: urlString)!
        var request = URLRequest(url: url)
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpMethod = "POST"
        request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        do {
            let (data, _) = try await URLSession.shared.upload(for: request, from: encoded)
            // handle the result
        } catch {
            print("Device state change failed.")
        }
    }
    func getCurrentState() {
        /*self.currentState.temp = "83.4  F"
        self.currentState.tds = "234 PPM"
        self.currentState.lights.deviceState = true
        self.currentState.lights.stateUpdatedByController = true
        self.currentState.co2.deviceState = true
        self.currentState.co2.stateUpdatedByController = true
        */
        guard let url = URL(string: "http://\(AqControllerIP)/getCurrentState") else { fatalError("Missing URL") }
        
        var urlRequest = URLRequest(url: url)
        urlRequest.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        let dataTask = URLSession.shared.dataTask(with: urlRequest) { (data, response, error) in
            if let error = error {
                print("Request error: ", error)
                return
            }
            
            guard let response = response as? HTTPURLResponse else { return }
            if response.statusCode == 200 {
                guard let data = data else { return }
                DispatchQueue.main.async {
                    do {
                        let decodedState = try JSONDecoder().decode(CurrentStateJSON.self, from: data)
                        self.currentStateJSON = decodedState
                        print(self.currentStateJSON)
                        self.currentState.temp = self.currentStateJSON.temp
                        self.currentState.tds = self.currentStateJSON.tds
                        if (self.currentState.lights.deviceState != self.currentStateJSON.lights) {
                            self.currentState.lights.stateUpdatedByController = true;
                            self.currentState.lights.deviceState = self.currentStateJSON.lights
                            
                        }
                        if (self.currentState.filter.deviceState != self.currentStateJSON.filter) {
                            self.currentState.filter.stateUpdatedByController = true;
                            self.currentState.filter.deviceState = self.currentStateJSON.filter
                            
                        }
                        if (self.currentState.co2.deviceState != self.currentStateJSON.co2) {
                            self.currentState.co2.stateUpdatedByController = true;
                            self.currentState.co2.deviceState = self.currentStateJSON.co2
                            
                        }
                        if (self.currentState.air.deviceState != self.currentStateJSON.air) {
                            self.currentState.air.stateUpdatedByController = true;
                            self.currentState.air.deviceState = self.currentStateJSON.air
                            
                        }
                        if (self.currentState.heater.deviceState != self.currentStateJSON.heater) {
                            self.currentState.heater.stateUpdatedByController = true;
                            self.currentState.heater.deviceState = self.currentStateJSON.heater
                            
                        }
                        if (self.currentState.maint.deviceState != self.currentStateJSON.maint) {
                            self.currentState.maint.stateUpdatedByController = true;
                            self.currentState.maint.deviceState = self.currentStateJSON.maint
                            
                        }
                    } catch let error {
                        print("Error decoding: ", error)
                    }
                }
            }
        }
        
        dataTask.resume()
         
    }
    func checkToggleChange(device: CurrentState.Device) async {
        print("checkToggleChange() called for " + device.name)
        //guard (!device.stateUpdatedByController) else {device.stateUpdatedByController = false; return}
        guard let encoded = try? JSONEncoder().encode("") else {
            print("Failed to encode JSON")
            return
        }
        var urlString: String = "http://\(AqControllerIP)/" + device.name
        if (device.deviceState == true) {
            urlString = urlString + "On"
            
        } else {
            urlString = urlString + "Off"
        }
        let url = URL(string: urlString)!
        var request = URLRequest(url: url)
        request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpMethod = "POST"
        do {
            let (data, _) = try await URLSession.shared.upload(for: request, from: encoded)
            // handle the result
        } catch {
            print("Device state change failed.")
        }
    }
}
    /*function checkClickFunc(device) {
                var checkbox = document.getElementById(device);
                if (checkbox.checked == true) {
                    if (device=='air') {
                        document.getElementById('co2').checked = false;
                    }
                    if (device=='co2') {
                        document.getElementById('air').checked = false;
                    }
                    if (device=='maint') {
                        filter = document.getElementById('filter');
                        filter.checked = false;
                        filter.disabled = true;
                        document.getElementById('heaterState').innerHTML = "OFF";
                    }
                    fetch('http://\(AqControllerIP)/'+device+'On', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ "id": 78912 })
                    })
                    .then (response => {
                        console.log(response.status);
                    });
                }
                else {
                    fetch('http://\(AqControllerIP)/'+device+'Off', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ "id": 78913 })
                    })
                    .then (response => {
                        console.log(response.status);
                        if (response.status == 200) {
                            if (device=='maint') {
                                filter = document.getElementById('filter');
                                filter.checked = true;
                                filter.disabled = false;
                            }
                        }
                    });
                }
            }
}
*/
