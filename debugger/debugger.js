//
// GLOBALS
// 

let tabSelected = "code";
let memoryPage = 0x20;
let singleCycleCounter = 0;

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
// CODE (advanced)
//

function advancedChecked(event) {
    const elem = e("advancedText");
    elem.style.display = event.checked ? "flex" : "none";
}

async function advancedStep() {
    const ss = await apiStep();
    const row = `<tr>
        <td>${singleCycleCounter++}</td>
        <td>${!ss.mreq ? hex(ss.addr, 4) : ""}</td>
        <td>${!ss.mreq ? hex(ss.data, 2) : ""}</td>
        <td>${!ss.m1 ? "0" : ""}</td>
        <td>${!ss.iorq ? "0" : ""}</td>
        <td>${!ss.busak ? "0" : ""}</td>
        <td>${!ss.wait ? "0" : ""}</td>
        <td>${!ss.int ? "0" : ""}</td>
        <td>${!ss.wr ? "0" : ""}</td>
        <td>${!ss.rd ? "0" : ""}</td>
        <td>${!ss.mreq ? "0" : ""}</td>
    </tr>`;
    e("advanced-body").innerHTML = row + e("advanced-body").innerHTML;
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

async function memoryRequestSet(address) {
    const data = prompt(`New data for address 0x${hex(address, 4)} (in hex):`);
    if (data.trim() == "")
        return;
    const value = Number(`0x${data}`);
    if (isNaN(value)) {
        alert("Invalid value.");
        return;
    }
    
    await apiMemoryWrite(address, value);
    await memoryRequestUpdate();
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
            data.addEventListener("dblclick", () => memoryRequestSet((memoryPage << 8) + (i * 16) + j));
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
// SELF-TEST
//

async function runSelfTest() {
    const text = (await apiSelfTest()).map(r => `${r.test}: ${r.result ? "<span style='color: green;'>OK</span>" : "<span style='color: red;'>ERROR</span>"}`);
    e("selftest-results").innerHTML = text.map(t => `<div>${t}</div>`).join('');
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

async function apiMemoryWrite(page, value) {
    return callApi(`/memory/${page}`, {
        method: "POST",
        body: JSON.stringify({ value: value })
    });
}

async function apiSelfTest() {
    return callApi(`/post`, { method: "POST" });
}

async function apiStep() {
    return callApi(`/step`, { method: "POST" });
}

//
// UTILS
//

const e = (id) => document.getElementById(id);
const hex = (num, digits) => num.toString(16).toUpperCase().padStart(digits || 2, '0');