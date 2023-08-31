//
//  Network.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/1/23.
//

import Foundation
import SwiftUI

class Network: ObservableObject {
    @Published var messages = [String]()
    //@Published var currentState: CurrentState
    var controllerState: ControllerState?
    //var settingsState: SettingsState
    //@State var currentState: CurrentState
    //var currentState: CurrentState
    private var webSocketTask: URLSessionWebSocketTask?
    //var currentStateJSON: CurrentStateJSON
    //var settingsStateJSON: SettingsStateJSON
    var comps: DateComponents
    let bearerToken: String = "31f18cfbab58825aedebf9d0e14057dc"
    //var AqControllerIP: String = "AquariumController.freeddns.org"
    var AqControllerIP: String = "10.0.0.42"
    
    init() {
        //self.controllerState = nil
        //self.settingsState = settingsState
        //currentStateJSON = CurrentStateJSON.init()
        //settingsStateJSON = SettingsStateJSON.init()
        comps = DateComponents()
    }
    
    func attachControllerState(controllerState: ControllerState) {
        self.controllerState = controllerState
    }

    func connectWebSocket() {
        guard let url = URL(string: "ws://\(AqControllerIP):80/") else { return }
        let request = URLRequest(url: url)
        webSocketTask = URLSession.shared.webSocketTask(with: request)
        webSocketTask?.resume()
        receiveMessage()
            
    }
    func receiveMessage() {
        webSocketTask?.receive { result in
            switch result {
                case .failure(let error):
                    print(error.localizedDescription)
                case .success(let message):
                    switch message {
                        case .string(let text):
                            //self.messages.append(text)
                            self.processStringMessage(WebSocketMessage: text)
                        case .data(let data):
                            // Handle binary data
                            break
                        @unknown default:
                            break
                    }
            }
        }
    }
    func processStringMessage(WebSocketMessage:String) {
        guard let controllerState = controllerState else { return }
        let jsonMessage = WebSocketMessage.data(using: .utf8)!
        let decodedMessage = try! JSONDecoder().decode(AqControllerMessage.self, from: jsonMessage)
        if (decodedMessage.messageType == .StateUpdate) {
            if let maintMode = decodedMessage.maintenanceMode {
                controllerState.maintenanceMode = maintMode
            }
            if let sensors = decodedMessage.sensors {
                for sensor in sensors {
                    var s = controllerState.getSensorByName(sensor.name)
                    s.value = sensor.value
                }
            }
            if let devices = decodedMessage.devices {
                for device in devices {
                    let d = controllerState.getDeviceByName(device.name)
                    d.stateUpdatedByController = true
                    d.state = device.state
                }
            }
        }
        if (decodedMessage.messageType == .SettingsUpdate) {
            if let thermostat = decodedMessage.aqThermostat {
                controllerState.aqThermostat = thermostat
            }
            if let tasks = decodedMessage.tasks {
                var dateComps = DateComponents()
                var hours: Int
                var minutes: Int
                var seconds: Int
                var taskTime: Int
                for task in tasks {
                    var t = controllerState.getTaskByName(task.name)
                    taskTime = task.time
                    seconds = taskTime%60
                    taskTime -= seconds
                    minutes = (taskTime%3600)/60
                    taskTime -= (minutes*60)
                    hours = taskTime/3600
                    dateComps.hour = hours
                    dateComps.minute = minutes
                    dateComps.second = seconds
                    t.time = Calendar.current.date(from: dateComps)!
                    t.setTaskTypeWithString(task.taskType.rawValue)
                    t.isDisabled = task.isDisabled
                    if let connectedTask = task.connectedTask {
                        t.connectedTask = ControllerState.Task(connectedTask.name)
                        taskTime = connectedTask.time
                        seconds = taskTime%60
                        taskTime -= seconds
                        minutes = (taskTime%3600)/60
                        taskTime -= (minutes*60)
                        hours = taskTime/3600
                        dateComps.hour = hours
                        dateComps.minute = minutes
                        dateComps.second = seconds
                        t.connectedTask.time = Calendar.current.date(from: dateComps)!
                        t.connectedTask.setTaskTypeWithString(connectedTask.taskType.rawValue)
                        t.connectedTask.isDisabled = connectedTask.isDisabled
                    }
                }
            }
        }
    }
    func sendMessage(_ message: String) {
        guard let data = message.data(using: .utf8) else { return }
        webSocketTask?.send(.string(message)) { error in
            if let error = error {
                print(error.localizedDescription)
            }
        }
    }
    func getSettingsState() {
        guard let controllerState = self.controllerState else { return }
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
                        let decodedMessage = try JSONDecoder().decode(AqControllerMessage.self, from: data)
                        if (decodedMessage.messageType == .SettingsUpdate) {
                            if let thermostat = decodedMessage.aqThermostat {
                                controllerState.aqThermostat = thermostat
                            }
                            if let tasks = decodedMessage.tasks {
                                var dateComps = DateComponents()
                                var hours: Int
                                var minutes: Int
                                var seconds: Int
                                var taskTime: Int
                                for task in tasks {
                                    var t = controllerState.getTaskByName(task.name)
                                    taskTime = task.time
                                    seconds = taskTime%60
                                    taskTime -= seconds
                                    minutes = (taskTime%3600)/60
                                    taskTime -= (minutes*60)
                                    hours = taskTime/3600
                                    dateComps.hour = hours
                                    dateComps.minute = minutes
                                    dateComps.second = seconds
                                    t.time = Calendar.current.date(from: dateComps)!
                                    t.setTaskTypeWithString(task.taskType.rawValue)
                                    t.isDisabled = task.isDisabled
                                    if let connectedTask = task.connectedTask {
                                        t.connectedTask = ControllerState.Task(connectedTask.name)
                                        taskTime = connectedTask.time
                                        seconds = taskTime%60
                                        taskTime -= seconds
                                        minutes = (taskTime%3600)/60
                                        taskTime -= (minutes*60)
                                        hours = taskTime/3600
                                        dateComps.hour = hours
                                        dateComps.minute = minutes
                                        dateComps.second = seconds
                                        t.connectedTask.time = Calendar.current.date(from: dateComps)!
                                        t.connectedTask.setTaskTypeWithString(connectedTask.taskType.rawValue)
                                        t.connectedTask.isDisabled = connectedTask.isDisabled
                                    }
                                }
                            }
                        }
                        //------------airPump
                        
                        /*self.comps.hour = self.settingsStateJSON.timers.airPump.onHr
                        self.comps.minute = self.settingsStateJSON.timers.airPump.onMin
                        self.settingsState.timers.airPump.onTime = Calendar.current.date(from: self.comps)!
                        self.comps.hour = self.settingsStateJSON.timers.airPump.offHr
                        self.comps.minute = self.settingsStateJSON.timers.airPump.offMin
                        self.settingsState.timers.airPump.offTime = Calendar.current.date(from: self.comps)!*/
                        
                    } catch let error {
                        print("Error decoding: ", error)
                    }
                }
            }
        }
        
        dataTask.resume()
         
    }
    func setSettingsState() async {
        guard let controllerState = self.controllerState else { return }
        var newMessage = AqControllerMessage()
        newMessage.messageType = .SettingsUpdate
        newMessage.aqThermostat = controllerState.aqThermostat
        
        for task in controllerState.tasks {
            
            var taskDateComp = Calendar.current.dateComponents([.hour, .minute, .second], from: task.time)
            var taskTime: Int = 0
            var newTask = AqControllerMessage.Task()
            newTask.name = task.name
            newTask.isDisabled = task.isDisabled
            taskTime = taskDateComp.hour! * 3600
            taskTime += taskDateComp.minute! * 60
            taskTime += taskDateComp.second!
            newTask.time = taskTime
            
            if (task.connectedTask != nil) {
                newTask.connectedTask = AqControllerMessage.Task()
                newTask.connectedTask!.name = task.connectedTask.name
                newTask.connectedTask!.isDisabled = task.connectedTask.isDisabled
                taskDateComp = Calendar.current.dateComponents([.hour, .minute, .second], from: task.connectedTask.time)
                taskTime = taskDateComp.hour! * 3600
                taskTime += taskDateComp.minute! * 60
                taskTime += taskDateComp.second!
                newTask.connectedTask!.time = taskTime
            }
            newMessage.addTask(newTask)
        }
        
        guard let encoded = try? JSONEncoder().encode(newMessage) else {
            print("Failed to encode JSON")
            return
        }
        print(encoded)
        let urlString: String = "http://\(AqControllerIP)/setSettingsState"
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
        guard let controllerState = self.controllerState else { return }
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
                        let decodedMessage = try JSONDecoder().decode(AqControllerMessage.self, from: data)
                        
                        print(decodedMessage)
                        if (decodedMessage.messageType == .StateUpdate) {
                            if let maintMode = decodedMessage.maintenanceMode {
                                controllerState.maintenanceMode = maintMode
                            }
                            if let sensors = decodedMessage.sensors {
                                for sensor in sensors {
                                    var s = controllerState.getSensorByName(sensor.name)
                                    s.value = sensor.value
                                }
                            }
                            if let devices = decodedMessage.devices {
                                for device in devices {
                                    var d = controllerState.getDeviceByName(device.name)
                                    d.stateUpdatedByController = true
                                    d.state = device.state
                                }
                            }
                        }
                    } catch let error {
                        print("Error decoding: ", error)
                    }
                }
            }
        }
        
        dataTask.resume()
    }
    func maintenanceToggleChange(state: Bool) async {
        print("maintenanceStateChange() called")
        guard let encoded = try? JSONEncoder().encode("") else {
            print("Failed to encode JSON")
            return
        }
        var urlString: String = "http://\(AqControllerIP)/maintMode"
        if (state == true) {
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
    func deviceToggleChange(device: ControllerState.Device) async {
        print("deviceToggleChange() called for " + device.name)
        guard let controllerState = self.controllerState else { return }
        //guard (!device.stateUpdatedByController) else {device.stateUpdatedByController = false; return}
        guard let encoded = try? JSONEncoder().encode("") else {
            print("Failed to encode JSON")
            return
        }
        var urlString: String = "http://\(AqControllerIP)/" + device.name
        if (device.state == true) {
            urlString = urlString + "On"
            if (device.name == "CO2") {
                print("settings Air Pump to false")
                controllerState.getDeviceByName("Air Pump").state = false
            }
            else if (device.name == "Air Pump") {
                controllerState.getDeviceByName("CO2").state = false
            }
            
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
