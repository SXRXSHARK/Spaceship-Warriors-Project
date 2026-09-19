#include "mbed.h"
#include "N5110.h"
#include "joystick.h"
#include "bitmaps.h"

BusOut SegDis(PB_15, PB_14, PB_12, PA_11, PA_12, PB_1, PB_2);

PwmOut buzzer(PA_15); 
 
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);

DigitalOut redLed(PB_0); 
DigitalOut blueLed(PA_4); 
DigitalOut greenLed(PA_1);

DigitalIn pauseButton(PD_2);

Joystick joystick(PC_0,PC_1);
DigitalIn SelectButton(PC_10, PullUp);

bool enemiesGenerated = false; 
int score = 0; 
int hexDis[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void updateScoreDisplay() {
    SegDis = hexDis[score];  
}

void onEnemyShot() {
    if (enemiesGenerated) {
        score = (score + 1) % 10; 
        updateScoreDisplay(); 
    }
}

void setup() {
    score = 0;
    SegDis = 0;
    updateScoreDisplay();
}

void fillRect(int x, int y, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            lcd.setPixel(x + i, y + j, 1); 
        }
    }
}

void displayBackground() {
    lcd.clear();
    for (int i = 0; i < 48; i++) {
        for (int j = 0; j < 84; j++) {
            if (spacebackground[i][j]) {
                lcd.setPixel(j, i, 1);
            }
        }
    }
 lcd.refresh();
}

void displayBitmap(const uint8_t bitmap[48][84]) {
    lcd.clear();
    for (int i = 0; i < 48; i++) {
        for (int j = 0; j < 84; j++) {
            if (bitmap[i][j]) {
                lcd.setPixel(j, i, 1);
            }
        }
    }
    lcd.refresh();
}

const uint8_t* images[] = {
    (const uint8_t*)spaceship,
    (const uint8_t*)spaceship1,
    (const uint8_t*)spaceship2
};

const uint8_t* images1[] = {
    (const uint8_t*)minispaceship,
    (const uint8_t*)minispaceship1,
    (const uint8_t*)minispaceship2
};
int currentImageIndex = 0;
bool spaceshipSelected = false;

const uint8_t* images2[] = {
   (const uint8_t*)gameover_restart,
   (const uint8_t*)gameover_exit

};

bool gameEnded = false;

int currentGameOverIndex = 0;

void handleSpaceshipSelection() {
    static int lastImageIndex = -1;
    int direction = joystick.get_direction();

    if (direction == E) {
        currentImageIndex = (currentImageIndex + 1) % 3; 
        ThisThread::sleep_for(200ms);
    } else if (direction == W) {
        currentImageIndex = (currentImageIndex - 1 + 3) % 3; 
        ThisThread::sleep_for(200ms);
    }

    if (currentImageIndex != lastImageIndex) {
        displayBitmap(*(const uint8_t(*)[48][84])images[currentImageIndex]);
        lastImageIndex = currentImageIndex;

        redLed = (currentImageIndex == 0);  
        blueLed = (currentImageIndex == 1); 
        greenLed = (currentImageIndex == 2);  
    }
}


void displayPressStartAnimation() {
    while (true) {
        displayBitmap(spaceship_warriors_start1);
        ThisThread::sleep_for(500ms);

        if (SelectButton.read() == 0) {
            lcd.clear();
            displayBitmap(select);
            lcd.refresh();
            ThisThread::sleep_for(3000ms);
            break;
        }

        displayBitmap(spaceship_warriors_start2);
        ThisThread::sleep_for(500ms);

        if (SelectButton.read() == 0) {
            lcd.clear();
            displayBitmap(select);
            lcd.refresh();
            ThisThread::sleep_for(3000ms);
            break;
        }
    }
}

void selectSpaceship() {
    while (!spaceshipSelected) {
        handleSpaceshipSelection();
        ThisThread::sleep_for(200ms);

      
        if (SelectButton.read() == 0) {
            spaceshipSelected = true;
            ThisThread::sleep_for(500ms);
            redLed = 0;
            greenLed = 0;
            blueLed = 0;
            displayBitmap(select_level);
            lcd.refresh();
            ThisThread::sleep_for(3000ms);
        }
    }
}

int x_pos = 40; 
int y_pos = 42; 

