0. CMD Normal Exit press: q
Clear Terminal: clear
Ctrl + C = stop/interrupt a running process
Ctrl + Z = pause and background a process (not needed here)

1.Update system:
sudo apt update && sudo apt upgrade -y

2.System Services:
systemctl
systemctl list-units --type=service # See all running services
systemctl --failed                  # See all failed services
sudo systemctl daemon-reload        # Reload systemd after changes
journalctl -xe                      # View logs
                      
3.System target:
systemctl get-default                           # See current target
sudo systemctl set-default multi-user.target    # Switch to multi-user (no GUI)
sudo systemctl set-default graphical.target     # Switch back to GUI
sudo reboot
sudo shutdown -h now

4.nano editor:
To save: Ctrl + O (then press Enter to confirm)
To exit: Ctrl + X

5. vi / vim editor:
To exit without saving: :q!
To save and exit: :wq (or) :x

sudo apt install gcc



# 1. Check kernel version and architecture
uname -a
cat /proc/version
dpkg -l | wc -l                   # How many packages are installed?
journalctl -b                     # See systemd logs from the current boot.
sudo dmesg | less                 # See kernel boot messages

# 2. See storage layout
lsblk
df -h
df -h /                           # Size of current OS drive?
du -sh /* 2>/dev/null | sort -h   # What's taking up space?
ls -la /                          # What's in root filesystem?

# 3. Check memory information
free -h
cat /proc/meminfo | head -10

# 4. See CPU information
lscpu
cat /proc/cpuinfo | head -20
ps aux                          # See all processes
ps aux | grep loop              # Find your loop service
pstree                          # See process tree (parent-child relationships)
top                             # Live view of processes