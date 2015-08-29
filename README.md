tetris-solver
=============

Solves tetris-fill puzzles expressed in a simple file format.

Usage:
------

    ./solver simple.puzzle

simple3.puzzle:
---------------

    x = 4
    y = 4
    
    L = 1
    T = 2
    Z = 1

Output:
-------

    Win!
    
    Piece Z at [0,0] orientation 0
    Piece L at [2,0] orientation 1
    Piece T at [1,2] orientation 2
    Piece T at [0,1] orientation 3
                0 1 2 3
                -------
             0 |ZZZ LLL|
               |  Z   L|
             1 |T ZZZ L|
               |T     L|
             2 |TTT T L|
               |T   T  |
             3 |T TTTTT|
                -------
