#include <LiquidCrystal.h>

// Pin definitions
#define BUTTON_PIN 10
#define TWIST_PIN A0
#define TILT_PIN 9
#define PIEZO_PIN 13

// Initialize lcd pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(11,12,5,4,3,2); 

// Game variables
int mode; // 0=startup, 1=bop it, 2=twist it, 3=tilt it, 4=game over
int previous_mode = -1; // Tracks previous mode to detect changes

// Timer variables
unsigned long prev_time = 0;
unsigned long current_time;
unsigned long delta_time; 
int t0 = 5000; // Initial timer value (5 seconds)
int time = t0;

// Input states
int button = 0;
float twist = 0, twist_start = -1.0;
int tilt = 0;

// Score tracking
int score = 0;
int high_score = 0;
// Pause timer for game over animations
int pause_ms = 0;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A1)); // Seed random (A1 pin is floating)
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(PIEZO_PIN, OUTPUT); // Piezo buzzer as output
  mode = 0; // Start at startup mode
}


void loop() {
  // Track elapsed time (milliseconds)
  current_time = millis();
  delta_time = current_time - prev_time;
  prev_time = current_time;

  // Decrease timer in active mode
  if (mode != 0 && mode != 4) time -= delta_time;

  // If time runs out, go to game over
  if (time < 0) {
    game_over();
  }

  // Read inputs
  button = digitalRead(BUTTON_PIN); // button: 1 on, 0 off
  twist = analogRead(TWIST_PIN)*5.0/1023.0; // potentiometer: 0.0-5.0V
  if (twist_start == -1.0) {
    twist_start = twist; // Save starting twist position
  }
  tilt = digitalRead(TILT_PIN); // tilt sensor: 0 tilted, 1 flat

  // Handle button press
  if (button) {
    if (mode == 0 || mode == 4) { // starting new game
      mode = 1; // always start with "bop it"
      time = t0;
      button = 0;
      lcd.clear();
      score = 0;
    }
    if (time < t0-250) { // make sure button isn't triggered twice within 250ms ("debouncing")
      if (mode == 1) { // successful "bop it"
        input_success(mode);
      } else { // button pressed during wrong mode
        game_over();
      }
    }
    button = 0; // clear button state
  }

  if (abs(twist - twist_start) > 2) { // Detect significant change (potentiometer changes by >2V, or more than 2/5th of turn)
    if (mode == 0 || mode == 4) { // starting new game
      correct_sound(2); // play happy sound just for fun/testing, will still wait for button press
    } else if (mode == 2) { // successful "twist it"
      input_success(mode);
    } else { // twisted during wrong mode
      game_over();
    }
    twist_start = -1.0; // reset twister
  }

  if (tilt==0) {
    if (mode == 0 || mode == 4) { // starting new game
      Serial.println("tilted!");
    }
    if (time < t0-500) { // make sure tilt isn't triggered twice within 0.5 second (give player time to reset, avoid "bouncing")
      if (mode == 3) { // successful "tilt it"
        input_success(mode);
      } 
      // No penalty if tilted during wrong mode (tilt sensor is too sensitive)
    }
    tilt = 1; // Clear tilt state
  }

  // clear screen when mode changes
  if (mode != previous_mode) { 
    lcd.clear();
    previous_mode = mode;
  }

  // display the game on LCD
  print_mode(mode, time, t0, delta_time); 
  
}

// Prints the progress bar for time remaining
void print_progress_bar(int time, int t0) {
    lcd.setCursor(0,1);
    int bars = time / (float)t0 * 16;
    for (int i=0; i<bars; i++) {
      lcd.print('#');
    }
    for (int i=bars; i<16; i++) {
      lcd.print(' ');
    }
}

// Displays the current mode (gameplay and game over screens)
void print_mode(int mode, int time, int t0, int delta_time) {
  if (mode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Press button    ");
    lcd.setCursor(0, 1); // column 0, line 1
    lcd.print("to start Bop It!");
  } else if (mode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("-> Bop it!      ");
    print_progress_bar(time, t0);
  } else if (mode == 2) {
    lcd.setCursor(0, 0);
    lcd.print("-> Twist it!    ");
    print_progress_bar(time, t0);
  } else if (mode == 3) {
    lcd.setCursor(0, 0);
    lcd.print("-> Tilt it!");
    print_progress_bar(time, t0);
  } else if (mode == 4) {
    if (pause_ms < 1500) {
      lcd.print("Game Over!      ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("Score: ");
      lcd.print(score);
      int num_digits = (score == 0) ? 1 : (int)log10(score) + 1;
      for (int i = 0; i < 16 - (7 + num_digits); i++) {
        lcd.print(' '); // extra spaces to overwrite old content
      }
    } else if (pause_ms < 3000) {
      lcd.setCursor(0, 0);
      lcd.print("High Score:     ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("       ");
      lcd.print(high_score);
      int num_digits = (score == 0) ? 1 : (int)log10(score) + 1;
      for (int i = 0; i < 16 - (7 + num_digits); i++) {
        lcd.print(' '); // extra spaces to overwrite old content
      }
    } else if (pause_ms < 4500) {
      lcd.setCursor(0, 0);
      lcd.print("Press button    ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("to restart!     ");
      lcd.setCursor(0, 0);
    } else {
      pause_ms = 0; // Cycle back to beginning
    }
    pause_ms += delta_time;
  }
}

// Plays a sound for correct input
void correct_sound(int success_mode) {
  if (success_mode == 1) {
    tone(PIEZO_PIN, 880, 80);    // A5
    delay(90);
    noTone(PIEZO_PIN);
    delay(30);
    tone(PIEZO_PIN, 880, 80);    // A5
    delay(60);
    noTone(PIEZO_PIN);
  } 
  else if (success_mode == 2) {
    tone(PIEZO_PIN, 1000, 50);
    delay(60);
    tone(PIEZO_PIN, 800, 50);
    delay(60);
    tone(PIEZO_PIN, 1000, 50);
    delay(60);
    noTone(PIEZO_PIN);
  } 
  else if (success_mode == 3) {
    for (int freq = 400; freq <= 900; freq += 10) {
      tone(PIEZO_PIN, freq);
      delay(5);
    }
    noTone(PIEZO_PIN);
  }
}

// Plays sad sound at game over
void game_over_sound()
{
  tone(PIEZO_PIN, 220, 400); // A3, low and slow
  delay(400);
  tone(PIEZO_PIN, 196, 400); // G3, a little lower
  delay(400);
  tone(PIEZO_PIN, 174, 600); // F3, even lower and longer
  delay(600);
  noTone(PIEZO_PIN);
}

// Handles successful input
void game_over() {
    pause_ms = 0;
    mode = 4; 
    game_over_sound(); // << play sad sound once
    pause_ms -= 1400; // account for delay by audio influencing delta_time
    t0 = 5000; // starting speed back to 5 sec
    time = t0;
    high_score = (score > high_score) ? score : high_score;
}

// Handles successful input
void input_success(int success_mode) {
    correct_sound(success_mode);
    mode = random(3) + 1; // Pick random mode 1-3
    if (t0 > 2000) t0 -= 250; // time per action decreases from 5 to 2 seconds
    time = t0; // Reset timer
    score++;
}
