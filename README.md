<a href="#"><img src="images/logo_small.png" width="128" height="135" align="right"/></a>

# VK RcCat :cat:

![Build Car Firmware](https://github.com/VK/RcCat/workflows/Build%20Car%20Firmware/badge.svg)  
![Build Wlan Repeater Firmware](https://github.com/VK/RcCat/workflows/Build%20Wlan%20Repeater%20Firmware/badge.svg)

**RcCat provides next generation driving aids for Rc Cars.**

Ramp Jumps                     |  Garden Jumps                      |
:-----------------------------:|:----------------------------------:
[![RcCat Ramp Jumps](https://img.youtube.com/vi/5Ehqny6YLAg/0.jpg)](https://www.youtube.com/watch?v=5Ehqny6YLAg "RcCat Ramp Jumps")   |  [![RcCat Garden Jumps](https://img.youtube.com/vi/Mkig3it6fFk/0.jpg)](https://www.youtube.com/watch?v=Mkig3it6fFk "RcCat Garden Jumps")   |

A microcontroller with several sensors intercepts the drive commands of the receiver
to send optimized signals to the steering servo and the speed controller.
During the flights, a minimalistic neural network is used to determine the next action.
As a result, my RcCat can land on all fours if it falls to the ground from a distance of about one meter.

My prototype is based on 1:16 **Traxxas Mini E-Revo**:

Car top view                   |  Car bottom view                   | Monitor App
:-----------------------------:|:----------------------------------:|:----------------------:
![](images/car_top_small.png)  |  ![](images/car_bottom_small.png)  | ![](images/monitor.gif)


## Repo parts
* **[car_firmware](/car_firmware):** The core firmware of RcCat

* **[wlan_repeater](/wlan_repeater):** Serial to Wlan bridge to send telemetry data via wlan during driving and update drive parameters.

* **[pyapps](/pyapps):** Additional python tools to analyze RcCat telemetry data and adjust drive characteristics

