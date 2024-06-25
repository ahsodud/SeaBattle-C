//---------------------------------------------------------------------------------------------------------------------
// a4.c
//
// My implementation of Sea Battle game. Has custom size, number of fields and seed.
// Has open, dump, flag command. Also could save and load
//
//
// Group: Group 3
//
// Author: 12134500
//---------------------------------------------------------------------------------------------------------------------
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <wchar.h>
#include <limits.h>
#include <errno.h>

#define BORDER_H '='
#define BORDER_V '|'

#define DEFAULT_HEIGHT 9
#define DEFAULT_WIDTH 9
#define DEFAULT_MINES 10
#define DEFAULT_SEED 0

#define MAGIC_NUMBER "ESP\0"

//---------------------------------------------------------------------------------------------------------------------
// Cell struct. Stores information about adjacent mines, is open, is flagged and is mine 
//
// 
typedef struct 
{
  bool isMine;
  bool isOpen;
  bool isFlagged;
  int adjacentMines;
} Cell;

//---------------------------------------------------------------------------------------------------------------------
// Game state struct. Stores different variables about the game
//
// 
typedef struct 
{
  int height;
  int width;
  int numMines;
  int seed;
  int remainingFlags;
  Cell** board;
  bool gameStarted;
  bool gameOver;
} GameState;

//methods
void printBoard(GameState* game);
void dumpBoard(GameState* game);
int initializeBoard(GameState* game);
void generateMinefield(GameState* game, int startRow, int startCol);
void calculateAdjacentMines(GameState* game);
void openCell(GameState* game, int row, int col);
bool checkWin(GameState* game);
void printBoardLoss(GameState* game, int row, int col);
void printBoardWin(GameState* game);
void startGame(GameState* game, int row, int col);
void saveGame(GameState* game, const char* filename);
int loadGame(GameState* game, const char* filename);
void cleanup(GameState* game);
void trimWhitespace(char* str);
int isValidInt(const char *str);


