# Accelerometer over Radio

### Transceives data over 2.4Ghz RF wireless between:
* Arduino Uno connected to 10kΩ Potentiometer and NRF24L01+ radio module.
* Arduino Uno connected to ADXL345: 3-axis Digital Gravity Sensor Acceleration Module Tilt Sensor and NRF24L01+ radio module.

When potentiometer resistance value breaks a threshold, data is sent from the accelerometer node & displayed.<br/>
Data from each sensor is put through a software filter to remove noise.

![Explaination gif](https://github.com/SenanS/Accelerometer-over-Radio/blob/main/Acceler.gif)
