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

typedef enum : int {
       ZERO =     0,
     NOMOVE = 10000,
    RANDOM2,
    RANDOM1,
     FORCED,
     WINNER
} movetype_e;

#define debug(LEVEL, COMMAND) { if ((DbgLvl) >= (LEVEL)) { COMMAND; } }

//////////////////////////////////////
// Global Variables
//////////////////////////////////////

extern  int const Grid;
extern  int       Base;
extern  int       Humans;
extern  int       DbgLvl;
extern  bool      UseAnsi;

// Function declaratons
extern string itoa(int n, char s[], int const base = 10, int const pad = 0, char padchar = ' ', bool left = false);
extern string itoa(int n, int const base = 10, int const pad = 0, char padchar = ' ', bool left = false);
extern string to_string(vector<int> const &v);
extern void reverse(char s[]);
extern bool test_RLE();

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

#endif /* common_h */
