#include <LiquidCrystal.h>

#define BUTTON_PIN 10

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
  mode = 0;
}

void loop() {
  button = digitalRead(BUTTON_PIN);
  // time (milliseconds)
  current_time = millis();
  delta_time = current_time - prev_time;
  prev_time = current_time;

  Serial.println(time);
  if (time < 0) mode = 4;
  else if (mode != 0 && mode != 4) time -= delta_time;

  if ((mode == 0 || mode == 4) && button) { // starting new game
    mode = random(3)+1;
    time = t0; // 5 seconds
    button = 0;
    lcd.clear();
  } else if (mode == 1) {

  } else if (mode == 2) {

  } else if (mode == 3) { 


  } else if (mode == 4) {

  }

  if (mode != previous_mode) {
    lcd.clear();
    previous_mode = mode;
  }
  print_mode(mode, time, t0);
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
