//
//  score.h
//  TicTacToe
//
//  Created by trent on 8/7/21.
//

#ifndef score_h
#define score_h

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "common.h"

struct Score {
    movetype_e  key;
    int         value;
    vector<int> choices;


    string to_string() const {
        stringstream ss;
        ss  << string("{ key: ") << (
                key == ZERO    ? "ZERO   " :
                key == NOMOVE  ? "NOMOVE " :
                key == FORCED  ? "FORCED " :
                key == RANDOM1 ? "RANDOM1" :
                key == RANDOM2 ? "RANDOM2" :
                key == WINNER  ? "WINNER " : itoa(key, 10, 7))
            << " value: " << value
            << " choices: " << ::to_string(choices)
            << " }";

        return ss.str();
    } // Score::to_string()


    Score() {
        key = ZERO;
        value = 0;
    } // Score::Score()


    Score(movetype_e const k, int const v) {
        key = k;
        value = v;
    } // Score::Score(int const k, int const v)


    Score(movetype_e const k, int const v, vector<int> const &c)
        : key(k), value(v), choices(c) {
    } // Score::Score(int const k, int const v, vector<int> const &c)


    void clear() {
        key = ZERO;
        value = 0;
        choices.clear();
    } // Score::clear()


    bool empty() const {
        return key == ZERO &&
               value == 0 &&
               choices.empty();
    } // Score::empty()

};  // end of class Score

#include <map>
typedef std::map<int, Score> MoveTable;

#endif /* score_h */
