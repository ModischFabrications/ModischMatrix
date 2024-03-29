"use strict";

let updateTimer;
let c;

function init() {
    c = new Canvas(64, 32);

    updateAutoload();
    updateURL();

    // this is technically better for synchronisation, but every call costs 150ms
    // updateTimer = setInterval(updateDeviceConfig, 5 * 1000, 0);
    updateDeviceConfig();
    updateBuildTime();
}

async function updateDeviceConfig() {
    let m = await getValue("/mode");
    if (m == null || m == "") {
        clearInterval(updateTimer);
        return;
    }
    openTab(document.getElementById(`mode_${m}`), false);
}

async function updateBuildTime() {
    let v = await getValue("/build_time");
    let field = document.getElementById(`BUILD_FIELD`);
    field.textContent += new Date(parseInt(v) * 1000).toISOString();
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
        autoloaders[i].oninput?.();
        autoloaders[i].onclick?.();
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

async function postValue(url, value = null) {
    console.log(`Sending value "${value}" to ${url}`);

    let content = "";
    if (value == null) {
        content = "";
    }
    else if (typeof value == "object") {
        content = Object.keys(value).map(key => encodeURIComponent(key) + '=' + encodeURIComponent(value[key])).join('&');
    }
    else {
        content = "value=" + value;
    };

    // https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch
    // can't use plaintext because ESP32AsyncWebserver can't parse it
    let rep = await fetch(url, { method: 'POST', body: content, headers: { 'Accept': "text/plain", "Content-Type": "application/x-www-form-urlencoded" } });
    if (rep.status == 200)
        return;

    let msg = rep.status + ": " + rep.statusText;
    showSnackbar(msg);
}

// -------------- freeDraw

class Canvas {
    constructor(width, height) {
        this.width = width;
        this.height = height;
        this.canvas = document.getElementById("freeDrawCanvas");
        this.ctx = this.canvas.getContext('2d');
        this.rect = this.canvas.getBoundingClientRect();
        this.active = false;
        this.color = "#aaaaaa";
        this.lastPos = null;

        this.canvas.addEventListener("pointerdown", this);
        this.canvas.addEventListener("pointerup", this);
        this.canvas.addEventListener("pointercancel", this);
        this.canvas.addEventListener("pointerleave", this); // TODO better check if mouse is still down on enter
        this.canvas.addEventListener("pointermove", this);
        this.canvas.addEventListener("click", this);
        this.canvas.addEventListener("dblclick", this);
    }

    // "this" is a moving target in JS, so we return to this class asap instead of handling individually
    handleEvent(e) {
        // update regularly to account for transformations
        this.ctx = this.canvas.getContext('2d');
        this.rect = this.canvas.getBoundingClientRect();
        switch (e.type) {
            case "pointerdown":
                this.active = true;
                break;
            case "pointerup":
            case "pointercancel":
            case "pointerleave":
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
        let state = [x, y, this.color];
        if (state.toString() === this.lastPos?.toString()) return;
        this.ctx.fillRect(x, y, 1, 1); // creates a 50 X 50 rectangle with upper-left corner at (10,10)
        postValue("/draw/pixel", state, true);
        this.lastPos = state;
    }

    setColor(color) {
        this.color = color;
        this.ctx.fillStyle = color;
    }

    fill() {
        this.ctx.fillRect(0, 0, this.width, this.height);
        postValue("/draw/fill", this.color);
    }

    clear() {
        this.ctx.clearRect(0, 0, this.width, this.height);
        postValue("/draw/clear");
    }
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
