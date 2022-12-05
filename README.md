# TicTacToe
Game engine for games requiring several pieces to be placed "In-A-Row" such as TicTacToe and Gomoku. 

The board width, height, and the number of pieces in a row required to win can all be customized.

This engine will always play a perfect game resulting in a win or a draw.

```
// example output running 1000 games against itself on a 7x7 grid requiring 5 in-a-row:

turn = 1
   0 1 2 3 4 5 6
 A . . . . . . .
 B . . . . . . .
 C . . . . . . .
 D . . . . . . .
 E . . . . . . .
 F . . . . . . .
 G . . . . . . .
making move: { key: RANDOM1 value: D3 (24) }

turn = 2
   0 1 2 3 4 5 6
 A . . . . . . .
 B . . . . . . .
 C . . . . . . .
 D . . . X . . .
 E . . . . . . .
 F . . . . . . .
 G . . . . . . .
making move: { key: RANDOM1 value: E4 (32) }


< snipped out turns 3 - 47 ... >




turn = 48
   0 1 2 3 4 5 6
 A O X O O O O X
 B O X X O X X O
 C . O O O O X O
 D X O X X X O X
 E X X X X O . O
 F X X X X X O X
 G O X O O O O X
making move: { key: RANDOM2 value: E5 (33) }

turn = 49
   0 1 2 3 4 5 6
 A O X O O O O X
 B O X X O X X O
 C . O O O O X O
 D X O X X X O X
 E X X X X O O O
 F X X X X X O X
 G O X O O O O X
making move: { key: RANDOM2 value: C0 (14) }

   0 1 2 3 4 5 6
 A O X O O O O X
 B O X X O X X O
 C X O O O O X O
 D X O X X X O X
 E X X X X O O O
 F X X X X X O X
 G O X O O O O X

Draw!
900
800
700
600
500
400
300
200
100

Results[0] = 0      // player 0 win count
Results[1] = 0      // player 1 win count
Results[2] = 1,000  //         draw count

Grid Width: 7
Total games: 1000
Total spots: 49
Total time: 1.83873 seconds
Avg per game: 0.00183873 seconds
0 Variations
```
