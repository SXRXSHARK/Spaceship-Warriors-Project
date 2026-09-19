N5110_LCD_Test
===============

Function:               
The 'LCD Test' cycles through a variety of N5110 libary functions in order to facilitate the diagnosis of any display related faults. The test will only execute 
once and will continue to cycle through each function regardless if one function doesnt execute as intended. To restart the test at any point or to re-run the 
complete test, the NUCLEO's on-board reset button should be used. 
The order of the test functions (within main) are shown below along with their accompanied N5110 functions in which they test.

contrastTest()    : Tests the N5110::setContrast(float contrast) function in range of 0.0f and 0.95f
brightnessTest()  : Tests the N5110::setBrightness(float const brightness) function in range of 0.0f and 0.9f
modeTest()        : Tests the N5110::inverseMode() and the N5110::normalMode() functions by alternating between the two
printTest()       : Tests the following functions by printing to the LCD screen:
                     - N5110::drawCircle(unsigned int const x0, unsigned int const y0, unsigned int const radius, FillType const fill)
                     - N5110::drawLine(unsigned int const x0, unsigned int const y0, unsigned int const x1, unsigned int const y1, unsigned int const type)
                     - N5110::drawRect(unsigned int const x0, unsigned int const y0, unsigned int const width, unsigned int const height, FillType const fill)
                     - N5110::drawSprite(int x0, int y0, int nrows, int ncols, int *sprite)
                     - N5110::setPixel(unsigned int const x, unsigned int const y, bool const state = true)
                     - N5110::printChar(char const c, unsigned int const x, unsigned int const y)
                   Note: Where 'FillType const fill' is requested, both fill types ('FILL_BLACK' and 'FILL_TRANSPARENT') have been used. Additionally, where 
                         'unsigned int const type' has been requested, types '1' and '2' (black and dotted) have been used.
                          
powerDown()       : Tests the N5110::turnOff() function

Note: The following N5110 library functions are tested during the execution of the test script and therefore are not explicitly tested through the use of a main
      function:
      - N5110::init(LCD_Type const lcd)
      - N5110::clear()
      - N5110::refresh()
      - N5110::printString(char const *str, unsigned int const  x, unsigned int const  y)

Circuit Schematic No.:  9     : https://github.com/ELECXJEL2645/Circuit_Schematics
Required Libraries:     N5110 : https://github.com/ELECXJEL2645/N5110

Authored by:            Andrew Knowles
Date:                   2021
Collaberators:          Dr Craig Evans
                        Dr Alexander Valavanis
                        Dr Tim Amsdon
Version:                2.0
Revision Date:          06/2022 
MBED Studio Version:    1.4.1
MBED OS Version:        6.14.0
Board:	                NUCLEO L476RG