const int MAX_BULLETS = 5;
const int MAX_ENEMIES = 10;

int maxEnemiesAtOnce = 1;

struct Bullet {
    int x, y;
    bool active;
    uint8_t sprite[8][8]; 
};

struct Enemy {
    int x, y;
    bool active; 
    uint8_t sprite[8][8]; 

    Enemy() : x(0), y(0), active(false) { 
        memset(sprite, 0, sizeof(sprite)); 
    }

    Enemy(int x_pos, int y_pos) : x(x_pos), y(y_pos), active(true) {
        memset(sprite, 0, sizeof(sprite));
    }
};

Thread enemyThread;

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];

void initializeBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false; 
    }
}

const uint8_t* images3[] = {
    (const uint8_t*)easyLevel,
    (const uint8_t*)MediumLevel,
    (const uint8_t*)HardLevel,
};

int currentLevelIndex = 0;

int enemyCount = 0; 
int shooterspeed = 1;
int spaceshipSpeed = 1;


void setDifficulty(char level) {
    if (level == 'E') {
        maxEnemiesAtOnce = 1;
        shooterspeed = 1;
        spaceshipSpeed = 2;  
    } else if (level == 'M') {
        maxEnemiesAtOnce = 3;
        shooterspeed = 2;
        spaceshipSpeed = 3; 
    } else if (level == 'H') {
        maxEnemiesAtOnce = 5;
        shooterspeed = 3;
        spaceshipSpeed = 4;  
    }
}
bool roundComplete = true;  

void generateEnemies() {
    int activeEnemies = 0;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            activeEnemies++;
        }
    }

    if (activeEnemies == 0) {
        roundComplete = true;
    }

    if (roundComplete) {
        activeEnemies = 0;
        roundComplete = false;

        for (int i = 0; i < MAX_ENEMIES && activeEnemies < maxEnemiesAtOnce; i++) {
            if (!enemies[i].active) {
                bool positionValid = false;
                int newX = 0;
                int attempts = 0;

                while (!positionValid && attempts < 20) {
                    newX = rand() % (84 - 6);
                    positionValid = true;

                    for (int j = 0; j < MAX_ENEMIES; j++) {
                        if (enemies[j].active && abs(enemies[j].x - newX) < 7) {
                            positionValid = false;
                            break;
                        }
                    }
                    attempts++;
                }

                if (positionValid) {
                    enemies[i].x = newX;
                    enemies[i].y = 0;
                    enemies[i].active = true;
                    memcpy(enemies[i].sprite, alien, sizeof(alien));
                    enemiesGenerated = true;
                    activeEnemies++;
                }
            }
        }
    }
}

void playBuzzerTone(float frequency, float duration) {
    buzzer.period(1.0 / frequency);
    buzzer = 0.5f;
    ThisThread::sleep_for(std::chrono::milliseconds(int(duration * 1000)));
    buzzer = 0.0f;
}

void gameOverBuzzer() {
    playBuzzerTone(1000, 0.5);
    playBuzzerTone(800, 0.5);
    playBuzzerTone(600, 0.5);
    playBuzzerTone(400, 0.5);
    playBuzzerTone(200, 1.0);
}

void buzzerShoot() {
    buzzer.period(1.0 / 2000.0);
    buzzer = 0.5f;
    ThisThread::sleep_for(100ms);
    buzzer = 0.0f;
}

void RGBshoot(){
    for (int i = 0; i < 2; i++) {
        greenLed = 1;
        ThisThread::sleep_for(100ms);
        greenLed = 0;
    }
}

void displayBullet(int x, int y, const uint8_t sprite[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (sprite[i][j]) {
                lcd.setPixel(x + j, y + i, 1); 
            }
        }
    }
}

void fireBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x_pos + 2;  
            bullets[i].y = y_pos - 5;  
            bullets[i].active = true;

            switch (currentImageIndex) {
                case 0:
                    memcpy(bullets[i].sprite, minispaceship, sizeof(bullets[i].sprite));
                    break;
                case 1:
                    memcpy(bullets[i].sprite, minispaceship1, sizeof(bullets[i].sprite));
                    break;
                case 2:
                    memcpy(bullets[i].sprite, minispaceship2, sizeof(bullets[i].sprite));
                    break;
                default:
                    memcpy(bullets[i].sprite, minispaceship, sizeof(bullets[i].sprite)); 
                    break;
            }
            buzzerShoot(); 
            RGBshoot();

            return; 
        }
    }
}

void moveBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].y -= shooterspeed;  
            if (bullets[i].y < 0) {  
                bullets[i].active = false;
            }
        }
    }
}

void displayBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            displayBullet(bullets[i].x, bullets[i].y, bullets[i].sprite); 
        }
    }
}


void initializeEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i] = Enemy(rand() % 80, 0); 
    }
}

void displayEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            displayBullet(enemies[i].x, enemies[i].y, enemies[i].sprite); 
        }
    }
}

void handleJoystickInput() {
    static bool isShooting = false; 

    int direction = joystick.get_direction();
    if (joystick.get_direction() == N && !isShooting) {
        fireBullet(); 
        isShooting = true; 
    } else if (direction != N) {
        isShooting = false; 
    }

    if (direction == E) {
        x_pos += spaceshipSpeed;
    } else if (direction == W) {
        x_pos -= spaceshipSpeed;
    }

      if (x_pos < 1) {
        x_pos = 1;
    } else if (x_pos > 81) {
        x_pos = 81;
    }
}

void moveEnemies() {
    int missedAliens = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].y++; 
            
            if (enemies[i].y > 45) { 
                enemies[i].active = false; 
                missedAliens++;
            }
        }
    }
}

void updateEnemies() {
    moveEnemies();  
    generateEnemies(); 
}

void resetEnemyGeneration() {
    enemyThread.terminate();  
    enemyThread.start(updateEnemies);  
}


void renderGame() {
    lcd.clear();  
    displayBackground();
    displayEnemies();     
    displayBullets();     
    fillRect(x_pos, y_pos, 6, 6);  
    lcd.refresh();  
}

bool gameStarted = false;

void displayGameOverScreen() {
    lcd.clear();
    displayBitmap(*(const uint8_t(*)[48][84])images2[currentGameOverIndex]);
    lcd.refresh();
}

bool isPaused = false;  

void pauseGame() {
    isPaused = !isPaused;

    lcd.clear();
    if (isPaused) {
        displayBitmap(pause);
    }
    lcd.refresh();
    
    ThisThread::sleep_for(300ms);
}

int lives = 3;

void displayLives() {
    lcd.clear();
    if (lives == 3) {
    displayBitmap(threelives);
    } else if (lives == 2) {
    displayBitmap(twolives);
    blueLed = 1;
    ThisThread::sleep_for(1s);
    blueLed = 0;
    } else if (lives == 1) {
    displayBitmap(onelive);
    blueLed = 1;
    ThisThread::sleep_for(1s);
    blueLed = 0;
    }
    lcd.refresh();
    ThisThread::sleep_for(1s);
}

void checkCollisions() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (bullets[i].x >= enemies[j].x && bullets[i].x <= enemies[j].x + 5 &&
                        bullets[i].y >= enemies[j].y && bullets[i].y <= enemies[j].y + 5) {
                        bullets[i].active = false;
                        enemies[j].active = false;
                        onEnemyShot();
                        return;
                    }
                }
            }
        }
    }
}

void handleLevelInput() {
    static int lastLevelIndex = -1;
    int direction = joystick.get_direction();

    if (direction == S && currentLevelIndex < 2) {
        currentLevelIndex++;
        ThisThread::sleep_for(200ms);
    } else if (direction == N && currentLevelIndex > 0) {
        currentLevelIndex--;
        ThisThread::sleep_for(200ms);
    }

    if (currentLevelIndex != lastLevelIndex) {
        displayBitmap(*(const uint8_t(*)[48][84])images3[currentLevelIndex]);
        lastLevelIndex = currentLevelIndex;

         if (currentLevelIndex == 0) {
            setDifficulty('E');
        } else if (currentLevelIndex == 1) {
            setDifficulty('M'); 
        } else if (currentLevelIndex == 2) {
            setDifficulty('H');

    }
}
}

bool levelSelected = false;

