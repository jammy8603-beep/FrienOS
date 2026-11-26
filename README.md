# FrienOS (Beta)

> **Beta Notice:** FrienOS (`v0.4.0`) is currently in **beta**. Features, installer behavior, and system configurations are under active development—use in a virtual machine or on dedicated test hardware rather than a primary workstation.

FrienOS is a lightweight, 64-bit (`amd64`) Debian 12-based live and installable Linux operating system featuring an XFCE desktop environment, automated live login, and a custom GTK 3 graphical installer, built automatically via GitHub Actions.

---

## Features

* **Debian 12 (`bookworm`) Base:** Built from scratch using `debootstrap` with the `amd64` Linux kernel and `live-boot`.
* **XFCE Graphical Desktop:** Preconfigured with `xfce4`, `xserver-xorg`, `lightdm`, `lightdm-gtk-greeter`, and automatic live login.
* **Custom GTK 3 Installer (`frenos-installer`):** Graphical system installer with desktop and autostart launchers that handles GPT partitioning, `ext4` formatting, system file cloning (`rsync`), user creation, and GRUB bootloader installation.
* **Dual BIOS & UEFI Boot:** Packaged into a bootable hybrid `.iso` via `grub-mkrescue` and `squashfs-tools`.
* **VMware Ready:** Includes `open-vm-tools` and `open-vm-tools-desktop` out of the box for clipboard sharing and display scaling.
* **Automated CI/CD Builds:** GitHub Actions workflow (`.github/workflows/build-iso.yml`) compiles the live filesystem and publishes release ISOs automatically.

---

## Default Live Credentials

* **Username:** `friend`
* **Password:** `friend` *(Note: Terminal password prompts hide typed characters by design.)*

---

## System Requirements

* **Architecture:** 64-bit (`x86_64` / `amd64`) processor (2+ vCPUs recommended)
* **RAM (Memory):** 2 GB minimum (required for live `initramfs` unpacking); 4 GB recommended
* **Storage:** 10 GB minimum disk space for installation (20–25 GB recommended)
* **Display & Network:** 1024×768 minimum display resolution; standard Ethernet/virtual NIC (e.g., Intel E1000)

---

## Quick Start (Virtual Machine)

1. Download the latest `FrienOS-*.iso` release from the **Releases** page.
2. Create a new 64-bit Debian/Linux virtual machine with at least **2 GB RAM** and a **10+ GB virtual hard drive**.
3. Attach the downloaded `.iso` file to the VM's CD/DVD drive and ensure **Connect CD/DVD Drive** is enabled.
4. Power on the VM to boot into the live XFCE desktop.
5. Launch **Install FrienOS** (from the desktop shortcut or autostart window) to partition and install FrienOS to your virtual drive.
6. Once installation finishes, power off the VM, disconnect/remove the `.iso` from the CD/DVD drive, and power on to boot from your installed disk.

---

## Building from Source

FrienOS is built using GitHub Actions on `ubuntu-24.04`:

1. Fork or clone this repository.
2. Navigate to the **Actions** tab on GitHub.
3. Select **Build FrienOS ISO** and click **Run workflow**.
4. Once complete, download the generated `.iso` artifact from **Releases**.

---

## Project Structure

* `.github/workflows/build-iso.yml` — Automated build pipeline that bootstraps Debian 12, installs desktop/installer packages, generates `filesystem.squashfs`, builds the bootable ISO with `grub-mkrescue`, and publishes the GitHub release.
