//Public avaialable cordova functions (loaded on initialization)
var bt;
var push;

var app = {
    // Application Constructor
    initialize: function() {
        console.log("Initializing app");
        this.bindEvents();
    },
    bindEvents: function() {
        console.log("Binding events");

        document.addEventListener('deviceready', this.onDeviceReady, false);
    },
    onDeviceReady: function() {
        log("Cordova start", "Device ready");

        //Bluetooth
        bt = new Bluetooth(ble);
        console.log("Bluetooth plugin " + JSON.stringify(bt));

        //Notification status
        notification.isEnabled(function(granted) {
            notificationStatus(granted);
        });

        //Start nofication listener
        console.log("Notification listener " + notificationListener);
        setNotificationListenerCallback();

        cordova.plugins.backgroundMode.setDefaults({ title: 'LotWatch', text: '已连接' });
        cordova.plugins.backgroundMode.overrideBackButton();
        cordova.plugins.backgroundMode.enable();

    }
};

app.initialize();

var setNotificationListenerCallback = function() {
    notificationListener.listen(function(n) {
        console.log("Received notification " + JSON.stringify(n));

        var message = n.title + '\n';
        if (n.textLines) message += n.textLines;
        else message += n.text;

        bt.send("!nt:" + message);

    }, function(e) {
        setTimeout(setNotificationListenerCallback, 1000);
        console.log("Notification Error " + e);
    })
}