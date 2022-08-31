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
#include <ctype.h>
#include <time.h>
#include <map>

#include "common.h"
#include "move.h"
#include "line.h"

using std::stringstream;
using std::ostream;
using std::pair;
using std::cout;
using std::cerr;
using std::cin;
using std::map;

#ifdef GRID
const int Grid   = GRID;
#else
const int Grid   = 7;
#endif

#ifdef BASE
int       Base   = BASE;
#else
int       Base   = 7;
#endif

int       DbgLvl = 1;
bool      Human       = false;
bool      UseCoords   = true;
bool      Legend      = true;
bool      ShowChoices = false;
bool      UseAnsi     = false;

///
/// @
///
///
class TimeUsed {
private:
    double   *m_accum;
    clock_t   m_start;
public:

    TimeUsed(double& accumulator)
        : m_accum(&accumulator) {
        m_start = clock();
    }

    ~TimeUsed() {
        double cpu_time_used = ((double) (clock() - m_start)) / CLOCKS_PER_SEC;
        *m_accum += cpu_time_used;
        m_accum = nullptr;
    }
};


string const to_string(vector<int> const &v) {
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
    static char constexpr m_dispPieces[3] { '.', 'O', 'X' };
    int           m_board[Grid * Grid];
    vector<Line>  m_lines;
    int           m_lastmove;
    vector<int>   m_windexes;
    vector<Move> m_history;
    double        m_tm_total;

public:


    InARowGame() {
        init();
        m_tm_total = 0.0;
    } // InARowGame::InARowGame()

    
    void init() {
        init_board();
        init_lines();
    } // InARowGame::init()

    // perform a sanity check on a board index
    void validate_index(const int index, string const &errmsg="", const int stop=0) {
        if (index < 0 || index >= Grid * Grid) {
            cout << "invalid board index: " << index << " " << errmsg;
            if (stop) assert(false);
        }
    }


    // perform a sanity check on a line
    void validate_line(Line const &line, string const &errmsg="", const int stop=0) {
        // validate the member values
        validate_index(line.m_offset, "m_offset", 1);

        for (int i=0; i < Base; ++i) {
            int index = line.m_offset + line.m_delta * i;
            stringstream ss;
            ss << "line cell member " << i << " ";
            validate_index(index, ss.str(), 1);
        }
    };

    void init_lines() {
        assert(Grid >= Base);
        const int slack = Grid - Base;
        
        m_lines.clear();

        debug(2, cout << "Generated Lines:\n");

        // create horizontal lines
        for (int i=0; i < Grid; ++i) {
            for (int k=0; k <= slack; ++k) {
                debug(3, cout << ".");
                stringstream ss;
                ss << "Check Line: " << i << " ";
                m_lines.push_back( { Grid * i + k, 1 } );
                validate_line(m_lines.back(), ss.str(), 1);
            }
        }

        // create vertical lines
        for (int i=0; i < Grid; ++i) {
            for (int k=0; k <= slack; ++k) {
                debug(3, cout << ".");
                stringstream ss;
                ss << "Check Line: " << i << " ";
                m_lines.push_back( { i + k * Grid, Grid } );
                validate_line(m_lines.back(), ss.str(), 1);
            }
        }

        // create diagonal lines
        for (int i=0; i <= slack; ++i) {
            for (int k=0; k <= slack; ++k) {
                debug(3, cout << "..");
                stringstream ss;
                m_lines.push_back( { Grid * i + k, Grid + 1 } );
                ss << "Check Line: " << i << " ";
                validate_line(m_lines.back(), ss.str(), 1);

                ss.clear();
                ss << "Check Line: " << i << " ";
                m_lines.push_back( { Grid * i + ((Grid - 1) - k), Grid - 1 } );
                validate_line(m_lines.back(), ss.str(), 1);
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
        m_history.clear();
    } // InARowGame::init_board()


    void show_lines() {
        int num = 1;
        for (const Line& line : m_lines) {
            init_board();
            for (int i=0; i < Base; ++i) {
                m_board[line.m_offset + line.m_delta * i] = 2;
            }
            debug(1, cout << "Line " << num++ << ":\n");
            display();
            debug(1, cout << "\n");
        }
    }


    void display(bool showLegend=Legend) {
        string legend;

        if (showLegend) {
            legend = "  ";
            for (int index=0; index < Grid; ++index) {
                legend += itoa(index, 26, 2);
            }
            legend += "\n";
        }

        debug(1, cout << legend);

        for (int index=0; index < Grid * Grid; ++index) {
            bool highlight = index == m_lastmove;
            for (const int & w : m_windexes) {
                if (w == index) {
                    highlight = true;
                    break;
                }
            }

            legend.clear();
            if ((index % Grid == 0) && showLegend) {
                legend = " ";
                legend += 'A' + index / Grid;
                legend += " ";
            }

            debug(1, cout
                << legend
                << (UseAnsi && highlight ? boldAttr : "")
                << m_dispPieces[m_board[index]]
                << (UseAnsi && highlight ? resetAttr : "")
                <<  (index % Grid < (Grid-1) ? " " : "\n"));
        }
    } // InARowGame::display()


    Move human_move() const {
        while (true) {
            cout << "Enter the square to move to (0-" << (Grid * Grid) - 1 << "): ";
            cout.flush();
            int n = -1;
            if (Legend) {
                string in;
                cin >> in;
                if (in.length() == 2) {
                    char c1 = tolower(in[0]);
                    char c2 = tolower(in[1]);
                    c1 -= 'a';
                    if (c2 >= 'a') {
                        c2 -= 'a';
                    } else {
                        c2 -= '0';
                    }
                    n = c1 * Grid + c2;
                }
            } else {
                cin >> n;
            }

            if (n < 0 || n >= (Grid * Grid) || m_board[n] != 0) {
                cout << "invalid square.\n";
            } else {
                return Move(FORCED, n);
            }
        }
    } // InARowGame::human_move()


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
    inline Move analyze() {
        map<movetype_e, Move> moves;
        map<int, map<int, int>> counts;
        Move score;

        for (Line &line : m_lines) {
            Move s = line.process(m_board);
            if (s.key == RANDOM1 || s.key == RANDOM2) {
                for (int c : s.choices) {
                    moves[s.key].choices.push_back(c);
                    counts[s.key][c]++;
                }
            } else if (s.key == WINNER) {
                moves[s.key] = s;
                m_windexes = s.choices;
            } else {
                moves[s.key] = s;
            }
        }

        assert(!moves.empty());

        // Maps are sorted by their keys.
        // We take the first one since key (movetype_e) values
        // are defined in order of their precedence
        // thus the highest precedence score is first

        score = (*(moves.rbegin())).second;
        score.key = (*(moves.rbegin())).first;
        size_t movetypes = moves.size();
        size_t cp = movetypes;
        cp = cp ? cp : 0;

        if (score.key == RANDOM1 || score.key == RANDOM2) {
            vector<int> choices;
            int highest_count = 0;
            for (auto count : counts[score.key]) {
                if (count.second > highest_count)
                    highest_count = count.second;
            }
            for (auto count : counts[score.key]) {
                if (count.second == highest_count)
                    choices.push_back(count.first);
            }
            score.choices = choices;
        }

        switch (score.key) {
            case WINNER:
                debug(1, cout << "\n");
                display();
                debug(1, cout << "\n" << m_dispPieces[score.value] << " Wins!\n");
                return score;

            case NOMOVE:
                debug(1, cout << "\n");
                display();
                debug(1, cout << "\nDraw!\n");
                return score;

            case FORCED:
                return score;

            case RANDOM1:
                score.value = score.choices[rand() % score.choices.size()];
                return score;

            case RANDOM2:
                score.value = score.choices[rand() % score.choices.size()];
                return score;

            case ZERO:
            default:
                display();
                cout << "invalid Move (move stance): " << score.to_string() << "\n";
                if (score.key == ZERO)
                    cout << "Move stance is ZERO - must choose at least one available move from all Lines.\n";
                assert(false);
                break;
        }

        assert(false);
    } // InARowGame::analyze()


    Move make_move(Move const &result, const int turn, bool flag=true) {
        const int player = ((turn == 0) ? 1 : 2);

        switch (result.key) {
            case ZERO:
                assert(false);

            case NOMOVE:    // no open spots are available?
                return result;
                
            case WINNER:    // game has been won
                assert(m_windexes.size() == Base);
                return result;

            case FORCED:    // must move to spot to either block or win?
                m_history.push_back(result);
                m_lastmove = result.value;
                m_board[m_lastmove] = player;
                debug(1, cout << "making move: " << result.to_string(flag) << "\n");
                return result;

            case RANDOM1:
                m_history.push_back(result);
                m_lastmove = result.value;
                m_board[m_lastmove] = player;
                debug(1, cout << "making move: " << result.to_string(flag) << "\n");
                return result;

            case RANDOM2:
                m_history.push_back(result);
                m_lastmove = result.value;
                m_board[m_lastmove] = player;
                debug(1, cout << "making move: " << result.to_string(flag) << "\n");
                return result;
        }

        assert(false);
    } // InARowGame::make_move(Move const &result, int turn)


    inline Move process(const int turn) {
        // get the next move
        Move result = (Human && turn) ? human_move() : analyze();

        // process the move
        assert(result.key != ZERO);
        make_move(result, turn, ShowChoices);
        
        result = analyze();

        return result;
    } // InARowGame::process(const int turn)


    string const state() const {
        string res;
        for (const int & c : m_board) {
            res += m_dispPieces[c];
        }
        
        return res;
    }
    
};  // class InARowGame


void playback(InARowGame &board) {
    vector<Move> moves = board.m_history;
    Move result;

    board.init();
    for (int i=0; i < moves.size(); ++i)  {
        result = moves[i];
        debug(1, cout << "\nturn " << i + 1 << ":\n");
        board.display();
        board.make_move(result, (i + 1) & 1, ShowChoices);
        result = board.analyze();
        if (result.key == NOMOVE || result.key == WINNER) {
            break;
        }
    }
}

Move tictactoe(InARowGame &board) {
    Move result;
    TimeUsed timer(board.m_tm_total);
    
    for (int turn=1; turn <= Grid * Grid; ++turn) {
        debug(1, cout << "\nturn = " << commas(turn) << "\n");
        board.display();
        result = board.process(turn & 1);
        if (result.key == NOMOVE || result.key == WINNER) {
            break;
        }
    }
    
    return result;
} // tictactoe(InARowGame &board)


//#include <unistd.h> // for sleep()


#include <tuple>
using std::tuple;

map<string, string> options;

tuple<string, string> process_param(int argc, char * argv[]) {
    enum { PARM_KEY, PARM_VALUE } state = PARM_KEY;
    string key, value;

    for (int index=1; index < argc; ++index) {
        string param = argv[index];

        switch (state) {
            case PARM_KEY:
                key = param;
                state = PARM_VALUE;
                continue;

            case PARM_VALUE:
                if (param != "=" && param != ":") {
                    value = param;
                    return { key, value };
                }
                state = PARM_VALUE;
                break;
        }
    }

    return { key, value };
}

int process_cmdline(int argc, char *argv[]) {
    for (int index=1; index < argc; ++index) {
        string param = argv[index];
        if (!param.empty()) {
            string key, value;

            if (!param.empty() && param[0] == '-') {
                param = &param[1]; // strip it
                if (!param.empty() && param[0] != '-') {
                    // handle "-" prefixed option
                    param = &param[1];

                    tuple<string, string> result = process_param(argc, argv);
                    key = std::get<0>(result);
                    value = std::get<1>(result);
                } else {
                    // handle "--" prefixed option
                    param = &param[1]; // strip it

                    tuple<string, string> result = process_param(argc, argv);
                    key = std::get<0>(result);
                    value = std::get<1>(result);
                }
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    // wins, losses, draws
    int results[3] {};
    double time_used = 0.0;

    // seed the random number generator
    srand((unsigned) time(nullptr));

    int increment = 1000;
    
    map<string, size_t> variations;
    InARowGame board;
    int num_games = 0;

    int count = increment;

#ifdef USEANSI
    UseAnsi = true;
#endif

#ifdef HUMAN
    Human = true;
#endif

#ifdef DEBUG
    DbgLvl = 1;
#endif

    {
    TimeUsed timer(time_used);

    while (count--) {
        Move result = tictactoe(board);
        
        ++num_games;
        
        switch (result.key) {
            default:        cout << "bug: game finished with " << result.to_string() << "\n"; board.display(); assert(false);
            case RANDOM1:   cout << "bug: game finished with status of RANDOM1?\n"; break;
            case RANDOM2:   cout << "bug: game finished with status of RANDOM2?\n"; break;
            case FORCED:    cout << "bug: game finished with status of FORCED?\n";  break;
            case ZERO:      cout << "bug: game finished with status of ZERO?\n";    break;
            case WINNER:
                results[result.value - 1]++;
                if (variations.find(board.state()) == variations.end()) {
                    size_t table_size = variations.size();
                    variations[board.state()] = table_size;
                    count = ((count / increment) + 1) * increment;
                    //cout << board.state() << " -> " << table_size << "    " << "\n" ;
                }
                break;

            case NOMOVE:
                results[2]++;
//                if (variations.find(board.state()) == variations.end()) {
//                    size_t table_size = variations.size();
//                    variations[board.state()] = table_size;
//                    count = ((count / increment) + 1) * increment;
//                    cout << board.state() << " -> " << table_size << "    " << "\n" ;
//                }
                break;
        }

//        if (!Human && result.key == WINNER) {
//            DbgLvl = 1;
//            playback(board);
//            break;
//        }

        if (DbgLvl == 0 && count != 0 && count % 100 == 0)
            cout << commas(count) << "\n";
        
        DbgLvl = 0;
        
        board.init_board();
    }
    }

    DbgLvl = 1;

    cout << "\n";

    cout << "Results[0] = " << commas(results[0]) << "\n";
    cout << "Results[1] = " << commas(results[1]) << "\n";
    cout << "Results[2] = " << commas(results[2]) << "\n";

    cout << "\n";

    cout << "Grid Width: " << Grid << "\n";
    cout << "Total games: " << num_games << "\n";
    cout << "Total spots: " << Grid * Grid << "\n";

    char buff[128];
    sprintf(buff, "%g", time_used);
    cout << "Total time: " << buff << " seconds\n";
    sprintf(buff, "%g", time_used / num_games);
    cout << "Avg per game: " << buff << " seconds\n";

    cout << variations.size() << " Variations\n";

    cout << "\n";

    if ((0)) {
        
        int line = 0;
        bool do_disp = true;
        for (auto dv : variations) {
            string const &contents = dv.first;
            //size_t const &table_size = dv.second;
            cout << ++line << ") ";
            if (do_disp) {
                cout << "\n";
                for (int digit=0; digit < contents.size(); ++digit)
                    cout << contents[digit] << ((digit+1) % Grid == 0 ? "\n" : " ");
            } else {
                cout << contents << "\n";
            }
        }
    }
}
