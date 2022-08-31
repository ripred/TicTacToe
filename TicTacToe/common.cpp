//
//  common.cpp
//  TicTacToe
//
//  Created by trent on 8/7/21.
//

#include <cstring>
#include "common.h"
#include "move.h"
#include "line.h"


string coords(int const num, int const base) {
    string str;
    str += 'A' + num / base;
    str += itoa(num % base, Grid);
    return str;
}


void reverse(char s[]) {
    size_t first = 0, last = strlen(s) - 1;
    while (first < last) {
        int tmp = s[first];
        s[first++] = s[last];
        s[last--] = tmp;
    }
}

string commas(int num) {
    string s = itoa(num);
    char buff[s.size() * 2];
    string tmp;

    strcpy(buff, s.c_str());
    reverse(buff);
    for (int i=0; i < strlen(buff); ++i) {
        char c = buff[i];
        tmp += c;
        if ( i < (strlen(buff) - 1) && (i + 1) % 3 == 0)
            tmp += ',';
    }
    strcpy(buff, tmp.c_str());
    reverse(buff);

    return buff;
}


/**
 * @summary itoa(..) Convert int to string with optional field padding
 *
 * @param num     the number to convert
 * @param s       the char buffer to format the string in
 * @param base    the number base to use. default is 10
 * @param padlen  pad to right justify to this field length.
 * @param padchar the character to pad the length to. default pad char is ' '
 * @param left    left justify instead of right if true
 *
 * @returns string representaion of the integer value
 */
string const itoa(int num, char s[], int const base, int const padlen, char padchar, bool left) {
    int i, sign;
    
    if ((sign = num) < 0)     /* record sign */
        num = -num;           /* make n positive */

    i = 0;
    do {    /* generate digits in reverse order */
        char digit = num % base;
        digit += (digit <= 9) ? '0' : 'a' - 10;
        s[i++] = digit;             /* get next digit */
    } while ((num /= base) > 0);    /* delete it */

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
string const itoa(int num, int const base, int const padlen, char padchar, bool left) {
    char tmp[32];
    return itoa(num, tmp, base, padlen, padchar, left);
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
