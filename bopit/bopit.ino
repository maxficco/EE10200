#include <LiquidCrystal.h>

#define BUTTON_PIN 10
#define TWIST_PIN A0
#define TILT_PIN 9
#define PIEZO_PIN 13

LiquidCrystal lcd(12,11,5,4,3,2); // order is: RS, EN, D4, D5, D6, D7

int mode; // 0=startup, 1=bop it, 2=twist it, 3=tilt it, 4=game over
int previous_mode = -1; // so we know when to clear screen


unsigned long prev_time = 0;
unsigned long current_time;
unsigned long delta_time; 
int t0 = 5000;
int time = t0;

int button = 0;
int twist = 0, twist_side = 0;
int tilt = 0;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(PIEZO_PIN, OUTPUT); // Piezo
  mode = 0;
}


void loop() {
  button = digitalRead(BUTTON_PIN);
  // time (milliseconds)
  current_time = millis();
  delta_time = current_time - prev_time;
  prev_time = current_time;

  Serial.println(time);
  if (time < 0) mode = 4; // if time runs out, game over
  else if (mode != 0 && mode != 4) time -= delta_time; //increment timer

  if ((mode == 0 || mode == 4) && button) { // starting new game
    // mode = random(3)+1;
    mode = 2;
    time = t0; // 5 seconds
    button = 0;
    lcd.clear();

  } else if (mode == 1) { // Bop it
    if (button && time < t0-250) { // Button was pressed
      correct_sound();
      delay(250); // Short delay after correct
      mode = random(3) + 1; // Pick random mode 1-3
      time = t0; // Reset timer
      button = 0; // Clear button state
    }
  } else if (mode == 2) { // Twist it
    if (twist_side == 0) {
      twist_side = analogRead(TWIST_PIN); // Save starting twist position
    }
    twist = analogRead(TWIST_PIN);
    if (abs(twist - twist_side) > 50) { // Detect significant change
      correct_sound();
      delay(250);
      mode = random(3) + 1;
      time = t0;
      twist_side = 0;
    }
  } else if (mode == 3) {  // Tilt it
    tilt = digitalRead(TILT_PIN);  // Read the tilt sensor
    // Serial.print("Tilt: ");
    // Serial.println(tilt);
    if (tilt == HIGH) {  // If tilt is detected (HIGH could mean tilted)
        correct_sound();
        delay(250);
        mode = random(3) + 1;  // Pick random mode (1-3)
        time = t0;  // Reset timer
    }
  } else if (mode == 4) { // Game over

  }

  if (mode != previous_mode) { // clear screen when mode changes
    lcd.clear();
    if (mode == 4) {
      game_over_sound(); // << play sad sound once
    }
    previous_mode = mode;
  }

  print_mode(mode, time, t0); // display the game on LCD
  
}

void print_progress_bar(int time, int t0) {
    int bars = time / (float)t0 * 16;
    Serial.println(bars);
    for (int i=0; i<bars; i++) {
      lcd.setCursor(i,1);
      lcd.print("#");
    }
    for (int i=bars; i<16; i++) {
      lcd.setCursor(i,1);
      lcd.print(" ");
    }
}

void print_mode(int mode, int time, int t0) {
  if (mode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Press button");
    lcd.setCursor(0, 1); // column 0, line 1
    lcd.print("to start Bop It!");
  } else if (mode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("-> Bop it!");
    print_progress_bar(time, t0);
  } else if (mode == 2) {
    lcd.setCursor(0, 0);
    lcd.print("-> Twist it!");
    print_progress_bar(time, t0);
  } else if (mode == 3) {
    lcd.setCursor(0, 0);
    lcd.print("-> Tilt it!");
    print_progress_bar(time, t0);
  } else if (mode == 4) {
    static int pause_ms = 0;
    if (pause_ms < 2000) {
      lcd.setCursor(0, 0);
      lcd.print("Game Over!      ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("Score:          ");
    } else if (pause_ms < 4000) {
      lcd.setCursor(0, 0);
      lcd.print("Press button    ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("to restart!     ");
    } else {
      pause_ms = 0;
    }
    pause_ms += delta_time;
  }
}


void correct_sound()
{
  tone(PIEZO_PIN, 880, 80);    // A5
  delay(90);
  tone(PIEZO_PIN, 1175, 80);   // D6
  delay(90);
  tone(PIEZO_PIN, 1568, 80);   // G6
  delay(90);
  noTone(PIEZO_PIN);
}


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