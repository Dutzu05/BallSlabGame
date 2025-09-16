#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
const int potPin = A0;
const int showScoreButton = 2;
const int stopButton = 3;
const int startButton = 4;
const int configButton = 5;
const int upButton = 6;
const int downButton = 7;
const int selectButton = 8;

// Game states
enum State { START_MENU, CONFIG_MENU, GAME_RUNNING, SHOW_SCORE };
State gameState = START_MENU;

int difficulty = 1;
int selectedOption = 0;

// Game vars
int slabX;
int slabWidth = 30;
int ballX, ballY;
bool ballActive = true;
unsigned long lastBallTime = 0;
unsigned long ballInterval = 30;

int caught = 0;
int missed = 0;

void setup() {
  Serial.begin(9600);
  pinMode(showScoreButton, INPUT_PULLUP);
  pinMode(stopButton, INPUT_PULLUP);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(configButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Press START");
  display.display();
}

void resetGame() {
  caught = 0;
  missed = 0;
  ballY = 0;
  ballX = random(0, SCREEN_WIDTH);
  ballActive = true;
  lastBallTime = millis();
}

void drawSlab() {
  slabX = map(analogRead(potPin), 0, 1023, 0, SCREEN_WIDTH - slabWidth);
  display.fillRect(slabX, SCREEN_HEIGHT - 5, slabWidth, 3, SSD1306_WHITE);
}

void drawBall() {
  display.fillRect(ballX, ballY, 3, 3, SSD1306_WHITE);
}

void updateBall() {
  if (millis() - lastBallTime > (100 - difficulty * 25)) {
    lastBallTime = millis();
    ballY += 3;

    if (ballY >= SCREEN_HEIGHT - 5) {
      if (ballX + 3 >= slabX && ballX <= slabX + slabWidth) {
        caught++;
      } else {
        missed++;
      }
      ballY = 0;
      ballX = random(0, SCREEN_WIDTH);
    }
  }
}

void drawScore() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Caught: "); display.println(caught);
  display.print("Missed: "); display.println(missed);
  display.display();
}

void drawMenu() {
  display.clearDisplay();
  display.setCursor(10, 0);
  display.println("Game Difficulty");
  for (int i = 1; i <= 3; i++) {
    if (selectedOption == i - 1) display.print("> ");
    else display.print("  ");
    display.print("Level ");
    display.println(i);
  }
  display.display();
}

void loop() {
  switch(gameState) {
    case START_MENU:
      if (digitalRead(startButton) == LOW) {
        resetGame();
        gameState = GAME_RUNNING;
        delay(200);
      } else if (digitalRead(configButton) == LOW) {
        gameState = CONFIG_MENU;
        delay(200);
      }
      break;

    case CONFIG_MENU:
      drawMenu();
      if (digitalRead(upButton) == LOW && selectedOption > 0) {
        selectedOption--;
        delay(200);
      } else if (digitalRead(downButton) == LOW && selectedOption < 2) {
        selectedOption++;
        delay(200);
      } else if (digitalRead(selectButton) == LOW) {
        difficulty = selectedOption + 1;
        gameState = START_MENU;
        delay(200);
      }
      break;

    case GAME_RUNNING:
      if (digitalRead(stopButton) == LOW) {
        gameState = START_MENU;
        delay(200);
        break;
      }
      if (digitalRead(showScoreButton) == LOW) {
        gameState = SHOW_SCORE;
        delay(200);
        break;
      }

      display.clearDisplay();
      drawSlab();
      if (ballActive) {
        drawBall();
        updateBall();
      }
      display.display();
      break;

    case SHOW_SCORE:
      drawScore();
      if (digitalRead(showScoreButton) == HIGH) {
        gameState = GAME_RUNNING;
        delay(200);
      }
      break;
  }
}
