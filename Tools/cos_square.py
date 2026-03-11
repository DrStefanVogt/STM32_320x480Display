# -*- coding: utf-8 -*-
"""
Created on Tue Mar 10 19:49:38 2026

@author: stevo
"""

import numpy as np
import matplotlib.pyplot as plt
import os
import string

TXT_FILE = "cossquare.txt"

x = np.arange(0, 90, 1)
cossquare = (255 * np.cos(x/180*np.pi)**2).astype(int)
coss = (255 * np.cos(x/180*np.pi)).astype(int)

plt.plot(x,cossquare,coss)

print(f"{x}", cossquare)
print((cossquare[54]/255)-np.cos(54/180*np.pi)**2)

with open(TXT_FILE, "w") as f:
    f.write(f"[")
    for number in coss:
       f.write(f"{number}, ")
    f.write(f"]")