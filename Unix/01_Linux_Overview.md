### 1: The Linux Stack (Bottom to Top)

[Linux Stack]
┌─────────────────────────────────────────┐
│ Your Application & Services (systemd)   │  ← What users interact with
├─────────────────────────────────────────┤
│ Root Filesystem (libraries, binaries)   │  ← OS-side work starts HERE
├─────────────────────────────────────────┤
│ Linux Kernel (drivers, process mgmt)    │  ← Hardware interaction
├─────────────────────────────────────────┤
│ Bootloader (U-Boot, GRUB)               │  ← First code that runs
├─────────────────────────────────────────┤
│ Hardware (CPU, Memory, Storage)         │
└─────────────────────────────────────────┘


### 2: Understanding the Boot Process
What happens when you power on Linux:

Power On
   ↓
Bootloader (U-Boot) loads from flash
   ↓
Bootloader loads kernel into RAM
   ↓
Bootloader jumps to kernel entry point
   ↓
Kernel decompresses itself
   ↓
Kernel mounts root filesystem
   ↓
Kernel starts /sbin/init (or systemd)
   ↓
Init starts all services
   ↓
System ready


Key concepts:

BIOS/UEFI: Firmware that starts the bootloader

GRUB: Bootloader that loads the Linux kernel

Kernel: Linux core that initializes hardware

initramfs: Temporary root filesystem in memory

systemd: First userspace process (PID 1) that starts everything else



## The Core Concept: Layers of Boot
┌──────────────────────────────────────────┐
│  1. BOOTLOADER (lives in boot partition) │  ← First thing that runs
│     - Loads kernel into RAM              │
│     - Passes kernel command line         │
└──────────────────────────────────────────┘
              ↓
┌──────────────────────────────────────────┐
│  2. KERNEL (Linux binary)                │  ← Second thing that runs
│     - Initializes hardware               │
│     - Mounts root filesystem             │
│     - Starts init (systemd)              │
└──────────────────────────────────────────┘
              ↓
┌──────────────────────────────────────────┐
│  3. ROOT FILESYSTEM (what's in /)        │  ← Where all your files live
│     - Contains /bin, /lib, /etc          │
│     - Contains systemd                   │
│     - Contains all your packages         │
└──────────────────────────────────────────┘
              ↓
┌──────────────────────────────────────────┐
│  4. INIT SYSTEM (systemd)                │  ← Init starts all services
│     - Starts all services                │
│     - Manages running system             │
└──────────────────────────────────────────┘


### 3: Understanding the Root Filesystem

Your rootfs has a specific structure:
/
├── bin/        # Essential commands (ls, cat, etc.)
├── sbin/       # System admin commands (init, ifconfig)
├── lib/        # Shared libraries (.so files)
├── etc/        # Configuration files
│   ├── systemd/    # systemd configs
│   ├── network/    # network configs
│   └── init.d/     # legacy init scripts (if using)
├── usr/        # User programs and libraries
│   ├── bin/
│   ├── sbin/
│   └── lib/
├── var/        # Variable data (logs, tmp files)
├── tmp/        # Temporary files
├── dev/        # Device files (created dynamically)
├── proc/       # Process info (virtual filesystem)
├── sys/        # System info (virtual filesystem)
└── home/       # User home directories

### 4: How Buildroot Works

Buildroot uses Kconfig (like Linux kernel) + Makefiles:

buildroot/
├── configs/          # Board defconfigs
├── package/          # All software packages
├── board/            # Board-specific files
├── output/           # Build results
│   ├── images/       # Final images (rootfs, kernel)
│   ├── build/        # Where packages are built
│   └── host/         # Cross-compilation tools
└── dl/               # Downloaded sources