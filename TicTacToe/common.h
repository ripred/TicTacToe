//
//  common.h
//  TicTacToe
//
//  Created by trent on 8/7/21.
//

#ifndef common_h
#define common_h

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

// Game Move States
// The highest state available should be chosen
// (or one chosen from many equal to the highest state available)
typedef enum : int {
       ZERO =     0,    // invalid move stance
     NOMOVE = 10000,    // no moves are available
    RANDOM2,            // only moves on mixed rows available
    RANDOM1,            // one or more open spots available but none are critical
     FORCED,            // a spot that will win the game has been found
     WINNER             // the game has been won
} movetype_e;

#define debug(LEVEL, COMMAND) { if ((DbgLvl) >= (LEVEL)) { COMMAND; } }

// Global Function declaratons
extern string const itoa(int n, char s[], int const base = 10, int const pad = 0, char padchar = ' ', bool left = false);
extern string const itoa(int n, int const base = 10, int const pad = 0, char padchar = ' ', bool left = false);
extern string const to_string(vector<int> const &v);
extern void reverse(char s[]);
extern string commas(int num);
extern string coords(int const num, int const base);

// ANSI console escape sequences
extern string CSI;          // ANSI escape sequence prefix
extern string resetAttr;    // Reset
extern string boldAttr;     // Bold
extern string faintAttr;    // Faint
extern string underAttr;    // Underline
extern string reverseAttr;  // Reverse Video
extern string italicAttr;   // Italic
extern string strikeAttr;   // Strike-through
extern string cursOff;      // Turn the cursor Off
extern string cursOn;       // Turn the cursor On
extern string clearEOL;     // Clear the display from cursor to end of the line
extern string cursPos(int col, int row);    // Position the cursor to a column and row:

//////////////////////////////////////
// Global Variables
//////////////////////////////////////

extern  int const Grid;
extern  int       Base;
extern  int       DbgLvl;
extern  bool      UseAnsi;
extern  bool      UseCoords;
#endif /* common_h */
