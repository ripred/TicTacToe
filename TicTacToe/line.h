///
///  @file line.h
///  @brief the declaration of the Line class
///
///  @author trent m. wyatt
///  @date August 7, 2021
///

#ifndef line_h
#define line_h

#include <iostream>
using std::ostream;

#include <map>
using std::pair;
using std::map;

#include "common.h"
#include "move.h"

/// @brief Line represents a series of board cells with a starting offset
/// into the board and a delta value to add to the current offset to get to
/// the next cell in the Line.
///
struct Line {
public:
    int const   m_offset;   // position on board where this line starts
    int const   m_delta;    // delta to add to get to next cell in this line
    vector<int> m_cells;    // the cell's contents that make up this line
    Move       m_results;   // results after loading the cells and analyzing


    string to_string(void) const {
        stringstream ss;
        ss \
          << (m_delta == 1 ? " H" : m_delta == Grid ? " V" : m_delta == (Grid + 1) ? "D+" : "D-")
          << " { offset: " << itoa(m_offset, 10, 2)
          << " delta: " << itoa(m_delta, 10, 2)
          << " results: " << m_results.to_string()
          << " }";

        return ss.str();
    } // Line::to_string()

    /**
     *
     */
    Move evaluate(const int board[]) {
        vector<int> moves, randset;
        map<int, int> values;
        int which;

        m_cells.clear();
        for (int i=0; i < Base; ++i) {
            const int index = m_delta * i + m_offset;
            m_cells.push_back(board[index]);
            moves.push_back(index);
        }
        
        // count the value occurrances for 0, 1, or 2 for this line
        for (int i=0; i < m_cells.size(); ++i) {
            const int c = m_cells[i];
            if (c == 0) randset.push_back(m_delta * i + m_offset);
            values[c]++;
        }

        which = randset.empty() ? 0 : randset[rand() % randset.size()];
        const pair<const int, const int> &entry = *(values.begin());
        switch (values.size()) {
            case 3:
                // this line involves both players (values 1 and 2) and empty spots (value 0)
                if (Grid - randset.size() >= Base) {
                    return Move(RANDOM1, which, randset);
                }
                return Move(RANDOM2, which, randset);

            case 2:
                // only 2 values involved; (0,1), (0, 2), or (1, 2)
                if (entry.first == 0) {                     // if there are empty spot(s)
                    if (entry.second <= 1) {                // and only 1 empty spot
                        return Move(FORCED, which);        // we know we only have one empty (0) so randset[0] is it's index
                    }
                    return Move(RANDOM1, which, randset);  // pick from one of the empty spots
                }
                return Move(NOMOVE, 0);

            case 1:
                // all cells have the same value
                if (entry.first == 0) {                     // if they are all empty
                    return Move(RANDOM1, which, randset);  // pick one of them
                }

                // the game has been won.
                // remember the indexes of the winning spots
                for (int i=0; i < m_cells.size(); ++i) {
                    randset.push_back(m_delta * i + m_offset);
                }

                return Move(WINNER, entry.first, moves);

            default:
                std::cerr << "error - values.size() = " << values.size() << "\n";
                break;
        }

        assert(false);
    } // Line::evaluate()
    

public:
    Line(int const offset, int const delta) :
        m_offset(offset),
        m_delta(delta) {
    } // Line::Line(int const offset, int const delta)


    inline Move process(int const board[]) {
        m_results = evaluate(board);

        return m_results;
    } // Line::process(...)

};  // class Line

#endif /* line_h */
