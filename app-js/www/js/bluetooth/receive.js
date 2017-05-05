var TAG = "Receive";

var onReceive = function(msg){
  drawChat("receive", msg);

  if (msg.contains("!")){
    var command = msg.trim().substring(1, 3);
    var value = msg.trim().substring(4);
    processCommand(command, value);
  } else {
    log(TAG, "Unrecognized message: "+msg, "error");
  }
}

var processCommand = function (command, value) {
  log(TAG, "Command " +command + " value " + value);
  switch(command) {
    case "pg":
        console.log("Received pong");
        break;
    case "tm":
        sendTime();
        break;
    case "wt":
        sendForecast();
        break;
    case "tf":
          sendTflStops(value);
          break;
    case "tw":
          sendTwitterTimeline(value);
          break;
    case "nt":
          notification.cancelLast();
          break;
    case "bt":
          log(TAG, "Battery notification "+value);
          battery.add(value);
          break;
    case "sl":
          log(TAG, "Disconnecting on sleep");
          bt.disconnect();
    default:
      log(TAG, "Unrecognized command: "+command, "error");
    }
}
