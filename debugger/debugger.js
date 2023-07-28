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

async function memoryChangePage(offset) {
    memoryPage += offset;
    if (memoryPage > 0xff)
        memoryPage = 0x0;
    else if (memoryPage < 0x00)
        memoryPage = 0xff;

    e("memory-page").innerHTML = `0x${hex(memoryPage)}`;

    memoryRequestUpdate();
}

async function memoryRequestUpdate() {
    e("memory-holder").style.visibility = "hidden";
    
    const array = await apiMemoryRead(memoryPage);

    const tbody = e("memory-body");
    tbody.innerHTML = '';
    
    for (let i = 0; i < 16; ++i) {
        const tr = document.createElement("tr");

        const addr = document.createElement("td");
        addr.classList.add("memory-address");
        addr.innerHTML = hex(memoryPage << 8 | (i << 4), 4);
        tr.appendChild(addr);

        for (let j = 0; j < 16; ++j) {
            const data = document.createElement("td");
            data.classList.add("memory-data");
            if (j == 7)
                data.classList.add("memory-data-7");
            data.innerHTML = hex(array[(i * 16) + j]);
            tr.appendChild(data);
        }

        const str = [];
        for (let j = 0; j < 16; ++j) {
            const c = array[(i * 16) + j];
            if (c < 32 || c >= 127)
                str.push('.');
            else
                str.push(String.fromCharCode(c));
        }
        const text = document.createElement("td");
        text.classList.add("memory-text");
        text.innerHTML = str.join("");
        tr.appendChild(text);

        tbody.appendChild(tr);
    }

    e("memory-holder").style.visibility = "visible";
}

//
// CURSOR
//

function setHourglass(v) {
    if (v) {
        document.body.classList.add("executing");
        document.querySelectorAll('button').forEach(t => t.classList.add("executing"));
    } else {
        document.body.classList.remove("executing");
        document.querySelectorAll('button').forEach(t => t.classList.remove("executing"));
    }
}

//
// API
//

const apiUrl = window.location.href.replace(/\/$/, "");

async function callApi(path, options) {
    try {
        setHourglass(true);
        const response = await fetch(apiUrl + path, options);
        if (!response.ok)
            throw new Error(await response.text());
        
        return await response.json();
    } catch (ex) {
        e("error").style.display = "block";
        e("error").innerHTML = ex.message;
        throw ex;
    } finally {
        setHourglass(false);
    }
}

async function apiMemoryRead(page) {
    return callApi(`/memory/${page}`);
}

//
// UTILS
//

const e = (id) => document.getElementById(id);
const hex = (num, digits) => num.toString(16).toUpperCase().padStart(digits || 2, '0');