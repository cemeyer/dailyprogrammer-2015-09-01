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
                0 1 2 3
                --------
             0 |###     |
             1 |  ###   |
             2 |        |
             3 |        |
                --------
    Piece L at [2,0] orientation 1
                0 1 2 3
                --------
             0 |    ### |
             1 |      # |
             2 |      # |
             3 |        |
                --------
    Piece T at [0,1] orientation 3
                0 1 2 3
                --------
             0 |        |
             1 |#       |
             2 |###     |
             3 |#       |
                --------
    Piece T at [1,2] orientation 2
                0 1 2 3
                --------
             0 |        |
             1 |        |
             2 |    #   |
             3 |  ##### |
                --------
