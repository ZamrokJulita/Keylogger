This is a keylogger for windows created in the kernel mode.
A keylogger operates in kernel mode using a keyboard input filter driver provided by Microsoft.
There is a user-mode application for real-time display and file logging of intercepted keys, communicating with the driver via IOCTL.
The filter driver proviaded by Microsoft is: Keyboard Input WDF Filter Driver (Kbfiltr).
This keylogger was tested on Windows 10 Virtual Machine. 
