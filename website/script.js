"use strict";

function init() {
    getValue("/mode").then(m => { openTab(document.getElementById(`mode_${m ?? 0}`), false); });
    updateAutoload();
    updateURL();
}

function updateURL() {
    // TODO share button for URL
    document.getElementById("URL").innerHTML = document.URL;
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
    }

}

function onValueChange(element, targetVar) {
    let newValue = element.value;
    //console.log(`"${targetVar}" changed to ${newValue}`);

    switch (targetVar) {
        case 'mode':
            break;
        case 'brightness':
            brightnessOutput.value = newValue + "%";
            break;
        case 'timeout':
            let x = new Date(newValue * 1000).toISOString().substr(11, 8);
            timeoutOutput.value = x;
        default:
            break;
    }

}

async function getValue(url) {
    let rep = await fetch(url);
    let msg = rep.status == 200 ? rep.text() : null;
    console.log(`${url} -> "${msg ?? ""}"`);
    return msg;
}

async function postValue(url, value) {
    console.log(`Sending value "${value}" to ${url}`);

    if (brightnessEnabled.value) {
        // TODO append "?brightness=${newValue/100*255}"
    }
    if (timeoutEnabled.value) {
        // TODO append "?brightness=${newValue/100*255}"
    }

    // https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch
    // can't use plaintext because ESP32AsyncWebserver can't parse it
    let rep = await fetch(url, { method: 'POST', body: "value=" + value, headers: { 'Accept': "text/plain", "Content-Type": "application/x-www-form-urlencoded" } });

    let msg = rep.status + ": ";
    if (rep.status == 200) msg += rep.text();
    else msg += rep.statusText;
    showSnackbar(msg);
}

// TODO prevent flashing after 5s
let snackbarTimeout = null;
function showSnackbar(text) {
    let el = document.getElementById('snackbar');
    el.innerHTML = text;
    el.className = 'show';

    clearTimeout(snackbarTimeout);
    setTimeout(function () {
        el.className = el.className.replace('show', '');
    }, 5000)
}
