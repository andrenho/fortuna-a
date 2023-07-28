/*
 * TABS
 */

let selectedTab = "code";

function selectTab(tab) {
    const el = e(`tab-${tab}`);   
    if (el) {
        for (const elem of ["code", "memory", "selftest"]) {
            e(`tab-${elem}`).classList.remove("tab-selected");
            e(elem).style.display = "none";
        }

        el.classList.add("tab-selected");
        e(tab).style.display = "flex";
    }
}

/*
 * UTILS
 */

function e(id) { return document.getElementById(id); }
