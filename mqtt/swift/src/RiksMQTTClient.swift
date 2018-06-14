import Riks

class RiksMQTTClient {

    private var rikskit: Riks.RiksKit

    init(
        uid: String,
        password: String,
        host: String,
        port: Int,
        config: String) {
        
        func allowedForKey(
            _ uid: String,
            _ keySpace: String,
            _ keyID: String,
            _ callback: ((Bool) -> Void)) {
            
            // Allow all requests
            print(String(format: "%s requested access to topic %s. Granting access.", uid, keySpace))
            callback(true)
        }

        func newKey(
            _ keySpace: String,
            _ keyID: String) {
        }

        self.rikskit = Riks.RiksKit(
            uid: uid,
            password: password,
            allowedForKey: allowedForKey,
            newKey: newKey,
            config: config
        )
    }

}
