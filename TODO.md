# Phase 0 - initial setup

- [x] Debugger card
- [x] Memory card
- [x] Debugger for memory
- [x] Documentation

# Phase 1 - basic computer

- [x] CPU card
- [x] Debugger for CPU
  - [x] Determine protocol
  - [x] Modify interface
    - [x] Reload and reset
    - [x] Step cycle
    - [x] Step
    - [x] Reset
    - [x] Step with debugging info (requires NMI)
    - [x] Breakpoints
    - [x] Debug run (requires breakpoints)
    - [x] Next (requires breakpoints)
    - [x] Normal run (requires CLKENA)
  - [x] Write tests

# Phase 2 - UART

- [x] Serial card
  - [ ] Add beeper
- [x] Documentation
  - [x] CPU card
  - [x] Serial card
  - [x] Memory map and monitor
- [ ] Z80 Monitor
  - [x] Read memory
  - [x] Write memory
  - [x] Other commands
  - [ ] Print registers
  - [ ] Save state and restore

# Phase 3 - storage

- [ ] Storage card
- [ ] Z80 routines

# Phase 4 - video

- [ ] Video card
- [ ] Video card code

# Emulator improvements

- [ ] Emulator: stop execution
- [ ] Emulator: UART
- [ ] Better registers
- [ ] Bank + RAMONLY
- [ ] Choose file + symbol
- [ ] Hide addresses (checkbox)
- [ ] Bug: show breakpoints after reset
- [ ] Bug: fast run will not work without an initial step
