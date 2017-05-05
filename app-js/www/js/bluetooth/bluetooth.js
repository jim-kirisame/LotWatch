function Bluetooth(bluetooth) {

    var TAG = "Bluetooth";
    var CONNECTED = "connected",
        DISCONNECTED = "disconnected";

    var MAX_MSG_SIZE = 10;
    var MAX_QUEUE_SIZE = 5;

    var device = {
        id: "B4:99:4C:51:C0:51", //assembled watchduino
        //id:"78:A5:04:3E:CD:B6" //Prototype board
    };

    var messageQueue = [];
    var reconnect = true;
    var queueSystem = true;
    var autoconnecting = false;

    // ----------------
    // PERIODIC FUNCTIONS (INTERVALS)
    // ----------------

    setInterval(function() {
        //log(TAG, "Processing message queue " + messageQueue.length);

        $("#message-queue").html(messageQueue.length);
        if (messageQueue.length > 0 && device.status === CONNECTED) {
            var firstMessage = messageQueue.shift();
            bt.sendNow(firstMessage);
        }
    }, 1000);

    setInterval(function() {

        if (!autoconnecting && reconnect && device.id) {
            bluetooth.isConnected(device.id, function() {}, function() {
                if (device.status === CONNECTED) changeStatus("Disconnected", "断开连接");
                device.status = DISCONNECTED;
                autoConnect(device.id);
            })
        }
    }, 1000);

    // ----------------
    // PUBLIC FUNCTIONS
    // ----------------
    this.setAutoreconnect = function(value) {
        log(TAG, "Reconnect set to  " + value);

        reconnect = value;
    };

    //Sample peripheral data https://github.com/don/cordova-plugin-ble-central#peripheral-data

    this.isEnabled = function() {
        bluetooth.isEnabled(onSuccess, onError);
    };
    this.enable = function() {
        bluetooth.enable(onSuccess, onError);
    };

    this.send = function(msg) {

        if (queueSystem) {
            log(TAG, "Adding message to bluetooth queue: " + msg.replace(/\n/g, "\\n"));

            if (messageQueue.length > MAX_QUEUE_SIZE) messageQueue.shift();

            //messageQueue.push("!db");
            messageQueue.push(msg);
            log("Messages in queue ", messageQueue);
        } else this.sendNow(msg);
    };

    this.sendNow = function(msg) {
        if (device.status === CONNECTED) {

            log(TAG, "Writing bluetooth: " + msg.replace(/\n/g, "\\n"));
            var endChar = "\r";
            var fullMsg = endChar + endChar + msg + endChar;

            drawChat("send", msg);

            if (fullMsg.length > MAX_MSG_SIZE) sendInParts(fullMsg);
            else {
                var bytes = stringToBytes(fullMsg);
                bluetooth.writeWithoutResponse(device.peripheral.id, device.service, device.characteristic, bytes, function() {}, onError);
            }
        } else {
            log(TAG, "Cannot send msg, Bluetooth device is not connected", "error");
        }
    };

    this.sendRaw = function(msg) {
        if (device.status === CONNECTED) {

            log(TAG, "Writing bluetooth: " + stringToHexString(msg).replace(/\n/g, "\\n"));

            if (msg.length > MAX_MSG_SIZE) log(TAG, "Cannot send msg, it's too large", "error");
            else {
                var bytes = stringToBytes(msg);
                bluetooth.writeWithoutResponse(device.peripheral.id, "6e400001-b5a3-f393-e0a9-e50e24dcca9e", "6e400002-b5a3-f393-e0a9-e50e24dcca9e", bytes, function() { log(TAG, "send success", "info"); }, onError);
            }
        } else {
            log(TAG, "Cannot send msg, Bluetooth device is not connected", "error");
        }
    }

    this.disconnect = function() {

        ble.disconnect(device.id, onSuccess, onError);
        device.status = DISCONNECTED;
        changeStatus("Manually disconnected", "未连接");
    }

    this.scan = function() {
        bluetooth.isEnabled(function() {
            log(TAG, "Scanning devices");
            bluetooth.stopScan(function() {

                bluetooth.scan([], 10, onScan, onError);
                //setStatus("Scanning devices... ");
                $("#list-devices").empty();
                $("#scan-loading").show();

                drawChat("info", "Scanning devices...");

                setTimeout(function() {
                    $("#scan-loading").hide();
                }, 10000);

            }, onError);

        }, function() {
            changeStatus("Bluetooth is not enabled", "disconnected");
        });
    };

    // ----------------
    // PRIVATE FUNCTIONS
    // ----------------

    // ----------------
    // PRIVATE : connection
    // ----------------

    var onScan = function(peripheral) {
        log(TAG, "Scan found: " + JSON.stringify(peripheral));
        printScanDevice(peripheral);
    };

    var onConnect = function(peripheral) {
        //app.status("Connected to " + peripheral.id);
        $("#scan-loading").hide();


        log(TAG, "onConnect peripheral: " + JSON.stringify(peripheral));
        device.status = CONNECTED;

        changeStatus("Connected to " + peripheral.name, "已连接");

        device.peripheral = peripheral; //Sample peripheral data https://github.com/don/cordova-plugin-ble-central#peripheral-data
        //printConnectedDevice(peripheral);
        log(TAG, "onConnect device: " + JSON.stringify(device));

        autoNotifyCharacteristic(peripheral);
        $('#modal1').closeModal();
    };

    var autoNotifyCharacteristic = function(peripheral) {
        for (var i = 0; i < peripheral.characteristics.length; i++) {
            var c = peripheral.characteristics[i];
            var p = c.properties;
            if (p.indexOf("Notify") >= 0 && c.service == "6e400001-b5a3-f393-e0a9-e50e24dcca9e") {
                log(TAG, "Listening to " + peripheral.name + ": " + c.service + ", " + c.characteristic);

                device.service = c.service;
                device.characteristic = c.characteristic;
                device.id = peripheral.id;

                //bluetooth.notify(peripheral.id, c.service, c.characteristic, onData, onError); //deprecated

                //bluetooth.stopNotification(peripheral.id, c.service, c.characteristic, onSuccess, onError);
                bluetooth.startNotification(peripheral.id, c.service, c.characteristic, onData, onError);
                return;
            }
        }
        log(TAG, "Device " + peripheral.id + " " + peripheral.name + " doesn't have property Notify", "error");
    };

    var onDisconnect = function(reason) {
        log(TAG, "onDisconnect reason: " + reason);
        device.status = DISCONNECTED;

        changeStatus("Disconnected: " + reason, "未连接");
    };

    var autoConnect = function(id) {

        log(TAG, "Auto connecting bluetooth " + id);

        autoconnecting = true;

        setTimeout(function() {
            autoconnecting = false;
            bluetooth.stopScan();
        }, 10000);

        //changeStatus("Bluetooth scanning", "disconnected");
        log(TAG, "Connecting, with message queue: " + messageQueue.length);
        bluetooth.connect(device.id, onConnect, function() {
            log(TAG, "Unable to connect to device, performing scan");

            bluetooth.scan([], 10, function(peripheral) {
                if (peripheral.id == id) {
                    //var isSleeping = peripheral.name.indexOf('Z') != -1;
                    changeStatus("Connecting to " + peripheral.name + ": " + peripheral.id, "连接中");
                    bluetooth.connect(device.id, onConnect, onDisconnect);
                    autoconnecting = false;
                }
            }, onError);
        });
    };

    // ----------------
    // PRIVATE : send messages
    // ----------------

    var sendInParts = function(msg) {
        var part = msg.substring(0, MAX_MSG_SIZE);
        var nextMessage = msg.substring(MAX_MSG_SIZE);

        log(TAG, "Sending message part " + part + ", " + nextMessage);

        var bytes = stringToBytes(part);
        bluetooth.writeWithoutResponse(device.peripheral.id, device.service, device.characteristic, bytes, onSuccess, onError);

        if (nextMessage.length > 0)
            setTimeout(function() {
                sendInParts(nextMessage);
            }, 100);
    };

    // ----------------
    // PRIVATE : draw
    // ----------------

    var printScanDevice = function(peripheral) {
        var template = $('#device-template').html();
        Mustache.parse(template);
        var rendered = Mustache.render(template, { device: peripheral, title: JSON.stringify(peripheral) });

        var $item = $(rendered);
        $item.click(function() {
            changeStatus("Connecting to " + peripheral.name + ": " + peripheral.id, "连接中");
            bluetooth.connect(peripheral.id, onConnect, onDisconnect);
        });
        $("#list-devices").append($item);
    };

    //@deprecated Use autoNotifyCharacteristic instead
    var printConnectedDevice = function(peripheral) {
        var template = $('#characteristic-template').html();
        Mustache.parse(template);
        var rendered = Mustache.render(template, { device: peripheral, title: JSON.stringify(peripheral) });

        log(TAG, "rendered: " + rendered);

        var $items = $(rendered);
        $items.find(".characteristic").click(function() {
            var id = $(this).attr('id');
            var service = id.split('-')[0];
            var characteristic = id.split('-')[1];

            var dev = $(this).parents("ul");
            var deviceName = dev.attr('device-name');
            var deviceId = dev.attr('id');

            changeStatus("Connected to " + deviceName + " (" + service + ", " + characteristic + ")", "已连接");

            device.id = deviceId;
            device.service = service;
            device.characteristic = characteristic;

            bluetooth.notify(deviceId, service, characteristic, onData, onError);

            $('#modal1').closeModal();
        });
        $("#list-devices").html($items);
    };

    var onData = function(buffer) {
        var data = bytesToString(buffer);
        log(TAG, "onData data: " + data);

        //See receive.js
        onReceive(data);
    };
    var onSuccess = function(response) {
        log(TAG, "Successfully executed " + response);
    };
    var onError = function(reason) {
        log(TAG, "There was an error " + reason, "error");
    };

}