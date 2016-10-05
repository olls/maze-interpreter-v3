# C Maze Interpreter

This is a interpreter for the programming language [Maze](http://esolangs.org/wiki/Maze).

Maze is a language based on an ASCII art 'maze' for 'cars' (variables) to move around.  The shape of your maze describes the program flow and the branches the variables take.  It makes a lot more sense if you look at an example, here is a recording of a multiplication program running:

[![Demo](https://fat.gfycat.com/KeyFlickeringGalapagosdove.gif)](https://gfycat.com/KeyFlickeringGalapagosdove)

Multiplying `3 * 3`, the results are not currently displayed, but are correct!


## WIP

This Maze interpreter is currently being developed, it is mostly functional at this point in terms of executing Maze programs, but it may be buggy and/or slower than ideal.  Please feel free to report bugs in the Github issues!

Things to keep in mind if you are using it:
 - Input cells are not currently implemented.
 - Text values in cars and functions are not currently supported.  Haven't yet decided whether to implement them at all.
 - Must be run from a terminal in order to see the output from the output cells.

The long term plan is to make the program into a kind of 'Maze IDE' eventually.


## Maze Version

This third version of the Maze interpreter uses my own variant of the Maze language, which has slight differences from the original Maze described on the Esolangs Wiki.  These changes were made to make the language nicer and easier to use, as well as behaving more like a cellular automata. My old version of the interpreter is (mostly) compatible with the wiki version of Maze, see [Maze Interpreter V2](https://github.com/olls/maze-interpreter-v2)

The [Maze wiki page](http://esolangs.org/wiki/Maze) is a good start to understanding the basics of the language, this readme will only explain the differences.

### Signals

The most significant change is to signals, signals allow cars to interact with each other.  In the original Maze a signal could transmit from any location in the maze to any other location instantaneously.  This makes Maze programming a bit too magic in some cases, and also makes it tricky to use multiple signals without them interfering.

In this version of Maze, signals are replaced by a '[direction] unless detect' cell.  These cells change the cars direction depending on the presence of another car in the four neighbouring cells (above, to the left, to the right, and below).

If a car is sitting on a 'unless detect' cell and there is at least one other car neighbouring the cell, the car will be left alone as if it were on a path cell.  Otherwise, if there are no cars neighbouring the cell, the car's direction will be changed to the direction specified by the cell's type.

The 'detect unless' cells are written as `*U`, `*D`, `*L`, `*R`, for up, down, left, and right respectively.

### Syntactic Niceties

These changes do not change the functionality of the Maze language, and are backwards compatible with the wiki version of Maze.  (i.e. you can use the syntax on the wiki too)

 - Commas are not needed to separate cells from one another, they will be ignored if used.
 - Walls can be written as ``` `` ``` and `  ` (two spaces) as well as `##`.
 - Any unrecognised syntax will be skipped.
