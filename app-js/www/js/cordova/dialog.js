var showToast = function (text) {
    setTimeout(function () {
        if (device.platform != 'windows') {
            window.plugins.toast.showShortBottom(text);
        } else {
            showDialog(text);
        }
    }, 100);
};
var showDialog = function (text) {
    if (dialog) {
        dialog.content = text;
        return;
    }
    dialog = new Windows.UI.Popups.MessageDialog(text);
    dialog.showAsync().done(function () {
        dialog = null;
    });
};
