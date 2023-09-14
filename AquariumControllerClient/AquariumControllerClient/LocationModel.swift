//
//  LocationModel.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 9/13/23.
//
/*
import Foundation
import SystemConfiguration.CaptiveNetwork
import CoreLocation
class LocationModel: NSObject, ObservableObject {
    private let locationManager = CLLocationManager()
    @Published var authorisationStatus: CLAuthorizationStatus = .notDetermined
    var currentNetworkInfos: Array<NetworkInfo>? {
        get {
            return SSID.fetchNetworkInfo()
        }
    }
    override init() {
        super.init()
        self.locationManager.delegate = self
        self.requestAuthorisation(always: true)
        
    }

    public func requestAuthorisation(always: Bool = false) {
        if always {
            self.locationManager.requestAlwaysAuthorization()
        } else {
            self.locationManager.requestWhenInUseAuthorization()
        }
    }
}

extension LocationModel: CLLocationManagerDelegate {

    func locationManager(_ manager: CLLocationManager, didChangeAuthorization status: CLAuthorizationStatus) {
        self.authorisationStatus = status
        print("in LocationModel locationManager()")
        if status != .notDetermined {
            if let currentNetworkInfos = currentNetworkInfos {
                print("SSID: \(currentNetworkInfos.first?.ssid ?? "")")
            }
            
        }
    }
}
public class SSID {
class func fetchNetworkInfo() -> [NetworkInfo]? {
    if let interfaces: NSArray = CNCopySupportedInterfaces() {
        var networkInfos = [NetworkInfo]()
        for interface in interfaces {
            let interfaceName = interface as! String
            var networkInfo = NetworkInfo(interface: interfaceName,
                                          success: false,
                                          ssid: nil,
                                          bssid: nil)
            if let dict = CNCopyCurrentNetworkInfo(interfaceName as CFString) as NSDictionary? {
                networkInfo.success = true
                networkInfo.ssid = dict[kCNNetworkInfoKeySSID as String] as? String
                //print("SSID")
                //print(networkInfo.ssid)
                networkInfo.bssid = dict[kCNNetworkInfoKeyBSSID as String] as? String
            }
            networkInfos.append(networkInfo)
        }
        return networkInfos
    }
    return nil
  }
}

struct NetworkInfo {
var interface: String
var success: Bool = false
var ssid: String?
var bssid: String?
}
*/
