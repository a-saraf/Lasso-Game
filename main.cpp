#include <simplecpp>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
using namespace simplecpp;

//movingObject.h
#ifndef _MOVINGOBJECT_INCLUDED_
#define _MOVINGOBJECT_INCLUDED_

#include <simplecpp>
#include <vector>
#include <composite.h>
#include <sprite.h>

using namespace simplecpp;

int j = 0; // for changing colors
int i = 1; // counts the no. of coins

class MovingObject : public Sprite {
  vector<Sprite*> parts;
  double vx, vy;
  double ax, ay;
  bool paused;

  void initMO(double argvx, double argvy, double argax, double argay, bool argpaused=true) {
    vx=argvx;
    vy=argvy;
    ax=argax;
    ay=argay;
    paused=argpaused;
  }
 public:
 MovingObject(double argvx, double argvy, double argax, double argay, bool argpaused=true)
    : Sprite() {
    initMO(argvx, argvy, argax, argay, argpaused);
  }
 MovingObject(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : Sprite() {
   double angle_rad = angle_deg*PI/180.0;
   double argvx = speed*cos(angle_rad);
   double argvy = -speed*sin(angle_rad);
   initMO(argvx, argvy, argax, argay, argpaused);
  }
  void set_vx(double argvx) { vx = argvx; }
  void set_vy(double argvy) { vy = argvy; }
  void set_ax(double argax) { ax = argax; }
  void set_ay(double argay) { ay = argay; }
  double getXPos();
  double getYPos();
  void reset_all(double argx, double argy, double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta);

  void pause() { paused = true; }
  void unpause() { paused = false; }
  bool isPaused() { return paused; }

  void addPart(Sprite* p) {
    parts.push_back(p);
  }
  void nextStep(double t);
  void getAttachedTo(MovingObject *m);
};

#endif

//MovingObject.cpp

void MovingObject::nextStep(double t) {
  if(paused) { return; }
  //cerr << "x=" << getXPos() << ",y=" << getYPos() << endl;
  //cerr << "vx=" << vx << ",vy=" << vy << endl;
  //cerr << "ax=" << ax << ",ay=" << ay << endl;

  for(size_t i=0; i<parts.size(); i++){
    parts[i]->move(vx*t, vy*t);
  }
  vx += ax*t;
  vy += ay*t;
} // End MovingObject::nextStep()

double MovingObject::getXPos() {
  return (parts.size() > 0) ? parts[0]->getX() : -1;
}

double MovingObject::getYPos() {
  return (parts.size() > 0) ? parts[0]->getY() : -1;
}

void MovingObject::reset_all(double argx, double argy, double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) {
  for(size_t i=0; i<parts.size(); i++){
    parts[i]->moveTo(argx, argy);
  }
  double angle_rad = angle_deg*PI/180.0;
  double argvx = speed*cos(angle_rad);
  double argvy = -speed*sin(angle_rad);
  vx = argvx; vy = argvy; ax = argax; ay = argay; paused = argpaused;
} // End MovingObject::reset_all()

void MovingObject::getAttachedTo(MovingObject *m) {
  double xpos = m->getXPos();
  double ypos = m->getYPos();
  for(size_t i=0; i<parts.size(); i++){
    parts[i]->moveTo(xpos, ypos);
  }
  initMO(m->vx, m->vy, m->ax, m->ay, m->paused);
}

//coin.h
#ifndef __COIN_H__
#define __COIN_H__



class Coin : public MovingObject {
  double coin_start_x;
  double coin_start_y;
  double release_speed;
  double release_angle_deg;
  double coin_ax;
  double coin_ay;
  int color_code[5][3];
  double* angle_add;

  // Moving parts
  public:
  Circle coin_circle;

 public:
 Coin(double speed, double &angle_deg, double argax, double argay, bool argpaused, bool rtheta, int color[][3]) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta) {
    release_speed = speed;
    release_angle_deg = angle_deg;
    coin_ax = argax;
    coin_ay = argay;
    angle_add = &angle_deg;
    for(int i = 0; i<5; i++){
        for(int j = 0; j<3; j++) color_code[i][j] = color[i][j];
    }
    initCoin();
  }

  void initCoin();
  void resetCoin();

}; // End class Coin

#endif

