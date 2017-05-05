var TAG = "Receive";

var onReceive = function(msg) {
    //drawChat("receive", msg);

    if (msg.contains("\x55")) {
        var command = msg.substring(1, 2);
        var value = msg.substring(2);
        processCommand(command, value);
    } else {
        log(TAG, "Unrecognized message: " + msg, "error");
    }
}

var processCommand = function(command, value) {
    log(TAG, "Command " + command + " value " + value);
    switch (command) {
        case "\x01":
            console.log("Received pong");
            drawChat("receive", "Pong");
            break;
        case "\x02":
            sendTime();
            drawChat("receive", "Time");
            break;
        case "nt":
            notification.cancelLast();
            break;
        case "\x00":
            log(TAG, "Battery notification " + value);
            battery.add(value.substring(0, 4));
            drawChat("receive", "Batt:" + value.substring(0, 4));
            break;
        case "sl":
            log(TAG, "Disconnecting on sleep");
            bt.disconnect();
        default:
            drawChat("receive", stringToHexString(command) + ":" + stringToHexString(value));
            log(TAG, "Unrecognized command: " + command, "error");
    }
}