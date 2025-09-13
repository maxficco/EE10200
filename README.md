# Arduino Bop It Game

## Overview
This project recreates a simplified version of the classic **Bop It!** game using an Arduino Uno and basic electronic components.  
Players must quickly respond to random commands—**Bop it (button press)**, **Twist it (potentiometer)**, or **Tilt it (tilt sensor)**—before the countdown timer expires. Each correct move scores points and speeds up the game, while mistakes or delays trigger a game-over sound.

## Features
- Randomized commands displayed on an **LCD screen**
- Three unique physical inputs:
  - Button → “Bop it!”
  - Potentiometer → “Twist it!”
  - Tilt sensor → “Tilt it!”
- **Piezo buzzer** feedback:
  - Distinct tones for each successful move
  - Custom game-over sequence
- Countdown timer that decreases after each round (from 5 seconds to 2 seconds) for increasing difficulty
- High-score tracking during each session

## Hardware Components
- Arduino Uno  
- LCD display (16x2)  
- Push button  
- Potentiometer  
- Tilt sensor  
- Piezo buzzer  
- Breadboard & jumper wires  

## How to Play
1. Power on the device – LCD displays: *“Press button to start Bop It!”*  
2. Perform the displayed command before the timer runs out.  
3. Each correct action plays a success sound and increases your score.  
4. Failure to act in time triggers game over and displays your score + high score.  
5. Press the button to restart anytime.  

## Code Highlights
- Written in **C++ for Arduino**  
- Uses the `LiquidCrystal` library to manage the display  
- Implements state-based game logic with multiple modes (startup, play, game over)  
- Progress bar on LCD to show remaining time visually  
- Input debouncing and sensitivity adjustments for fair play  

## Future Improvements
- More precise tilt sensor to improve reliability  
- External housing for durability and aesthetics  
- Expanded command set (e.g., voice prompts, additional sensors)  

## Authors
- **Turner Piercy**  
- **Max Ficco**

---
> EE10200 Final Project, Spring 2025

![Arduino & Breadboard](project.jpg)
