/**
 * @file reversi.c
 * @author Charles Henville
 * @brief This file is used for APS105 Lab 8. 2023W version
 * @date 2023-03-14
 *
 */
#if !defined(TESTER_P1) && !defined(TESTER_P2)
#include "reversi.h"
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

void printBoard(char board[][26], int n)
{
    char basechar = 'a';
    printf("  ");
    for (int i = 0; i < n; i++)
    {
        printf("%c", basechar + i);
    }
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%c ", basechar + i);
        for (int j = 0; j < n; j++)
        {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

void updatePos(int *row, int *col, int dRow, int dCol)
{
    if (dRow == 1 && dCol == -1)
    {
        *row += 1;
        *col -= 1;
    }
    if (dRow == 1 && dCol == 0)
    {
        *row += 1;
    }
    if (dRow == 1 && dCol == 1)
    {
        *row += 1;
        *col += 1;
    }
    if (dRow == 0 && dCol == -1)
    {
        *col -= 1;
    }
    if (dRow == 0 && dCol == 1)
    {
        *col += 1;
    }
    if (dRow == -1 && dCol == -1)
    {
        *row -= 1;
        *col -= 1;
    }
    if (dRow == -1 && dCol == 0)
    {
        *row -= 1;
    }
    if (dRow == -1 && dCol == 1)
    {
        *row -= 1;
        *col += 1;
    }
}

bool positionInBounds(int n, int row, int col)
{
    if (row > n || col > n || row < 0 || col < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol)
{
    int numUpdates = 0;

    if (board[row][col] != 'U')
        return false;

    while (positionInBounds(n, row, col))
    {

        updatePos(&row, &col, deltaRow, deltaCol);
        if (!positionInBounds(n, row, col))
            return false;
        numUpdates++;

        if (numUpdates == 1 && (board[row][col] == colour || board[row][col] == 'U'))
        {
            return false;
        }
        else if (board[row][col] == 'U')
        {
            return false;
        }
        else if (board[row][col] == colour)
        {
            return true;
        }
    }
    return false;
}

int updateBoard(char *board, int n, char colToPlace, int row, int col)
{

    char boardFreshCopy[26][26];
    int rowCopy = row;
    int colCopy = col;
    int amtOppsFlipped = 0;
    bool valid = false;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            boardFreshCopy[i][j] = *(board + i * 26 + j);
        }
    }

    for (int dRow = -1; dRow < 2; dRow++)
    {
        for (int dCol = -1; dCol < 2; dCol++)
        {

            if (!(dRow == 0 && dCol == 0))
            {
                if (checkLegalInDirection(boardFreshCopy, n, row, col, colToPlace, dRow, dCol))
                {

                    valid = true;
                    while ((boardFreshCopy[row][col] != colToPlace) && positionInBounds(n, row, col))
                    {

                        if (*(board + row * 26 + col) != 'U')
                        {
                            amtOppsFlipped++;
                        }

                        *(board + row * 26 + col) = colToPlace;
                        updatePos(&row, &col, dRow, dCol);
                    }

                    row = rowCopy;
                    col = colCopy;
                }
            }
        }
    }

    return amtOppsFlipped;
}

int makeMove(char board[26][26], int n, char turn, int *row, int *col, int *movesAvailable)
{
    // The score for each candidate position is defined as the total number of the human player’s tiles that
    // would be flipped if the computer were to lay a tile at that position.

    bool flag = false;
    char boardFreshCopy[26][26];
    char boardSto[26][26];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            boardFreshCopy[i][j] = board[i][j];
            boardSto[i][j] = board[i][j];
        }
    }

    int movesReadable[672][2];
    for (int i = 0; i < 672; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            movesReadable[i][j] = *(movesAvailable + i * 2 + j);
        }
    }

    int score = 0;
    char moveToMake[2] = {0, 0};
    int testFlip = 0;

    for (int i = 0; movesReadable[i][0] != 100; i++)
    {

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                boardFreshCopy[i][j] = boardSto[i][j];
            }
        }

        testFlip = updateBoard(&boardFreshCopy[0][0], n, turn, movesReadable[i][0], movesReadable[i][1]);

        if (testFlip > score)
        {
            score = testFlip;
            moveToMake[0] = movesReadable[i][0];
            moveToMake[1] = movesReadable[i][1];
        }
    }

    (*row) = moveToMake[0];
    (*col) = moveToMake[1];

    return 0;
}

bool checkFinished(char board[26][26], int n)
{

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == 'U')
            {
                return false;
            }
        }
    }

    return true;
}

