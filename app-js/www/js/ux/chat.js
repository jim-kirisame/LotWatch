var fillWithZero = function(i) {
    if (i < 10) return "0" + i;
    else return i;
}
var drawChat = function(type, text) {
    var MAX_LOG_SIZE = 20;

    var date = new Date();
    var time = date.getHours() + ":" + fillWithZero(date.getMinutes()) + ":" + fillWithZero(date.getSeconds());
    var template = $('#chat-template').html();
    Mustache.parse(template);
    var rendered = Mustache.render(template, { time: time, text: text, type: type });

    var $chat = $("#chat");
    var top = $chat.scrollTop;
    var maxHeight = $chat.scrollHeight;
    var height = $chat.outerHeight();

    var isAtBottom = top + height >= maxHeight;

    $chat.append(rendered);
    var $chatLog = $chat.find('div');

    if ($chatLog.length > MAX_LOG_SIZE) {
        var $sliceChat = $chatLog.slice($chatLog.length - MAX_LOG_SIZE);
        $chat.html($sliceChat);
    }

    //Scroll to bottom of chat
    if (isAtBottom) $chat.scrollTop(maxHeight);
}

var isDivAtBottom = function(id) {

    var $div = $(id);
    var top = $div[0].scrollTop;
    var maxHeight = $div[0].scrollHeight;
    var height = $div.outerHeight();

    return top + height >= maxHeight;
}
var scrollToBottom = function(id) {
    var $div = $(id);
    var maxHeight = $div[0].scrollHeight;
    $div.scrollTop(maxHeight);
}

var lastMessageType = "";
if (!isCordova()) {

    setInterval(function() {
        if (lastMessageType === "") lastMessageType = "info";
        else if (lastMessageType === "receive") lastMessageType = "send";
        else lastMessageType = "receive";

        drawChat(lastMessageType, "Sample text");
    }, 2000);

    //drawChat("info", "Sample text");

}