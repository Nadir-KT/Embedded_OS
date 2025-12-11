# macOS Command-Line Utilities and Tips

A curated list of useful macOS commands and tips for power users.

## Table of Contents

- [Power Management](#power-management)
- [Finder & File Management](#finder--file-management)
- [System Information & Diagnostics](#system-information--diagnostics)
- [Application Management](#application-management)
- [Network Commands](#network-commands)
- [Terminal & Shell](#terminal--shell)
- [Keyboard & Input](#keyboard--input)
- [Performance & Maintenance](#performance--maintenance)
- [Homebrew Package Manager](#homebrew-package-manager)
- [Spotlight](#spotlight)
- [Screenshots](#screenshots)
- [Quick Look](#quick-look)
- [Security](#security)
- [Date & Time](#date--time)
- [Audio & Volume](#audio--volume)
- [Bluetooth](#bluetooth)

## Power Management

The `pmset` command is used to manage power management settings.

- `-a` - for all power modes (AC and battery)
- `-b` - for battery power
- `-c` - for charger (AC) power

### Disable Sleep Mode

- **On all power sources:**
  ```bash
  sudo pmset -a disablesleep 1
  ```
- **On battery only:**
  ```bash
  sudo pmset -b disablesleep 1
  ```
- **On AC power only:**
  ```bash
  sudo pmset -c disablesleep 1
  ```

### Enable Sleep Mode

- **On all power sources:**
  ```bash
  sudo pmset -a disablesleep 0
  ```
- **On battery only:**
  ```bash
  sudo pmset -b disablesleep 0
  ```
- **On AC power only:**
  ```bash
  sudo pmset -c disablesleep 0
  ```

### Other Useful `pmset` Commands

- **Set display sleep time (in minutes):**
  ```bash
  sudo pmset -a displaysleep 10
  ```
- **Set system sleep time (in minutes):**
  ```bash
  sudo pmset -a sleep 30
  ```
- **View all current power settings:**
  ```bash
  pmset -g
  ```
- **Schedule sleep at a specific time (e.g., weekdays at 10:30 PM):**
  ```bash
  sudo pmset repeat sleep MTWRF 22:30:00
  ```

### `caffeinate` Command

The `caffeinate` command prevents the system from sleeping.

- **Prevent sleep for 1 hour (3600 seconds):**
  ```bash
  caffeinate -t 3600 &
  ```
- **Prevent sleep as long as the command is running:**
  ```bash
  caffeinate
  ```
- **Prevent display sleep:**
  ```bash
  caffeinate -d
  ```

## Finder & File Management

### Show/Hide Hidden Files

- **Show hidden files:**
  ```bash
  defaults write com.apple.finder AppleShowAllFiles YES; killall Finder
  ```
- **Hide hidden files:**
  ```bash
  defaults write com.apple.finder AppleShowAllFiles NO; killall Finder
  ```
- **Toggle with shortcut:** `Cmd + Shift + .`

### File Operations

- **Empty Trash:** `rm -rf ~/.Trash/*`
- **Check file or folder size:** `du -sh <file_or_folder>`
- **Find large files (e.g., >100MB) in the home directory:** `find ~ -size +100M`
- **Copy file path to clipboard:** `realpath <file> | pbcopy`

### Extended Attributes (`xattr`)

- **List extended attributes of a file:**
  ```bash
  xattr <file_path>
  ```
- **Remove an extended attribute from a file:**
  ```bash
  xattr -d com.apple.quarantine <file_path>
  ```

### Image Manipulation (`sips`)

- **Convert an image to another format (e.g., PNG to JPG):**
  ```bash
  sips -s format jpeg input.png --out output.jpg
  ```
- **Resize an image (maintaining aspect ratio):**
  ```bash
  sips -Z 600 image.jpg
  ```

## System Information & Diagnostics

- **macOS Version:** `sw_vers`
- **Hardware Overview:** `system_profiler SPHardwareDataType`
- **CPU Info:** `sysctl -n machdep.cpu.brand_string`
- **Memory Info:** `sysctl -n hw.memsize | awk '{print $0/1073741824 " GB"}'`
- **Disk Usage:** `df -h`
- **Real-time System Activity:** `top` (or `htop` if installed via Homebrew)
- **Running Processes:** `ps aux`
- **Battery Health:** `system_profiler SPPowerDataType | grep "Health Information"`
- **List loaded launchd services:** `launchctl list`

## Application Management

- **Kill an application by name:** `killall <app_name>`
- **Force quit an application:** `osascript -e 'quit app "AppName"'`
- **Open a file with a specific application:** `open -a "TextEdit" file.txt`
- **Uninstalling Applications:** For a complete uninstall, use an app like AppCleaner or manually remove the `.app` from `/Applications` and related files from `~/Library/Application Support/`, `~/Library/Caches/`, and `~/Library/Preferences/`.

## Network Commands

- **List Network Services:** `networksetup -listallnetworkservices`
- **Wi-Fi Network Name:** `/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I | awk -F': ' '/ SSID/{print $2}'`
- **IP Address (Wi-Fi):** `ipconfig getifaddr en0`
- **DNS Info:** `scutil --dns`
- **Flush DNS Cache:** `sudo dscacheutil -flushcache; sudo killall -HUP mDNSResponder`
- **Test Connection:** `ping -c 4 8.8.8.8`
- **Show Open Ports:** `lsof -i -P -n | grep LISTEN`
- **Trace Route:** `traceroute google.com`
- **Turn Wi-Fi On/Off:**
  ```bash
  networksetup -setairportpower en0 on
  networksetup -setairportpower en0 off
  ```

## Terminal & Shell

- **Default Shell:** `echo $SHELL`
- **Change Shell to Zsh:** `chsh -s /bin/zsh`
- **Change Shell to Bash:** `chsh -s /bin/bash`
- **Create Temporary Alias:** `alias <name>='<command>'`
- **Make Alias Permanent:** Add to `~/.zshrc` (Zsh) or `~/.bash_profile` (Bash) and run `source <file>`.
- **Open File with Default App:** `open <file_name>`
- **Open Current Directory in Finder:** `open .`
- **Clipboard:**
  - `pbcopy < file.txt` (copy file content)
  - `pbpaste > new_file.txt` (paste to file)
- **Text-to-Speech:** `say "Hello, world"`

## Keyboard & Input

- **Enable Key Repeat:** `defaults write NSGlobalDomain ApplePressAndHoldEnabled -bool false`
- **Disable Key Repeat (for accent menu):** `defaults write NSGlobalDomain ApplePressAndHoldEnabled -bool true`
- **Set Key Repeat Speed (lower is faster):** `defaults write NSGlobalDomain KeyRepeat -int 2`
- **Set Initial Key Repeat Delay (lower is faster):** `defaults write NSGlobalDomain InitialKeyRepeat -int 15`

## Performance & Maintenance

- **Verify Disk:** `diskutil verifyVolume /`
- **Repair Disk (in Recovery Mode):** `diskutil repairVolume /`
- **Rebuild Spotlight Index:** `sudo mdutil -E /`
- **Clear Temporary Files:** `sudo rm -rf /private/var/tmp/*` and `sudo rm -rf /tmp/*`
- **Real-time System Logs:** `log stream`
- **Detailed Memory Usage:** `vm_stat`

## Homebrew Package Manager

- **Install:** `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
- **Update/Upgrade:** `brew update && brew upgrade`
- **Search:** `brew search <package>`
- **Install:** `brew install <package>`
- **Uninstall:** `brew uninstall <package>`
- **List Installed:** `brew list`
- **Diagnose Issues:** `brew doctor`

## Spotlight

- **Search by Name:** `mdfind -name "MyFile.txt"`
- **Search by Content:** `mdfind "some content"`
- **Exclude Directory from Indexing:** Add it in `System Preferences > Spotlight > Privacy`.
- **Rebuild Index:** `sudo mdutil -E /`

## Screenshots

- **Entire Screen:** `Cmd + Shift + 3`
- **Selected Portion:** `Cmd + Shift + 4`
- **Specific Window:** `Cmd + Shift + 4`, then `Spacebar`, then click window.
- **Change Save Location:** `defaults write com.apple.screencapture location ~/Pictures; killall SystemUIServer`
- **Change File Format (e.g., JPG):** `defaults write com.apple.screencapture type jpg; killall SystemUIServer`

## Quick Look

- **Preview File:** Select file and press `Spacebar`.
- **Extend with Plugins:** Install with Homebrew (e.g., `brew install --cask qlmarkdown`).

## Security

- **Check FileVault Status:**
  ```bash
  fdesetup status
  ```
- **Enable FileVault:**
  ```bash
  sudo fdesetup enable
  ```

## Date & Time

- **Print Current Date and Time:** `date`
- **Set Timezone (e.g., to Pacific Standard Time):**
  ```bash
  sudo systemsetup -settimezone "America/Los_Angeles"
  ```

## Audio & Volume

- **Set System Volume (0-10):**
  ```bash
  osascript -e "set volume output volume 50"
  ```
- **Mute/Unmute:**
  ```bash
  osascript -e "set volume output muted true"
  osascript -e "set volume output muted false"
  ```

## Bluetooth

- **Check Bluetooth Status:** `system_profiler SPBluetoothDataType`
- **Turn Bluetooth On/Off (requires `blueutil` from Homebrew):**
  ```bash
  brew install blueutil
  blueutil --power 1 # On
  blueutil --power 0 # Off
  ```
