//
//  common.cpp
//  TicTacToe
//
//  Created by trent on 8/7/21.
//

#include "common.h"
#include "score.h"
#include "line.h"
#include "rle.h"

/**
 * @summary itoa(..) Convert int to string with optional field padding
 *
 * @param num     the number to convert
 * @param s       the char buffer to format the string in
 * @param base    the number base to use. default is 10
 * @param padlen  pad to right justify to this field length.
 * @param padchar the character to pad the length to. default pad char is ' '
 * @param left    left justify instead of right if != 0
 *
 * @returns string representaion of the integer value
 */
string itoa(int num, char s[], int const base, int const padlen, char padchar, bool left) {
    int i, sign;
    
    if ((sign = num) < 0)     /* record sign */
        num = -num;           /* make n positive */

    i = 0;
    do {            /* generate digits in reverse order */
        s[i++] = num % base + '0';    /* get next digit */
    } while ((num /= base) > 0);      /* delete it */

    if (sign < 0)
        s[i++] = '-';

    while (!left && i < padlen)
        s[i++] = padchar;       /* pad */

    s[i] = '\0';
    reverse(s);

    while (left && i < padlen)
        s[i++] = padchar;       /* pad */

    s[i] = '\0';

    
    return s;
}

/**
 * @summary version of itoa() that doesn't require you to
 * supply the char buffer if you don't need that. Still
 * returns the formatted string but easier to call since
 * you don't have to declare a buffer at the call site first
 *
 */
string itoa(int n, int const base, int const pad, char padchar, bool left) {
    char tmp[32];
    return itoa(n, tmp, base, pad, padchar, left);
}


//////////////////////////////////////////////////////////
// Useful ANSI console sequences

// ansi escape sequence
string CSI         = "\x1b[";
// Reset
string resetAttr   = CSI + "0m";
// Bold
string boldAttr    = CSI + "1m";
// Faint
string faintAttr   = CSI + "2m";
// Underline
string underAttr   = CSI + "4m";
// Reverse Video
string reverseAttr = CSI + "7m";
// Italic
string italicAttr  = CSI + "3m";
// Strike-through
string strikeAttr  = CSI + "9m";

//////////////////////////////////////////////////////////
// Cursor visibility and erasing Controls

//////////////////////////////////////////////////////////
// Turn the cursor Off:
string cursOff = CSI + "?25l";

//////////////////////////////////////////////////////////
// Turn the cursor On:
string cursOn = CSI + "?25h";

//////////////////////////////////////////////////////////
// Clear the display from cursor to end of the line:
string clearEOL = CSI + "K";

//////////////////////////////////////////////////////////
// Position the cursor to a column and row:
string cursPos(int col, int row) { return string(CSI) + itoa(row) + ";" + itoa(col) + "f"; }

//////////////////////////////////////////////////////////
