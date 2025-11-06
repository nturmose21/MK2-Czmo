# **Cozmo-MK2**

**Cozmo-MK2** is a small ESP32-powered robot inspired by Anki’s Cozmo. It uses dual servos for movement, an OLED display for animated emotions, and a mobile-friendly **Wi-Fi control interface** hosted directly on the ESP32.  

Once powered, the robot broadcasts a simple web page that lets you steer it from any phone or computer connected to the same Wi-Fi network.

---

## 🚀 Features

- Wi-Fi control via browser (no external app required)  
- Real-time driving controls with “hold-to-move” touch buttons  
- Animated OLED facial expressions (loaded from `Emotions.h`)  
- Dual servo-based wheel movement with “wiggle” action  
- Easy setup using Arduino IDE  

---

## 🧠 Hardware Used

| Component              | Description                                  |
|------------------------|----------------------------------------------|
| **ESP32-C3 SuperMini**  | Main controller and Wi-Fi host               |
| **SSD1306 0.96" OLED**  | Displays facial animations                   |
| **2× SG90 or MG90S servos** | Left and right wheel drive               |
| **Battery**             | 3.7 V Li-ion / Li-Po pack                    |
| **Misc.**               | Wires, breadboard or custom frame, 3D printed parts |

---

## 🧩 3D-Printed Parts

Print the following to complete your chassis assembly:

- 2 × Servo Gear Wheels  
- 2 × Screw Wheels  
- 2 × Servo Brackets  
- 2 × Spacers  

All STL files are located in the **`3D_Models/`** folder.
---

## ⚙️ Setup and Upload

### 1. **Install Dependencies**
Open Arduino IDE and install these libraries via **Library Manager**:
- ESP32Servo  
- Adafruit GFX  
- Adafruit SSD1306  
- ESPAsyncWebServer  
- AsyncTCP  

### 2. **Connect Hardware**
| ESP32 Pin | Connection |
|-----------|------------|
| GPIO 6    | Left Servo signal |
| GPIO 7    | Right Servo signal |
| 3.3 V / GND | Power and ground |
| SDA/SCL   | OLED display (I²C, address 0x3C) |

<img width="774" height="713" alt="Screenshot 2025-11-07 at 12 17 39 am" src="https://github.com/user-attachments/assets/81946741-2f16-407f-b6b6-1ddebcfbd79e" />


### 3. **Configure Wi-Fi**
In the sketch, replace:
```cpp
const char* ssid = "WIFI NAME";
const char* password = "PASSWORD";



