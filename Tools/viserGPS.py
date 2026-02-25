import serial
import threading
import numpy as np
import viser
import os
import sys
import time
import string
from collections import deque

PORT = "COM7"
BAUDRATE = 115200  # ggf. 9600
run_GPS = True

MAX_POINTS = 300
TIME_SCALE = 0.02
SPEED_SCALE = 0.05

# --------- Zeitspuren ----------
times = deque(maxlen=MAX_POINTS)
lats = deque(maxlen=MAX_POINTS)
lons = deque(maxlen=MAX_POINTS)
speeds = deque(maxlen=MAX_POINTS)

plot_N = 200
points = np.random.normal(size=(plot_N, 2, 3)) * 3.0
colors = np.random.randint(0, 255, size=(plot_N, 2, 3))

def gps_thread(point_cloud, gui_elements):
    global serial_connection
    
    try:
        print(f"Opening serial port {PORT} @ {BAUDRATE} baud")
        serial_connection = serial.Serial(PORT, BAUDRATE, timeout=1)
        lat_before = 0
        while run_GPS:
            os.system('clear')
            if not serial_connection or not serial_connection.is_open:
                break
            line = serial_connection.readline().decode(errors="ignore").strip()
   
            if not line:
                continue
            
            parts = line.split(",")
            lat = float(parts[3])
            lon = float(parts[5])
            speed = float(parts[7])
            course = parts[8]
            utc_time = parts[1]
            
            gui_elements['lat'].value="hey"
            # RAW Konsole aktualisieren
 
            gui_elements["time"].value =f"{parts[1]}"
            gui_elements["lat"].value = f"{lat_before - float(parts[3])}"
            gui_elements["lon"].value = f"{parts[5]}"
            gui_elements["speed"].value = f"{parts[7]}"
            gui_elements["course"].value = f"{parts[8]}"
            lat_before = float(parts[3])
            
            times.append(utc_time)
            lats.append(lat)
            lons.append(lon)
            speeds.append(speed)
                    
            # -------- Live Plot (Polyline!) --------
            
            points = np.random.normal(size=(plot_N, 2, 3)) * 3.0
            colors = np.random.randint(0, 255, size=(plot_N, 2, 3))
            server.scene.add_line_segments(
                "/line_segments",
                points=points,
                colors=colors,
                line_width=3.0,
)

                  

    except serial.SerialException as e:
        print("Serial Fehler:", e)

    except Exception as e:
        print("Allgemeiner GPS Fehler:", e)

    finally:
        if serial_connection and serial_connection.is_open:
            print("Closing serial port (gps_thread end)")
            serial_connection.close()


def main():
    global server
    global server
    server = viser.ViserServer()
    print("Viser läuft auf http://localhost:8080")
    
    
    restart_button = server.gui.add_button("End")

    @restart_button.on_click
    def _(_event):
        print("Ending.")
        global run_GPS
        run_GPS = False
        server.stop()
        time.sleep(1)
        os._exit(0)
            

    # --- 3D Punkt ---
    point_cloud = server.scene.add_point_cloud(
    name="/gps_point",
    points=np.array([[0.0, 0.0, 0.0]]),
    colors=np.array([[1.0, 0.2, 0.2]]),
    point_size=0.1,
    )

    # --- GUI ---
    server.gui.add_markdown("## 🛰️ GPS Status")

    lat_text = server.gui.add_text("Latitude", "")
    lon_text = server.gui.add_text("Longitude", "")
    speed_text = server.gui.add_text("Speed", "")
    course_text = server.gui.add_text("Course", "")
    time_text = server.gui.add_text("UTC Time", "")

    raw_data = server.gui.add_text("raw","")

    gui_elements = {
        "lat": lat_text,
        "lon": lon_text,
        "speed": speed_text,
        "course": course_text,
        "time": time_text,
        "raw": raw_data,
    }


    gps_thread1= threading.Thread(
        target=gps_thread,
        args=(point_cloud, gui_elements),
        daemon=True,
    )
    gps_thread1.start()
    threading.Event().wait()
        

if __name__ == "__main__":
    main()
