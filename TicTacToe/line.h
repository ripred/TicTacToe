//
//  line.h
//  TicTacToe
//
//  Created by trent on 8/7/21.
//

#ifndef line_h
#define line_h

#include "common.h"
#include "score.h"
#include "rle.h"

struct Line {
public:
    int const   m_offset;   // position on board where this line starts
    int const   m_delta;    // delta to add to get to next cell in this line
    Score       m_results;  // results after loading the cells and analyzing

    // BUGBUG! Finish implementing faster reference-cells!
    vector<int> m_cells;


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


    inline int get_index(vector<int> const &cells) const {
        int num = int(cells.size());
        int index = 0;
        int i, k, cur;

        for (i=0; i < num; ++i) {
            cur = cells[num - i - 1];
            for (k=0; k < i; ++k) {
                cur *= Base;
            }
            index += cur;
        }

        return index;
    } // Line::get_index(vector<int> const &cells)


    inline Score score(vector<int> &cells) {
        extern std::map<int, Score> scoremap;
        m_results = scoremap[get_index(cells)];
        return m_results;
    } // Line::score(vector<int> &cells)


public:
    Line(int const offset, int const delta) :
        m_offset(offset),
        m_delta(delta) {
    } // Line::Line(int const offset, int const delta)


    Score toGlobal(Score &local) {
        switch (local.key) {
            case 0:
            case NOMOVE:
            case WINNER:
                return local;
            case FORCED:
                local.value = m_offset + m_delta * local.value;
                return local;
            case RANDOM1:
            case RANDOM2:
                for (int &choice : local.choices) {
                    choice = m_offset + m_delta * choice;
                }
                return local;
            default:
                assert(false);
        }
    }

    /**
     * @summary: Load the cell values for this line.
     * 'Grid' number of cells are loaded.
     *
     * @param board the array of board values
     * @param cells the container to load this line's values into
     *
     * @returns: { WINNER, who } if game is won
     *           { FORCED, index } if block or win move available
     *           0 otherwise
     */
    inline Score load(int const board[], vector<int> &cells) {
        int index, item, i;
        RLE rle1;

        for (i=0; i < Grid; ++i) {
            index = m_offset + m_delta * i;
            item = board[index];
            cells.push_back(item);
            rle1.feed(item);
        }

        m_results = rle1.score();

        assert(rle1.value >= 0 && rle1.value < Grid * Grid);

        return m_results;
    } // Line::load(...)


    inline Score process(int const board[]) {
        vector<int> cells;

        // load the cell values
        load(board, cells);
        m_results = toGlobal(m_results);
        switch (m_results.key) {
            case ZERO:
                break;

            case NOMOVE:
            case WINNER:
            case FORCED:
                return m_results;

            case RANDOM1:
            case RANDOM2:
                assert(false);  // random's should not be generated from load()
                return m_results;
        }

        // lookup the Score for this Board state
        score(cells);
        m_results = toGlobal(m_results);
        switch (m_results.key) {
            case 0:
            case NOMOVE:
            case WINNER:
            case FORCED:
                return m_results;
            case RANDOM1:
            case RANDOM2:
                m_results.value = m_results.choices[rand() % m_results.choices.size()];
                return m_results;
        }

        // should never get here
        assert(false);
    } // Line::process(...)

};  // class Line

#endif /* line_h */
