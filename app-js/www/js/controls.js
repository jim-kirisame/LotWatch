var TAG = "controls";

$("#custom-message-button").click(function() {
    var customMessage = $("#custom-message").val();
    log("Controls", "Sending custom message: " + customMessage);
    bt.send(customMessage);
});

$("#send-time").click(function() {
    sendTime();
});
$("#send-ping").click(function() {
    drawChat("send", "Ping");
    bt.sendRaw("\x55\x01");
});
$("#bt-connect").click(function() {

    bt.connect();
});
$("#bt-disconnect").click(function() {

    bt.disconnect();
});
$("#bt-scan").click(function() {

    bt.scan();
});

$("#clear-chat").click(function() {
    $("#chat").empty();
});

$("#send-notification").click(function() {
    var customMessage = $("#custom-message").val();
    log("Controls", "Sending notification: " + customMessage);
    bt.send("!nt:" + customMessage);
});

$("#send-notif-enable").click(function() {
    notification.enable();
});

$("#send-notif-trigger").click(function() {
    var customMessage = $("#custom-message").val();
    notification.notificate(customMessage);
});

$("#send-notif-cancel").click(function() {
    notification.cancelAll();
});

$("#reconnect:checkbox").change(function() {

    var check = $(this).is(':checked');
    //Either "checked" or undefined ?
    bt.setAutoreconnect(check);
});

$("#clear-battery").change(function() {
    battery.clear();
});

$("#ongoing-notification:checkbox").change(function() {

    var check = $(this).is(':checked');
    settings.notification = check;

    if (!check) notification.cancelStatus();
});