int *getMovesList(char board[26][26], int n, char turn)
{

    int *movesAvailable = malloc(672 * 2 * sizeof(int));

    int flag = false;
    int count = 0;

    for (int i = 0; i < 672; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            *(movesAvailable + i * 2 + j) = 100;
        }
    }

    // Get a list of legal moves
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {

            for (int dRow = -1; dRow < 2; dRow++)
            {
                for (int dCol = -1; dCol < 2; dCol++)
                {

                    if (!(dRow == 0 && dCol == 0))
                    {
                        if (!flag && checkLegalInDirection(board, n, i, j, turn, dRow, dCol))
                        {

                            *(movesAvailable + count * 2) = i;
                            *(movesAvailable + count * 2 + 1) = j;
                            count++;
                            flag = true;
                        }
                    }
                }
            }
            flag = false;
        }
    }

    return movesAvailable;
}

#ifndef TESTER_P2

int main(void)
{
    int n;
    char computerColour;
    char board[26][26];
    char currentMove = 'B';
    char humanColour;
    int compRow, compCol;
    char humaRow, humaCol;
    char winner = 'U';
    int *currentMovesList;

    printf("Enter the board dimension: ");
    scanf("%d", &n);
    printf("Computer plays (B/W): ");
    scanf(" %c", &computerColour);

    if (computerColour == 'W')
    {
        humanColour = 'B';
    }
    else
    {
        humanColour = 'W';
    }

    // initialize the gameboard
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            board[i][j] = 'U';
        }
    }
    board[n / 2 - 1][n / 2 - 1] = 'W';
    board[n / 2 - 1][n / 2] = 'B';
    board[n / 2][n / 2 - 1] = 'B';
    board[n / 2][n / 2] = 'W';

    printBoard(board, n);

    bool finished = false;
    bool doubleNullFlag = false;
    while (!finished)
    {

        if (checkFinished(board, n))
        {
            break;
        }
        // Computer move case
        if (currentMove == computerColour)
        {

            currentMovesList = getMovesList(board, n, computerColour);

            if (*(currentMovesList) != 100)
            {

                makeMove(board, n, currentMove, &compRow, &compCol, currentMovesList);
                printf("Computer places %c at %c%c.\n", computerColour, (char)(compRow + 97), (char)(compCol + 97));
                updateBoard(&board[0][0], n, computerColour, compRow, compCol);
                printBoard(board, n);

                currentMove = humanColour;
                doubleNullFlag = false;
            }
            else
            {
                if (doubleNullFlag)
                {
                    finished = true;
                    break;
                }
                else if (*(getMovesList(board, n, humanColour)) == 100)
                {
                    break;
                }
                printf("%c player has no valid move.\n", computerColour);
                currentMove = humanColour;
                doubleNullFlag = true;
            }
        }

        // Human move case
        else
        {

            currentMovesList = getMovesList(board, n, humanColour);

            if (*(currentMovesList) != 100)
            {
                printf("Enter move for colour %c (RowCol): ", humanColour);
                scanf(" %c%c", &humaRow, &humaCol);

                int usrRow = humaRow - 'a';
                int usrCol = humaCol - 'a';
                bool valid = false;

                for (int dRow = -1; dRow < 2; dRow++)
                {
                    for (int dCol = -1; dCol < 2; dCol++)
                    {

                        if (!(dRow == 0 && dCol == 0))
                        {
                            if (checkLegalInDirection(board, n, usrRow, usrCol, humanColour, dRow, dCol))
                            {
                                valid = true;
                            }
                        }
                    }
                }

                if (!valid)
                {
                    printf("Invalid move.\n");
                    winner = computerColour;
                    finished = true;
                    break;
                }
                else
                {
                    updateBoard(&board[0][0], n, humanColour, usrRow, usrCol);
                }

                currentMove = computerColour;
                printBoard(board, n);
                doubleNullFlag = false;
            }
            else
            {
                if (doubleNullFlag)
                {
                    finished = true;
                    break;
                }
                else if (*(getMovesList(board, n, computerColour)) == 100)
                {
                    break;
                }
                printf("%c player has no valid move.\n", humanColour);
                currentMove = computerColour;
                doubleNullFlag = true;
            }
        }
    }

    int blackCount = 0;
    int whiteCount = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == 'B')
            {
                blackCount++;
            }
            else if (board[i][j] == 'W')
            {
                whiteCount++;
            }
        }
    }

    if (winner == 'U')
    {
        if (whiteCount > blackCount)
        {
            winner = 'W';
        }
        else
        {
            winner = 'B';
        }
    }

    printf("%c player wins.", winner);
    return 0;
}
#endif
