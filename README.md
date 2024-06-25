# SeaBattle-C
Minesweeper is a single-player game played on a 2-dimensional grid of fields. The goal of the game is open all fields,
except for the ones which hide mines. If a field with a mine is opened, the mine explodes and the game is lost. Each 
opened field bears the number of mines on the 8 fields surrounding it, or no number if there are no mines. Whenever a 
field is opened, if it bears no number, all adjacent fields are also opened until each opened field bears a number. If
the player suspects a field to hide a mine, they can put a flag on it. The game is over when all fields which do not
contain mines are opened; in this case, the player wins. If a field containing a mine is opened, the player loses.

The first opened field never contains a mine. Opening the first field triggers the generation of the game map.

## Command Line Arguments
The program receives three optional command line arguments: `--size`, `--mines` and `--seed`. Each of these arguments
comes with some parameters, as described in the table below. The arguments can appear in any order, however, the
parameters must directly follow the argument; otherwise, an error is thrown (see further down). If any of the
arguments is not given in the command line, its default value should be used, as described in the table below.  

| Argument  | Num. Params (Type) | Parameters      | Default | Description                            |
| --------- | ------------------ | --------------- | ------- | -------------------------------------- |
| `--size`  | 2 (int, int)       | `height width ` | **9 9** | (usual field size of *Beginner* level) |
| `--mines` | 1 (int)            | `count`         | **10**  | (usual mine count of *Beginner* level) |
| `--seed`  | 1 (int)            | `seed`          | **0**   |                                        |

## Course of the Game
1. The first prompt is shown without printing the field.
2. The player starts the game by using the command `start <row> <col>`, indicating the first field to be opened.
3. If the coordinates are valid, the map is generated and the field is opened.
4. Then, each turn the player can use `open <row> <col>` to open or `flag <row> <col>` to flag the specified field.
5. If the player either wins or loses the game ends immediately. The field is printed once more, revealing all mines.
6. If the player lost the game, the mine which caused the loss is indicated with a 
   red background.
**Example Field Layout**

These example layouts are shown **without prompts** and **without the correct character colors**.
Complete program sequences with characters in correct colors can be found in the **public testcases**.

```
  ¶: 8
  ========= 
 |░░░░░░░░░|
 |░¶░░░░░░░|
 |░211░░░░░|
 |11·1░░░░░|
 |···1░░░░░|
 |1111¶2░░░|
 |░░░░░░░░░|
 |░░░░░░░░░|
 |░░░░░░░░░|
  =========
```
*Field size: 9 x 9, 10 mines hidden, 2 flags placed.*

## Commands
**start**

After the start message the player is prompted to input a command. At this point only the commands
*load* and the *start* command are available. Using the *start* command will generate the map and open the first field

**dump**

The *dump* command will print the playing field twice: The first output shows all mines and every field indicates the
number of adjacent mines with a number (every field is opened). The second output shows the playing field in its normal
state during the previous turn

**open**

The *open* command opens a field and reveals either a mine or the number of mines in adjacent fields If the opened
field is a mine the game ends immediately. If the number of mines adjacent to the opened field is `1-8`, this number is 
indicated at this field from this point on for all the following turns. If the number of mines adjacent to the opened
field is `0` adjacent fields are opened until the border to closed fields is filled with numbers between `1` and `8`.
This means, each time the command `open` is used, numbers are revealed (or the game ends if the field contains a mine)

**flag**

Flags can be used to mark fields which the player suspects are mines. If the `flag` command is called on a field which
was already flagged before, the flag is removed (this can be repeated indefinitely). If such a field is opened during a
later stage of the game and it does not contain mines, the field is opened like every unmarked field. For this, it does
not matter if the field was opened directly or is part of the opening process of an adjacent field.
The flag on an open field will not be shown when printing and it does not count towards the remaining flags. However,
the field will still be marked as flagged in a save file as long as the flag is not removed again using the `flag`
command

**save and load**

The game can be saved at any turn using the command *save*. The filename needs to be specified. After saving, the current state of the game is
printed again. If the game is loaded using the command *load* the imported game overwrites the current game

**quit**

The game can be ended at any time with the "quit" command. After using this command, the playing field is printed once
more
