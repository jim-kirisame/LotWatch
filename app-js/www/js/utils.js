if (!String.prototype.contains) {
    String.prototype.contains = function() {
        'use strict';
        return String.prototype.indexOf.apply(this, arguments) !== -1;
    };
}

var log = function(tag, txt, level) {
    if (!level) level = "info";

    var date = formatDate(new Date());
    var entryText = "[" + date + "] (" + tag + ") " + level.toUpperCase() + ": " + txt;
    var entry = "<p class='" + level + "'>" + entryText + "</p>";

    if (level == "error") console.error(entryText);
    else console.info(entryText);

    //$("#log").prepend(entry);

}

var isCordova = function() {
    console.log("URL ", document.URL);
    //var app = document.URL.indexOf( 'http://' ) === -1 && document.URL.indexOf( 'https://' ) === -1  && document.URL.indexOf( 'file://' ) === -1;
    return document.URL.indexOf('android_asset') !== -1;
}

var formatDate = function(d) {
    var date = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate(); // Returns the year
    var time = d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds();
    return date + " " + time;
}

// ASCII only
function stringToBytes(string) {
    var array = new Uint8Array(string.length);
    for (var i = 0, l = string.length; i < l; i++) {
        array[i] = string.charCodeAt(i);
    }
    return array.buffer;
}

// ASCII only
function bytesToString(buffer) {
    return String.fromCharCode.apply(null, new Uint8Array(buffer));
}

function intToBytes(num) {
    num = parseInt(num);
    var result = "";
    while (num != 0) {
        result = String.fromCharCode(num % 256) + result;
        num = parseInt(num / 256);
    }
    return result;
}

function stringToHexString(string) {
    var str = "";
    for (var i = 0, l = string.length; i < l; i++) {
        var str2 = (string.charCodeAt(i)).toString(16);
        if (str2.length == 1)
            str += "0" + str2;
        else
            str += str2;
    }
    return str;
}