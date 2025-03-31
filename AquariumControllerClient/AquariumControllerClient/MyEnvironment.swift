#if PRODUCTION
//Environment: Prod
 struct env {
    static let bearerToken: String = "ayAkxuh5A1JFLmDkpO8iEFiq1RYhQztX"
    static let publicDNS: String = "aquariumcontroller.tplinkdns.com:8008"
    static let privateIP: String = "192.168.0.2:8008"
}
#elseif DEVELOPMENT
//Environment: Dev
struct env {
    static let bearerToken: String = "ayAkxuh5A1JFLmDkpO8iEFiq1RYhQztX"
    static let publicDNS: String = "192.168.0.109:8008"
    static let privateIP: String = "192.168.0.109:8008"
}
#else
struct env {
    static let bearerToken: String = "ayAkxuh5A1JFLmDkpO8iEFiq1RYhQztX"
    static let publicDNS: String = "MyDNS.com:8008"
    static let privateIP: String = "192.168.0.10:8008"
}
#endif
