# Research Reaction Time

ESP32 reaction-time experiment built with ESP-IDF. The app turns on an LED after a random preparation delay, timestamps a button press in a GPIO interrupt, and reports both human reaction time and ISR-to-task wake latency.

## Hardware

- ESP32 development board
- LED on GPIO 25
- Momentary button on GPIO 27, wired active-low with the ESP32 internal pull-up enabled

## Behavior

- Runs 10 valid reaction trials
- Uses a random 1-4 second preparation delay
- Detects false starts during the preparation window and repeats the current trial
- Logs per-trial reaction time and ISR wake latency
- Logs average, best, worst, and false-start count at the end

## Build and Flash

```powershell
idf.py set-target esp32
idf.py build
idf.py flash monitor
```
