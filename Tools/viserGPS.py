import serial
import threading
import numpy as np
import viser
import os
import sys
import time
import string

PORT = "COM3"
BAUDRATE = 9600  # ggf. 9600
run_GPS = True

def nmea_to_decimal(coord, direction):
    if not coord:
        return None

    # Latitude = 2 Gradstellen, Longitude = 3
    if direction in ["N", "S"]:
        degrees = float(coord[:2])
        minutes = float(coord[2:])
    else:
        degrees = float(coord[:3])
        minutes = float(coord[3:])

    decimal = degrees + minutes / 60.0
    if direction in ["S", "W"]:
        decimal *= -1

    return decimal


def gps_thread(point_cloud, gui_elements):
    global serial_connection
    
    try:
        print(f"Opening serial port {PORT} @ {BAUDRATE} baud")
        serial_connection = serial.Serial(PORT, BAUDRATE, timeout=1)
        
        while run_GPS:
            os.system('clear')
            if not serial_connection or not serial_connection.is_open:
                break
            line = serial_connection.readline().decode(errors="ignore").strip()
            lon_at = line.find('$GPGLL')
            satelites_at = line.find('$GPGSV')
            gptxt_at = line.find('$GPTXT')
            if(not lon_at):
                gui_elements["lon"].value = f"{line[lon_at+6:-1]}"
            if(not satelites_at):
                gui_elements["speed"].value = f"{line[satelites_at:-1]}"
                print(line)
            if(not gptxt_at):
                 gui_elements["course"].value = f"{line[lon_at+6:-1]}"
            
            if not line:
                continue
            gui_elements['lat'].value="hey"
            # RAW Konsole aktualisieren
            gui_elements["raw"].value =f"{line}"

            if line.startswith("$GPRMC"):
                parts = line.split(",")

                if len(parts) > 8 and parts[2] == "A":  # gültiger Fix
                    lat = nmea_to_decimal(parts[3], parts[4])
                    lon = nmea_to_decimal(parts[5], parts[6])
                    speed = parts[7]
                    course = parts[8]
                    utc_time = parts[1]

                    if lat is not None and lon is not None:
                        pos = np.array([[lon, lat, 0.0]])
                        point_cloud.points = pos

                        # GUI aktualisieren
                        gui_elements["lat"].value = f"{lat:.6f}"
                        gui_elements["lon"].value = f"{lon:.6f}"
                        gui_elements["speed"].value = f"{speed} kn"
                        gui_elements["course"].value = f"{course}°"
                        gui_elements["time"].value = utc_time

    except serial.SerialException as e:
        print("Serial Fehler:", e)

    except Exception as e:
        print("Allgemeiner GPS Fehler:", e)

    finally:
        if serial_connection and serial_connection.is_open:
            print("Closing serial port (gps_thread end)")
            serial_connection.close()


def main():
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
        point_size=10.0,
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