//lasso.h
#ifndef __LASSO_H__
#define __LASSO_H__

//#define WINDOW_X 1200
//#define WINDOW_Y 960

#define WINDOW_X 800
#define WINDOW_Y 600

#define STEP_TIME 0.05

#define PLAY_X_START 100
#define PLAY_Y_START 0
#define PLAY_X_WIDTH (WINDOW_X-PLAY_X_START)
#define PLAY_Y_HEIGHT (WINDOW_Y-100)

#define LASSO_X_OFFSET 100
#define LASSO_Y_HEIGHT 100
#define LASSO_BAND_LENGTH LASSO_X_OFFSET
#define LASSO_THICKNESS 5

#define COIN_GAP 1

#define RELEASE_ANGLE_STEP_DEG 5
#define MIN_RELEASE_ANGLE_DEG 0
#define MAX_RELEASE_ANGLE_DEG (360-RELEASE_ANGLE_STEP_DEG)
#define INIT_RELEASE_ANGLE_DEG 45

#define RELEASE_SPEED_STEP 20
#define MIN_RELEASE_SPEED 0
#define MAX_RELEASE_SPEED 200
#define INIT_RELEASE_SPEED 200

#define COIN_SPEED 150
#define COIN_ANGLE_DEG 90

#define LASSO_G 30
#define COIN_G 30

#define LASSO_SIZE 10
#define LASSO_RADIUS 50
#define COIN_SIZE 5

struct Lasso : public MovingObject {
  double lasso_start_x;
  double lasso_start_y;
  double release_speed;
  double release_angle_deg;
  double lasso_ax;
  double lasso_ay;

  // Moving parts
  Circle lasso_circle;
  Circle lasso_loop;

  // Non-moving parts
  Line lasso_line;
  Line lasso_band;

  // State info
  bool lasso_looped;
  Coin *the_coin;
  int num_coins;
  int tot_coins;

  void initLasso();
 public:
 Lasso(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta) {
    release_speed = speed;
    release_angle_deg = angle_deg;
    lasso_ax = argax;
    lasso_ay = argay;
    initLasso();
  }

  void draw_lasso_band();
  void yank();
  void loopit();
  void addAngle(double angle_deg);
  void addSpeed(double speed);

  void nextStep(double t);
  void check_for_coin(Coin *coin);
  int getNumCoins() { return num_coins; }
  int getTotalCoins() {return tot_coins; }

}; // End class Lasso

#endif

//coin.h

void Coin::initCoin() {
  coin_start_x = (-PLAY_X_START+2*WINDOW_X)/2;
  coin_start_y = PLAY_Y_HEIGHT;
  coin_circle.reset(coin_start_x, coin_start_y, COIN_SIZE);
  coin_circle.setColor(COLOR(color_code[j][0], color_code[j][1], color_code[j][2]));
  coin_circle.setFill(true);
  addPart(&coin_circle);
}

