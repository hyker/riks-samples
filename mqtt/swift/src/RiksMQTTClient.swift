import Riks

class RiksMQTTClient {

    private var rikskit: Riks.RiksKit

    init(
        uid: String,
        password: String,
        host: String,
        port: Int,
        config: String) throws {
        
        func allowedForKey(
            _ uid: String,
            _ keySpace: String,
            _ keyID: String,
            _ callback: ((Bool) -> Void)) {
            
            // Allow all requests
            print(String(format: "%s requested access to topic %s. Granting access.", uid, keySpace))
            callback(true)
        }

        let whitelist = Riks.Whitelist(
            allowedForKey: allowedForKey
        )

        self.rikskit = try Riks.RiksKit(
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
