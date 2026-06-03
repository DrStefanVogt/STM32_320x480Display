#!/bin/bash

set -e

echo "========================================="
echo " STM32 VSCode Environment Setup (Mint)"
echo "========================================="

echo ""
echo "[1/6] Updating package lists..."
sudo apt update

echo ""
echo "[2/6] Installing ARM GNU Toolchain..."
sudo apt install -y gcc-arm-none-eabi

echo ""
echo "[3/6] Installing build tools..."
sudo apt install -y build-essential

echo ""
echo "[4/6] Installing OpenOCD..."
sudo apt install -y openocd

echo ""
echo "[5/6] Installing ST-Link tools..."
sudo apt install -y stlink-tools

echo ""
echo "[6/6] Reloading udev rules..."
sudo udevadm control --reload-rules
sudo udevadm trigger

echo ""
echo "========================================="
echo " Installation complete"
echo "========================================="

echo ""
echo "Installed tools:"
echo ""

echo -n "GCC: "
arm-none-eabi-gcc --version | head -n 1

echo -n "MAKE: "
make --version | head -n 1

echo -n "OpenOCD: "
openocd --version 2>&1 | head -n 1

echo ""
echo "You can now test your ST-Link with:"
echo ""
echo "st-info --probe"
echo ""
echo "And build your project with:"
echo ""
echo "make"
echo ""
