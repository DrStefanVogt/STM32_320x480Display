# -*- coding: utf-8 -*-
"""
Created on Wed Mar 11 17:08:47 2026

@author: stevo
"""
import numpy as np
import folium
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtWidgets import QApplication
import math


inputGPS_N =  5429.4079
input2GPS_N = 5429.4089
inputGPS_E =  00943.9136
input2GPS_E = 00943.9145

def nmea_to_decimal(value, direction):
    """
    Convert NMEA coordinate (ddmm.mmmm or dddmm.mmmm) to decimal degrees.
    
    value: float (e.g. 5429.4117)
    direction: 'N','S','E','W'
    """
    degrees = int(value // 100)
    minutes = value - degrees * 100
    decimal = degrees + minutes / 60

    if direction in ['S', 'W']:
        decimal = -decimal

    return decimal
    
lat = nmea_to_decimal(inputGPS_N, 'N')
lon = nmea_to_decimal(inputGPS_E, 'E')

lat2 = nmea_to_decimal(input2GPS_N, 'N')
lon2 = nmea_to_decimal(input2GPS_E, 'E')

# Create a map object
m = folium.Map(location=[lat,lon], zoom_start=100)

folium.CircleMarker(
    location=[lat, lon],
    radius=4,          # Größe des Punktes
    color='red',       # Randfarbe
    fill=True,
    fill_color='red',  # Füllfarbe
    fill_opacity=1
).add_to(m)

folium.CircleMarker(
    location=[lat2, lon2],
    radius=4,          # Größe des Punktes
    color='blue',       # Randfarbe
    fill=True,
    fill_color='blue',  # Füllfarbe
    fill_opacity=1
).add_to(m)



# Beispielpunkte
p1 = (lat, lon)
p2 = (lat2,lon2)

# Haversine Funktion
def distance_m(p1, p2):
    R = 6371000  # Erdradius in Metern

    lat1 = math.radians(p1[0])
    lon1 = math.radians(p1[1])
    lat2 = math.radians(p2[0])
    lon2 = math.radians(p2[1])

    dlat = lat2 - lat1
    dlon = lon2 - lon1

    a = math.sin(dlat/2)**2 + math.cos(lat1)*math.cos(lat2)*math.sin(dlon/2)**2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))

    return R * c

dist = distance_m(p1, p2)

# Karte erstellen
center = [(p1[0]+p2[0])/2, (p1[1]+p2[1])/2]
m = folium.Map(location=center, zoom_start=14)

# Punkte
folium.CircleMarker(p1, radius=4, color='red', fill=True).add_to(m)
folium.CircleMarker(p2, radius=4, color='red', fill=True).add_to(m)

# Linie zwischen Punkten
folium.PolyLine([p1, p2]).add_to(m)

# Text mit Abstand
midpoint = [(p1[0]+p2[0])/2, (p1[1]+p2[1])/2]

folium.Marker(
    location=midpoint,
    icon=folium.DivIcon(
        html=f'<div style="font-size:12pt">{dist:.1f} m</div>'
    )
).add_to(m)


# Convert the map object to HTML
html_map = m._repr_html_()

# Create a PyQt5 application
app = QApplication([])

# Create a web view and load the HTML map
web_view = QWebEngineView()
web_view.setHtml(html_map)

# Display the web view in a separate window
web_view.show()

# Run the PyQt5 event loop
app.exec_()