input1
======

    $ ./solver input1.txt 
    
    Board @ depth=0 score=0 (piece remain=3)
                   |
                   |
                   |
                   |
                   |
                   |
                   |
                   |
                   |
                   |
    ---------------
    
    Board @ depth=3 score=1 (piece remain=0)
    NEAT           |
      I            |
    LARGE          |
      O            |
      N            |
                   |
                   |
                   |
                   |
                   |
    ---------------
    attempted 100000 boards
    attempted 200000 boards
    attempted 300000 boards
    
    Board @ depth=3 score=2 (piece remain=0)
    IRON           |
       E           |
      LARGE        |
       T           |
                   |
                   |
                   |
                   |
                   |
                   |
    ---------------
    attempted 400000 boards
    
input2
======

    It only finds 3 before running out of zobrist hash memory, because it's a dumb
    recursive-descent brute-force approach.
    
    $ ./solver input2.txt
    
    Board @ depth=0 score=0 (piece remain=4)
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
    ------------------------
    
    Board @ depth=4 score=1 (piece remain=0)
    COLORFUL                |
    DIVIDEND                |
        A                   |
        L                   |
        S                   |
        A                   |
    FOURTEEN                |
        I                   |
        A                   |
        N                   |
                            |
                            |
                            |
                            |
                            |
                            |
    ------------------------
    attempted 100000 boards
    attempted 200000 boards
    attempted 300000 boards
    attempted 400000 boards
    attempted 500000 boards
    attempted 600000 boards
    attempted 700000 boards
    attempted 800000 boards
    attempted 900000 boards
    attempted 1000000 boards
    attempted 1100000 boards
    attempted 1200000 boards
    attempted 1300000 boards
    attempted 1400000 boards
    attempted 1500000 boards
    
    Board @ depth=4 score=2 (piece remain=0)
    COLORFUL                |
         O                  |
         U                  |
         R                  |
         T                  |
    DIVIDEND                |
         E                  |
         N                  |
    ALSATIAN                |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
    ------------------------
    
    Board @ depth=4 score=3 (piece remain=0)
    COLORFUL                |
         O                  |
         U                  |
         R                  |
     ALSATIAN               |
    DIVIDEND                |
         E                  |
         N                  |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
                            |
    ------------------------
    attempted 1600000 boards
    attempted 1700000 boards

