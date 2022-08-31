//
//  rle.h
//  TicTacToe
//
//  Created by trent on 8/7/21.
//

#include "common.h"
#include "score.h"

#ifndef rle_h
#define rle_h

#include <iostream>
using std::cout;

#include <vector>
using std::vector;

#include <string>
using std::string;

struct RLE {
    vector<int> past;
    int         index;
    int         length;
    movetype_e  key;
    int         value;
    int         last;
    int         zero;
    int         lastzero;


    void set_zero(int const z) {
        zero = z;
    }


    Score feed(int const item) {
        past.push_back(item);

        // limit the savings to Base length
        if (past.size() > Base) {
            past.erase(past.begin());

            if (lastzero >= 0) {
                lastzero--;
            }
            
            if (key == FORCED) {
                value--;
                if (value < 0) {
                    key = ZERO;
                }
            }
        } else {
            index++;
        }

        if (empty()) {
            // this is the first item
            last = item;
            length = 1;
            if (item == zero) { lastzero = index - 1; }
        } else if (item == last) {
            // the item is the same as the last one
            length++;
            if (item == zero) {
                lastzero = index - 1;
            } else {
                if (length >= Base) {
                    key = WINNER;
                    value = item;
                } else if (length == Base - 1) {
                    if (index >= Base && past[past.size() - Base] == zero) {
                        if (FORCED > key) {
                            key = FORCED;
                            value = length - Base + 1;
                        }
                    }
                }
            }
        } else {
            // the item has changed
            if (item == zero) {
                lastzero = index - 1;
                if (length == (Base - 1)) {
                    // this is an empty cell and moving here would complete this line
                    if (FORCED > key) {
                        key = FORCED;
                        value = index - 1;
                    }
                }
            }
            last = item;
            length = 1;
            index++;
        }

        if (past.size() == Base && lastzero < 0 && key == ZERO) {
            key = NOMOVE;
        }
        
        return { key, value };
    } // RLE::feed(int const item)


    Score feed(vector<int> const &values) {
        for (int const &next : values) {
            feed(next);
            if (key != ZERO) {
                return { key, value };
            }
        }

        return { ZERO, 0 };
    } // RLE::feed(vector<int> const &values)


    RLE() {
        clear();
    } // RLE::RLE()


    RLE(int const item) {
        clear();
        feed(item);
    } // RLE::RLE(int const item)


    RLE(vector<int> const &values) {
        clear();
        feed(values);
    } // RLE::RLE(vector<int> const &values)


    void clear() {
        past.clear();
        index = 0;
        length = 0;
        key = ZERO;
        value = 0;
        last = -1;
        zero = 0;
        lastzero = -1;
    } // RLE::clear()


    bool empty() const {
        return last < 0;
    } // RLE::empty()


    Score score() const {
        return { key, value };
    } // RLE::score()

    
    Score restart(int const item) {
        int const tmp = zero;
        clear();
        zero = tmp;
        return feed(item);
    }


    Score restart(vector<int> const &values) {
        int const tmp = zero;
        clear();
        zero = tmp;
        return feed(values);
    } // RLE::restart(vector<int> const &values)



    // RLE unit tests
    static bool test_RLE() {
        RLE rle;

        debug(2, cout << "Running RLE unit tests...\n");

        // save original Base value
        int orig_base = Base;

        // test setup
        Base = 3;

        // test empty()
        debug(2, cout << "    test empty()...");
        if (!(rle.empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test feed()
        debug(2, cout << "    test feed()...");
        rle.feed(0);
        if (!(!rle.empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test clear()
        debug(2, cout << "    test clear()...");
        rle.clear();
        if (!(rle.empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test feed(vector)
        debug(2, cout << "    test feed(vector)...");
        rle.feed( { 1, 2, 3 } );
        if (!(!rle.empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test restart()
        debug(2, cout << "    test restart()...");
        rle.restart(0);
        if (!(!rle.empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test restart(vector)
        debug(2, cout << "    test restart(vector)...");
        rle.restart( { 1, 2, 3 } );
        if (rle.score().key != NOMOVE) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test FORCED avoidance
        debug(2, cout << "    test FORCED avoidance...");
        rle.clear();
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(2);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(3);
        if (rle.score().key != NOMOVE) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test score().empty()
        debug(2, cout << "    test score().empty()...");
        rle.clear();
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test false FORCED detection
        debug(2, cout << "    test false FORCED detection...");
        rle.clear();
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(0);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test FORCED detection
        debug(2, cout << "    test FORCED detection...");
        rle.clear();
        rle.feed(1);
        if (rle.key != ZERO) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (rle.key != ZERO) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(0);
        if (rle.key != FORCED) { debug(2, cout << "failed.\n"); return false; }
        if (rle.value != 2) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test score()
        debug(2, cout << "    test score()...");
        if (rle.score().key != FORCED) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test !score().empty()
        debug(2, cout << "    test !score().empty()...");
        if (rle.score().empty()) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test reverse FORCED detection
        debug(2, cout << "    test reverse FORCED detection...");
        rle.clear();
        rle.feed(0);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.key == FORCED)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 0)) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test WINNER avoidance
        debug(2, cout << "    test WINNER avoidance...");
        rle.clear();
        rle.feed(1);
        if (!rle.score().empty()) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(2);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(2);
        if (!(rle.key == NOMOVE)) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test WINNER detection
        debug(2, cout << "    test WINNER detection...");
        rle.clear();
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.key == WINNER)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 1)) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test WINNER.value correctness
        debug(2, cout << "    test WINNER.value correctness...");
        rle.clear();
        rle.feed(2);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(2);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(2);
        if (!(rle.key == WINNER)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 2)) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test FORCED to WINNER transition
        debug(2, cout << "    test FORCED to WINNER transition...");
        rle.clear();
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(0);
        if (!(rle.key == FORCED)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 2)) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        if (!(rle.key == FORCED)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 1)) { debug(2, cout << "failed.\n"); return false; }
        rle.feed(1);
        rle.feed(1);
        if (!(rle.key == WINNER)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 1)) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test set_zero()
        debug(2, cout << "    test set_zero()...");
        rle.clear();
        rle.set_zero('0');
        rle.feed('X');
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed('X');
        if (!(rle.score().empty())) { debug(2, cout << "failed.\n"); return false; }
        rle.feed('0');
        if (!(rle.key == FORCED)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 2)) { debug(2, cout << "failed.\n"); return false; }
        rle.restart( { '0', 'X', 'X' } );
        if (!(rle.key == FORCED)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 0)) { debug(2, cout << "failed.\n"); return false; }
        rle.feed('X');
        if (!(rle.key == WINNER)) { debug(2, cout << "failed.\n"); return false; }
        if (!(rle.value == 'X')) { debug(2, cout << "failed.\n"); return false; } else { debug(2, cout << "passed.\n"); }

        // test teardown
        Base = orig_base;

        debug(2, cout << "RLE unit tests passed successfully.\n");
        debug(2, cout << "\n");
        return true;
    }


};  // end of class RLE

#endif /* rle_h */
