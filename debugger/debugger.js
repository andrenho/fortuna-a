//
// GLOBALS
// 

let tabSelected = "code";
let memoryPage = 0x0;
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

window.addEventListener("load", async (e) => {
    await recompileAndReset();
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
// CODE
//

async function uploadRom(rom) {
    const codeDebug = e("code-debug");
    codeDebug.innerHTML = "Uploading ROM ";
    for (let i = 0; i < rom.length; i += 64) {
        codeDebug.innerHTML += ".";
        try {
            await apiMemoryWrite(i, rom.slice(i, i + 64));
        } catch (ex) {
            codeDebug.innerHTML += "X";
        }
    }
    codeDebug.innerHTML += " Done!";
}

function parseLine(line) {
    line = `<span class="code-line-header">${line.slice(0,32)}</span>${line.slice(32)}`;
    line = line.replace(/Source: "(.+?)"/, `Source: "<span class="code-line-filename">$1</span>"`);
    line = line.replace(/;(.+?)$/, `<span class="code-line-comment">;$1</span>`);
    return line;
}

function updateCodeLocation(pc) {
    for (let el of document.getElementsByClassName("code-line-pc"))
        el.classList.remove("code-line-pc");
    const el = e(`code-line-${pc}`);
    if (el) {
        el.classList.add("code-line-pc");
        el.scrollIntoView({ behavior: "smooth", block: "nearest" });
        /*
        const topPos = el.offsetTop;
        console.log(topPos);
        e("code-debug").scrollTo({
            top: 0,
        });
        */
    }
}

function updateCode(src) {
    const codeDebug = e("code-debug");
    codeDebug.innerHTML = "";
    
    for (const line of src.split("\n")) {
        const addr = parseInt(line.slice(3, 7), 16);
        const codeLine = document.createElement("div");
        codeLine.classList.add("code-line");
        if (!isNaN(addr))
            codeLine.id = `code-line-${addr}`;
        if (addr === 0)
            codeLine.classList.add("code-line-pc");
        codeLine.innerHTML = parseLine(line);
        codeDebug.appendChild(codeLine);
    }
}

async function updateRegisters(r) {
    e("reg-pc").innerHTML = hex(r.pc, 4);
}

async function step() {
    const r = await apiStep();
    updateCodeLocation(r.pc);
    updateRegisters(r);
}

async function reset() {
    await apiReset();
    updateCodeLocation(0);
}

async function recompileAndReset() {
    let r;
    try {
        r = await apiRecompile();
    } catch (ex) {
        r = JSON.parse(ex.message);
        e("error").innerHTML = `<pre>${r.stderr}</pre>`;
        return;
    } finally {
        if (r.stdout) 
            console.log(r.stdout);
        if (r.stderr)
            console.warn(r.stderr);
    }
    
    await apiReset();
    await uploadRom(r.rom);
    updateCode(r.src);
}

//
// CODE (advanced)
//

function advancedChecked(event) {
    const elem = e("advancedText");
    elem.style.display = event.checked ? "flex" : "none";
}

async function advancedStepCycle() {
    const ss = await apiStepCycle();
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

async function updatePage() {
    const data = prompt(`New page (in hex):`);
    if (data.trim() == "")
        return;
    const value = Number(`0x${data}`);
    if (isNaN(value)) {
        alert("Invalid value.");
        return;
    }
    
    await memorySetPage(value);
}

async function memoryChangePage(offset) {
    memorySetPage(memoryPage + offset);
}

async function memorySetPage(page) {
    memoryPage = page;
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
    
    await apiMemoryWrite(address, [value]);
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

async function apiMemoryWrite(page, data) {
    return callApi(`/memory/${page}`, {
        method: "POST",
        body: JSON.stringify({ data })
    });
}

async function apiSelfTest() {
    return callApi(`/post`, { method: "POST" });
}

async function apiStepCycle() {
    return callApi(`/step-cycle`, { method: "POST" });
}

async function apiRecompile() {
    return callApi(`/code`);
}

async function apiReset() {
    return callApi(`/reset`, { method: "POST" });
}

async function apiStep() {
    return callApi(`/step`, { method: "POST" });
}


//
// UTILS
//

const e = (id) => document.getElementById(id);
const hex = (num, digits) => num.toString(16).toUpperCase().padStart(digits || 2, '0');