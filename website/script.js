"use strict";

function init() {
    openTab(document.getElementById("mode_0"));
}

function openTab(element) {
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
    element.className  += " active";
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
