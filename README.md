# LED Temperature Monitor

This project uses vendor libraries to show device temperatures as a range of colors per device. 

Brands supported:

 * Asus
 * Logitech

Temperatures supported:

 * CPU
 * GPU

The current implementation is very basic. 
CPU temp is mapped to all attached Asus devices. 
GPU temp is mapped to all attached Logitech devices.

# Requirements to run

For just running this, it's require that the Asus aura service, Logitech Gaming Software (not G Hub), and OpenHardwareMonitor are running.
The vendor SDKs only work when their respective apps are working, and OpenHardwareMonitor only publishes data when the app is running. 

# Requirements to build

In order to build this, you need to have Logitech Gaming Software and Asus Aura services installed, but not necessarily running. 

OS: Windows 10. It may work on earlier versions, but is untested.
IDE: Either Visual Studio, or via CLI with `Developer Command Prompt for VS2019`, and calling `make` from the project root. 

Cleanup is done with `make clean`.

Debug executable is output to `Build\main.exe`

# Running

The app is a console app that runs indefinitely, until it's quit via `Ctrl+C` on the CLI, or other means.
It scans WMI for OpenHardwareMonitor sensor data once a second, sleeping between each reading.

Sample output:

```
...
GPU temp: 65C
CPU temp: 75C

GPU temp: 65C
CPU temp: 75C

GPU temp: 65C
CPU temp: 75C
...
```