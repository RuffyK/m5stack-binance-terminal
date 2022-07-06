/* Main */

function apiGet(url, callback, failCallback, timeout) {
    var xhttp = new XMLHttpRequest();
    if (timeout) {
        xhttp.timeout = timeout;
    }
    else {
        xhttp.timeout = 5000;
    }
    xhttp.addEventListener("load", callback);
    xhttp.addEventListener("error", failCallback);
    xhttp.addEventListener("error", failed)
    xhttp.open("GET", url, true);
    xhttp.setRequestHeader("Content-type", "application/json");
    xhttp.send();

    function failed() {
        console.log("The following API call failed: GET - " + url);
    }
}

function apiPost(url, data, callback, failCallback, timeout) {
    var json = JSON.stringify(data);
    var xhttp = new XMLHttpRequest();
    if (timeout) {
        xhttp.timeout = timeout;
    }
    else {
        xhttp.timeout = 5000;
    }
    xhttp.addEventListener("load", callback);
    xhttp.addEventListener("error", failCallback);
    xhttp.addEventListener("error", failed);
    xhttp.open("POST", url, true);
    xhttp.setRequestHeader("Content-type", "application/json; charset=utf-8");
    xhttp.send(json);

    function failed() {
        console.log("The following API call failed: POST - " + url);
    }
}

var snackbar = {
    status: {
        alert: "w3-red",
        warning: "w3-yellow",
        neutral: "w3-blue",
        good: "w3-green"
    },
    show: function (message, status) {
        var snakbarElement = document.getElementById("snackbar");
        if (snakbarElement) {
            if (message) {
                snakbarElement.innerText = message;
            }
            if (status) {
                snakbarElement.className = "show " + status;
            }
                setTimeout(snackbar.hide, 4000);
        }
    },
    hide: function () {
        var snakbarElement = document.getElementById("snackbar");
        if (snakbarElement) {
            snakbarElement.className = "";
        }
    }
}

/* WiFi API */


var networkList = {
    items: [],
    scan: function () {
        if (!connecting) {
            loadingIndicator.show();
            //networkList.indicateScanning();
            apiGet("/api/wifi/scan", function () {
                networkList.update(JSON.parse(this.responseText));
                loadingIndicator.hide();
            }, function () {
                // networkList.update(JSON.parse("[]"));
                loadingIndicator.hide();
            });
        }
    },
    clear: function () {
        networkList.items = [];
        var list = document.getElementById("networkList");
        while (list.hasChildNodes()) {
            list.removeChild(list.firstChild);
        }
        return list;
    },
    indicateScanning: function () {
        document.getElementById("networkButton").setAttribute("style", "display: none");
        var list = this.clear();
        var listItem = document.createElement("li");
        var label = document.createElement("span");
        label.innerText = "Scanning...";
        label.setAttribute("class", "w3-text-grey w3-medium");
        listItem.appendChild(label);
        list.appendChild(listItem);
        return list;
    },
    update: function (items) {
        var list = this.clear();
        networkList.items = items;
        items.forEach(function (item, index) {
            if (item.ssid !== wifiStatus.ssid || wifiStatus.status !== "connected") {
                var listItem = document.createElement("li");
                listItem.setAttribute("class", "w3-button w3-left-align");
                listItem.setAttribute("onclick", "connectWifi(" + index + ");");
                listItem.setAttribute("style", "width: 100%;")
                var nameLabel = document.createElement("span");
                nameLabel.innerText = item.ssid;

                appendSignalIcon(item.rssi, listItem);

                if (item.encryption != "none") {
                    var lock = document.createElement("img");
                    lock.setAttribute("src", "data:image/png;base64,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");
                    lock.setAttribute("height", "12");
                    lock.setAttribute("width", "12");
                    lock.setAttribute("class", "w3-right");
                    listItem.appendChild(lock);
                }

                listItem.appendChild(nameLabel);
                list.appendChild(listItem);
            }
        });
        //document.getElementById("networkButton").removeAttribute("style");
        return list;
    }
};

function appendSignalIcon(rssi, parent) {
    var signal = document.createElement("div");
    changeSignalIcon(rssi, signal);
    var bar1 = document.createElement("div");
    bar1.setAttribute("class", "first-bar bar");
    var bar2 = document.createElement("div");
    bar2.setAttribute("class", "second-bar bar");
    var bar3 = document.createElement("div");
    bar3.setAttribute("class", "third-bar bar");
    var bar4 = document.createElement("div");
    bar4.setAttribute("class", "fourth-bar bar");
    var bar5 = document.createElement("div");
    bar5.setAttribute("class", "fifth-bar bar");
    signal.appendChild(bar1);
    signal.appendChild(bar2);
    signal.appendChild(bar3);
    signal.appendChild(bar4);
    signal.appendChild(bar5);
    parent.appendChild(signal);
}

