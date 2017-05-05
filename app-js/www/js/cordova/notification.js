//Plugin: https://github.com/katzer/cordova-plugin-local-notifications

//Index sample: https://github.com/katzer/cordova-plugin-local-notifications/blob/example/www/index.html

var notification = new Notification();

function Notification(){
  var notificationId = 0;

  var logo = 'res://icon';

  this.isEnabled = function(callback){
    cordova.plugins.notification.local.hasPermission(callback);
  }
  this.enable = function(){
    cordova.plugins.notification.local.registerPermission(function (granted) {
        showToast(granted ? 'Yes' : 'No');
        notificationStatus(granted); //Update status of notification debug buttons
    });
  }

  this.notificate = function(msg) {
    notificationId++;
    cordova.plugins.notification.local.schedule({
        id: notificationId,
        text: msg,
        icon: logo,
        smallIcon: logo,
        sound: null,
        data: { test: notificationId }
    });
    //TODO add click action (as a new argument)
  }

  this.watchduinoStatus = function(msg) {
    cordova.plugins.notification.local.schedule({
        id: 0,
        text: msg,
        icon: logo,
        smallIcon: logo,
        sound: null,
        ongoing: true,
        data: {  }
    });
  }

  this.cancelStatus = function(){
    cordova.plugins.notification.local.cancel(0, function(){});
  };
  this.cancel = function(id) {
    cordova.plugins.notification.local.cancel(id, function(){});
  };
  this.cancelLast = function() {
    cordova.plugins.notification.local.cancel(notificationId, function(){});
  };

  this.cancelAll = function(){
    cordova.plugins.notification.local.cancelAll(function(){});
  };
}
