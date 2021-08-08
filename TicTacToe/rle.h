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

};  // end of class RLE

#endif /* rle_h */
