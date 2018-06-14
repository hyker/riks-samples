import RiksSwift
import CocoaMQTT

class RiksMQTTClient {
    
    private var rikskit: RiksSwift.RiksKit
    private var mqttClient: CocoaMQTT
    
    init(
        uid: String,
        password: String,
        host: String,
        port: UInt16,
        config: String) throws {

        self.mqttClient = CocoaMQTT(
            clientID: uid,
            host: host,
            port: port
        )
        
        func allowedForKey(
            _ uid: String,
            _ keySpace: String,
            _ keyID: String,
            _ callback: ((Bool) -> Void)) {
            
            // Allow all requests
            print(String(format: "%s requested access to topic %s. Granting access.", uid, keySpace))
            callback(true)
        }
        
        let whitelist = RiksSwift.Whitelist(
            allowedForKey: allowedForKey
        )
        
        rikskit = try RiksSwift.RiksKit(
            uid: uid,
            password: password,
            whitelist: whitelist,
            config: config
        )
    }
    
    public func subscribe(
        topic: String,
        callback: ((String) -> Void)) {
        
    }
    
    public func publish(
        topic: String,
        message: String) {
        
    }
    
}
