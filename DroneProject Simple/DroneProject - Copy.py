"""
Drone Vision Object Search - Starter Version
----------------------------------------------
- Opens a video source (your webcam or HDMI capture card)
- Runs YOLOv8 object detection on each frame
- Only draws boxes around the object you typed in (e.g. "chair", "person")
- Press 'q' to quit the video window
"""

import cv2
from ultralytics import YOLO

# ---- SETTINGS ----
CAMERA_INDEX = 0  # 0 = default webcam. If your capture card isn't showing,
                   # try 1, 2, 3... one of these will be it.

# ---- LOAD MODEL ----
# First run will auto-download the model file (yolov8n.pt), needs internet once.
print("Loading YOLO model...")
model = YOLO("yolov8n.pt")

# ---- ASK USER WHAT TO SEARCH FOR ----
target = input("What object do you want to search for? (e.g. chair, person, cup): ").strip().lower()

# ---- OPEN VIDEO SOURCE ----
cap = cv2.VideoCapture(CAMERA_INDEX)

if not cap.isOpened():
    print(f"ERROR: Could not open camera index {CAMERA_INDEX}. Try changing CAMERA_INDEX.")
    exit()

print(f"Searching for '{target}'. Press 'q' in the video window to quit.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame.")
        break

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

            # Draw box
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            label = f"{class_name} {confidence:.2f}"
            cv2.putText(frame, label, (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

    # Status text on screen
    status = f"Target: {target} | {'FOUND' if found else 'searching...'}"
    cv2.putText(frame, status, (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

    cv2.imshow("Drone Vision - Object Search", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
