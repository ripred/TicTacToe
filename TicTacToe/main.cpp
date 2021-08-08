/**
 * @file main.cpp
 * @author trent m. wyatt
 * @date August 2021
 *
 * @summary Game engine to play "In A Row" type games such
 * as Tic-Tac-Toe and Gomoku.
 *
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <map>

#include "common.h"
#include "score.h"
#include "line.h"
#include "rle.h"

using std::stringstream;
using std::ostream;
using std::pair;
using std::cout;
using std::cerr;
using std::cin;
using std::map;

int const Grid   = 5;
int       Base   = 3;
int       Human  = 0;
int       DbgLvl = 3;
bool      UseAnsi = false;

vector<vector<int>> randsets;
MoveTable scoremap;


string to_string(vector<int> const &v) {
    if (v.empty()) return "{ }";

    stringstream ss;
    ss  << "{ ";
    int num = 0;
    for (auto item : v) {
        ss << item;
        if (num++ < v.size() - 1) {
            ss << ", ";
        }
    }
    ss  << " }";

    return ss.str();
} // to_string(vector<int> const &v)


class InARowGame {
public:
    char const    m_dispPieces[3] { '.', 'O', 'X' };
    int           m_board[Grid * Grid];
    vector<Line>  m_lines;
    int           m_lastmove;
    vector<int>   m_windexes;

public:


    InARowGame() {
        init();
    } // InARowGame::InARowGame()

    
    void init() {
        init_board();
        init_lines();
    } // InARowGame::init()

    
    void init_lines() {
        assert(Grid >= Base);
        int const slack = Grid - Base;
        
        m_lines.clear();

        auto check_line = [&](Line const &line, int num) -> void {
            // perform a sanity check on the lines
            for (int i=0; i < Base; ++i) {
                if ((line.m_offset + line.m_delta * i) >= Grid * Grid) {
                    cout << "Found a Check Line that is partialy out of bounds (entry " << num << "):\n";
                    cout << line.to_string() << "\n";
                    assert(false);
                }
            }
        };

        debug(2, cout << "Generated Lines:\n");

        // create horizontal lines
        for (int i=0; i < Grid; ++i) {
            for (int k=0; k <= slack; ++k) {
                debug(3, cout << ".");
                m_lines.push_back( { Grid * i + k, 1 } );
                check_line(m_lines.back(), int(m_lines.size()) - 1);
            }
        }

        // create vertical lines
        for (int i=0; i < Grid; ++i) {
            for (int k=0; k <= slack; ++k) {
                debug(3, cout << ".");
                m_lines.push_back( { i + k * Grid, Grid } );
                check_line(m_lines.back(), int(m_lines.size()) - 1);
            }
        }

        // create diagonal lines
        for (int i=0; i <= slack; ++i) {
            for (int k=0; k <= slack; ++k) {
                debug(3, cout << "..");
                m_lines.push_back( { Grid * i + k, Grid + 1 } );
                check_line(m_lines.back(), int(m_lines.size()) - 1);
                m_lines.push_back( { Grid * i + ((Grid - 1) - k), Grid - 1 } );
                check_line(m_lines.back(), int(m_lines.size()) - 1);
            }
        }
        debug(3, cout << "\n");

        int num = 0;
        for (auto const &line : m_lines) {
            debug(2, cout << itoa(num++, 10, 3) << " " << line.to_string() << "\n");
        }
        debug(2, cout << "\n");
        
    } // InARowGame::init_lines()

    
    void init_board() {
        for (int n=0; n < Grid * Grid; ++n) {
            m_board[n] = 0;
        }
        m_windexes.clear();
        m_lastmove = -1;
    } // InARowGame::init_board()


    void display() {
        for (int ndx=0; ndx < Grid * Grid; ++ndx) {
            bool highlight = ndx == m_lastmove;
            for (int k=0; k < m_windexes.size(); ++k) {
                if (m_windexes[k] == ndx) {
                    highlight = true;
                    break;
                }
            }
            debug(1, cout
                << (UseAnsi && highlight ? boldAttr : "")
                << m_dispPieces[m_board[ndx]]
                << (UseAnsi && highlight ? resetAttr : "")
                <<  (ndx % Grid < (Grid-1) ? " " : "\n"));
        }
    } // InARowGame::display()


    /**
     * @summary: Analyze all lines.
     *           The cells for each check line will be loaded from the
     *           board and examined.
     *
     * @returns: { WINNER, {1 or 2} } = line contains 'Base' pieces in a row; Win.
     *           { NOMOVE, 0 }        = no open cells available; Draw.
     *           { FORCED, pos }      = must move at cell 'pos' to block or win.
     *           { 0, 0 }             = lookup pattern in table
     */
    inline Score analyze() {
        Line *best = nullptr;

        for (Line &line : m_lines) {
            // load and analyze the line's cells
            line.process(m_board);

            if (best == nullptr) {
                best = &line;
            } else {
                if (line.m_results.key > (*best).m_results.key) {
                    best = &line;
                }
            }
        }

        Score which;
        if (nullptr != best) {
            which = (*best).m_results;
        }
        
        int count = 0;
        for (int const &c : m_board) if (c != 0) ++count;
        if (Grid * Grid == count) {
            which = Score(NOMOVE, 0);
        }

        switch (which.key) {
            case ZERO:
            case FORCED:
            case RANDOM1:
            case RANDOM2:
                break;

            case NOMOVE:
                debug(1, cout << "\nDraw\n");
                display();
//              m_lastmove = -1;
                break;

            case WINNER:
                // save the winning spots
                m_windexes.clear();
                for (int i=0; i < Base; ++i) {
                    m_windexes.push_back((*best).m_offset + (*best).m_delta * i);
                }
                display();
                debug(1, cout << "\n" << m_dispPieces[which.value] << " Wins!\n");
                break;

            default:
                cout << "invalid Score.key: " << which.key << "\n";
                assert(false);
        }
    
        return which;
    } // InARowGame::analyze()


    Score human_move() const {
        while (true) {
            cout << "Enter the square to move to (0-8): ";
            cout.flush();
            int n = -1;
            cin >> n;
            if (n < 0 || n >= (Grid * Grid) || m_board[n] != 0) {
                cout << "invalid square.\n";
            } else {
                return Score(FORCED, n);
            }
        }
    } // InARowGame::human_move()


    Score computer_move(Score &result, int const turn) {
        // default to no available moves
        result = Score(NOMOVE, 0);

        // take center if available
        if (result.key == NOMOVE) {
            int index = (Grid / 2) * Grid + (Grid / 2);
            if (m_board[index] == 0) {
                result = Score(FORCED, index);
            }
        }

        if (result.key == NOMOVE) {
            // take any forced moves
            int linenum = 0;
            for (Line const &line : m_lines) {
                if (line.m_results.key == FORCED) {
                    result = Score(FORCED, line.m_results.value);

                    // perform sanity check on chosen move:
                    if (m_board[result.value] != 0) {
                        cout << "error: m_lines[" << linenum << "] spot not empty: "
                             << m_board[result.value]
                             << "\nLine = " << line.to_string() << "\n\n";
                        DbgLvl = 3;
                        display();
                        cout << "\n\n";
                        assert(false);
                    }
                    break;
                }
                linenum++;
            }
        }

        if (result.key == NOMOVE) {
            // take any random moves from lines
            // with 2 or more spots open:
            map<int, int> choices;

            // keep track of the spot usage counts from ALL lines
            int top = 0;

            // collect all RANDOM1 moves from ALL lines sorted by
            // spot usage count:
            for (Line const &line : m_lines) {
                if (line.m_results.key == RANDOM1) {
                    for (int const &choice : line.m_results.choices) {
                        if (++choices[choice] > top) {
                            top = choices[choice];
                        }
                    }
                }
            }

            // choose from only top used spots (if any)
            if (!choices.empty()) {
                vector<int> choices2;
                for (auto const &choice : choices) {
                    if (choice.second >= top) choices2.push_back(choice.first);
                }

                assert(!choices2.empty());

                result = Score(RANDOM1, choices2[rand() % choices2.size()], choices2);

                // perform sanity check on chosen move:
                if (m_board[result.value] != 0) {
                    cout << "error: result.key == RANDOM1 but spot ("
                         << result.value << ") not empty: "
                         << m_board[result.value] << "\n\n";
                    DbgLvl = 3;
                    display();
                    cout << "\n\n";
                    assert(false);
                }
            }
        }

        if (result.key == NOMOVE) {
            // take any random moves from lines with only one spot
            // open on line with mixed content (both players):
            map<int, int> choices;

            // keep track of the spot usage counts from ALL lines
            int top = 0;

            // collect all RANDOM2 moves from ALL lines sorted by
            // spot usage count:
            for (Line const &line : m_lines) {
                if (line.m_results.key == RANDOM2) {
                    for (int const &choice : line.m_results.choices) {
                        if (++choices[choice] > top) {
                            top = choices[choice];
                        }
                    }
                }
            }

            // choose from only top used spots (if any)
            if (!choices.empty()) {
                vector<int> choices2;
                for (auto const &choice : choices) {
                    if (choice.second >= top) choices2.push_back(choice.first);
                }

                assert(!choices2.empty());

                result = Score(RANDOM2, choices2[rand() % choices2.size()], choices2);

                // perform sanity check on chosen move:
                if (m_board[result.value] != 0) {
                    cout << "error: result.key == RANDOM2 but spot ("
                         << result.value << ") not empty: "
                         << m_board[result.value] << "\n\n";
                    DbgLvl = 3;
                    display();
                    cout << "\n\n";
                    assert(false);
                }
            }

            assert(result.key == NOMOVE
                || result.key == WINNER
                || result.key == FORCED
                || result.key == RANDOM1
                || result.key == RANDOM2);
        }

        return result;
    } // InARowGame::computer_move(Score &result, int const turn)


    Score handle_move(Score result, int const turn) {
        int const player = ((turn == 0) ? 1 : 2);
        int winner;

        switch (result.key) {
            case NOMOVE:    // no open spots are available?
                for (int spot=0; spot < Grid * Grid; ++spot) {
                    if (m_board[spot] == 0) {
                        cout << "error NOMOVE: empty board spot found (" << spot << ") but\n"
                             << "    result == " << result.to_string() << "\n";
                        assert(false);
                    }
                }
                return result;
                
            case WINNER:    // game has been won
                assert(m_windexes.size() == Base);
                winner = result.value;
                for (int const &winspot : m_windexes) {
                    if (m_board[winspot] != winner) {
                        cout << "error WINNER: board windexes not filled [" << winspot << "] = " << m_board[winspot] << "\n"
                             << "    but result == " << result.to_string() << "\n";
                        assert(false);
                    }
                }
                return result;

            // ignore other legitimate status values that don't
            // end the game unlike NOMOVE and WINNER:
            case FORCED:    // must move to spot to either block or win?
                if (result.value < 0 || result.value >= Grid * Grid) {
                    cout << "error FORCED: (invalid result.value) result == " << result.to_string() << "\n";
                }

                if (m_board[result.value] != 0) {
                    cout << "error FORCED: board[" << result.value << "] is not empty (" << m_board[result.value] << ")\n"
                         << "    but analyze() result == " << result.to_string() << "\n";
                    assert(false);
                }
                m_board[result.value] = player;
                debug(1, cout << "making move: " << result.to_string() << "\n");
                return result;

            case RANDOM1:
                if (m_board[result.value] != 0) {
                    cout << "error RANDOM1: board[" << result.value << "] is not empty (" << m_board[result.value] << ")\n"
                         << "    but analyze() result == " << result.to_string() << "\n";
                    assert(false);
                }
                m_board[result.value] = player;
                debug(1, cout << "making move: " << result.to_string() << "\n");
                return result;

            case RANDOM2:
                if (m_board[result.value] != 0) {
                    cout << "error RANDOM2: board[" << result.value << "] is not empty (" << m_board[result.value] << ")\n"
                         << "    but analyze() result == " << result.to_string() << "\n";
                    assert(false);
                }
                m_board[result.value] = player;
                debug(1, cout << "making move: " << result.to_string() << "\n");
                return result;

            case ZERO:
                return Score(ZERO, 0);

            // unknown result from analyze():
            default:
                cout << "error: unknown result of analyze(): "
                     << result.to_string() << "\n";
                assert(false);
        }
    }


    inline Score process(int const turn) {
        // get the next move
        Score result;
        
        if (Human && turn) {
            // get human player's move
            Score move = human_move();
            
            // check for win and draw
            result = analyze();

            if (result.key != WINNER && result.key != NOMOVE) {
                result = move;
            }
        } else {
            // check for win and draw
            Score preview = analyze();

            // if game is not over
            if (preview.key != WINNER && preview.key != NOMOVE) {

                // get a move for the computer
                result = computer_move(result, turn);

                // just for grins let's compare any moves suggested
                // by the preview against the deeper-scan move:
                debug(2, cout << "Just for curiosity compare analyze() move against deeper-scan move:\n");
                debug(2, cout <<
                      "analyze() move: " << preview.to_string() << "\n" <<
                      "deep-scan move: " <<  result.to_string() << "\n");

                if (preview.key > result.key) {
                    result = preview;
                }
            } else {
                result = preview;
            }
        }

        // process the move
        handle_move(result, turn);
        
        result = analyze();

        return result;
    } // InARowGame::process(int const turn)

};  // class InARowGame


