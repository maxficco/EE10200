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
float twist = 0, twist_start = -1.0;
int tilt = 0;

int score = 0;
int high_score = 0;
int pause_ms = 0; // for changing game_over screen

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A1)); // A1 pin is floating
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(PIEZO_PIN, OUTPUT); // Piezo
  mode = 0;
}


void loop() {
  // time (milliseconds)
  current_time = millis();
  delta_time = current_time - prev_time;
  prev_time = current_time;

  if (mode != 0 && mode != 4) time -= delta_time; // decrease timer

  if (time < 0) { // if time runs out, game over
    mode = 4; 
    game_over_sound(); // << play sad sound once
    t0 = 5000; // starting speed back to 5 sec
    time = t0;
    high_score = (score > high_score) ? score : high_score;
    score = 0;
  }


  // inputs
  button = digitalRead(BUTTON_PIN); // button: 1 on, 0 off
  twist = analogRead(TWIST_PIN)*5.0/123.0; // potentiometer: 0.0-5.0V
  if (twist_start == -1.0) {
    twist_start = twist; // Save starting twist position
  }
  tilt = digitalRead(TILT_PIN); // tilt sensor: 0 tilted, 1 flat

  if (button) {
    if (time < t0-250) { // make sure button isn't triggered twice within 250ms ("debouncing")
      if (mode == 0 || mode == 4) { // starting new game
        mode = 1; // always start with "bop it" ?
        time = t0; // 5 seconds
        button = 0;
        lcd.clear();
        pause_ms = 0;
      } else if (mode == 1) { // successful "bop it"
        input_success();
      } else { // button pressed during wrong mode
        game_over();
      }
    }
    button = 0; // Clear button state
  }

  if (abs(twist - twist_start) > 0.5) { // Detect significant change (potentiometer changes by >0.5V, or more than 1/10th)
    // (don't think we have to worry about "debouncing")
    if (mode == 0 || mode == 4) { // starting new game
      correct_sound(); // play happy sound just for fun/testing, will still wait for button press
    } else if (mode == 2) { // successful "twist it"
      input_success();
    } else { // twisted during wrong mode
      game_over();
    }
    twist_start = -1.0; // reset twister
  }

  if (tilt==0) {
    if (time < t0-1000) { // make sure tilt isn't triggered twice within 1 second (give player time to reset, avoid "bouncing")
      if (mode == 0 || mode == 4) { // starting new game
        correct_sound(); // play happy sound just for fun/testing, will still wait for button press
      } else if (mode == 3) { // successful "tilt it"
        input_success();
      } else { // button pressed during wrong mode
        game_over();
      }
    }
    tilt = 1; // Clear tilt state
  }

  if (mode != previous_mode) { // clear screen when mode changes
    lcd.clear();
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
    if (pause_ms < 1500) {
      lcd.setCursor(0, 0);
      lcd.print("Game Over!      ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("Score: ");
      lcd.print(score);
      int num_digits = (score == 0) ? 1 : (int)log10(score) + 1;
      for (int i = 0; i < 16 - (7 + num_digits); i++) {
        lcd.print(' '); // extra spaces to overwrite old content
      }
      lcd.print("Score: ");
    } else if (pause_ms < 3000) {
      lcd.setCursor(0, 0);
      lcd.print("High Score:     ");
      lcd.setCursor(0, 1); // column 0, line 1
      lcd.print("       ");
      lcd.print(score);
      int num_digits = (score == 0) ? 1 : (int)log10(score) + 1;
      for (int i = 0; i < 16 - (7 + num_digits); i++) {
        lcd.print(' '); // extra spaces to overwrite old content
      }
    } else if (pause_ms < 4500) {
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
  delay(50);
  tone(PIEZO_PIN, 1175, 80);   // D6
  delay(60);
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

void game_over() {
    mode = 4; 
    game_over_sound(); // << play sad sound once
    t0 = 5000; // starting speed back to 5 sec
    time = t0;
    high_score = (score > high_score) ? score : high_score;
    score = 0;
}

void input_success() {
    correct_sound();
    delay(250); // Short delay after correct
    mode = random(3) + 1; // Pick random mode 1-3
    if (t0 > 2000) t0 -= 250; // time per action decreases from 5 to 2 seconds
    time = t0; // Reset timer
    score++;
}
