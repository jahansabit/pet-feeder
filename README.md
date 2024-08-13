# Pet Feeder Web Application

This project is a web-based pet feeder application that allows users to schedule feeding times and manually dispense food. It consists of a backend server, a frontend web interface, and an ESP8266-based hardware component for controlling the pet feeder.

## Components

1.  Backend (Python/Flask)
2.  Frontend (HTML/CSS/JavaScript)
3.  ESP8266 Firmware (C++)

## Features

-   User authentication
-   Schedule feeding times
-   Manual food dispensing
-   MQTT communication for real-time control
-   WiFi configuration for the ESP8266 module

## Backend (main.py)

The backend is built with Flask and provides the following functionality:

-   User authentication
-   CRUD operations for feeding schedules
-   MQTT communication to control the pet feeder
-   SQLite database for storing user data and feeding schedules

## Frontend

The frontend consists of two main pages:

1.  Login page (login/index.html)
2.  Main application page (index.html)

The frontend uses Materialize CSS for styling and SweetAlert2 for user-friendly notifications.

## ESP8266 Firmware (esp_pet_dispense.ino)

The ESP8266 firmware handles:

-   WiFi connection and configuration
-   MQTT communication with the server
-   Control of the pet feeder hardware

## Setup and Installation

1.  Set up the Python environment and install required packages:
    
    Copy
    
    `pip install flask flask_cors paho-mqtt pytz`
    
2.  Set up the SQLite database by running the backend for the first time.
3.  Upload the ESP8266 firmware to your device.
4.  Configure the WiFi credentials for the ESP8266 using the AP mode.
5.  Update the MQTT broker address and credentials in both the backend and ESP8266 firmware.
6.  Run the Flask server:
    
    Copy
    
    `python main.py`
    
7.  Access the web application through a browser.

## Usage

1.  Log in using your user ID and password.
2.  Add feeding times using the time picker.
3.  Edit or delete existing feeding times as needed.
4.  Use the "Dispense" button for manual food dispensing.

## Security Notes

-   Ensure to use secure passwords and API keys.
-   The current implementation stores passwords in plain text. For production use, implement proper password hashing.
-   Consider using HTTPS for the web server and encrypted MQTT communication.