//---------------------------------------------------------------------------------------------------------------------
///
/// Main function. Depending on the type of command performs actions. There are 6 commands in total : open, flag
/// dump, start, save and load. Their work is similar to Sea Battle game
///
//
int main(int argc, char* argv[]) 
{

  GameState game = { DEFAULT_HEIGHT, DEFAULT_WIDTH, DEFAULT_MINES, DEFAULT_SEED, 0, NULL, false, false };

  for (int i = 1; i < argc; i++) 
  {
    if (strcmp(argv[i], "--size") == 0) 
    {
      if (i + 2 >= argc) 
      {
        printf("Invalid number of parameters given!\n");
        return 2;
      }
      if (!isValidInt(argv[i + 1]) || !isValidInt(argv[i + 2])) {
        printf("Invalid type for argument!\n");
        return 4;
      }

      char *endptr;
      errno = 0;
      uint64_t height = strtoull(argv[i + 1], &endptr, 10);
      if (errno == ERANGE || *endptr != '\0' || height > UINT64_MAX) 
      {
        printf("Invalid value for argument!\n");
        return 5;
      }

      errno = 0;
      uint64_t width = strtoull(argv[i + 2], &endptr, 10);
      if (errno == ERANGE || *endptr != '\0' || width > UINT64_MAX) 
      {
        printf("Invalid value for argument!\n");
        return 5;
      }

      game.height = (int)height;
      game.width = (int)width;

      // Skip the next two arguments as they have been processed
      i += 2;
    }
    else if (strcmp(argv[i], "--mines") == 0) 
    {
      if (i + 1 >= argc) 
      {
        printf("Invalid number of parameters given!\n");
        return 2;
      }
      if (!isValidInt(argv[i + 1])) 
      {
        printf("Invalid type for argument!\n");
        return 4;
      }
      int numMines = atoi(argv[++i]);
      if (numMines < 0 || numMines > (game.height * game.width - 1)) 
      {
        printf("Invalid value for argument!\n");
        return 5;
      }
      game.numMines = numMines;
    } 
    else if (strcmp(argv[i], "--seed") == 0) 
    {
      if (i + 1 >= argc) 
      {
        printf("Invalid number of parameters given!\n");
        return 2;
      }
      if (!isValidInt(argv[i + 1])) 
      {
        printf("Invalid type for argument!\n");
        return 4;
      }
      int seed = atoi(argv[++i]);
      if (seed < 0) 
      {
        printf("Invalid value for argument!\n");
        return 5;
      }
      game.seed = seed;
    }
    else 
    {
      printf("Unexpected argument provided!\n");
      return 3;
    }
  }

  game.remainingFlags = game.numMines;

  if(initializeBoard(&game))
  {
    printf("Out of memory!\n");
    cleanup(&game);
    return 1;
  }
  printf("Welcome to ESP Minesweeper!\n");
  printf("Chosen field size: %d x %d.\n", game.height, game.width);
  printf("After map generation %d mines will be hidden in the playing field.\n", game.numMines);

  char command[256];
  while (!game.gameOver) 
  {
    printf(" > ");
    if (fgets(command, sizeof(command), stdin) == NULL) break;

    // Trim leading and trailing whitespace from the command
    trimWhitespace(command);

    char* token = strtok(command, " ");
    if (token == NULL) continue;

    if (strcmp(token, "start") == 0) 
    {
    char *row_str = strtok(NULL, " ");
    char *col_str = strtok(NULL, " ");
    
    if (row_str == NULL || col_str == NULL) 
    {
      printf("Error: Command is missing arguments!\n");
    } 
    else if (strtok(NULL, " ") != NULL) 
    {
      printf("Error: Too many arguments given for command!\n");
    } 
    else if (!isValidInt(row_str) || !isValidInt(col_str)) 
    {
      printf("Error: Invalid arguments given!\n");
    } 
    else 
    {
      int row = atoi(row_str);
      int col = atoi(col_str);
      if (row < 0 || row >= game.height || col < 0 || col >= game.width) 
      {
        printf("Error: Coordinates are invalid for this game board!\n");
      } 
      else 
      {
        startGame(&game, row, col);
        printBoard(&game);
      }
    }
    } 
    else if (strcmp(token, "open") == 0) 
    {
      char *row_str = strtok(NULL, " ");
      char *col_str = strtok(NULL, " ");
      
      if (row_str == NULL || col_str == NULL) 
      {
        printf("Error: Command is missing arguments!\n");
      } 
      else if (strtok(NULL, " ") != NULL) 
      {
        printf("Error: Too many arguments given for command!\n");
      } 
      else if (!isValidInt(row_str) || !isValidInt(col_str)) 
      {
        printf("Error: Invalid arguments given!\n");
      } 
      else 
      {
        int row = atoi(row_str);
        int col = atoi(col_str);
        if (row < 0 || row >= game.height || col < 0 || col >= game.width) 
        {
          printf("Error: Coordinates are invalid for this game board!\n");
        } 
        else 
        {
          openCell(&game, row, col);
          if (!game.gameOver) 
          {
            printBoard(&game);
          }
        }
      }
    } 
    else if (strcmp(token, "flag") == 0) 
    {
      char *row_str = strtok(NULL, " ");
      char *col_str = strtok(NULL, " ");
      
      if (row_str == NULL || col_str == NULL) 
      {
        printf("Error: Command is missing arguments!\n");
      } 
      else if (strtok(NULL, " ") != NULL) 
      {
        printf("Error: Too many arguments given for command!\n");
      } 
      else if (!isValidInt(row_str) || !isValidInt(col_str)) 
      {
        printf("Error: Invalid arguments given!\n");
      } 
      else 
      {
        int row = atoi(row_str);
        int col = atoi(col_str);
        if (row < 0 || row >= game.height || col < 0 || col >= game.width) 
        {
          printf("Error: Coordinates are invalid for this game board!\n");
        } 
        else 
        {
          if (game.board[row][col].isOpen) continue;
          game.board[row][col].isFlagged = !game.board[row][col].isFlagged;
          game.remainingFlags += game.board[row][col].isFlagged ? -1 : 1;
          printBoard(&game);
        }
      }
    } 
    else if (strcmp(token, "dump") == 0) 
    {
      if (strtok(NULL, " ") != NULL) 
      {
        printf("Error: Too many arguments given for command!\n");
      } 
      else 
      {
        dumpBoard(&game);
        printBoard(&game);
      }
    } 
    else if (strcmp(token, "save") == 0) 
    {
      const char* filename = strtok(NULL, " ");
      
      if (filename == NULL) 
      {
        printf("Error: Command is missing arguments!\n");
      } 
      else if (strtok(NULL, " ") != NULL) 
      {
        printf("Error: Too many arguments given for command!\n");
      } 
      else 
      {
        saveGame(&game, filename);
        printBoard(&game);
      }
    } 
    else if (strcmp(token, "load") == 0) 
    {
      const char* filename = strtok(NULL, " ");
      
      if (filename == NULL) 
      {
        printf("Error: Command is missing arguments!\n");
      } 
      else if (strtok(NULL, " ") != NULL) 
      {
        printf("Error: Too many arguments given for command!\n");
      } 
      else 
      {
        if(!loadGame(&game, filename)) printBoard(&game);
      }
    } 
    else if (strcmp(token, "quit") == 0) 
    {
    if (strtok(NULL, " ") != NULL) 
      {
        printf("Error: Too many arguments given for command!\n");
      } 
      else 
      {
        game.gameOver = true;
        printBoard(&game);
      }
    } 
    else 
    {
    printf("Error: Unknown command!\n");
    }
  }

  cleanup(&game);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Trim trailing and leading whitespaces
///
/// @param kv key-value
///
//
void trimWhitespace(char* str) 
{
  // Trim leading whitespace characters
  while (isspace(*str)) 
  {
    str++;
  }

  // Trim trailing whitespace characters
  char* end = str + strlen(str) - 1;
  while (end > str && isspace(*end)) 
  {
    end--;
  }
  *(end + 1) = '\0';
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Check for input number
///
/// @return 0, if int, 1 otherwise
///
//
int isValidInt(const char *str) 
{
  if (*str == '-' || *str == '+') str++;
  if (!*str) return 0;  // Check for empty string after sign
  while (*str) 
  {
    if (!isdigit(*str)) return 0;
    str++;
  }
  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function generates 64 bit random number
///
/// @return random_number
///
//
unsigned long long generate64BitRandomNumber(void) 
{
  return (((unsigned long long)rand() << 32) | rand());
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Initializing board, allocating memory
///
/// @param game current game state
///
/// @return 0, if success, 1 if allocation failed
///
//
int initializeBoard(GameState* game) 
{
  game->board = malloc(game->height * sizeof(Cell*));
  if (game->board == NULL) 
  {
    return 1;
  }

  for (int i = 0; i < game->height; i++) 
  {
    game->board[i] = malloc(game->width * sizeof(Cell));
    if (game->board[i] == NULL) 
    {
      return 1;
    }

    for (int j = 0; j < game->width; j++) 
    {
      game->board[i][j] = (Cell){ .isMine = false, .isOpen = false, .isFlagged = false, .adjacentMines = 0 };
    }
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Generator for field. Based on random values assighns mines
///
/// @param game current game state
/// @param startRow start row
/// @param startCol start column
///
//
void generateMinefield(GameState* game, int startRow, int startCol) 
{
  srand(game->seed);
  int fieldsLeft = game->height * game->width - 1;
  int minesLeft = game->numMines;

  for (int r = 0; r < game->height; r++) 
  {
    for (int c = 0; c < game->width; c++) 
    {
      if (r == startRow && c == startCol) 
      {
        game->board[r][c].isMine = false;
        continue;
      }

      unsigned long long random_number = generate64BitRandomNumber() % fieldsLeft;
      if (random_number < (unsigned long long)minesLeft) 
      {
        game->board[r][c].isMine = true;
        minesLeft--;
      } 
      else 
      {
        game->board[r][c].isMine = false;
      }

      fieldsLeft--;
    }
  }
  calculateAdjacentMines(game);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Calculator for adjacent mines
///
/// @param game current game state
///
//
void calculateAdjacentMines(GameState* game) 
{
  for (int r = 0; r < game->height; r++) 
  {
    for (int c = 0; c < game->width; c++) 
    {
      if (game->board[r][c].isMine) continue;

      int count = 0;
      for (int dr = -1; dr <= 1; dr++) 
      {
        for (int dc = -1; dc <= 1; dc++) 
        {
          int nr = r + dr, nc = c + dc;
          if (nr >= 0 && nr < game->height && nc >= 0 && nc < game->width && game->board[nr][nc].isMine) 
          {
          count++;
          }
        }
      }
      game->board[r][c].adjacentMines = count;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Open cell function
///
/// @param game current game state
/// @param row row to open
/// @param col col to open
///
//
void openCell(GameState* game, int row, int col) 
{
  if (game->board[row][col].isOpen) return;
  if (game->board[row][col].isFlagged) 
  {
    game->board[row][col].isFlagged = false;
    game->remainingFlags++;
  }
  game->board[row][col].isOpen = true;
  if (game->board[row][col].isMine) 
  {
    game->gameOver = true;
    printBoardLoss(game, row, col);
    return;
  }

  if (game->board[row][col].adjacentMines == 0) 
  {
    for (int dr = -1; dr <= 1; dr++) 
    {
      for (int dc = -1; dc <= 1; dc++) 
      {
        int nr = row + dr, nc = col + dc;
        if (nr >= 0 && nr < game->height && nc >= 0 && nc < game->width && !game->gameOver) 
        {
          openCell(game, nr, nc);
        }
      }
    }
  }
  if (checkWin(game) && !game->gameOver) 
  {
    game->gameOver = true;
    printBoardWin(game);
  }
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function checks if all non-mine fields was open. If true, then player won
///
/// @param game current game state
///
/// @return true, if win, false otherwise
///
//
bool checkWin(GameState* game) 
{
  for (int r = 0; r < game->height; r++) 
  {
    for (int c = 0; c < game->width; c++) 
    {
      if (!game->board[r][c].isMine && !game->board[r][c].isOpen) 
      {
        return false; // There are still unopened non-mine cells
      }
    }
  }
  return true; // All non-mine cells are opened
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Display field if lost
///
/// @param game current game state
/// @param row row, where triggered mine located
/// @param col column, where triggered mine located
///
//
void printBoardLoss(GameState* game, int row, int col) 
{
  printf("\n=== You lost! ===\n");
  printf("\n  \033[31m¶\033[0m: %d\n", game->remainingFlags);
  printf("  ");
  for (int i = 0; i < game->width; i++) printf("%c", BORDER_H);
    printf(" \n");

  for (int r = 0; r < game->height; r++) 
  {
    printf(" %c", BORDER_V);
    for (int c = 0; c < game->width; c++) 
    {
      if (game->board[r][c].isMine) 
      {
        if (r == row && c == col) 
        {
          printf("\033[33m\033[41m@\033[0m"); // Highlight the mine with red background
        } 
        else 
        {
          printf("\033[33m@\033[0m"); // Print other mines normally
        }
      } 
      else if (game->board[r][c].isFlagged) 
      {
        printf("\033[31m¶\033[0m");  // Flagged field in red
      } 
      else if (!game->board[r][c].isOpen) 
      {
        printf("░");
      } 
      else if (game->board[r][c].adjacentMines == 0) 
      {
        printf("·");
      } 
      else 
      {
        printf("%d", game->board[r][c].adjacentMines);
      }
    }
    printf("%c\n", BORDER_V);
  }

  printf("  ");
  for (int i = 0; i < game->width; i++) printf("%c", BORDER_H);
  printf(" \n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Display field if won
///
/// @param game current game state
///
//
void printBoardWin(GameState* game) 
{
  printf("\n=== You won! ===\n");
  printf("\n  \033[31m¶\033[0m: %d\n", game->remainingFlags);
  printf("  ");
  for (int i = 0; i < game->width; i++) printf("%c", BORDER_H);
  printf(" \n");

  for (int r = 0; r < game->height; r++) 
  {
    printf(" %c", BORDER_V);
    for (int c = 0; c < game->width; c++) 
    {
      if (game->board[r][c].isMine) 
      {
        printf("\033[33m@\033[0m"); // Print mines normally
      } 
      else if (game->board[r][c].isFlagged) 
      {
        printf("\033[31m¶\033[0m");  // Flagged field in red
      } 
      else if (!game->board[r][c].isOpen) 
      {
        printf("░");
      } 
      else if (game->board[r][c].adjacentMines == 0) 
      {
        printf("·");
      } 
      else 
      {
        printf("%d", game->board[r][c].adjacentMines);
      }
    }
    printf("%c\n", BORDER_V);
  }

  printf("  ");
  for (int i = 0; i < game->width; i++) printf("%c", BORDER_H);
  printf(" \n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Start game. Generates minefield and opens first cell
///
/// @param game current game state
/// @param row start row
/// @param col start column
///
//
void startGame(GameState* game, int row, int col) 
{
  //if (game->gameStarted) return;
  generateMinefield(game, row, col);
  for (int r = 0; r < game->height; r++) 
  {
    for (int c = 0; c < game->width; c++) 
    {
      if (game->board[r][c].isOpen) 
      {
        game->board[r][c].isOpen = false;
      }
    }
  }
  openCell(game, row, col);
  game->gameStarted = true;

}

//---------------------------------------------------------------------------------------------------------------------
///
/// Display field
///
/// @param game current game state
///
//
void printBoard(GameState* game) 
{
  if(!game->gameStarted) printf("\n  \033[31m¶\033[0m: %d\n", 0);
  else printf("\n  \033[31m¶\033[0m: %d\n", game->remainingFlags);
  printf("  ");
  for (int i = 0; i < game->width; i++) printf("%c", BORDER_H);
  printf(" \n");

  for (int r = 0; r < game->height; r++) 
  {
    printf(" %c", BORDER_V);
    for (int c = 0; c < game->width; c++) 
    {
      if (game->board[r][c].isFlagged) 
      {
        printf("\033[31m¶\033[0m");  // Flagged field in red
      } 
      else if (!game->board[r][c].isOpen) 
      {
        printf("░");
      } 
      else if (game->board[r][c].isMine) 
      {
        printf("\033[33m@\033[0m");  // Mine in yellow
      } 
      else if (game->board[r][c].adjacentMines == 0) 
      {
        printf("·");
      } 
      else 
      {
        printf("%d", game->board[r][c].adjacentMines);
      }
    }
    printf("%c\n", BORDER_V);
  }

  printf("  ");
  for (int i = 0; i < game->width; i++) printf("%c", BORDER_H);
  printf(" \n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Display field with all cells open
///
/// @param game current game state
///
//
void dumpBoard(GameState* game) 
{
  printf("\n  \033[31m¶\033[0m: %d\n", game->remainingFlags);
  printf("  ");
  for (int col = 0; col < game->width; col++) 
  {
    printf("%c", BORDER_H);
  }
  printf(" \n");

  for (int row = 0; row < game->height; row++) 
  {
    printf(" %c", BORDER_V);
    for (int col = 0; col < game->width; col++) 
    {
      Cell* cell = &game->board[row][col];
      if (cell->isMine) 
      {
        printf("\033[33m@\033[0m");  // Mine in yellow
      } 
      else if (cell->isFlagged) 
      {
        printf("\033[31m¶\033[0m");  // Flagged field in red
      } 
      else if (cell->adjacentMines > 0) 
      {
        printf("%d", cell->adjacentMines);  // Open field with number
      } 
      else 
      {
        printf("·");  // Open field with no adjacent mines
      }
    }
    printf("%c\n", BORDER_V);
  }

  printf("  ");
  for (int col = 0; col < game->width; col++) 
  {
    printf("%c", BORDER_H);
  }
  printf(" \n");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Saves game bit by bit
///
/// @param game current game state
/// @param filename where to save
///
//
void saveGame(GameState* game, const char* filename) 
{
  FILE* file = fopen(filename, "wb");
  if (file == NULL) 
  {
    printf("Error: Failed to open file!\n");
    return;
  }

  fwrite(MAGIC_NUMBER, 1, 4, file);
  uint64_t height = game->height;
  uint64_t width = game->width;
  fwrite(&height, sizeof(uint64_t), 1, file);
  fwrite(&width, sizeof(uint64_t), 1, file);

  int total_cells = game->height * game->width;
  int total_blocks = (total_cells + 7) / 8;

  for (int block = 0; block < total_blocks; block++) 
  {
    uint32_t block_data = 0;

    for (int bit = 0; bit < 8; bit++) 
    {
      int cell_index = block * 8 + bit;
      if (cell_index >= total_cells) 
      {
        break;
      }

      int row = cell_index / game->width;
      int col = cell_index % game->width;

      block_data |= 1 << bit; // Set valid bit
      if (game->board[row][col].isMine) block_data |= 1 << (bit + 8);
      if (game->board[row][col].isOpen) block_data |= 1 << (bit + 16);
      if (game->board[row][col].isFlagged) block_data |= 1 << (bit + 24);
    }
    fwrite(&block_data, sizeof(uint32_t), 1, file);
  }

  fclose(file);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Load game from a file
///
/// @param game current game state
/// @param filename from where to load
///
/// @return 0, if successful, 1 otherwise
//
int loadGame(GameState* game, const char* filename) 
{
  FILE* file = fopen(filename, "rb");
  if (file == NULL) 
  {
    printf("Error: Failed to open file!\n");
    return 1;
  }

  char magic[4];
  fread(magic, 1, 4, file);
  if (strncmp(magic, MAGIC_NUMBER, 4) != 0) 
  {
    printf("Error: Invalid file content!\n");
    fclose(file);
    return 1;
  }

  uint64_t height;
  uint64_t width;
  fread(&height, sizeof(uint64_t), 1, file);
  fread(&width, sizeof(uint64_t), 1, file);

  game->height = height;
  game->width = width;
  game->remainingFlags = 0;
  game->numMines = 0;;

  cleanup(game);
  initializeBoard(game);

  int total_cells = game->height * game->width;
  int total_blocks = (total_cells + 7) / 8;

  for (int block = 0; block < total_blocks; block++) 
  {
    uint32_t block_data;
    fread(&block_data, sizeof(uint32_t), 1, file);

    for (int bit = 0; bit < 8; bit++) 
    {
      int cell_index = block * 8 + bit;
      if (cell_index >= total_cells) 
      {
        break;
      }

      int row = cell_index / game->width;
      int col = cell_index % game->width;

      if (block_data & (1 << bit)) 
      {
        game->board[row][col].isMine = block_data & (1 << (bit + 8));
        game->board[row][col].isOpen = block_data & (1 << (bit + 16));
        game->board[row][col].isFlagged = block_data & (1 << (bit + 24));
      }
    }
  }
  for (int r = 0; r < game->height; r++) 
  {
    for (int c = 0; c < game->width; c++) 
    {
      if (game->board[r][c].isMine) 
      {
        game->numMines++;
      }

      if (game->board[r][c].isFlagged) 
      {
        game->remainingFlags++;
      }
    }
  }
  game->remainingFlags = game->numMines - game->remainingFlags;

  calculateAdjacentMines(game);
  fclose(file);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
///
/// Destructor for a field
///
/// @param game current game state
///
//
void cleanup(GameState* game) 
{
  if (game->board != NULL) 
  {
    for (int count = 0; count < game->height; count++) 
    {
      free(game->board[count]);
    }
    free(game->board);
  }
}