function changeSignalIcon(rssi, signal) {
    if (rssi < -90) {
        signal.setAttribute("class", "w3-right signal-bars mt1 sizing-box bad one-bar");
    }
    else if (rssi < -80) {
        signal.setAttribute("class", "w3-right signal-bars mt1 sizing-box bad two-bars");
    }
    else if (rssi < -70) {
        signal.setAttribute("class", "w3-right signal-bars mt1 sizing-box ok three-bars");
    }
    else if (rssi < -60) {
        signal.setAttribute("class", "w3-right signal-bars mt1 sizing-box good four-bars");
    }
    else {
        signal.setAttribute("class", "w3-right signal-bars mt1 sizing-box good five-bars");
    }
}

var wifiStatus = {
    ip: "",
    status: "",
    ssid: "",
    rssi: 0,
    update: function () {
        if (!connecting) {
            apiGet("/api/wifi/status", function () {
                var response = JSON.parse(this.responseText);
                var statusPanel = document.getElementById("wifiStatus")
                if (statusPanel) {
                    if (response.status !== "connected") {
                        if (statusPanel.className.indexOf("w3-hide") < 0) {
                            statusPanel.className = statusPanel.className + " w3-hide";
                        }
                    }
                    else {
                        statusPanel.className = statusPanel.className.replace(" w3-hide", "");
                    }
                }

                var statusLabel = document.getElementById("wifiStatusLabel");
                if (statusLabel) {
                    statusLabel.innerText = response.status;
                    wifiStatus.status = response.status;
                }

                var ipLabel = document.getElementById("wifiIpLabel");
                if (ipLabel) {
                    ipLabel.innerText = response.ip;
                    wifiStatus.ip = response.ip;
                }
                var ssidLabel = document.getElementById("wifiSsidLabel");
                if (ssidLabel) {
                    ssidLabel.innerText = response.ssid;
                    wifiStatus.ssid = response.ssid;
                }
                var signal = document.getElementById("wifiRssiIndicator");
                if (signal) {
                    changeSignalIcon(response.rssi, signal);
                    wifiStatus.rssi = response.rssi;
                }
            });
        }
    }
}

function connectWifi(networkIndex) {
    var network = networkList.items[networkIndex];
    if (network.encryption !== "none") {
        var modal = document.getElementById("connectModal");
        var ssidLabel = document.getElementById("modalSsid");
        modal.style.display = 'block';
        ssidLabel.innerText = network.ssid;
    }
    else {
        connect(network.ssid, "");
    }
}

function connectModal() {
    var ssid = document.getElementById('modalSsid');
    var pw = document.getElementById('modalPw');
    connect(ssid.innerText, pw.value);
}

var connecting = false;
function connect(_ssid, _pw) {
    if (!connecting) {
        var modal = document.getElementById("connectModal");
        modal.style.display = 'none';

        var body = {
            ssid: _ssid,
            password: _pw
        }
        connecting = true;
        snackbar.show("Connecting...", snackbar.status.neutral);
        apiPost("/api/wifi/connect", body, function () {
            var response = JSON.parse(this.responseText);
            connecting = false;
            wifiStatus.update();
            networkList.scan();
            notifyConnectionStatus(response.success)
        }, function () {
            connecting = false;
            notifyConnectionStatus(false)
        }, 25000);
    }
}

function notifyConnectionStatus(success) {
    console.log("Success:");
    console.log(success);
    if (success) {
        document.getElementById("modalPw").Value = "";
        snackbar.show("Connected!", snackbar.status.good);
    }
    else {
        snackbar.show("Connection failed!", snackbar.status.alert);
    }
}

var loadingIndicator = {
    hide: function () {
        loader = document.getElementById("loader");
        if (loader) {
            if (loader.className.indexOf("w3-hide") < 0) {
                loader.className = loader.className + " w3-hide";
            }
        }
    },
    show: function () {
        loader = document.getElementById("loader");
        if (loader) {
            loader.className = loader.className.replace(" w3-hide", "");
        }
    }
};


