================================================================================
SYSTEMD & SYSTEMCTL NOTES
================================================================================

--------------------------------------------------------------------------------
1. WHAT IS SYSTEMD? (Service Management)
--------------------------------------------------------------------------------
Systemd is the "Init System" (Initialization) for modern Linux.
It is the very first process started by the Kernel during boot.

* Process ID (PID): Always 1.
* Role: The manager of the whole operating system.

In Embedded Linux, systemd is the "heart" that brings the device to life.

Key Responsibilities:
[+] Starts all services (parallelized boot).
[+] Manages dependencies (e.g., "Start Wi-Fi only after Hardware Init").
[+] Handles shutdown and reboot.
[+] Mounts filesystems.
[+] Manages logs (via journald).
[+] Supervises processes (automatically restarts crashed programs).

A service is a background process that performs specific tasks within your operating system. 
In systemd, services are defined as unit files that describe how to start and manage these processes.

--------------------------------------------------------------------------------
2. SYSTEMCTL (The Command Line Tool)
--------------------------------------------------------------------------------
'systemctl' is the tool you use to communicate with systemd. 
Think of it as the remote control for the OS services.

Common Commands:
$ sudo systemctl start <name>    # Turn service ON
$ sudo systemctl stop <name>     # Turn service OFF
$ sudo systemctl restart <name>  # Stop then Start
$ sudo systemctl status <name>   # Check health/logs
$ sudo systemctl enable <name>   # Set to auto-start at Boot
$ sudo systemctl disable <name>  # Stop auto-start at Boot

--------------------------------------------------------------------------------
3. Creating a Systemd Service
--------------------------------------------------------------------------------

Create a service unit file:
sudo nano /etc/systemd/system/hello.service

Paste the following:
[Unit]
Description=Hello Service

[Service]
ExecStart=/bin/echo "Hello from systemd"

[Install]
WantedBy=multi-user.target

Save and exit:
To save: Ctrl + O (then press Enter to confirm)
To exit: Ctrl + X
To display the contents of the file: cat /etc/systemd/system/hello.service

Reload and start:
sudo systemctl daemon-reload
sudo systemctl start hello
sudo systemctl status hello

Output:
    hello.service - Hello Service
    Hello from systemd
    hello.service: Deactivated successfully.

Why service shows "inactive (dead)":
    Because it's a oneshot service (runs once, exits immediately).

--------------------------------------------------------------------------------
4. SYSTEMD SERVICE MODES (TYPES)
--------------------------------------------------------------------------------

1. simple (Default)
   - The process runs continuously in the foreground.
   - Best for: Web servers, Python scripts, infinite loops.

2. oneshot
   - The process runs once and exits. Systemd waits for it to finish.
   - Best for: Initialization scripts (e.g., setting a static IP on boot).

3. forking
   - Old style (SysV). The process detaches itself from the terminal.
   - Rare in modern embedded apps.

--------------------------------------------------------------------------------
5. CREATING A CONTINUOUS BACKGROUND SERVICE
--------------------------------------------------------------------------------
Goal: Run a script forever, automatically restart it if it crashes, 
and start it when the device turns on.

Create a script file:
sudo nano /usr/local/bin/loop.sh

Paste the following:
#!/bin/bash
while true
do
   echo "$(date): loop running" >> /tmp/loop.log
   sleep 5
done

Explicitly mark the file as executable: 
sudo chmod +x /usr/local/bin/loop.sh

Create a service unit file:
sudo nano /etc/systemd/system/loop.service

Paste the following:
[Unit]
Description=Loop Test

[Service]
ExecStart=/usr/local/bin/loop.sh
Restart=always

[Install]
WantedBy=multi-user.target

Load & Start:
sudo systemctl daemon-reload
sudo systemctl start loop

Verify it's running:
sudo systemctl status loop
tail -f /tmp/loop.log

Stop the service:
sudo systemctl stop loop
sudo systemctl status loop

Make it start at boot:
sudo systemctl enable loop

Reboot and verify:
sudo reboot
systemctl status loop
tail -f /tmp/loop.log

View service logs:
journalctl -u loop -f

================================================================================
END OF NOTES
================================================================================