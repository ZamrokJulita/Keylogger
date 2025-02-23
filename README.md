# Windows Kernel-Mode Keylogger

## Description
This project is a **keylogger** for Windows, operating in **kernel mode**.

The keylogger utilizes a keyboard input filter driver provided by Microsoft, allowing key presses to be intercepted at the kernel level.

## How It Works
- Operates in **kernel mode**, using a keyboard input filter driver.
- Includes a **user-mode application** that:
  - Displays intercepted keys in real time.
  - Logs keystrokes to a file.
  - Communicates with the driver via **IOCTL**.

## Used Driver
This project is based on the Microsoft-provided driver:  
**Keyboard Input WDF Filter Driver (Kbfiltr)**

## Testing
The keylogger was tested on a **Windows 10** virtual machine.