void selectLevel() {
    while (!levelSelected) {
        
        handleLevelInput();
        ThisThread::sleep_for(200ms);

        if (SelectButton.read() == 0) {
            levelSelected = true;
            ThisThread::sleep_for(500ms);
        }
    }
}
void restartFromBeginning() {
    SegDis = 0;
    ThisThread::sleep_for(100ms);
    gameStarted = false;
    gameEnded = false;
    spaceshipSelected = false;
    levelSelected = false;
    currentImageIndex = 0;
    currentGameOverIndex = 0;
    currentLevelIndex = 0;
    score = 0;
    lives = 3;
    enemiesGenerated = false;
    lcd.clear();
    lcd.refresh();
    enemyThread.terminate();
    ThisThread::sleep_for(100ms);
    joystick.init();
    lcd.init(LPH7366_1);
    lcd.setContrast(0.55);
    lcd.setBrightness(0.5);
    displayPressStartAnimation();
    ThisThread::sleep_for(100ms);
    selectSpaceship();
    ThisThread::sleep_for(100ms);
    blueLed = 0;
    greenLed = 0;
    redLed = 0;
    selectLevel();
    ThisThread::sleep_for(100ms);
    lcd.clear();
    lcd.refresh();
    gameStarted = true;
    initializeBullets();
    initializeEnemies();
    displayBitmap(threelives);
    ThisThread::sleep_for(2s);
}

void handleGameOverInput() {
    lcd.clear();
    lcd.refresh();
    ThisThread::sleep_for(300ms);
    gameEnded = true;
    SegDis.write(0x00);
    redLed = 1;
    greenLed = 0;
    blueLed = 0;
    int score = 0;
    displayGameOverScreen();
    gameOverBuzzer();
    redLed = 0;

    while (true) {
        int direction = joystick.get_direction();

        if (direction == N) { 
            currentGameOverIndex = 0;  
        } else if (direction == S) { 
            currentGameOverIndex = 1; 
        }
        displayGameOverScreen();

        if (SelectButton.read() == 0) {  
            ThisThread::sleep_for(300ms);

            if (currentGameOverIndex == 0) {  
                restartFromBeginning();
                return;
            } else if (currentGameOverIndex == 1) { 
                while (true) {
                    displayBitmap(spaceship_warriors);
                    ThisThread::sleep_for(500ms);
                    displayBitmap(spaceship_warriors1);
                    ThisThread::sleep_for(500ms);
                }
            }
        }
    }
}

void handleLifeLoss() {
    lives--;
    if (lives > 0) {
        displayLives();
    } else {
        handleGameOverInput();
    }
}


void checkForMissedAliens() {
    if (!enemiesGenerated) {
        return;  
    }

    bool lifeLost = false;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active && enemies[i].y >= 45) { 
            enemies[i].active = false;
            lifeLost = true;
        }
    }
    
    if (lifeLost) {
        handleLifeLoss();
    }
}


bool lastPauseState = 1;

int main() {
    joystick.init();
    lcd.init(LPH7366_1);
    lcd.setContrast(0.55);
    lcd.setBrightness(0.5);
    SegDis.write(0x00);

    for (int i = 0; i < 5; i++) {
        displayBitmap(spaceship_warriors);
        ThisThread::sleep_for(500ms);
        displayBitmap(spaceship_warriors1);
        ThisThread::sleep_for(500ms);
    }

    displayPressStartAnimation();
    ThisThread::sleep_for(100ms);
    selectSpaceship();
    ThisThread::sleep_for(100ms);
    blueLed = 0;
    greenLed = 0;
    redLed = 0;
    selectLevel();
    ThisThread::sleep_for(100ms);

    lcd.clear();
    lcd.refresh();

    gameStarted = true;

    initializeBullets();
    initializeEnemies();
    displayBitmap(threelives);
    ThisThread::sleep_for(2s);

    while (gameStarted) {
        if (pauseButton.read() == 1 && lastPauseState == 0) {
            ThisThread::sleep_for(300ms);
            pauseGame();
        }
        lastPauseState = pauseButton.read();

        if (!isPaused) {
            handleJoystickInput();
            moveBullets();
            moveEnemies();
            checkCollisions();
            checkForMissedAliens();

            if (rand() % 20 == 0) {
                generateEnemies();
            }
            renderGame();
        }

        ThisThread::sleep_for(100ms);
    }
    }