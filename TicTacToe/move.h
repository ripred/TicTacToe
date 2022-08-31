///
///  @file move.h
///  @brief the declaration and definition of the Move class
///
///  @author trent m. wyatt
///  @date August 7, 2021
///

#ifndef move_h_inc
#define move_h_inc

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "common.h"

/// @brief
/// The Move class represents the status of a Line and indicates any response that may be required.
///
/// The Move ranking system is the prime factor in the InARow engine strategy.
///
/// At any given time a Line of board cells is always in one of the following states:
///
///     Enum       Description
///     =======================================================================================
///     WINNER     The Line contains 'Base' cells in a row for all one side (player).
///                The game is over with a Winner.
///     NOMOVE     The Line is completely filled with a mix of both players but with no winner.
///                The Line cannot be affected.
///                No Move available.
///     FORCED     The Line has an open cell that would win the game for one of the sides.
///                This is considered a forced move regardless of which side is moving because
///                either this move must be blocked to prevent the other side from winning or
///                this move should be made in order to win the game.
///     RANDOM1    The Line has one or more open cells and contains only one sides pieces.
///     RANDOM2    The Line has one or more open cells and contains both sides pieces.
///
struct Move {
    movetype_e  key;
    int         value;
    vector<int> choices;

    /// @name to_string(int const flag=1) const
    /// @brief return a human readable string for this Move
    /// @param flag if set to 0 then suppress the human readable Move choices
    /// @returns this Move as a human readable string
    string to_string(int const flag=1) const {
        string key_str, value_str, choices_str;

        if (flag > 0) {
            choices_str = " choices: " + ::to_string(choices);
        }

        key_str = "{ key: ";
        key_str += \
            key == ZERO    ? "ZERO   " :
            key == NOMOVE  ? "NOMOVE " :
            key == FORCED  ? "FORCED " :
            key == RANDOM1 ? "RANDOM1" :
            key == RANDOM2 ? "RANDOM2" :
            key == WINNER  ? "WINNER " : itoa(key, 10, 7);

        value_str = " value: " + (UseCoords ? coords(value, Grid) + " (" + itoa(value) + ")" : itoa(value));
        
        string result = key_str + value_str + choices_str + " }";

        return result;
    } // Move::to_string()


    /// @name Move()
    /// @brief default constructor for a Move
    Move() {
        key = ZERO;
        value = 0;
    } // Move::Move()


    /// @name Move(movetype_e const k, int const v)
    /// @brief construct a Move with the given move type and value
    /// @param k The move type
    /// @param v The move value
    Move(movetype_e const k, int const v) {
        key = k;
        value = v;
    } // Move::Move(int const k, int const v)


    /// @name Move(movetype_e const k, int const v)
    /// @brief construct a Move with the given move type, value, and choices
    /// @param k The move type
    /// @param v The move value
    /// @param c vector of choices
    Move(movetype_e const k, int const v, vector<int> const &c)
        : key(k), value(v), choices(c) {
    } // Move::Move(int const k, int const v, vector<int> const &c)


    /// @name clear()
    /// @brief default the Move and clear all choices
    void clear() {
        key = ZERO;
        value = 0;
        choices.clear();
    } // Move::clear()


    /// @name empty()
    /// @brief see if this contains a legal Move
    /// @returns true if this is a legal move
    bool empty() const {
        return key == ZERO &&
               value == 0 &&
               choices.empty();
    } // Move::empty()

};  // end of Move class/struct

#endif // #ifdef move_h_inc
