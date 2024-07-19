==BALLISTIC LANGUAGE==
[Esolang Ideas Physics](https://esolangs.org/wiki/List_of_ideas#Physics) > What about a language based on ballistics? You could have an "artillery piece", whose commands are the source code. It can rotate and elevate, and have muzzle velocity specified. It fires at a 2D grid placed in a square around it. The grid elements are the memory locations, and the angle of impact specifies what to do. This would likely need a lot of computation, though.

# Definitions:
## Memory Field:
A Memory Field is the "field" where all the memory is. It is a 2d geometric grid. It appears like the following: 
`
++++++++++
+-------6+
+--------+
+---3----+
+------2-+
++++++++++
6: "hello"
3: "world"
2: " "
`
It is created at the start of the program, but can be modified via accessing the `MemoryField.x.y` class, where x and y are points on the field.
It is bound by the possible printable characters (minus two due to + and -), as each point can only contain one character.
The width of the table must remain constant. The table is not able to be expanded after initial creation.
You can use the (Default Table)[#DefaultTable] by creating a table with `+default+` at the start in place of the normal one.
Illegal characters for use inside of the table when you are constructing and using it are "+", "-" (i.e. you can't assign "-" a value), and "." (has to do with internals)
## Cannon
The Cannon is what does the actual stuff inside the program. It acts sort of like an instruction pointer in a way. It says `do x at y point` basically.
Inside of the cannonCommands section of the program, there are several possible commands. Those commands are:
    - move
    - fire
The Cannon contains a `reg` (register) that holds one value, in string form, but can be converted. 

### Move
Move moves the cannon. It's called by `move(x, y)`. X and Y can be negative as long as they don't let the cannon's position go negative.

### Fire
Fire shoots the cannon. It's called by `fire(rotation, power)`, and is supposed to hit something.

# Syntax:
## Quick:
Define a [Memory Field](#MemoryField), and then proceed controlling the [Cannon](#Cannon) in order to do stuff.
Example:
.memField
+default+
.cannonCommands
fire(20, 10)
fire(40, 10)
.end
DO NOT INCLUDE SEMICOLONS JUST USE NEWLINES.
## Instruction Set
The Instruction Set is basically the angle of the impact. Each time a tile is impacted the angle will determine what operation to run on that tile.



# Technical:
## Memory Field Class:
The Memory Field created at the start of the program is 

