Spaceship_Warriors

Function: 'Spaceship Warriors' is an embedded system-based game inspired by the arcade classic Space Invaders, developed on an STM32 Nucleo board. The game combines an LCD screen, a seven-segment display, an RGB LED, a buzzer, a joystick with a built-in push button, and an additional push button to deliver a full arcade-style shooting experience.

The order of the game states (within main) are shown below along with their accompanied functions:

logoAnimation() : Displays the 'Spaceship Warriors' title screen for a few seconds before transitioning to the press-start animation. pressStartAnimation(): Alternates between two start screen images in a loop until the user presses a button, then proceeds to spaceship selection. spaceshipSelection() : Allows the player to select their spaceship using the joystick. The RGB LED illuminates in different colours to indicate the chosen spaceship. gameplay() : Handles the core game loop. The player launches their spaceship North using the joystick and navigates East/West. The RGB LED gives visual feedback during launching, the LCD screen displays the game and remaining lives, and the seven-segment display shows the score. Losing a life is equivalent to losing a round (e.g., failing to hit 2 of 3 simultaneous opponents costs one life). pauseGame() : Triggered by the additional push button, allows the player to pause the game at any point. gameOver() : Triggered when all lives are lost. Displays a game-over screen with options to restart or exit.

Difficulty Levels: Easy Level: Enemies appear one at a time; movement speed reduced for enhanced control. Suited for beginners. Medium Level: Three enemies generated simultaneously; movement speed moderately increased. Requires quicker reactions. Hard Level: Five enemies introduced at once; movement accelerated. Rapid-paced and demanding on reflexes and precision.

Note: Players start with three lives, represented by distinct bitmaps on the display. Sound effects are produced by the buzzer throughout gameplay, particularly when shooting at aliens.

Software Design: The architecture follows a function-based framework (non-object-oriented), segmented into modules that manage player controls, enemy movements, shooting mechanics, and rendering. The UI Layer handles all visual displays and user interactions, including the main menu, spaceship selection screen, pause screen, and game-over screen, and captures joystick/button inputs to trigger actions such as starting, pausing, or selecting a spaceship.

Hardware Required:

STM32 Nucleo board
LCD screen
Seven-segment display
RGB LED
Buzzer
Joystick with built-in push button
Additional push button

Authored by:            Sara Anurag Somani
Date:                   2025
Version:                2.0
MBED Studio Version:    1.4.1
MBED OS Version:        6.14.0
Board:	                NUCLEO L476RG
