"use strict";

let updateTimer;
let c;

function init() {
    updateAutoload();
    updateURL();

    updateTimer = setInterval(updateDeviceConfig, 5 * 1000, 0);
    updateDeviceConfig();

    c = new Canvas(64, 32);
}

async function updateDeviceConfig() {
    let m = await getValue("/mode");
    if (m == null || m == "") {
        clearInterval(updateTimer);
        return;
    }
    openTab(document.getElementById(`mode_${m}`), false);
}

function updateURL() {
    let urlField = document.getElementById("URL_FIELD")
    urlField.value = document.URL;
    urlField.style.width = urlField.value.length + "ch";

    // TODO read from somewhere
    let mDnsUrl = "http://modischMatrix.local";
    let mdns = document.getElementById("MDNS_FIELD");
    mdns.value = mDnsUrl;
    mdns.style.width = urlField.style.width;

    mdns.parentNode.style.display = document.URL.toLowerCase().includes("local") ? "none" : "block";
}

function copyToClipboard(msg) {
    navigator.clipboard.writeText(msg);
    showSnackbar("Copied to clipboard: " + msg);
}

function updateAutoload() {
    let autoloaders = document.getElementsByClassName("autoload");
    for (let i = 0; i < autoloaders.length; i++) {
        autoloaders[i].oninput();
    }
}

function openTab(element, shallSendUpdate = true) {
    // Declare all variables
    var i, tabcontent, tablinks;

    // Get all elements with class="tabcontent" and hide them
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }

    // Get all elements with class="tablinks" and remove the class "active"
    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }

    // Show the current tab, and add an "active" class to the button that opened the tab
    document.getElementById(element.id + "_tab").style.display = "block";
    element.className += " active";

    if (shallSendUpdate) {
        postValue("/mode", element.id.split('_')[1]);
        clearInterval(updateTimer);
        updateTimer = setInterval(updateDeviceConfig, 5 * 1000, 0);
    }

}

function checkEnter(event, delegate) {
    if ((event.ctrlKey || event.metaKey) && (event.keyCode == 13 || event.keyCode == 10)) {
        delegate();
    }
}

function sendText() {
    let msg = document.getElementById("printInput").value.substr(0, 50);
    postValue("/print", msg);
}

function sendGOLRules() {
    let msg = document.getElementById("golRuleInput").value.substr(0, 20);
    // TODO check validity via regex
    postValue("/gol/rule", msg);
}

function arrowPressed(code) {
    let player = document.querySelector('input[name="playerIndex"]:checked').value;
    postValue(`/snake/${player}`, code);
}

function onTimeoutChanged(element) {
    let x = new Date(element.value * 1000).toISOString().substr(11, 8);
    timeoutOutput.value = x;
}

async function getValue(url) {
    let rep = await fetch(url);
    let msg = rep.status == 200 ? await rep.text() : null;
    console.log(`${url} -> "${msg ?? ""}"`);
    return msg;
}

async function postValue(url, value) {
    if (value == null || value == "") throw "missing argument";
    console.log(`Sending value "${value}" to ${url}`);

    // https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch
    // can't use plaintext because ESP32AsyncWebserver can't parse it
    let rep = await fetch(url, { method: 'POST', body: "value=" + value, headers: { 'Accept': "text/plain", "Content-Type": "application/x-www-form-urlencoded" } });

    let msg = rep.status + ": ";
    if (rep.status == 200) msg += await rep.text();
    else msg += rep.statusText;
    showSnackbar(msg);
}

// -------------- freeDraw

class Canvas {
    constructor(width, height) {
        this.width = width;
        this.height = height;
        this.canvas = document.getElementById("freeDrawCanvas");
        this.ctx = this.canvas.getContext('2d');
        this.active = false;

        this.canvas.addEventListener("pointerdown", this);
        this.canvas.addEventListener("pointerup", this);
        this.canvas.addEventListener("pointercancel", this);
        this.canvas.addEventListener("pointermove", this);
        this.canvas.addEventListener("click", this);
        this.canvas.addEventListener("dblclick", this);
    }

    // "this" is a moving target in JS, so we return to this class asap instead of handling individually
    handleEvent(e) {
        this.rect = this.canvas.getBoundingClientRect();
        switch (e.type) {
            case "pointerdown":
                this.active = true;
                break;
            case "pointerup":
                this.active = false;
                break;
            case "pointercancel":
                this.active = false;
                break;
            case "pointermove":
                if (!this.active) return;
            case "click":
            case "dblclick":
                this.onPainting(e);
                break;
        }
    }

    onPainting(e) {
        var x = e.clientX - this.rect.left;
        var y = e.clientY - this.rect.top;
        x = Math.floor(this.width * x / this.canvas.clientWidth);
        y = Math.floor(this.height * y / this.canvas.clientHeight);

        this.draw(x, y);
    }

    draw(x, y) {
        this.ctx.fillRect(x, y, 1, 1); // creates a 50 X 50 rectangle with upper-left corner at (10,10)
    }

    setColor(color) {
        this.ctx.fillStyle = color;
    }

    setmode(i) { } // method to set the active tool 
    save() { } // method to save pixel art as image clear() {} // method to clear canvas 
    addFrame() { } // method to add current frame to frame list 
    deleteFrame(f) { } // method to delete a specific frame loadFrame(f) {} // method to load a specific frame onto canvas 
    renderGIF() { } // method to render a GIF using frames 
    undo() { } // method to undo a given step 
    redo() { } // method to redo a given step 
    addImage() { } // method to load an image as pixel art 
}

// -------------- snackbar

// TODO prevent flashing after 5s
let snackbarTimeout = null;
function showSnackbar(text) {
    let el = document.getElementById('snackbar');
    el.innerHTML = text;
    el.className = 'show';

    clearTimeout(snackbarTimeout);
    setTimeout(function () {
        el.className = el.className.replace('show', '');
    }, 3000)
}
