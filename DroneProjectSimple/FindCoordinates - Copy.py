"""
Coordinate Finder Helper
-------------------------
Run this FIRST to find the exact screen coordinates of your mirrored
phone window (the LonelyScreen/5KPlayer window showing your drone feed).

How to use:
1. Open your mirrored phone window (LonelyScreen/5KPlayer) somewhere on screen.
2. Run this script.
3. Move your mouse to the TOP-LEFT corner of the video area inside that
   window and note the X, Y printed in the terminal.
4. Move your mouse to the BOTTOM-RIGHT corner of the video area and note
   that X, Y too.
5. Give me those two coordinate pairs (or just plug them into the main
   script yourself - see comments there).

Press Ctrl+C in the terminal to stop.
"""

import pyautogui
import time

print("Move your mouse around the mirrored window.")
print("Position will print every second. Press Ctrl+C to stop.\n")

try:
    while True:
        x, y = pyautogui.position()
        print(f"Mouse position: X={x}, Y={y}")
        time.sleep(1)
except KeyboardInterrupt:
    print("\nStopped.")