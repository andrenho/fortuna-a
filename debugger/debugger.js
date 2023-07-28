//
// GLOBALS
// 

let tabSelected = "code";
let memoryPage = 0x20;

//
// EVENTS
//

document.addEventListener("keydown", (e) => {
    if (e.key == "1")
        tabSelect("code");
    else if (e.key == "2")
        tabSelect("memory");
    else if (e.key == "3")
        tabSelect("selftest");
});

//
// TABS
//

function tabSelect(tab) {
    const el = e(`tab-${tab}`);   
    if (el) {
        for (const elem of ["code", "memory", "selftest"]) {
            e(`tab-${elem}`).classList.remove("tab-selected");
            e(elem).style.display = "none";
        }

        el.classList.add("tab-selected");
        e(tab).style.display = "flex";
    }

    tabSelected = tab;

    if (tab == "memory")
        memoryRequestUpdate();
}

//
// MEMORY
//

function memoryChangePage(offset) {
    memoryPage += offset;
    if (memoryPage > 0xff)
        memoryPage = 0x0;
    else if (memoryPage < 0x00)
        memoryPage = 0xff;

    memoryRequestUpdate();
}

function memoryRequestUpdate()
{
}

//
// API
//

const apiUrl = window.location.href;

function apiMemoryRead(page)
{

}

//
// UTILS
//

function e(id) { return document.getElementById(id); }
