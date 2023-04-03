/**
 * @file reversi.c
 * @author Charles Henville
 * @brief This file is used for APS105 Lab 8. 2023W version
 * @date 2023-03-14
 */

#if !defined(TESTER_P1) && !defined(TESTER_P2)
#include "reversi.h"
#endif

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define FLIPPER_WT 1
#define AMT_WT 1
#define H_WT 7
#define MOBILITY_WT 0.1
#define CORNER_WT 28
#define DEPTH 3
#define PRUNE_DEPTH 3

// int globalcount = 0;

int *getMovesList(char board[26][26], int n, char turn);
bool isFinished(char board[26][26], int n);

char getColour(bool black)
{
    if (black)
    {
        return 'B';
    }
    return 'W';
}
int getSign(bool nullParameter)
{
    if (nullParameter)
    {
        return -1;
    }
    return 1;
}
int max(int x, int y)
{
    if (x >= y)
    {
        return x;
    }
    return y;
}
int min(int x, int y)
{
    if (x <= y)
    {
        return x;
    }
    return y;
}
int getAmt(char board[26][26], int n, char search)
{
    int amt = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == search)
            {
                amt++;
            }
        }
    }
    return amt;
}
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

        row += deltaRow;
        col += deltaCol;

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

int dRow[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dCol[] = {-1, 0, 1, -1, 1, -1, 0, 1};

int updateBoard(char *board, int n, char colToPlace, int row, int col, bool fromLegal)
{

    char boardFreshCopy[26][26];
    int rowCopy = row;
    int colCopy = col;
    int amtOppsFlipped = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            boardFreshCopy[i][j] = *(board + i * 26 + j);
        }
    }

    for (int k = 0; k < 8; k++)
    {
        if (fromLegal || checkLegalInDirection(boardFreshCopy, n, row, col, colToPlace, dRow[k], dCol[k]))
        {
            while ((boardFreshCopy[row][col] != colToPlace) && positionInBounds(n, row, col))
            {

                if (*(board + row * 26 + col) != 'U')
                {
                    amtOppsFlipped++;
                }

                *(board + row * 26 + col) = colToPlace;

                row += dRow[k];
                col += dCol[k];
            }

            row = rowCopy;
            col = colCopy;
        }
    }

    return amtOppsFlipped;
}

// https://courses.cs.washington.edu/courses/cse573/04au/Project/mini1/RUSSIA/Final_Paper.pdf
int positionWeights[8][8] =
    {{4, -3, 2, 2, 2, 2, -3, 4},
     {-3, -4, -1, -1, -1, -1, -4, -3},
     {2, -1, 1, 0, 0, 1, -1, 2},
     {2, -1, 0, 1, 1, 0, -1, 2},
     {2, -1, 0, 1, 1, 0, -1, 2},
     {2, -1, 1, 0, 0, 1, -1, 2},
     {-3, -4, -1, -1, -1, -1, -4, -3},
     {4, -3, 2, 2, 2, 2, -3, 4}};
int currentHeuristicBoardScore(char board[][26], int n)
{
    int score = AMT_WT * (getAmt(board, n, 'W') - getAmt(board, n, 'B'));

    // Failsafe in-case we are not playing on a conventional sized board
    if (n != 8)
    {
        score += (board[0][0] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[0][n - 1] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 1][0] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 1][n - 1] == 'B') ? -1 * CORNER_WT : CORNER_WT;

        score += (board[1][1] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[1][n - 2] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 2][1] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 2][n - 2] == 'B') ? -1 * CORNER_WT : CORNER_WT;

        score += (board[0][1] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[0][n - 2] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 1][1] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 1][n - 2] == 'B') ? -1 * CORNER_WT : CORNER_WT;

        score += (board[1][0] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[1][n - 1] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 2][0] == 'B') ? -1 * CORNER_WT : CORNER_WT;
        score += (board[n - 2][n - 1] == 'B') ? -1 * CORNER_WT : CORNER_WT;

        return score;
    }

    // Factoring in peices and the weight of each position
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == 'B')
            {
                score -= H_WT * positionWeights[i][j];
            }
            else if (board[i][j] == 'W')
            {
                score += H_WT * positionWeights[i][j];
            }
        }
    }

    // Factoring in the Mobility of each player (amount of available moves)
    int *whiteAvailable = getMovesList(board, n, 'W');
    int *blackAvailable = getMovesList(board, n, 'B');
    int whiteCount = 0;
    int blackCount = 0;

    for (int i = 0; *(whiteAvailable + 2 * i) != 100; i++)
    {
        whiteCount++;
    }
    free(whiteAvailable);
    for (int i = 0; *(blackAvailable + 2 * i) != 100; i++)
    {
        blackCount++;
    }
    free(blackAvailable);

    score += MOBILITY_WT * (whiteCount - blackCount);

    return score;
}

