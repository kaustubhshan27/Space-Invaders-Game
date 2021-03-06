#include "TM4C123.h"
#include "Nokia5110.h"
#include "random.h"
#include <stdlib.h>
#include <time.h>

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground


// Potentiometer
// -------------
// Analog Input  connected to PB5

// Push Buttons
// ------------
// Push Button 1 (Single Laser) connected to PB3

// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10
const unsigned char SmallEnemy30PointA[] = {
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x0F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
 0xFF, 0x0F, 0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// image of the player's ship
// includes two blacked out columns on the left and right sides of the image to prevent smearing when moved 2 pixels to the left or right
// width=18 x height=8
const unsigned char PlayerShip0[] = {
 0x42, 0x4D, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00,
 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xAA, 0xAA, 0xAA,
 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
	
// small shield floating in space to cover the player's ship from enemy fire (undamaged)
// width=18 x height=5
const unsigned char Bunker0[] = {
 0x42, 0x4D, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0x00,
 0x00, 0x00, 0xAA, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
 0xAA, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0xFF};

// small shield floating in space to cover the player's ship from enemy fire (moderate generic damage)
// width=18 x height=5
const unsigned char Bunker1[] = {
 0x42, 0x4D, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0x00,
 0x00, 0x00, 0xAA, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0xAA, 0xAA, 0xAA, 0xAA, 0xA0, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0x0A, 0xA0, 0xA0, 0xAA,
 0xAA, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xA0, 0x00, 0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00, 0xFF};

// small shield floating in space to cover the player's ship from enemy fire (heavy generic damage)
// width=18 x height=5
const unsigned char Bunker2[] = {
 0x42, 0x4D, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xAA, 0x00,
 0x00, 0x00, 0xAA, 0x0A, 0xA0, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0xA0, 0xAA, 0xA0, 0xA0, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x0A, 0x00, 0xA0, 0xA0,
 0xA0, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// blank space used to cover a bunker that is destroyed
// width=18 x height=5
const unsigned char Bunker3[] = {
 0x42, 0x4D, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// large explosion that can be used upon the demise of the player's ship (first frame)
// width=18 x height=8
const unsigned char BigExplosion0[] = {
 0x42, 0x4D, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x0A, 0x00, 0x09, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x90, 0x00, 0x90, 0xB0, 0x0A, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xA0, 0x00, 0xE0, 0x00, 0x00,
 0x90, 0x90, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0xA0, 0x00, 0x00, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00,
 0x00, 0xE0, 0x0A, 0x0A, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x09, 0x00, 0x00, 0x00, 0x90, 0x00, 0x0A, 0x00, 0x00, 0x00, 0xFF};

// large explosion that can be used upon the demise of the player's ship (second frame)
// width=18 x height=8
const unsigned char BigExplosion1[] = {
 0x42, 0x4D, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x0E, 0x00, 0x09, 0x00, 0x09, 0x00, 0xB0, 0x00, 0xA0, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA9, 0x00, 0x00, 0x00, 0x00, 0x90,
 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x0A, 0x00, 0x90, 0x00, 0xB0, 0x00, 0x09, 0x00, 0x00, 0x00, 0xFF};

// small explosion best used for the demise of an enemy
// width=16 x height=10
const unsigned char SmallExplosion0[] = {
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
 0x0F, 0x00, 0x0F, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0xF0, 0xF0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// blank space following the small explosion for the demise of an enemy
// width=16 x height=10
const unsigned char SmallExplosion1[] = {
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// a laser burst in flight
// includes one blacked out row on the top and bottom of the image to prevent smearing when moved 1 pixel up or down
// width=2 x height=9
const unsigned char Laser0[] = {
 0x42, 0x4D, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0xBB, 0x00,
 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// blank space to cover a laser after it hits something
// width=2 x height=9
const unsigned char Laser1[] = {
 0x42, 0x4D, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
	
// ********** Capture image dimensions out of BMP **********
#define BUNKERW     ((unsigned char)Bunker0[18])
#define BUNKERH     ((unsigned char)Bunker0[22])
#define ENEMY30W    ((unsigned char)SmallEnemy30PointA[18])
#define ENEMY30H    ((unsigned char)SmallEnemy30PointA[22])
#define LASERW      ((unsigned char)Laser0[18])
#define LASERH      ((unsigned char)Laser0[22])
#define PLAYERW     ((unsigned char)PlayerShip0[18])
#define PLAYERH     ((unsigned char)PlayerShip0[22])
	
// ********** Max. number of missiles **********	
#define USER_MISSILE_MAX 		6 		
#define ENEMY_MISSILE_MAX		6

//PB5 to potentiometer(input), PB4 to special missile button(input), PB3 to normal missile button(input),
//3bit DAC to  PB2, PB1, PB0(outputs)
void portB_init(void);				
void portF_init(void);								//PF2 onboard LED(Blue). Used for debugging purposes. PF4 for game reset
void PLL_init(void);									//setting clock at 80MHz
void systick_interrupt_init(void);		//SysTick timer interrupt initialization
void timer0A_init(void);							//Timer0A initialization to generate sound
void ADC1_interrupt_init(void);				//ADC1 initialization and PB5(analog input to ADC1) configuration
void sprites_pos_init(void);					//initializing enemy positions on screen
void lasers_init(void);

void GPIOB_Handler(void);							//positive edge triggered interrupts for push button lasers
void GPIOF_Handler(void);							//PF4 used as reset button for game
void SysTick_Handler(void);						//SysTick interrupt ISR
void TIMER0A_Handler(void);						//TIMER0A interrupt handler for sound generation at 10kHz
void ADC1SS3_Handler(void);						//ADC1 SS3 interrupt for sampling potentiometer inputs

void DAC_out(unsigned char dac_input);//input to external 3bit DAC
void Draw(void);											//to draw images on Nokia5110
void Delay100ms(unsigned long count); //multiples of 100ms delay
void game_start_message(void);				
void game_end_message(void);

//3 bit DAC input to create Sine Wave (100Hz)
static const uint8_t SineWave[16] = {4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3};
static uint8_t index = 0;							//index used to traverse SineWave array
static uint32_t adc_value = 0;				//ADC input. Value between 0-4095
static uint8_t semaphore = 0;					//to handle synchronization between the main thread and the SysTick thread
//static uint32_t timer0A_count = 0;		//to count number of times timer0A interrupt has occurred
static uint8_t prev_position = 32;		//initial player ship poistion
static uint8_t current_position = 32;	//current player ship poistion
static uint8_t laser_movement_count = 0; //counter in systick to move missiles
static uint8_t enemy_laser_generation_count = 0;//counter in systick to generate enemy missiles
static uint8_t countOf_user_lasers = 0;
static uint8_t countOf_enemy_lasers = 0;
static uint8_t bunker_hit_count = 0;
static uint8_t gameOverFlag = 0;
static uint8_t killCount = 0;
static uint8_t prev_choice;

struct State {
  uint8_t x;      						// x coordinate
  uint8_t y;      						// y coordinate
  const unsigned char *image; // ptr->image
  uint8_t life;         			// 0=dead, 1=alive
};         
typedef struct State STyp;
STyp Enemy[3];
STyp User;
STyp Bunker;

struct Laser_State {
	uint8_t x;      						// x coordinate
  int8_t y;      							// y coordinate, can be negative
  const unsigned char *image; // ptr->image
	int8_t hit_status;					//to see if missile is destined to hit enemy or not
	uint8_t life;         			// 0=dead, 1=alive
};
typedef struct Laser_State Laser; 
Laser Enemy_Laser[ENEMY_MISSILE_MAX];
Laser User_Laser[USER_MISSILE_MAX];

struct tm time_data;

int main(void)
{
	__set_PRIMASK(1);	//disabling all interrupt
	Nokia5110_Init();
	game_start_message();
  Delay100ms(50);   // delay 5 sec
	sprites_pos_init();
	lasers_init();
	Draw();
	portF_init();
	portB_init();
	systick_interrupt_init();
	ADC1_interrupt_init();
	Random_Init(time_data.tm_sec);
	timer0A_init();
	__set_PRIMASK(0);	//enabling all interrupts
	
  while(1)
	{
		if(semaphore == 1 && gameOverFlag == 0)
		{
			Draw();
			semaphore = 0;
		}
		else if(gameOverFlag == 1)
		{
			Draw();
			Delay100ms(20);
			break;
		}
  }
	__set_PRIMASK(1);	//disabling all interrupt
	game_end_message();
}

void portB_init(void)													
{
	SYSCTL->RCGCGPIO |= (1 << 1);
	GPIOB->CR |= 0x3F;
	GPIOB->DEN |= 0x1F;
	GPIOB->AFSEL &= ~(0x1F);
	GPIOB->AMSEL &= ~(0x1F);		
	GPIOB->PCTL = 0x00000000;		//does not affect PB5 analog input to ADC
	GPIOB->DIR |= 0x07;					//PB5, PB4, PB3-input & PB2, PB1, PB0-output			
}

void portF_init(void)						 
{
	SYSCTL->RCGCGPIO |= (1 << 5);
	GPIOF->CR |= (1 << 2)|(1 << 4);
	GPIOF->DEN |= (1 << 2)|(1 << 4);
	GPIOF->AFSEL &= ~(1 << 2);
	GPIOF->AFSEL &= ~(1 << 4);
	GPIOF->AMSEL &= ~(1 << 2);
	GPIOF->PUR |= (1 << 4);
	GPIOF->PCTL = 0x00000000;
	GPIOF->DIR |= (1 << 2);
	GPIOF->DIR &= ~(1 << 4);
	GPIOF->IS &= ~(1 << 4);	//edge sensitive
	GPIOF->IBE &= ~(1 << 4);//single edge
	GPIOF->IEV &= ~(1 << 4);//falling edge 
	GPIOF->ICR |= (1 << 4);//set acknowledge, interrupt is cleared
	NVIC->IP[GPIOF_IRQn] = 0;//IP registers are byte accessible and can be accessed by using IRQn as index. Priority set as 0
	GPIOF->IM |= (1 << 4);//arm the interrupt
	NVIC->ISER[0] |= (1 << GPIOF_IRQn);//GPIOF_IRQn = 30, part of ISER[0]
	GPIOF->DATA &= ~(1 << 2);			//initially LED is OFF
}

void PLL_init(void)
{
	SYSCTL->RCC2 |= (1 << 11);		//PLL is bypassed
	SYSCTL->RCC |= (0x15 << 6);   //Specify the crystal frequency(16MHz) in the four XTAL bits using the code “10101”
	SYSCTL->RCC2 &= ~(1 << 4);		//The OSCSRC2 bits are cleared to select the main oscillator as the oscillator clock source
	SYSCTL->RCC2 &= ~(1 << 13);		//Clear PWRDN2 (bit 13) to activate the PLL
	SYSCTL->RCC2 |= (0x05 << 23);	//To get the desired 80 MHz from the 400 MHz PLL, we need to divide by 5
	while((SYSCTL->RIS & (1 << 6)) != 1);//wait for the PLL to stabilize by waiting for PLLRIS (bit 6)
	SYSCTL->RCC2 &= ~(1 << 11);		//PLL is connected
}

void systick_interrupt_init(void)
{
	SysTick->CTRL = 0x06;					//enable = 0, clk_src = 1 (16 MHz Bus Clock), interrupt enabled
	SysTick->LOAD = 533334 - 1;	  //30Hz(=0.033secs) frequency for systick thread 
	SysTick->VAL = 0x00;					//to clear by writing to the current value register
	SCB->SHP[11] = 1;							//systick exception priority = 2, BASEPRI = 0 (implicit)
	SysTick->CTRL |= (1 << 0);		//enable and start timer
}

void timer0A_init(void)
{
	SYSCTL->RCGCTIMER |= (1 << 0);//enabling 16/32 bit timer module 0 clock
	TIMER0->CTL &= ~(1 << 0);			//timer A is disabled in module 0
	TIMER0->CFG = 0x00000000;			//timer A0 configured as 32 bit timer
	TIMER0->TAMR |= 0x02;					//Write a value of 0x2 for periodic mode into GPTMTnMR
	TIMER0->TAMR &= ~(1 << 4);		//setting timer A0 as down counter
	TIMER0->TAILR = 0x00000640;		//default clock of uC is 16Mhz, so this count gives us 0.1 ms of count = 10kHz
	TIMER0->IMR |= (1 << 0);			//enabling the timer0A interrupt
	TIMER0->CTL |= (1 << 0);			//Set the TnEN bit in the GPTMCTL register to enable the timer and start counting
}

void ADC1_interrupt_init(void)
{
	SYSCTL->RCGCADC |= (1 << 1);//Enable the ADC clock using the RCGCADC register. Enabling ADC1
	GPIOB->DIR &= ~(1 << 5);		//PB5 as input
	GPIOB->AFSEL |= (1 << 5);		//AIN11 - PB5
	GPIOB->DEN &= ~(1 << 5);		//PB5
	GPIOB->AMSEL |= (1 << 5);		//PB5
	ADC1->ACTSS &= ~(1 << 3);		//Using SS3, disabling for configuration
	ADC1->EMUX |= (0x0 << 12);	//Processor(default)
	ADC1->SSMUX3 = 11;					//ani11 - PB5
	ADC1->SSPRI |= (0x0 << 12) | (0x3 << 8) | (0x2 << 4) | (0x1 << 0);//setting priority of the sample sequencers
	ADC1->SSCTL3 |= 0x06;				//enabling interrupt
	ADC1->IM |= (1 << 3);				//set the corresponding MASK bit in the ADCIM register
	ADC1->ACTSS |= (1 << 3);		//Using SS3
	ADC1->ISC |= (1 << 3);			//Clear the interrupt bit
	NVIC_EnableIRQ(ADC1SS3_IRQn);//NVIC enable of interrupt
}

void sprites_pos_init(void)
{ 
  for(int i = 0; i < 3; i++)
	{
    Enemy[i].x = 32*i;
    Enemy[i].y = ENEMY30H - 1;
    Enemy[i].image = SmallEnemy30PointA;
    Enemy[i].life = 1;
  }
	
	//user
	User.x = 32;
	User.y = 47;
	User.image = PlayerShip0;
	User.life = 1;
	
	//bunker
	Bunker.x = 32;
	Bunker.y = 47 - PLAYERH;
	Bunker.image = Bunker0;
	Bunker.life = 3;
}

void SysTick_Handler(void)
{
	GPIOF->DATA ^= 0x04;					//toggle LED 
	
	ADC1->PSSI |= (1 << 3);				//Begin sampling on Sample Sequencer 3
	current_position = (-0.016*adc_value) + 66;
	if(current_position != prev_position)
	{
		User.x = current_position;
		prev_position = current_position;
	}
	ADC1->PSSI &= ~(1 << 3);
	
//movement of user and enemy lasers	
	laser_movement_count = (laser_movement_count + 1) % 15;
	if(laser_movement_count == 0)
	{
		if(countOf_user_lasers > 0)
		{
			for(uint8_t index = 0; index < USER_MISSILE_MAX; index++)
			{
				if(User_Laser[index].life > 0)
				{
					User_Laser[index].y -= 3;			//move up by 3 pixels
					
					if(User_Laser[index].hit_status >= 0)		//to check if user laser hitting enemy or not -- CASE 1
					{
						if((User_Laser[index].y - LASERH) <= Enemy[User_Laser[index].hit_status].y)
						{
							Enemy[User_Laser[index].hit_status].image = SmallExplosion0;
							User_Laser[index].image = Laser1;
							User_Laser[index].hit_status = -1;
							User_Laser[index].life = 0;
							countOf_user_lasers--;
						}
					}
					else
					{
						if(User_Laser[index].y <= 0)				//to check of laser has gone out of screen -- CASE 2
						{
							User_Laser[index].image = Laser1;
							User_Laser[index].hit_status = -1;
							User_Laser[index].life = 0;
							countOf_user_lasers--;
						}
					}	
				}
			}
		}
		
		if(countOf_enemy_lasers > 0)
		{
			for(uint8_t index = 0; index < ENEMY_MISSILE_MAX; index++)
			{
				if(Enemy_Laser[index].life > 0)
				{
					Enemy_Laser[index].y += 4;		//move down by 4 pixels
					
					if(Enemy_Laser[index].hit_status == 1)	//to check if enemy laser is hitting bunker -- CASE 1
					{
						if((Enemy_Laser[index].y >= Bunker.y - BUNKERH) && (Bunker.life > 0)) 
						{
							bunker_hit_count++;
							switch(bunker_hit_count)
							{
								case 1: 
									Bunker.image = Bunker1;
									break;
								case 2:
									Bunker.image = Bunker2;
									break;
								case 3:
									Bunker.image = Bunker3;
									Bunker.life = 0;
									break;
								default:
									Bunker.image = Bunker3;
									Bunker.life = 0;
									break;
							}
							Enemy_Laser[index].hit_status = 0;
							Enemy_Laser[index].life = 0;
							countOf_enemy_lasers--;
						}
					}
					else if((Enemy_Laser[index].x >= User.x) && (Enemy_Laser[index].x <= User.x + PLAYERW))	//to check if enemy missile is hitting user -- CASE 2
					{
						if(Enemy_Laser[index].y >= User.y - PLAYERH)
						{
							User.image = BigExplosion0;
							Enemy_Laser[index].image = Laser1;
							Enemy_Laser[index].life = 0;
							gameOverFlag = 1;
							countOf_enemy_lasers--;
						}
					}
					else if(Enemy_Laser[index].y - LASERH >= 47)	//to check if laser goes out of the screen -- CASE 3
					{
						Enemy_Laser[index].image = Laser1;
						Enemy_Laser[index].life = 0;
						countOf_enemy_lasers--;
					}
				}
			}
		}	
	}

//enemy laser random generation	
	enemy_laser_generation_count = (enemy_laser_generation_count + 1) % 75;
	if((enemy_laser_generation_count == 0) && (countOf_enemy_lasers < ENEMY_MISSILE_MAX))
	{
		uint8_t enemy_choice = Random() % 3;
		if(enemy_choice == prev_choice)
			enemy_choice = (enemy_choice + 1) % 3;
		prev_choice = enemy_choice;
		if(Enemy[enemy_choice].image != SmallExplosion0)
		{
			countOf_enemy_lasers++;
			for(uint8_t index = 0; index < ENEMY_MISSILE_MAX; index++)
			{
				if(Enemy_Laser[index].life == 0)
				{
					Enemy_Laser[index].x = Enemy[enemy_choice].x + ENEMY30W/2 - 1;
					Enemy_Laser[index].y = Enemy[enemy_choice].y + ENEMY30H;
					Enemy_Laser[index].image = Laser0;
					Enemy_Laser[index].life = 1;
					if(((Enemy_Laser[index].x >= Bunker.x) && (Enemy_Laser[index].x <= Bunker.x + BUNKERW)) && Bunker.life != 0)
					{
						Enemy_Laser[index].hit_status = 1;
					}
					else
					{
						Enemy_Laser[index].hit_status = 0;
					}
					break;
				}
			}
		}
	}
	
	if(((GPIOB->DATA & 0x08) == 0x08) && (countOf_user_lasers < 6))	//single_missile button (PB3) pressed
	{
		countOf_user_lasers++;
		for(uint8_t index = 0; index < USER_MISSILE_MAX; index++)
		{
			if(User_Laser[index].life == 0)
			{
				User_Laser[index].x = User.x + (PLAYERW/2) - 1;
				User_Laser[index].y = 47 - PLAYERH;
				User_Laser[index].image = Laser0;
				User_Laser[index].life = 1;
				if((User_Laser[index].x >= Enemy[0].x) && (User_Laser[index].x <= Enemy[0].x + ENEMY30W))
				{
					User_Laser[index].hit_status = 0;
				}
				else if((User_Laser[index].x >= Enemy[1].x) && (User_Laser[index].x <= Enemy[1].x + ENEMY30W))
				{
					User_Laser[index].hit_status = 1;
				}
				else if((User_Laser[index].x >= Enemy[2].x) && (User_Laser[index].x <= Enemy[2].x + ENEMY30W))
				{
					User_Laser[index].hit_status = 2;
				}
				else
					User_Laser[index].hit_status = -1;
				break;
			}
		}
		Delay100ms(2);//to prevent button debounce
	}
	
	killCount = 0;
	for(uint8_t index = 0; index < 3; index++)
	{
		if(Enemy[index].image == SmallExplosion0)
			killCount++;
	}
	if(killCount == 3)
		gameOverFlag = 1;
	
	semaphore = 1;
}

void TIMER0A_Handler(void)
{
	index = (index+1) % 16;				//keep index in the range between 0-31
	DAC_out(SineWave[index]);
}

void ADC1SS3_Handler(void)
{
	adc_value = ADC1->SSFIFO3;	//each FIFO in ADCs is 32 bits
	ADC1->ISC |= (1 << 3);			//Clear the interrupt bit
}

void DAC_out(unsigned char dac_input)
{
	GPIOB->DATA |= (dac_input & 0x07);
}

void Draw(void)
{ 
  Nokia5110_ClearBuffer();
  for(int i = 0; i < 3; i++)
	{
    if(Enemy[i].life > 0)
		{
     Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, Enemy[i].image, 0);
    }
  }
	
	if(User.life > 0)
		Nokia5110_PrintBMP(User.x, User.y, User.image, 0);
	
	if(Bunker.life > 0)
		Nokia5110_PrintBMP(Bunker.x, Bunker.y, Bunker.image, 0);
	else
		Nokia5110_PrintBMP(Bunker.x, Bunker.y, Bunker3, 0);

	for(uint8_t index = 0; index < USER_MISSILE_MAX; index++)
	{
		if(User_Laser[index].life > 0)
			Nokia5110_PrintBMP(User_Laser[index].x, User_Laser[index].y, User_Laser[index].image, 0);
	}
	
	for(uint8_t index = 0; index < ENEMY_MISSILE_MAX; index++)
	{
	if(Enemy_Laser[index].life > 0)
			Nokia5110_PrintBMP(Enemy_Laser[index].x, Enemy_Laser[index].y, Enemy_Laser[index].image, 0);
	}
	
  Nokia5110_DisplayBuffer();  // draw buffer
}

void Delay100ms(unsigned long count)
{
	unsigned long time;
  while(count > 0)
		{
    time = 727240;  
    while(time)
		{
	  	time--;
    }
    count--;
  }
}

void game_start_message(void)
{
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("SPACE");
	Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("INVADERS");
  Nokia5110_SetCursor(1, 4);
  Nokia5110_OutString("Begin Game!");
}

void game_end_message(void)
{
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("GAME OVER");
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Nice try,");
  Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("Earthling!");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutUDec(killCount*50);
}

void lasers_init(void)
{
	for(uint8_t index = 0; index < USER_MISSILE_MAX; index++)
	{
		User_Laser[index].hit_status = -1;	//if user laser is going to hit enemy sprite, sprite0:hit_status=0, sprite1:hit_status=1, sprite2:hit_status=2 else hit_status=-1 
		User_Laser[index].image = Laser1;		//image initialized to blank space
		User_Laser[index].x = 0;
		User_Laser[index].y = 0;
		User_Laser[index].life = 0;
	}
	for(uint8_t index = 0; index < ENEMY_MISSILE_MAX; index++)
	{
		Enemy_Laser[index].hit_status = 0;	//if enemy is going to hit bunker hit_status=1 else hit_status=0 
		Enemy_Laser[index].image = Laser1;	//image initialized to blank space
		Enemy_Laser[index].x = 0;
		Enemy_Laser[index].y = 0;
		Enemy_Laser[index].life = 0;
	}
}
