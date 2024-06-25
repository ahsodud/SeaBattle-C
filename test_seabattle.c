// test_seabattle.c

#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Seabattle.c"

// Setup and Teardown functions
int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

// Test functions
void test_initializeBoard(void) {
    GameState game = { 5, 5, 10, 0, 0, NULL, false, false };
    int result = initializeBoard(&game);
    CU_ASSERT(result == 0);
    CU_ASSERT(game.board != NULL);
    for (int i = 0; i < game.height; i++) {
        for (int j = 0; j < game.width; j++) {
            CU_ASSERT(game.board[i][j].isMine == false);
            CU_ASSERT(game.board[i][j].isOpen == false);
            CU_ASSERT(game.board[i][j].isFlagged == false);
            CU_ASSERT(game.board[i][j].adjacentMines == 0);
        }
    }
    cleanup(&game);
}

void test_generateMinefield(void) {
    GameState game = { 5, 5, 5, 0, 0, NULL, false, false };
    initializeBoard(&game);
    generateMinefield(&game, 0, 0);
    int mineCount = 0;
    for (int i = 0; i < game.height; i++) {
        for (int j = 0; j < game.width; j++) {
            if (game.board[i][j].isMine) {
                mineCount++;
            }
        }
    }
    CU_ASSERT(mineCount == game.numMines);
    cleanup(&game);
}

void test_calculateAdjacentMines(void) {
    GameState game = { 5, 5, 5, 0, 0, NULL, false, false };
    initializeBoard(&game);
    game.board[1][1].isMine = true;
    game.board[2][2].isMine = true;
    calculateAdjacentMines(&game);
    CU_ASSERT(game.board[0][0].adjacentMines == 1);
    CU_ASSERT(game.board[1][2].adjacentMines == 2);
    cleanup(&game);
}

void test_openCell(void) {
    GameState game = { 5, 5, 5, 0, 0, NULL, false, false };
    initializeBoard(&game);
    generateMinefield(&game, 0, 0);
    openCell(&game, 0, 0);
    CU_ASSERT(game.board[0][0].isOpen == true);
    cleanup(&game);
}

void test_checkWin(void) {
    GameState game = { 5, 5, 5, 0, 0, NULL, false, false };
    initializeBoard(&game);
    game.board[0][0].isOpen = true;
    game.board[0][1].isOpen = true;
    game.board[0][2].isOpen = true;
    game.board[0][3].isOpen = true;
    game.board[0][4].isOpen = true;
    game.board[1][0].isOpen = true;
    game.board[1][1].isOpen = true;
    game.board[1][2].isOpen = true;
    game.board[1][3].isOpen = true;
    game.board[1][4].isOpen = true;
    game.board[2][0].isOpen = true;
    game.board[2][1].isOpen = true;
    game.board[2][2].isOpen = true;
    game.board[2][3].isOpen = true;
    game.board[2][4].isOpen = true;
    game.board[3][0].isOpen = true;
    game.board[3][1].isOpen = true;
    game.board[3][2].isOpen = true;
    game.board[3][3].isOpen = true;
    game.board[3][4].isOpen = true;
    game.board[4][0].isOpen = true;
    game.board[4][1].isOpen = true;
    game.board[4][2].isOpen = true;
    game.board[4][3].isOpen = true;
    game.board[4][4].isOpen = true;
    CU_ASSERT(checkWin(&game) == true);
    cleanup(&game);
}

void test_trimWhitespace(void) {
    char str1[] = "  hello  ";
    trimWhitespace(str1);
    CU_ASSERT_STRING_EQUAL(str1, "hello");

    char str2[] = "\thello\t";
    trimWhitespace(str2);
    CU_ASSERT_STRING_EQUAL(str2, "hello");

    char str3[] = "hello";
    trimWhitespace(str3);
    CU_ASSERT_STRING_EQUAL(str3, "hello");
}

// Main function to run the tests
int main() {
    CU_pSuite pSuite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    pSuite = CU_add_suite("SeaBattleTestSuite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test_initializeBoard", test_initializeBoard)) ||
        (NULL == CU_add_test(pSuite, "test_generateMinefield", test_generateMinefield)) ||
        (NULL == CU_add_test(pSuite, "test_calculateAdjacentMines", test_calculateAdjacentMines)) ||
        (NULL == CU_add_test(pSuite, "test_openCell", test_openCell)) ||
        (NULL == CU_add_test(pSuite, "test_checkWin", test_checkWin)) ||
        (NULL == CU_add_test(pSuite, "test_trimWhitespace", test_trimWhitespace)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