/* FS API */
function ExploreBrowser() {
    var self = this;

    var data;
    
    this.treeControl = {
        domElement: document.getElementById("explore-tree")
    };

    this.viewControl = {
        domElement: document.getElementById("explore-view"),

        clear: function () {
            while (this.domElement.hasChildNodes()) {
                this.domElement.removeChild(this.domElement.firstChild);
            }
        },

        show: function () {
            var filecontrol = document.getElementById("file-control");
            var explorebutton = document.getElementById("explore-button");

            if (filecontrol.className.indexOf("w3-hide-small") < 0) {
                filecontrol.className += " w3-hide-small";
            }

            this.domElement.className = this.domElement.className.replace(" w3-hide-small w3-hide-medium", "");
            explorebutton.className += " w3-text-blue w3-light-gray w3-border-light-gray";
            this.isHidden = false;
        },

        hide: function () {
            var filecontrol = document.getElementById("file-control");
            var explorebutton = document.getElementById("explore-button");

            if (this.domElement.className.indexOf("w3-hide-small") < 0) {
                this.domElement.className += " w3-hide-small";
            }
            if (this.domElement.className.indexOf("w3-hide-medium") < 0) {
                this.domElement.className += " w3-hide-medium";
            }

            explorebutton.className = explorebutton.className.replace(" w3-text-blue w3-light-gray w3-border-light-gray", "");
            filecontrol.className = filecontrol.className.replace(" w3-hide-small", "");
            this.isHidden = true;
        },

        isHidden: false,

        toggle: function () {
            if (this.isHidden) {
                this.show();
            }
            else {
                this.hide();
            }
        },

        update: function () {
            this.clear();
            var ctrl = this;
            var folders = {};
            var files = self.data.files;
            var filesToAdd = [];
            files.sort(sortByProperty('path'));
            files.forEach(function (item) {

                var file = item.path;
                var folderpath ="";
                var index = -1;
                var parent = ctrl.domElement;

                while ((index = file.indexOf("/")) >= 0) {

                    folderpath += file.substring(0, index + 1);
                    var foldername = file.substring(0, index);
                    file = file.substring(index + 1);
                    if (folderpath !== "/") {
                        if (!folders[folderpath]) {
                            var folderItem = document.createElement("div");
                            parent.appendChild(folderItem);

                            var folderButton = document.createElement("div");
                            folderButton.className = "w3-button w3-block w3-left-align";
                            folderButton.setAttribute("onclick", "toggleFolder('fs-" + folderpath + "');");
                            folderButton.innerText = foldername;

                            var folderIcon = document.createElement("i");
                            folderIcon.className = "material-icons w3-left w3-text-blue";
                            folderIcon.innerText = "folder";
                            folderIcon.style.height = "100%";

                            folderButton.appendChild(folderIcon);

                            folderItem.appendChild(folderButton);

                            var foldercontent = document.createElement("div");
                            foldercontent.id = "fs-" + folderpath;
                            foldercontent.className = "w3-hide";
                            foldercontent.style.paddingLeft = "12px";

                            folderItem.appendChild(foldercontent);

                            folders[folderpath] = foldercontent;
                        }
                        parent = folders[folderpath];
                    }
                }

                var fileIcon = document.createElement("i");
                fileIcon.className = "material-icons w3-left w3-text-blue";
                fileIcon.innerText = "insert_drive_file";
                fileIcon.style.height = "100%";
                
                var listItem = document.createElement("div")
                listItem.className = "w3-button w3-block w3-left-align";
                listItem.setAttribute("onclick", "viewFile('" + item.path + "');");
                listItem.innerText = file;
                listItem.appendChild(fileIcon);
                //parent.appendChild(listItem);
                filesToAdd.push({
                    parentElement: parent,
                    childElement: listItem
                });
            });

            filesToAdd.forEach(function (item) {
                item.parentElement.appendChild(item.childElement);
            });
        },

        folders: []
    };

    

    this.getFiles = function (dir, onSuccess) {
        var query = dir ? ("&dir=" + dir) : "";
        apiGet("/api/fs" + query, function () {
            self.data = JSON.parse(this.responseText);
            onSuccess();
        });
    };


}

function toggleFolder(id) {
    var folder = document.getElementById(id);
    var icon = folder.parentElement.getElementsByTagName("i")[0];
    if (folder.className.indexOf("w3-show") == -1) {
        folder.className += " w3-show";
        icon.innerText = "folder_open";
    } else {
        folder.className = folder.className.replace(" w3-show", "");
        icon.innerText = "folder";
    }
}


function viewFile(path) {
    var ctrl = document.getElementById("file-control");
    apiGet("/api/fs/file?path=" + path, function () {
        ctrl.value = this.responseText;
        var lines = this.responseText.split(/\r\n|\r|\n/).length;
        ctrl.setAttribute("rows", lines);
        var name = path.split("/")[path.split("/").length - 1];
        document.getElementById("explore-header").innerText = name;
        explorer.viewControl.hide();
    });
}

var sortByProperty = function (property) {

    return function (x, y) {

        return ((x[property] === y[property]) ? 0 : ((x[property] > y[property]) ? 1 : -1));

    };

};

var explorer;
function initExplorer() {
    explorer = new ExploreBrowser();
    explorer.getFiles(null, function () {
        explorer.viewControl.update();
    });
}

