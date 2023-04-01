# MOBILE ROBOT PATHPLANNING
(mobile_robot_pathplanning)

---
>### Abstract ###

Security systems have suggested various preferred approaches like biometric and
password to enhance security. The technology is developing day by day and growing with
the usage of different equipments. The trend has been moved from fingerprint to face
recognition. So face recognition technique has been utilized in this system for unlocking
the gate. Facial recognition technology can be used in various industries and corporate
sectors. This is about door unlocking system that mainly uses facial recognition
technology. The camera is used to capture the face images and the images are sent to
the database. If the image matches with the administration image then gate will be
unlocked and an acknowledgement is saved in the database system. If face is not
matched, then a warning message will be sent to the admin device, as someone is trying
to unlock the gate.

---
>### Hardware Components ###

**1. Arduino Uno**

<img src="image/arduino.png" style="width:300px;height:233px">

**2. Servo Motor**

<img src="image/servo_motor.png" style="width:300px;height:300px">

**3. Web Cam**

<img src="image/webcam.png" style="width:300px;height:257px">

---
>### Software Components ### 

**1. Pycharm IDE**

PyCharm is an integrated development environment
used in computer programming, specifically for the
Python programming language. It is developed by the
Czech company JetBrains.

<img src="image/pycharm_ide.png" style="width:300px;height:187px">

[![Click to Download](http://img.shields.io/:Click_to_Download-Pycharm_IDE-green.svg?style=flat-square)](https://www.jetbrains.com/pycharm/download)

**a) Django**

Django is a high-level Python web framework that
encourages rapid development and clean, pragmatic
design. Django makes it easier to build better web apps
quickly and with less code.

**b) OpenCV-python**

OpenCV is a library of programming functions mainly
aimed at real-time computer vision. Originally developed
by Intel.

**c) Pyserial**

PySerial is a Python API module which is used to read
and write serial data to Arduino or any other
Microcontroller.

**d) Pyttsx 3**

Pyttsx 3 is a text-to-speech conversion library in Python.
Unlike alternative libraries, it works offline and is
compatible with both Python 2 and 3.

<img src="image/interpreter.png" style="width:600px;height:318px">

**2. Arduino IDE**

The open-source Arduino Software (IDE) makes it easy
to write code and upload it to the board. This software
can be used with any Arduino board.

<img src="image/arduino_ide.png" style="width:300px;height:207px">

[![Click to Download](http://img.shields.io/:Click_to_Download-Arduino_IDE-green.svg?style=flat-square)](https://www.arduino.cc/en/software)

---
>### Circuit Diagram ###

<img src="image/circuit_diagram.png" style="width:600px;height:363px">

- Servo Motor Signal Pin will be Connected to the arduino
PWM Pin Number 9. The 5V pin will be connected to
Arduino 5v pin and Ground pin will be connected to Arduino
Ground pin. Arduino must be connected to PC through
arduino Cable.
- At first the python code will be write on any of the python
IDE than arduino_code will be uploaded to arduino Uno
board using arduino IDE.

---
>### Result ###

1. Run the command ```python manage.py runserver``` on pycharm terminal

<img src="image/terminal1.png" style="width:600px;height:318px">

2. Open starting development server URL

<img src="image/terminal2.png" style="width:600px;height:318px">

3. Home UI

<img src="image/ui_home.png" style="width:600px;height:318px">

4. Log In UI

```shell
Username: aswinbkk
Password: aswinbkk
```
<img src="image/ui_login.png" style="width:600px;height:318px">

6. Dashboard UI

<img src="image/ui_dashboard.png" style="width:600px;height:318px">

6. Add new face instructions, it will collect the sample pictures

<img src="image/ui_add.png" style="width:600px;height:318px">

7. Confirm face instructions, it will start the webcam and take pictures. If the picture match with database upto 83% then it will send a signal to the
arduino and arduino will drive the servo motor, gate will be open for 5 seconds.

<img src="image/ui_confirm.png" style="width:600px;height:318px">

9. Our team

<img src="image/ui_about.png" style="width:600px;height:318px">

---
>### Impotant Commands ###

```python manage.py runserver```
```python manage.py migrate```
```python manage.py createsuperuser```
```Get-ExecutionPolicy -List```
```Set-ExecutionPolicy Unrestricted```

---
>### Future Scope ###

This project is currently implemented in small scale structures such as household .It can
be widely installed for large scale purposes like gates of institutions ,factories, large
organisational structure etc. to provide higher security . It can completely reduce
manpower in the future . Use of higher facial features recognition in biometrics technology
can be included . The technology can be mapped into mobiles phones and other easy to
use technologies which will contribute to future scopes and improvements.

---
>### Conclusion ###

In this proposed gate access system by using the face recognition technology the images
are stored in a database. This system is used door lock access for Residential purposes and
Commercial Purposes. Here we have designed a highly secured gate locking system by
using webcam. This paper concludes the advanced implements in the face recognition
process in various places. The main purpose of the project to ensure security at the low
cost.

---
>### License

[![License](http://img.shields.io/:License-Aswin_BKK-blue.svg?style=flat-square)](https://en.wikipedia.org/wiki/Copyright)

- **[Aswin BKK license](https://en.wikipedia.org/wiki/Open-source_license)**
- Copyright 2022 © <a href="https://www.linkedin.com/in/aswinbkk" target="_blank">Aswin BKK</a>.