/* reverse:  reverse string s in place */
void reverse(char s[]) {
    int i, j;
    char c;

    for (i=0, j=int(strlen(s) - 1); i < j; ++i, --j) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


vector<int> const gen_random_set(string &s) {
    vector<int> results;
    int position = 0;
    for (char c : s) {
        if (c == '0') {
            results.push_back(position);
        }
        position++;
    }

    assert(!results.empty());
    return results;
}


int offer(vector<int> const &v, vector<vector<int>> &rndsets) {
    int result = -1;
    auto found = find(rndsets.begin(), rndsets.end(), v);
    if (found == rndsets.end()) {
        result = int(rndsets.size());
        rndsets.push_back(v);
    } else {
        result = int(found - rndsets.begin());
    }

    assert(result != -1);
    return result;
}


void add_random1(int const j, string &pattern, ostream &value) {
    int rset = offer(gen_random_set(pattern), randsets);
    assert(scoremap.find(j) == scoremap.end());
    scoremap[j] = Score(RANDOM1, int(randsets[rset].size()), randsets[rset]);
    value << "RANDOM1 " << itoa(rset, 10, 2) << "\n";
}


void add_random2(int const j, string &pattern, ostream &value) {
    int rset = offer(gen_random_set(pattern), randsets);
    assert(scoremap.find(j) == scoremap.end());
    scoremap[j] = Score(RANDOM2, int(randsets[rset].size()), randsets[rset]);
    value << "RANDOM2 " << itoa(rset, 10, 2) << "\n";
}


void add_forced(int const j, string &pattern, ostream &value) {
    int pos = int(find(pattern.begin(), pattern.end(), '0') - pattern.begin());
    assert(scoremap.find(j) == scoremap.end());
    scoremap[j] = Score(FORCED, pos);
    value << "FORCED  " << itoa(pos, 10, 2) << "\n";
}


void add_winner(int const j, int const who, ostream &value) {
    assert(scoremap.find(j) == scoremap.end());
    scoremap[j] = Score(WINNER, who);
    value << "WINNER  " << itoa(who, 10, 2) << "\n";
}


void add_nomove(int const j, string &pattern, ostream &value) {
    assert(scoremap.find(j) == scoremap.end());
    scoremap[j] = Score(NOMOVE, 0);
    value << "NOMOVE\n";
}


Score analyze_digits(string const &pattern) {
    RLE rle1;
    for (char const c : pattern) {
        rle1.feed(c - '0');
    }
    return rle1.score();
}


void process_digits(int const j, string &pattern, map<int, int> const &digits, ostream &value) {

    Score sc = analyze_digits(pattern);
    if (sc.key == WINNER || sc.key == FORCED) {
        scoremap[j] = sc;
        value << (sc.key == WINNER ? "WINNER  " : "FORCED  ") << itoa(sc.value, 10, 2) << "\n";
        return;
    }

    pair<int const, int const> const &entry = *(digits.begin());
    switch (digits.size()) {
        case 3:
            if (entry.first == 0 && entry.second == 1) {
                add_random2(j, pattern, value);
            } else {
                add_random1(j, pattern, value);
            }
            break;
        case 2:
            if (entry.first == 0) {
                if (entry.second == 1) {
                    add_forced(j, pattern, value);
                } else {
                    add_random1(j, pattern, value);
                }
            } else {
                add_nomove(j, pattern, value);
            }
            break;
        case 1:
            if (entry.first == 0) {
                add_random1(j, pattern, value);
            } else {
                add_winner(j, entry.first, value);
            }
            break;
        default:
            value << "error - digits.size() = " << digits.size() << "\n";
            break;
    }
}


void display_rand_sets(void) {
    int rset = 0;
    cout << randsets.size() << " Random Sets:\n";
    for (auto r : randsets) {
        cout << "set " << itoa(rset++, 10, 2) << ": { ";
        int count = int(r.size());
        for (int i=0; i < count; ++i) {
            cout << r.front();
            r.erase(r.begin());
            cout << ((i < count - 1) ? ", " : " }\n");
        }
    }
    cout << "\n";
}


void makeLookupTable(int const base, char *names[] = nullptr) {
    int max = pow(3, base);
    char buff1[128];
    int len = base;

    for (int j=0; j < max; ++j) {
        itoa(j, buff1, 3, len, '0');
        string pattern = buff1;
        itoa(j, buff1, 10, len);

        stringstream ss;
        ss << "table1[" << buff1 << "] = " << pattern << " -> ";

        map<int, int> digits;
        for (char const c : pattern) {
            digits[c - '0']++;
        }
        process_digits(j, pattern, digits, ss);
        cout << ss.str();
    }

    cout << "\n";
    display_rand_sets();
}


Score tictactoe(InARowGame &board) {
    Score result;

    for (int pass=1; pass <= Grid * Grid; ++pass) {
        debug(1, cout << "\npass = " << pass << "\n");
        board.display();
        result = board.process(pass & 1);
        if (result.key == NOMOVE || result.key == WINNER) {
            break;
        }
    }

    assert(!result.empty());
    
    return result;
} // tictactoe(InARowGame &board)


// RLE unit tests
bool test_RLE() {
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


void test_bug() {
    // wins, losses, draws
    int results[3] {};

    // test bug where engine calls a draw with open cells left
    InARowGame test;
    memcpy(test.m_board, (int[Grid * Grid])
    {
        2, 0, 0,
        2, 2, 1,
        1, 0, 1
    }, sizeof(test.m_board));
    Score val;
    int turn = 5;

    do {
        cout << "pass " << turn << "\n";
        test.display();
        val = test.process(turn & 0);
        
        switch (val.key) {
            default:        cout << "bug: game finished with " << val.to_string() << "\n"; test.display(); assert(false);
            case RANDOM1:   cout << "bug: game finished with status of RANDOM1?\n"; break;
            case RANDOM2:   cout << "bug: game finished with status of RANDOM2?\n"; break;
            case FORCED:    cout << "bug: game finished with status of FORCED?\n";  break;
            case ZERO:      cout << "bug: game finished with status of ZERO?\n";    break;
            case WINNER:    results[val.value - 1]++; break;
            case NOMOVE:    results[2]++; break;
        }
        turn++;

    } while (turn < Grid * Grid && val.key != WINNER && val.key != NOMOVE && val.key != ZERO);

    cout << "Result = " << val.to_string() << "\n";
}


int main(int argc, char *argv[]) {
    // wins, losses, draws
    int results[3] {};

    DbgLvl = 0;
    bool passed = test_RLE();
    if (!passed) {
        cout << "RLE unit tests failed.\n";
        return -1;
    }

    // seed the random number generator
    srand((unsigned) time(nullptr));

    makeLookupTable(Grid);
    cout << "\n";

    InARowGame board;
    int count = 10000;

    while (count--) {
        Score result = tictactoe(board);
        
        switch (result.key) {
            default:        cout << "bug: game finished with " << result.to_string() << "\n"; board.display(); assert(false);
            case RANDOM1:   cout << "bug: game finished with status of RANDOM1?\n"; break;
            case RANDOM2:   cout << "bug: game finished with status of RANDOM2?\n"; break;
            case FORCED:    cout << "bug: game finished with status of FORCED?\n";  break;
            case ZERO:      cout << "bug: game finished with status of ZERO?\n";    break;
            case WINNER:    results[result.value - 1]++; break;
            case NOMOVE:    results[2]++; break;
        }
        board.init_board();
    }

    DbgLvl = 3;

    cout << "Results[0] = " << results[0] << "\n";
    cout << "Results[1] = " << results[1] << "\n";
    cout << "Results[2] = " << results[2] << "\n";
}
