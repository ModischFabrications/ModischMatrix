"use strict";

function init() {
}

function onValueChange(element, targetVar) {
    let newValue = element.value;
    console.log(`"${targetVar}" changed to ${newValue}`);

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

function sendUpdate() {
    if (brightnessEnabled.value) {
        // TODO append "?brightness=${newValue/100*255}"
    }

}

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