int evalBoard(char board[][26], int n, bool isBlack, int initdepth, int alpha, int beta)
{
    int *availableMoves;

    if (isBlack)
    {
        availableMoves = getMovesList(board, n, 'B');
    }
    else
    {
        availableMoves = getMovesList(board, n, 'W');
    }

    if (initdepth >= DEPTH || isFinished(board, n))
    {
        free(availableMoves);
        return currentHeuristicBoardScore(board, n);
    }

    int bestValue = isBlack ? INT_MAX : INT_MIN;
    for (int i = 0; *(availableMoves + 2 * i) != 100; i++)
    {

        char currentMoveCopy[26][26];
        for (int k = 0; k < n; k++)
        {
            for (int j = 0; j < n; j++)
            {
                currentMoveCopy[k][j] = board[k][j];
            }
        }

        int currentFlip = getSign(isBlack) * FLIPPER_WT * updateBoard(&currentMoveCopy[0][0], n, getColour(isBlack), *(availableMoves + 2 * i), *(availableMoves + 2 * i + 1), true);
        currentFlip += currentHeuristicBoardScore(currentMoveCopy, n);
        // globalcount++;
        int evaluation = currentFlip + evalBoard(currentMoveCopy, n, !isBlack, initdepth + 1, alpha, beta);

        if (isBlack)
        {
            bestValue = min(bestValue, evaluation);
            alpha = min(bestValue, alpha);
        }
        else
        {
            bestValue = max(bestValue, evaluation);
            beta = max(bestValue, beta);
        }

        if (initdepth >= PRUNE_DEPTH)
        {
            if (beta <= alpha)
            {
                break;
            }
        }
    }
    free(availableMoves);
    return bestValue;
}

bool getBoolGivenChar(char colour)
{
    if (colour == 'W')
    {
        return false;
    }
    return true;
}

int makeMove(char board[26][26], int n, char turn, int *row, int *col)
{
    int *movesAvailable = getMovesList(board, n, turn);
    char boardSto[26][26];
    int bestMoveIdx;
    int bestEval = turn == 'W' ? INT_MIN : INT_MAX;
    int currentEval;

    for (int i = 0; *(movesAvailable + i * 2) != 100; i++)
    {

        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                boardSto[j][k] = board[j][k];
            }
        }
        updateBoard(&boardSto[0][0], n, turn, *(movesAvailable + i * 2), *(movesAvailable + i * 2 + 1), true);
        currentEval = currentHeuristicBoardScore(boardSto, n) + evalBoard(boardSto, n, !getBoolGivenChar(turn), 0, INT_MIN, INT_MAX);

        // printf("\n%d: %d: %c%c", i, currentEval, *(movesAvailable + i * 2) + 'a', *(movesAvailable + i * 2 + 1) + 'a');

        if (getBoolGivenChar(turn)) // If the turn is black
        {
            if (currentEval < bestEval)
            {
                bestEval = currentEval;
                bestMoveIdx = i;
            }
        }
        else // If the turn is white
        {
            if (currentEval > bestEval)
            {
                bestEval = currentEval;
                bestMoveIdx = i;
            }
        }
    }

    (*row) = *(movesAvailable + bestMoveIdx * 2);
    (*col) = *(movesAvailable + bestMoveIdx * 2 + 1);

    free(movesAvailable);

    // printf("%d", globalcount);
    // globalcount=0;
    return 0;
}

bool isFinished(char board[26][26], int n)
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

    int *movesAvailable = malloc(n * n * 2 * sizeof(int));

    int count = 0;

    for (int i = 0; i < n * n; i++)
    {
        *(movesAvailable + i * 2) = 100;
        *(movesAvailable + i * 2 + 1) = 100;
    }

    // Get a list of legal moves
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == 'U')
            {
                for (int k = 0; k < 8; k++)
                {
                    if (checkLegalInDirection(board, n, i, j, turn, dRow[k], dCol[k]))
                    {
                        *(movesAvailable + count * 2) = i;
                        *(movesAvailable + count * 2 + 1) = j;
                        count++;
                        break;
                    }
                }
            }
        }
    }

    movesAvailable = realloc(movesAvailable, (count + 1) * 2 * sizeof(int));
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

        if (isFinished(board, n))
        {
            break;
        }
        // Computer move case
        if (currentMove == computerColour)
        {
            currentMovesList = getMovesList(board, n, computerColour);

            if (*(currentMovesList) != 100)
            {

                makeMove(board, n, currentMove, &compRow, &compCol);
                printf("Computer places %c at %c%c.\n", computerColour, (char)(compRow + 97), (char)(compCol + 97));
                updateBoard(&board[0][0], n, computerColour, compRow, compCol, false);
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
                else if (*(currentMovesList) == 100)
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
                    updateBoard(&board[0][0], n, humanColour, usrRow, usrCol, false);
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
                else if (*currentMovesList == 100)
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
