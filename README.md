# Fortuna Box

A modular Z80 computer, made up of independent boards connected vertically:

* **CPU board**: Z80 cpu, power circuit, reset circuit, clock
* **I/O board**: select peripheral board, identifiy boards connected
* **Memory board**: 512k RAM, 8/16k ROM (BIOS) and memory management (bankswitching)
* **Serial board**: connects a computer serial port via UART, beeps
* **Storage board**: allows storage in the form of a SDCard
* **Video board**: VGA output terminal
* **Audio board**: outputs audio (?)
* **Debugger board**: the debugger board is optional. It'll sit on the bottom of the computer and allow for step-by-step debugging of the computer when connected to a PC via a USB cable.
