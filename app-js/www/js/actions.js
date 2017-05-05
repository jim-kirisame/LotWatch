var sendTime = function() {
    var date = new Date();
    var seconds = date.getTime() / 1000;
    //bt.send("!tm:" + parseInt(secondsUTC));
    bt.sendRaw("\x55\x02" + intToBytes(seconds) + "\x00");
    drawChat("send", "Time:" + seconds);
};

var changeStatus = function(message, status) {

    if (settings.notification) notification.watchduinoStatus(status);
    cordova.plugins.backgroundMode.configure({ text: status });
    drawChat("info", status);
    log("Chat", message + " " + status)

    $("#status-bar").removeClass().addClass("row " + status);
    $("#bt-status").html(message);
};

var notificationStatus = function(enabled) {

    $("#send-notif-enable").prop('disabled', enabled);
    $("#send-notif-trigger").prop('disabled', !enabled);
    $("#send-notif-cancel").prop('disabled', !enabled);
};