void Coin::resetCoin() {
  i++;
  double coin_speed = COIN_SPEED;
  double *add_temp;
  add_temp = angle_add + i%10;
  double coin_angle_deg;
  coin_angle_deg = *add_temp;
  coin_ax = 0;
  coin_ay = COIN_G;
  bool paused = true,
  rtheta = true;

  if(i%9 == 0) j = 1;
  else if(i%7 == 0) j = 3;
  else if(i%5 == 0) j = 2;
  else if(i%3 == 0) j = 4;
  else j = 0;

  reset_all(coin_start_x, coin_start_y, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin_circle.setColor(COLOR(color_code[j][0], color_code[j][1], color_code[j][2]));
}

//lasso.cpp

void Lasso::draw_lasso_band() {
  double len = (release_speed/MAX_RELEASE_SPEED)*LASSO_BAND_LENGTH;
  double arad = release_angle_deg*PI/180.0;
  double xlen = len*cos(arad);
  double ylen = len*sin(arad);
  lasso_band.reset(lasso_start_x, lasso_start_y, (lasso_start_x-xlen), (lasso_start_y+ylen));
  lasso_band.setThickness(LASSO_THICKNESS);
}
// End Lasso::draw_lasso_band()

void Lasso::initLasso() {
  lasso_start_x = (PLAY_X_START+LASSO_X_OFFSET);
  lasso_start_y = (PLAY_Y_HEIGHT-LASSO_Y_HEIGHT);
  lasso_circle.reset(lasso_start_x, lasso_start_y, LASSO_SIZE);
  lasso_circle.setColor(COLOR(102, 34, 0));
  lasso_circle.setFill(true);
  lasso_loop.reset(lasso_start_x, lasso_start_y, LASSO_SIZE/2);
  lasso_loop.setColor(COLOR(204, 204, 204));
  lasso_loop.setFill(true);
  addPart(&lasso_circle);
  addPart(&lasso_loop);
  lasso_looped = false;
  the_coin = NULL;
  num_coins = 0;
  tot_coins = 0;

  lasso_line.reset(lasso_start_x, lasso_start_y, lasso_start_x, lasso_start_y);
  lasso_line.setColor(COLOR(102, 34, 0));

  lasso_band.setColor(COLOR(102, 34, 0));
  draw_lasso_band();

}
// End Lasso::initLasso()

void Lasso::yank() {
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
  lasso_loop.reset(lasso_start_x, lasso_start_y, LASSO_SIZE/2);
  lasso_loop.setFill(true);
  lasso_looped = false;
  tot_coins++;

  if(the_coin != NULL) {
    if(i%9 == 0){
        tot_coins -= 2;
    }
    else if(i%7 == 0){
        num_coins += 5;
        tot_coins += 4;
    }
    else if(i%5 == 0) tot_coins += 1000;
    else if(i%3 == 0){
        num_coins += 2;
        tot_coins++;
    }
    else num_coins++;
    the_coin->resetCoin();
    the_coin = NULL;
  }
} // End Lasso::yank()

void Lasso::loopit() {
  if(lasso_looped) { return; } // Already looped
  lasso_loop.reset(getXPos(), getYPos(), LASSO_RADIUS);
  lasso_loop.setFill(false);
  lasso_looped = true;
} // End Lasso::loopit()

void Lasso::addAngle(double angle_deg) {
  release_angle_deg += angle_deg;
  if(release_angle_deg < MIN_RELEASE_ANGLE_DEG) { release_angle_deg = MIN_RELEASE_ANGLE_DEG; }
  if(release_angle_deg > MAX_RELEASE_ANGLE_DEG) { release_angle_deg = MAX_RELEASE_ANGLE_DEG; }
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
} // End Lasso::addAngle()

void Lasso::addSpeed(double speed) {
  release_speed += speed;
  if(release_speed < MIN_RELEASE_SPEED) { release_speed = MIN_RELEASE_SPEED; }
  if(release_speed > MAX_RELEASE_SPEED) { release_speed = MAX_RELEASE_SPEED; }
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
} // End Lasso::addSpeed()

void Lasso::nextStep(double stepTime) {
  draw_lasso_band();
  MovingObject::nextStep(stepTime);
  if(getYPos() > PLAY_Y_HEIGHT) { yank(); }
  lasso_line.reset(lasso_start_x, lasso_start_y, getXPos(), getYPos());
} // End Lasso::nextStep()

void Lasso::check_for_coin(Coin *coinPtr) {
  double lasso_x = getXPos();
  double lasso_y = getYPos();
  double coin_x = coinPtr->getXPos();
  double coin_y = coinPtr->getYPos();
  double xdiff = (lasso_x - coin_x);
  double ydiff = (lasso_y - coin_y);
  double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
  if(distance <= LASSO_RADIUS) {
    the_coin = coinPtr;
    the_coin->getAttachedTo(this);
  }
} // End Lasso::check_for_coin()


main_program {

  initCanvas("Lasso", WINDOW_X, WINDOW_Y);
  int stepCount = 0;
  float stepTime = STEP_TIME;
  float runTime = -1; // sec; -ve means infinite
  float currTime = 0;
  bool highscore_tab = false;

  while(true){
  double release_speed = INIT_RELEASE_SPEED; // m/s
  double release_angle_deg = INIT_RELEASE_ANGLE_DEG; // degrees
  double lasso_ax = 0;
  double lasso_ay = LASSO_G;
  bool paused = true;
  bool rtheta = true;

  int color_coin[5][3] = {{255, 173, 51},{230, 46, 0}, {77, 77, 77}, {51, 51, 255}, {102, 255, 153}};
  /* 1st coin gold common, 1pt
     2nd coin red, gives an extra life
     3rd coin black, ends the game
     4th coin blue, gives 5pts
     5th coin green, gives 2 points
  */

  // main menu
  while(true){
    string headerstr = "LASSO GAME - INTRODUCTION";
    Text header(400, 60, headerstr);
    Line underline1(395 - textWidth(headerstr)/2, 60+ textHeight()/3*2, 405 + textWidth(headerstr)/2, 60+ textHeight()/3*2);
    Line underline2(400 - textWidth(headerstr)/2, 60+ textHeight(), 400 + textWidth(headerstr)/2, 60+ textHeight());

    Text name(400, 95, "By: Anmol Saraf 200070007");
    name.setColor(COLOR(0, 134, 179));

    //instructions
    Text main(400, 125, "A cannon launches coins and we have to collect them using a lasso.");
    Text main2(400, 150, "Not able to catch a coin after throwing results in a lose of a life.");
    Text control_t(400, 175,"Press 't' to throw the lasso.");
    Text control_y(400, 200,"Press 'y' to yank the lasso towards you.");
    Text control_l(400, 225,"Press 'l' to open the lasso's loop and catch the coin.");
    Text control_neg(400, 250,"Press '-' to decrease the lasso's speed.");
    Text control_equal(400, 275,"Press '=' to increase the lasso's speed.");
    Text control_left(400, 300,"Press '[' to rotate the lasso's mouth clockwise.");
    Text control_right(400, 325,"Press ']' to rotate the lasso's mouth anti-clockwise.");

    Circle goldenc(300, 375, 5);
    Circle greenc(290, 400, 5);
    Circle bluec(310,425, 5);
    Circle blackc(285, 450, 5);
    Circle redc(280, 475, 5);

    goldenc.setColor(COLOR(255, 173, 51));
    greenc.setColor(COLOR(102, 255, 153));
    bluec.setColor(COLOR(51, 51, 255));
    blackc.setColor(COLOR(77, 77, 77));
    redc.setColor(COLOR(230, 46, 0));

    goldenc.setFill(true);
    greenc.setFill(true);
    bluec.setFill(true);
    blackc.setFill(true);
    redc.setFill(true);

    Text golden(400, 375,"COMMON: gives 1 point");
    Text green(400, 400,"UNCOMMON: gives 2 points");
    Text blue(400, 425,"RARE: gives 5 points");
    Text black(400, 450, "DANGER: instantly kills you");
    Text red(400, 475, "LEGENDARY: gives an extra life");
    // end instructions

    Rectangle playgame(470, 550, 100, 40);
    Text playgametxt(470, 550, "PLAY GAME");
    // button to play
    Rectangle highScores(330, 550, 100, 40);
    Text highscorestxt(330, 550,"HIGHSCORES");
    // button to see highscore window
    highScores.setColor(COLOR(153, 102, 255));
    highscorestxt.setColor(COLOR(153, 102, 255)); // setting colors for the buttons
    playgame.setColor(COLOR(0, 153, 51));
    playgametxt.setColor(COLOR(0, 153, 51));

    int click = getClick();
    int clickx = click/65536;
    int clicky = click%65536;
    // highscore button condition

    if(clickx <= 380 && clickx >= 280 && clicky <= 570 && clicky >= 530){
        highscore_tab = true;
        break;
    }

    // play game button clicking condition
    if(clickx <= 520 && clickx >= 420 && clicky <= 570 && clicky >= 530){
        break;
    }
    else continue;
}
  // end of main menu
  // highscore menu
  while(highscore_tab){
    Text leaderboard(400, 150, "** LEADERBOARD **");
    string txtname = "** LEADERBOARD **";
    Text click_anywhere2(400, 470, "-- CLICK ANYWHERE TO EXIT --");
    click_anywhere2.setColor(COLOR(204, 204, 204));
    int width_txt = textWidth(txtname);
    Line underline1(400 - width_txt/2, 160, 400 + width_txt/2, 160);
    ifstream input("highscores.txt");
    int arr[5];
    for(int i = 0; i<5; i++) input >> arr[i];

    Text num1(350, 200, "1.");
    Text num2(350, 220, "2.");
    Text num3(350, 240, "3.");
    Text num4(350, 260, "4.");
    Text num5(350, 280, "5.");

    Text rank1(450, 200, arr[0]);
    Text rank2(450, 220, arr[1]);
    Text rank3(450, 240, arr[2]);
    Text rank4(450, 260, arr[3]);
    Text rank5(450, 280, arr[4]);

    Rectangle playgamebox(400, 400, 100, 40);
    Text play(400, 400, "PLAY GAME");
    playgamebox.setColor(COLOR(0, 153, 51));
    play.setColor(COLOR(0, 153, 51));

    int a = getClick();
    int click_ax = a/65536;
    int click_ay = a%65536;

    if(click_ax >= 350 && click_ax <= 450){
        if(click_ay >= 380 && click_ay <= 420){
            highscore_tab = false;

        }
    }
    else exit(0);

  }

  // end of highscore

  // Draw lasso at start position
  Lasso lasso(release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);

  Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
  b1.setColor(COLOR(0,0,0));
  Line b2(PLAY_X_START, 0, PLAY_X_START, WINDOW_Y);
  b2.setColor(COLOR(0,0,0));

  string msg("Cmd: _");
  Text charPressed(PLAY_X_START+50, PLAY_Y_HEIGHT+20, msg);
  char coinScoreStr[256];
  char lives[256];
  int lives_left;
  sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
  Text coinScore(PLAY_X_START+50, PLAY_Y_HEIGHT+50, coinScoreStr);
  sprintf(lives, "Lives Left: %d", lives_left);
  Text lives_display(PLAY_X_START+50, PLAY_Y_HEIGHT+80, lives);

  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg[10] = {150,120,90,110,150,95,110,135,95,110};
   // different angles which will get rotated with each reset of the ball
  double coin_ax = 0;
  double coin_ay = COIN_G;


  Rectangle cannon_head((-PLAY_X_START+2*WINDOW_X)/2, PLAY_Y_HEIGHT, 50,20);
  cannon_head.setFill(true);
  Circle cannon_mouth((-PLAY_X_START+2*WINDOW_X)/2, PLAY_Y_HEIGHT, 20);
  cannon_mouth.setFill(true);
  Circle cannon_mouth1((-PLAY_X_START+2*WINDOW_X)/2, PLAY_Y_HEIGHT, 15);
  cannon_mouth1.setColor(COLOR(255, 117, 26));
  Circle cannon_mouth2((-PLAY_X_START+2*WINDOW_X)/2, PLAY_Y_HEIGHT, 10);
  cannon_mouth2.setFill(true);

  Coin coin(coin_speed, coin_angle_deg[0], coin_ax, coin_ay, paused, rtheta, color_coin);
  // After every COIN_GAP sec, make the coin jump
  double last_coin_jump_end = 0;

  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When l is pressed, loop lasso
  // When q is pressed, quit

  for(;;) {
    if((runTime > 0) && (currTime > runTime)) { break; }

    XEvent e;
    bool pendingEv = checkEvent(e);
    if(pendingEv) {
      char c = charFromEvent(e);
      msg[msg.length()-1] = c;
      charPressed.setMessage(msg);
      switch(c) {
      case 't':
	lasso.unpause();
	break;
      case 'y':
	if(!lasso.isPaused()) { lasso.yank(); }
	break;
      case 'l':
	lasso.loopit();
	lasso.check_for_coin(&coin);
	wait(STEP_TIME*5);
	break;
      case '[':
	if(lasso.isPaused()) { lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);	}
	break;
      case ']':
	if(lasso.isPaused()) { lasso.addAngle(+RELEASE_ANGLE_STEP_DEG); }
	break;
      case '-':
	if(lasso.isPaused()) { lasso.addSpeed(-RELEASE_SPEED_STEP); }
	break;
      case '=':
	if(lasso.isPaused()) { lasso.addSpeed(+RELEASE_SPEED_STEP); }
	break;
      default:
	break;
      }
    }

    lasso.nextStep(stepTime);

    coin.nextStep(stepTime);
    if(coin.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin.unpause();
      }
    }

    if(coin.getYPos() > PLAY_Y_HEIGHT) {
      coin.resetCoin();
      last_coin_jump_end = currTime;
    }
    int lives_initial = 3;
    lives_left = lives_initial - lasso.getTotalCoins() + lasso.getNumCoins();
    sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
    coinScore.setMessage(coinScoreStr);

    if(lives_left <= 0){
        int score = lasso.getNumCoins();

        ifstream input("highscores.txt");
        int highscores_arr[5];
        for(int i = 0; i<5; i++) input >> highscores_arr[i];

        ofstream output("highscores.txt");
        if(score >= highscores_arr[4]){
            highscores_arr[4] = score;
            sort( highscores_arr,highscores_arr + 5, greater<int>());
        }

        for(int i = 0; i<5; i++) output << highscores_arr[i] << endl;
        break;
    }
    sprintf(lives, "Lives Left: %d", lives_left);
    lives_display.setMessage(lives);

    stepCount++;
    currTime += stepTime;
    wait(stepTime);
  } // End for(;;)
  // Ending the game screen by removing all game components
  lasso.lasso_circle.hide();
  lasso.lasso_loop.hide();
  lasso.lasso_line.hide();
  lasso.lasso_band.hide();
  coin.coin_circle.hide();
  b1.hide();
  b2.hide();
  charPressed.hide();
  lives_display.hide();
  coinScore.hide();
  cannon_head.hide();
  cannon_mouth.hide();
  cannon_mouth1.hide();
  cannon_mouth2.hide();
  // end of removal of game elements

  // start of the design of the page
  char end_total[256];
  sprintf(end_total, "Total Points: %d", lasso.getNumCoins());
  Text total_score_display(400, 275, end_total);
  Text game_over(400, 200, "GAME OVER");
  game_over.setColor(COLOR("red"));

  Text play_again(470, 370, "PLAY AGAIN");
  Rectangle box_play_again(470, 370, 100, 50);
  Text highscores(330, 370, "HIGHSCORES");
  Rectangle box_highscores(330, 370, 100, 50);
  highscores.setColor(COLOR(128, 179, 255));
  box_highscores.setColor(COLOR(128, 179, 255));
  play_again.setColor(COLOR(255, 102, 102));
  box_play_again.setColor(COLOR(255, 102, 102));
  Text click_anywhere(400, 450, "-- CLICK ANYWHERE TO EXIT --");
  click_anywhere.setColor(COLOR(204, 204, 204));
  // design of the page end
  int p = getClick();
  int px = p/65536;
  int py = p%65536;
  bool clicked = false;
  if(px >= 420 && px <= 520 && py >= 345 && py <= 395){
    lasso.num_coins = 0;
    lasso.tot_coins = -1;
    i = 1;
    continue;
  }

  else if(px >= 280 && px <= 380 && py >= 345 && py <= 395){
    // hiding all the game over window elements
    total_score_display.hide();
    game_over.hide();
    play_again.hide();
    box_play_again.hide();
    highscores.hide();
    box_highscores.hide();
    click_anywhere.hide();
    // end of play again menu

    //highscore leaderboard page
    Text leaderboard(400, 150, "** LEADERBOARD **");
    string txtname = "** LEADERBOARD **";
    int width_txt = textWidth(txtname);
    Line underline1(400 - width_txt/2, 160, 400 + width_txt/2, 160);
    ifstream input("highscores.txt");
    int arr[5];
    for(int i = 0; i<5; i++) input >> arr[i];

    Text num1(350, 200, "1.");
    Text num2(350, 220, "2.");
    Text num3(350, 240, "3.");
    Text num4(350, 260, "4.");
    Text num5(350, 280, "5.");

    Text rank1(450, 200, arr[0]);
    Text rank2(450, 220, arr[1]);
    Text rank3(450, 240, arr[2]);
    Text rank4(450, 260, arr[3]);
    Text rank5(450, 280, arr[4]);

    // end of highscore leaderboard
    click_anywhere.show();
    play_again.show();
    box_play_again.show();
    play_again.move(-70,0);
    box_play_again.move(-70,0);
    // play again button activated

    int click_highscore = getClick();
    int hsx = click_highscore/65536;
    int hsy = click_highscore%65536;

    if(hsx >= 350 && hsx <= 450 && hsy >= 345 && hsy>= 395){
        lasso.num_coins = 0;
        lasso.tot_coins = 0;
        clicked = true;
    }
    if(clicked) break;
  }
  else break;

  }
} // End main_program
