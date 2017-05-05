var sendTime = function() {
    var date = new Date();
    var seconds = date.getTime() / 1000;
    var secondsUTC = seconds - (date.getTimezoneOffset() * 60);
    bt.send("!tm:" + parseInt(secondsUTC));
};

var changeStatus = function(message, status) {

    if (settings.notification) notification.watchduinoStatus(status);
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