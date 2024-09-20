# CMPT433_ALLIN
GatekeeperInsights - CMPT433 Project
Overview
GatekeeperInsights is a customer traffic monitoring and analysis system designed to enhance the shopping experience while providing store owners with valuable data on daily customer traffic. The system uses a combination of hardware components (motion sensors, laser-photocell setups) and software to track customer entries and exits and display the data on a real-time web interface.

Features
1. Customer Counting:
Motion sensors detect customer entries, increasing the count by one.
Laser and photocell detect customer exits, decreasing the count by one.
Real-time data is transmitted via UDP to a web interface, displaying the current number of people inside the store.

2. Web Dashboard:
Displays daily entry counts, average hourly and daily customer counts, and peak customer counts.
Visual representation using Chart.js for weekly and daily averages and hourly maxima.

3. Interactive Hardware:
NeoPixel Color Shifts: Provides occupancy status (green: low, yellow: medium, red: high).
Entry/Exit Alerts: Visual and audio alerts for customer entry and exit using the NeoPixel and audio feedback.
Joystick Control: Users can switch display modes (People Count, Temperature, Smile) on the Mini 8x8 LED Matrix and interact with other display modes.

4. Other Key Features:
System uptime monitoring.
Adjustable volume for audio feedback.
Displays current time and ambient temperature.
Room occupancy indication via colored lights.
Program termination option via joystick control.

Hardware Components
BeagleBone Green (BBG): Central controller for input/output signals and data processing.
Motion Sensor: Detects customer entries.
Laser Diode and Photocell: Detects customer exits.
NeoPixel: Displays room capacity status and flashes during customer entry/exit.
Temperature Sensor (MCP9808): Provides ambient temperature data.
14-Seg Display: Shows current people count or temperature.
Mini 8x8 Yellow-Green LED Matrix: Displays modes (People Count, Temperature, Smile).
Grove 4-Digit Display: Displays current time in GMT-7.
Joystick: Allows users to switch between display modes and terminate the system.

Software
UDP Protocol: Used for real-time data transmission from the sensors to the web dashboard.
Web Interface: Created using JavaScript and HTML, displaying customer traffic data.
IndexedDB: Stores daily, hourly, and minute-level customer data.
Audio Feedback: Music plays during customer entry and exit, enhancing the shopping experience.

Challenges
Sensitivity of motion sensors and fixed laser setup made it difficult to detect all customers accurately.
Limited time for real data collection, so random data was used for the demo.
Inaccurate temperature readings occasionally due to hardware limitations.

Installation
1. Hardware Setup:
Connect all sensors (motion sensor, laser-photocell setup, temperature sensor) to the BBG.
Set up displays and joystick for interactive features.
Software Setup:

2. Clone the repository to your local machine.
Install necessary dependencies for the web interface (JavaScript, Chart.js).
Configure the BBG to handle sensor input/output and run the UDP protocol for real-time data transmission.
3. Running the System:
Start the BBG server.
Open the web interface to monitor customer traffic.
Use the joystick for mode switching and display control.

Authors:
Xiaohong Xu
Lingjie Li
Lihao Qian
Haowei Hu
