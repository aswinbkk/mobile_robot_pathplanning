# Autonomous Navigation Control of Mobile Robot Using Path Planning Algorithm
(mobile_robot_pathplanning)

---
>### Abstract ###

This project focuses on implementing a real-time path planning algorithm for the Firebird V robot in a two dimensional grid-based environment, with the goal of achieving autonomous following while avoiding obstacles. To overcome
the limitations of computation and storage, we will use PC to
facilitate efficient computation. The robot’s movement will be
limited to turning 90 and 180 degrees and moving forward,
backward, left, or right to simplify path planning. Additionally,
we will explore the use of a gripper or robotic manipulator
to enable the Firebird V to access goal objects that are near.
Our objective is to develop a safe and reliable method of object
retrieval that can be integrated into the robot’s autonomous
following capabilities.
The project presents several challenges, including optimizing
the algorithm to function efficiently on the ATMEGA2560 microcontroller, balancing competing constraints such as computation, storage, and movement limitations. However, through this
project, we hope to develop our skills in software and hardware engineering and gain comprehensive knowledge of implementing
academic algorithms in real-world products. The results of this
project could have practical applications in various industries,
including manufacturing, agriculture, and logistics. Overall, this
project is an exciting opportunity to apply academic concepts to a
real-world problem and gain valuable experience in engineering.

---
>### Index Terms ###
autonomous navigation, real-time, firebird v,
path planning, a* algorithm, gripper, image processing, xbee.

---
>### Impotant Commands ###

```python manage.py runserver```
```python manage.py migrate```
```python manage.py createsuperuser```
```Get-ExecutionPolicy -List```
```Set-ExecutionPolicy Unrestricted```

---
>### Conclusion ###

In conclusion, our project encountered a few challenges
during the development phase. One of the major issues was
drifting, despite using a stepper motor. This made it necessary
to reduce the grid size to avoid errors. In the future, we may
need to add localization methods to support larger environments. The lack of wireless communication was another issue,
as it made it difficult to display the robot’s real-time path.
Although we were able to view the output using the console
while the robot was connected to the computer, there was no
visual guide once it was unplugged. However, our prototype
was able to meet all the requirements we had set at the
beginning of the project. It can generate an optimal path from
an initial starting point and re-generate an optimal path when
obstacles are detected. Overall, it meets the basic requirements
and can be further improved with additional features such as
wireless compatibility.


---
>### License

[![License](http://img.shields.io/:License-Aswin_BKK-blue.svg?style=flat-square)](https://en.wikipedia.org/wiki/Copyright)

- **[Aswin BKK license](https://en.wikipedia.org/wiki/Open-source_license)**
- Copyright 2022 © <a href="https://www.linkedin.com/in/aswinbkk" target="_blank">Aswin BKK</a>.
