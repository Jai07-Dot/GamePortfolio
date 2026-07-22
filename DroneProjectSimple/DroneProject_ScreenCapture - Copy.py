"""
Drone Vision Object Search - Screen Capture Version
-----------------------------------------------------
- Captures a REGION OF YOUR SCREEN (where your mirrored phone/drone
  video is showing) instead of a webcam.
- Runs YOLOv8 object detection on each captured frame.
- Only draws boxes around the object you typed in (e.g. "chair", "person").
- Press 'q' (with the detection window focused) to quit.

BEFORE RUNNING:
1. Open your mirrored phone window (LonelyScreen/5KPlayer) and position
   it somewhere on screen - keep it in the same spot while this runs.
2. Run FindCoordinates.py to get the top-left (left, top) and
   bottom-right corner of the video area inside that window.
3. Fill in the REGION values below based on what you found.
"""

# =========================================================================
# AUTHOR: Jaira Settles
# PROJECT: Drone Vision Object Search
# COPYRIGHT: (c) 2026 Jaira Settles. All Rights Reserved.
# =========================================================================

__author__ = "Jaira Settles"
__version__ = "1.0.0"

import sys
import cv2
import numpy as np
import mss
from ultralytics import YOLO

# Startup Watermark
print("=" * 50)
print(f" Core Vision Engine | Author: {__author__}")
print(f" Version: {__version__}")
print(" Copyright (c) 2026 Jaira Settles. All Rights Reserved.")
print("=" * 50)

# ---- SCREEN REGION SETTINGS ----
# Update these based on FindCoordinates.py results.
# left/top = top-left corner of the mirrored video area
# width/height = size of that area (right - left, bottom - top)
REGION = {
    "left": 100,    # <-- change this
    "top": 100,     # <-- change this
    "width": 640,   # <-- change this
    "height": 480,  # <-- change this
}

# ---- LOAD MODEL ----
print("Loading YOLO model...")
model = YOLO("yolov8n.pt")

# ---- ASK USER WHAT TO SEARCH FOR ----
target = input("What object do you want to search for? (e.g. chair, person, cup): ").strip().lower()

print(f"Searching for '{target}'. Press 'q' in the video window to quit.")

# ---- CREATE OUTPUT WINDOW IN A FIXED, KNOWN POSITION ----
# This forces the output window to always appear in the same spot,
# far from the REGION being captured, so it can never overlap it
# and can't visually "stack" from leftover processes.
WINDOW_NAME = f"Drone Vision - Object Search [Dev: {__author__}]"
cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_NORMAL)
cv2.moveWindow(WINDOW_NAME, 50, 700)  # <-- adjust if this overlaps your mirror window

with mss.mss() as sct:
    while True:
        # Grab the screen region and convert it to an OpenCV-friendly format
        screenshot = sct.grab(REGION)
        frame = np.array(screenshot)
        frame = cv2.cvtColor(frame, cv2.COLOR_BGRA2BGR)

        # Run detection on this frame
        results = model(frame, verbose=False)[0]

        found = False

        for box in results.boxes:
            class_id = int(box.cls[0])
            class_name = model.names[class_id].lower()
            confidence = float(box.conf[0])

            if class_name == target and confidence > 0.4:
                found = True
                x1, y1, x2, y2 = map(int, box.xyxy[0])

                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                label = f"{class_name} {confidence:.2f}"
                cv2.putText(frame, label, (x1, y1 - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

        status = f"Target: {target} | {'FOUND' if found else 'searching...'}"
        cv2.putText(frame, status, (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        cv2.imshow(WINDOW_NAME, frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cv2.destroyAllWindows()
