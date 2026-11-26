# FrienOS (Beta)

> **Beta Notice:** FrienOS (`0.6.0-beta`) is an early **beta** release in active development. Test it in a virtual machine rather than on a primary computer.

FrienOS is a 64-bit (`x86_64`) operating system that boots via GRUB Multiboot2 into a `1024x768x32` graphical framebuffer interface and setup/login screen.

---

## Features

* **64-Bit (`x86_64`) Kernel:** Uses a custom Multiboot2 entry, 64-bit Long Mode transition, and C kernel (`frenos.bin`).
* **Direct Graphical Framebuffer Boot:** Requests a `1024x768x32` linear framebuffer and draws the desktop backdrop, top bar, and centered setup/login window on boot.
* **Lightweight Startup:** Runs a direct graphical boot path suited for fast virtual machine testing.
* **Automated ISO Builds:** Uses GitHub Actions to assemble, compile, link, and package a bootable `.iso` via `grub-mkrescue`.

---

## What Changed Since Earlier Builds

* **New 64-Bit Kernel & Boot Path:** Replaced the previous setup with a Multiboot2 `x86_64` kernel that renders directly to the framebuffer.
* **Fixed Text-Console (`tty1`) Boot Fallback:** Resolved startup issues that previously dropped boots to a command-line login prompt (`tty1`) instead of opening the graphical screen.

---

## System Requirements

* **Architecture:** 64-bit (`x86_64`) processor or virtual machine
* **RAM:** 2 GB recommended
* **Graphics:** Standard VESA / UEFI linear framebuffer support (`1024x768` or higher)
* **Virtualization:** VMware Fusion or compatible `x86_64` hypervisor

---

## Quick Start (Virtual Machine)

1. Download the `FrienOS-*.iso` release image from the **Releases** page.
2. Create or open a 64-bit virtual machine with at least **2 GB RAM** and attach the `.iso` to its CD/DVD drive with the drive connected.
3. Power on the virtual machine and select **FrienOS** in the boot menu to launch into the graphical screen.

---

## Building from Source

1. Clone or open the repository containing the bootloader, linker script, kernel source, and `.github/workflows/build-iso.yml`.
2. Run the **Build FrienOS** workflow in the **Actions** tab on GitHub.
3. Download the resulting `.iso` asset from the published release once the workflow finishes.

---

## Project Layout

* `src/boot.asm` — Multiboot2 header, page tables, and 64-bit Long Mode entry.
* `src/kernel.c` — Framebuffer discovery and graphical desktop/window rendering.
* `src/linker.ld` — Kernel memory layout and section placement.
* `.github/workflows/build-iso.yml` — Automated build and release pipeline for the bootable ISO.
