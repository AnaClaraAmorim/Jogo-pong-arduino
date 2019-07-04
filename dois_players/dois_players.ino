#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(4);

int resolution[2] = {128, 32}, ball[2] = {20, (resolution[1] / 2)};
const int PIXEL_SIZE = 2, WALL_WIDTH = 1, PADDLE_WIDTH = 2, BALL_SIZE = 2, SPEED = 6;
int playerScore = 0, player2score = 0, playerPos = 0, aiPos = 0, player2Pos = 0;
char ballDirectionHori = 'R', ballDirectionVerti = 'S';
boolean inProgress = true;

void setup()   {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void loop() {
  if (player2score > 2 || playerScore > 2) {
    // check game state
    inProgress = false;
  }

  if (inProgress) {
    eraseScore();
    eraseBall(ball[0], ball[1]);

    if (ballDirectionVerti == 'U') {
      // move ball up diagonally
      ball[1] = ball[1] - SPEED;
    }

    if (ballDirectionVerti == 'D') {
      // move ball down diagonally
      ball[1] = ball[1] + SPEED;
    }

    if (ball[1] <= 0) { 
    // bounce the ball off the top 
      ballDirectionVerti = 'D'; } 
      if (ball[1] >= resolution[1]) {
      // bounce the ball off the bottom
      ballDirectionVerti = 'U';
    }

    if (ballDirectionHori == 'R') {
      ball[0] = ball[0] + SPEED; // move ball
      if (ball[0] >= (resolution[0] - 6)) {
        // ball is at the P2 edge of the screen
        if ((player2Pos + 6) >= ball[1] && (player2Pos - 6) <= ball[1]) { 
        // ball hits P2 paddle 
          if (ball[1] > (player2Pos + 2)) {
            // deflect ball down
            ballDirectionVerti = 'D';
          }
          else if (ball[1] < (player2Pos - 2)) {
            // deflect ball up
            ballDirectionVerti = 'U';
          }
          else {
            // deflect ball straight
            ballDirectionVerti = 'S';
          }
          // change ball direction
          ballDirectionHori = 'L';
        }
        else {
          // GOAL!
          ball[0] = 6; // move ball to other side of screen
          ballDirectionVerti = 'S'; // reset ball to straight travel
          ball[1] = resolution[1] / 2; // move ball to middle of screen
          ++playerScore; // increase player score
        }
      }
    }

    if (ballDirectionHori == 'L') {
      ball[0] = ball[0] - SPEED; // move ball
      if (ball[0] <= 3) { // ball is at the player edge of the screen 
      if ((playerPos + 6) >= ball[1] && (playerPos - 6) <= ball[1]) { // ball hits player paddle 
        if (ball[1] > (playerPos + 2)) {
            // deflect ball down
            ballDirectionVerti = 'D';
          }
          else if (ball[1] < (playerPos - 2)) { // deflect ball up 
          ballDirectionVerti = 'U'; } else { // deflect ball straight 
          ballDirectionVerti = 'S'; } // change ball direction 
          ballDirectionHori = 'R'; } else { ball[0] = resolution[0] - 3; // move ball to other side of screen 
          ballDirectionVerti = 'S'; // reset ball to straight travel 
          ball[1] = resolution[1] / 2; // move ball to middle of screen 
          ++player2score; // increase P2 score 
          } } } 
          
          drawBall(ball[0], ball[1]); 
          erasePlayerPaddle(playerPos); 
          eraseP2Paddle(player2Pos);
          playerPos = analogRead(A2);
          player2Pos = analogRead(A3); 
          // read player potentiometer 
          playerPos = map(playerPos, 0, 1023, 4, 28); // convert value from 0 - 1023 to 8 - 54 
          player2Pos = map(player2Pos, 0, 1023, 4, 28); // convert value from 0 - 1023 to 8 - 54 
          drawPlayerPaddle(playerPos);
          drawP2Paddle(player2Pos); 
          drawNet(); 
          drawScore(); } else { 
          // somebody has won 
          display.clearDisplay(); 
          display.setTextSize(1); 
          display.setTextColor(WHITE); 
          display.setCursor(0, 0); 
          // figure out who 
          if (player2score > playerScore) {
      display.println("Player 2 vence!");
    }
    else if (playerScore > player2score) {
      display.println("Player 1 vence!");
    }
  }

  display.display();
}


void drawScore() {
  // draw P2 and player scores
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(45, 0);
  display.println(playerScore);

  display.setCursor(75, 0);
  display.println(player2score);
}

void eraseScore() {
  // erase P2 and player scores
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(45, 0);
  display.println(playerScore);

  display.setCursor(75, 0);
  display.println(player2score);
}

void drawNet() {
  for (int i = 0; i < (resolution[1] / WALL_WIDTH); ++i) {
    drawPixel(((resolution[0] / 2) - 1), i * (WALL_WIDTH) + (WALL_WIDTH * i), WALL_WIDTH);
  }
}

void drawPixel(int posX, int posY, int dimensions) {
  // draw group of pixels
  for (int x = 0; x < dimensions; ++x) {
    for (int y = 0; y < dimensions; ++y) {
      display.drawPixel((posX + x), (posY + y), WHITE);
    }
  }
}

void erasePixel(int posX, int posY, int dimensions) {
  // erase group of pixels
  for (int x = 0; x < dimensions; ++x) {
    for (int y = 0; y < dimensions; ++y) {
      display.drawPixel((posX + x), (posY + y), BLACK);
    }
  }
}

void erasePlayerPaddle(int row) {
  erasePixel(0, row - (PADDLE_WIDTH), PADDLE_WIDTH);
  erasePixel(0, row - PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(0, row, PADDLE_WIDTH);
  erasePixel(0, row + PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(0, row + (PADDLE_WIDTH), PADDLE_WIDTH);
}

void drawPlayerPaddle(int row) {
  drawPixel(0, row - (PADDLE_WIDTH), PADDLE_WIDTH);
  drawPixel(0, row - PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(0, row, PADDLE_WIDTH);
  drawPixel(0, row + PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(0, row + (PADDLE_WIDTH), PADDLE_WIDTH);
}

void drawP2Paddle(int row) {
  int column = resolution[0] - PADDLE_WIDTH;
  drawPixel(column, row - (PADDLE_WIDTH), PADDLE_WIDTH);
  drawPixel(column, row - PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(column, row, PADDLE_WIDTH);
  drawPixel(column, row + PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(column, row + (PADDLE_WIDTH), PADDLE_WIDTH);
}

void eraseP2Paddle(int row) {
  int column = resolution[0] - PADDLE_WIDTH;
  erasePixel(column, row - (PADDLE_WIDTH), PADDLE_WIDTH);
  erasePixel(column, row - PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(column, row, PADDLE_WIDTH);
  erasePixel(column, row + PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(column, row + (PADDLE_WIDTH), PADDLE_WIDTH);
}

void drawBall(int x, int y) {
  display.drawCircle(x, y, BALL_SIZE, WHITE);
}

void eraseBall(int x, int y) {
  display.drawCircle(x, y, BALL_SIZE, BLACK);
}
