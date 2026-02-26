# -*- coding: utf-8 -*-
"""
Created on Thu Feb 26 18:35:25 2026

@author: stevo
"""

# STM32F411 Ressourcenberechnung

FLASH_TOTAL = 512 * 1024   # Bytes
RAM_TOTAL   = 128 * 1024   # Bytes

text = 26560
data = 508
bss  = 6324

flash_used = text + data
ram_used   = data + bss

print(f"Flash: {flash_used} B / {FLASH_TOTAL} B "
      f"({flash_used / FLASH_TOTAL * 100:.2f} %)")

print(f"RAM:   {ram_used} B / {RAM_TOTAL} B "
      f"({ram_used / RAM_TOTAL * 100:.2f} %)")