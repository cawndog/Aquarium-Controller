//
//  Network.swift
//  Aquarium Controller
//
//  Created by Connor Williams on 5/1/23.
//

import Foundation
import SwiftUI
import SystemConfiguration.CaptiveNetwork

class Network: ObservableObject {
    
    var messages = [String]()
    var controllerState: ControllerState?
    private var webSocketTask: URLSessionWebSocketTask?
    var webSocketConnected: Bool
    var comps: DateComponents
    //let bearerToken = ProcessInfo.processInfo.environment["API_KEY"] ?? ""
    var bearerToken: String = env.bearerToken
    let publicDNS: String = env.publicDNS
    let privateIP: String = env.privateIP
    var aqConnectionString: String
    
    init() {
        aqConnectionString = publicDNS
        comps = DateComponents()
        webSocketConnected = false
    }
    
    func attachControllerState(controllerState: ControllerState) {
        self.controllerState = controllerState
    }
    
    func connectWebSocket() {
        guard let url = URL(string: "ws://\(aqConnectionString)/ws") else { return }
        let request = URLRequest(url: url)
        webSocketTask = URLSession.shared.webSocketTask(with: request)
        webSocketTask?.resume()
        print("Web socket connected.")
        webSocketConnected = true;
        receiveMessage()
        
    }
    func disconnectWebSocket() {
        guard let webSocketTask = self.webSocketTask else {return}
        webSocketTask.cancel(with: .goingAway, reason: nil)
        webSocketConnected = false
        print("WebSocket disconnected.")
    }
    func determineIP() async {
        let urlString: String = "http://\(publicDNS)/esp_alv"
        
        let url = URL(string: urlString)!
        var request = URLRequest(url: url)
        //request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        //request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpMethod = "GET"
        request.timeoutInterval = 20
        do {
            let (_, response) = try await URLSession.shared.data(for: request)
            if let httpResponse = response as? HTTPURLResponse {
                if httpResponse.statusCode == 204 {
                    //Server was reachable by the DNS alias pointing to its public IP. App will use DNS alias.
                    print("Setting aqConnectionString to publicDNS.")
                    aqConnectionString = publicDNS
                } else {
                    //Server was not reachable by the DNS alias pointing to its public IP. App will switch to using the server's private IP Address.
                    print("Setting aqConnectionString to privateIP.")
                    aqConnectionString = privateIP
                }
            }
        } catch {
            //Server was not reachable by the DNS alias pointing to its public IP. App will switch to using the server's private IP Address.
            print("Setting aqConnectionString to privateIP")
            aqConnectionString = privateIP
        }
    }
    func receiveMessage() {
        if (self.webSocketConnected) {
            webSocketTask?.receive { result in
                switch result {
                case .failure(let error):
                    print(error.localizedDescription)
                case .success(let message):
                    switch message {
                        case .string(let text):
                            print("Received a text message")
                            print(text)
                            //self.messages.append(text)
                            self.processStringMessage(WebSocketMessage: text)
                        case .data(let data):
                            print("Received a data message")
                            // Handle binary data
                            print(data)
                            break
                        @unknown default:
                            break
                    }
                }
                self.receiveMessage()
            }
        }
        
    }
    func processStringMessage(WebSocketMessage:String) {
        guard let controllerState = controllerState else { return }
        let jsonMessage = WebSocketMessage.data(using: .utf8)!
        let decodedMessage = try! JSONDecoder().decode(AqControllerMessage.self, from: jsonMessage)
        DispatchQueue.main.async {
            if let sensors = decodedMessage.sensors {
                for sensor in sensors {
                    var s = controllerState.getSensorByName(sensor.name)
                    s.value = sensor.value
                }
            }
            if let devices = decodedMessage.devices {
                for device in devices {
                    let d = controllerState.getDeviceByName(device.name)
                    d.setState(newState: device.state)
                    //d.stateUpdatedByController = true
                }
            }
            if let settings = decodedMessage.settings {
                if let generalSettings = settings.generalSettings {
                    for setting in generalSettings {
                        let s = controllerState.getGeneralSettingByName(setting.name)
                        s.setValue(newValue: setting.value)
                    }
                }
                if let alarms = settings.alarms{
                    for alarm in alarms {
                        let a = controllerState.getAlarmByName(alarm.name)
                        a.setAlarmState(newState: alarm.alarmState)
                    }
                }
                if let tasks = settings.tasks {
                    var dateComps = DateComponents()
                    var hours: Int
                    var minutes: Int
                    var seconds: Int
                    var taskTime: Int
                    for task in tasks {
                        let t = controllerState.getTaskByName(task.name)
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
                        t.timeInSeconds = task.time
                        t.setTaskTypeWithString(task.taskType.rawValue)
                        t.enabled = task.enabled
                        if let connectedTask = task.connectedTask {
                            if (t.connectedTask == nil) {
                                t.connectedTask = AqTask(connectedTask.name)
                            }
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
                            t.connectedTask.enabled = connectedTask.enabled
                        }
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
        print("getSettingsState() called.")
        guard let controllerState = controllerState else { return }
        guard let url = URL(string: "http://\(aqConnectionString)/getSettingsState") else { fatalError("Missing URL") }
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
                    let jsonString = NSString(data: data, encoding: String.Encoding.utf8.rawValue)
                    print(jsonString!)
                    self.processStringMessage(WebSocketMessage: jsonString! as String)
                }
            }
        }
        
        dataTask.resume()
        
    }
    func setSettingsState() async {/*
                                    guard let controllerState = self.controllerState else { return }
                                    var newMessage = AqControllerMessage()
                                    newMessage.messageType = .SettingsUpdate
                                    newMessage.aqThermostat = controllerState.aqThermostat
                                    
                                    for task in controllerState.tasks {
                                    
                                    var taskDateComp = Calendar.current.dateComponents([.hour, .minute, .second], from: task.time)
                                    var taskTime: Int = 0
                                    var newTask = AqControllerMessage.AqTask()
                                    newTask.name = task.name
                                    newTask.enabled = task.enabled
                                    taskTime = taskDateComp.hour! * 3600
                                    taskTime += taskDateComp.minute! * 60
                                    taskTime += taskDateComp.second!
                                    newTask.time = taskTime
                                    
                                    if (task.connectedTask != nil) {
                                    newTask.connectedTask = AqControllerMessage.AqTask()
                                    newTask.connectedTask!.name = task.connectedTask.name
                                    newTask.connectedTask!.enabled = task.enabled
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
                                    
                                    //let jsonString = NSString(data: encoded, encoding: String.Encoding.utf8.rawValue)
                                    print("In setSettingsState()")
                                    //print(jsonString)
                                    let urlString: String = "http://\(aqConnectionString)/setSettingsState"
                                    let url = URL(string: urlString)!
                                    var request = URLRequest(url: url)
                                    request.setValue("application/json", forHTTPHeaderField: "Content-Type")
                                    request.httpMethod = "POST"
                                    request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
                                    do {
                                    let (data, _) = try await URLSession.shared.upload(for: request, from: encoded)
                                    // handle the result
                                    } catch {
                                    print("setSettingsState() failed.")
                                    }*/
    }
    func setTaskState(task: AqTask) async {
        guard let controllerState = controllerState else { return }
        var newMessage = AqControllerMessage()
        var newSettings = AqControllerMessage.Settings()
        var newTask = AqControllerMessage.Settings.Task()
        newTask.name = task.name
        newTask.enabled = task.enabled
        //newTask.time = taskTime
        
        if (task.taskType == .TIMED_TASK) {
            newTask.time = task.timeInSeconds
        }
        else {
            var taskDateComp = Calendar.current.dateComponents([.hour, .minute, .second], from: task.time)
            var taskTime: Int = 0
            taskTime = taskDateComp.hour! * 3600
            taskTime += taskDateComp.minute! * 60
            taskTime += taskDateComp.second!
            newTask.time = taskTime
            if (task.connectedTask != nil) {
                newTask.connectedTask = AqControllerMessage.Settings.Task()
                newTask.connectedTask!.name = task.connectedTask.name
                newTask.connectedTask!.enabled = task.enabled
                taskDateComp = Calendar.current.dateComponents([.hour, .minute, .second], from: task.connectedTask.time)
                taskTime = taskDateComp.hour! * 3600
                taskTime += taskDateComp.minute! * 60
                taskTime += taskDateComp.second!
                newTask.connectedTask!.time = taskTime
            }
        }
        newSettings.addTask(newTask)
        newMessage.addSettings(newSettings)
        guard let encoded = try? JSONEncoder().encode(newMessage) else {
            print("Failed to encode JSON")
            return
        }
        //let jsonString = NSString(data: encoded, encoding: String.Encoding.utf8.rawValue)
        print("In setTaskState(task)")
        //print(jsonString)
        let urlString: String = "http://\(aqConnectionString)/setSettingsState"
        let url = URL(string: urlString)!
        var request = URLRequest(url: url)
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpMethod = "POST"
        request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        do {
            let (data, _) = try await URLSession.shared.upload(for: request, from: encoded)
            // handle the result
        } catch {
            print("setTaskState(task) failed.")
        }
    }
    func setGeneralSettingState (generalSetting: GeneralSetting) async {
        guard let controllerState = controllerState else { return }
        var newMessage = AqControllerMessage()
        var newSettings = AqControllerMessage.Settings()
        var newGeneralSetting = AqControllerMessage.Settings.GeneralSetting()
        newGeneralSetting.name = generalSetting.name
        newGeneralSetting.value = generalSetting.value
        newSettings.addGeneralSetting(newGeneralSetting)
        newMessage.addSettings(newSettings)
        guard let encoded = try? JSONEncoder().encode(newMessage) else {
            print("Failed to encode JSON")
            return
        }
        //let jsonString = NSString(data: encoded, encoding: String.Encoding.utf8.rawValue)
        print("In setGeneralSettingState(generalSetting)")
        //print(jsonString)
        let urlString: String = "http://\(aqConnectionString)/setSettingsState"
        let url = URL(string: urlString)!
        var request = URLRequest(url: url)
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpMethod = "POST"
        request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        do {
            let (data, _) = try await URLSession.shared.upload(for: request, from: encoded)
            // handle the result
        } catch {
            print("setGeneralSettingState(generalSetting) failed.")
        }
        
    }
    func setAlarmState (alarm: Alarm) async {
        guard let controllerState = self.controllerState else { return }
        var newMessage = AqControllerMessage()
        var newSettings = AqControllerMessage.Settings()
        var newAlarm = AqControllerMessage.Settings.Alarm()
        newAlarm.name = alarm.name
        newAlarm.alarmState = alarm.alarmState
        newSettings.addAlarm(newAlarm)
        newMessage.addSettings(newSettings)
        guard let encoded = try? JSONEncoder().encode(newMessage) else {
            print("Failed to encode JSON")
            return
        }
        let jsonString = NSString(data: encoded, encoding: String.Encoding.utf8.rawValue)
        //print("In setAlarmState(alarm)")
        //print(jsonString!)
        //print(encoded)
        let urlString: String = "http://\(aqConnectionString)/setSettingsState"
        let url = URL(string: urlString)!
        var request = URLRequest(url: url)
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpMethod = "POST"
        request.addValue("Bearer \(bearerToken)", forHTTPHeaderField: "Authorization")
        do {
            let (data, _) = try await URLSession.shared.upload(for: request, from: encoded)
            // handle the result
        } catch {
            print("setAlarmState(alarm) failed.")
        }
        
    }
    func getCurrentState() {
        guard let controllerState = self.controllerState else { return }
        guard let url = URL(string: "http://\(aqConnectionString)/getCurrentState") else { fatalError("Missing URL") }
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
                print("in getCurrentState()")
                
                DispatchQueue.main.async {
                    let jsonString = NSString(data: data, encoding: String.Encoding.utf8.rawValue)
                    print(jsonString!)
                    self.processStringMessage(WebSocketMessage: jsonString! as String)
                }
            }
        }
        dataTask.resume()
    }
    
    func deviceToggleChange(device: Device) async {
        print("deviceToggleChange() called for " + device.name)
        guard let controllerState = self.controllerState else { return }
        //guard (!device.stateUpdatedByController) else {device.stateUpdatedByController = false; return}
        var newMessage = AqControllerMessage()
        var newDevice = AqControllerMessage.Device()
        newDevice.name = device.name
        newDevice.state = device.state
        newMessage.addDevice(newDevice)
        guard let encoded = try? JSONEncoder().encode(newMessage) else {
            print("Failed to encode JSON")
            return
        }
        
        let urlString: String = "http://\(aqConnectionString)/setDeviceState"
        DispatchQueue.main.async {
            if (device.state == true) {
                if (device.name == "CO2") {
                    controllerState.getDeviceByName("Air Pump").setState(newState: false)
                }
                else if (device.name == "Air Pump") {
                    controllerState.getDeviceByName("CO2").setState(newState: false)
                }
                
            }
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
