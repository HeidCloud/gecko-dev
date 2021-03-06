/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

let gTestTab;
let gContentAPI;
let gContentWindow;

Components.utils.import("resource:///modules/UITour.jsm");
Components.utils.import("resource://gre/modules/Services.jsm");

function test() {
  UITourTest();
}

let tests = [
  function test_openSearchPanel(done) {
    let searchbar = document.getElementById("searchbar");

    // If suggestions are enabled, the panel will attempt to use the network to connect
    // to the suggestions provider, causing the test suite to fail.
    Services.prefs.setBoolPref("browser.search.suggest.enabled", false);
    registerCleanupFunction(() => {
      Services.prefs.clearUserPref("browser.search.suggest.enabled");
    });

    ok(!searchbar.textbox.open, "Popup starts as closed");
    gContentAPI.openSearchPanel(() => {
      ok(searchbar.textbox.open, "Popup was opened");
      searchbar.textbox.closePopup();
      ok(!searchbar.textbox.open, "Popup was closed");
      done();
    });
  },
];
