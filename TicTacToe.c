#include <stdio.h>
#include <math.h>
#include <limits.h>

#define SIZE 3

#define BOOLEAN int
#define TRUE 1
#define FALSE 0

typedef enum {
    EMPTY = 0,
    PLAYER = 1,
    BOT = -1
} gridElement;

typedef enum {
    WIN = 1,
    LOSS = -1,
    DRAW = 0
} result;

typedef enum {
    EASY = 2,
    MODERATE = 3,
    HARD = 4,
    IMPOSSIBLE = 100
} difficultyLevel;

typedef struct move {
    int rowNum;
    int colNum;
} Move;

typedef gridElement board[SIZE][SIZE];

// utility function to find the maximum of two numbers
int max(int num1, int num2) {
    if (num1 > num2)
        return num1;
    return num2;
}

// utility function to find the minimum of two numbers
int min(int num1, int num2) {
    if (num1 < num2)
        return num1;
    return num2;
}

// function to return the appropriate grid character corresponding to the grid element
char gridChar(gridElement gridValue) {
    switch (gridValue) {
        case EMPTY:
            return '-';
        case PLAYER:
            return 'X';
        case BOT:
            return 'O';
        default:
            return '-';
    }
}

// function that outputs the current state of the game board.
void printBoard(const board state) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            printf("%c ", gridChar(state[i][j]));
        printf("\n");
    }
}

// utility function to check if the board is full or still has empty spaces left.
BOOLEAN isFull(const board state) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (state[i][j] == EMPTY)
                return FALSE;
        }
    }
    return TRUE;
}

BOOLEAN checkForWin(const board state, gridElement element) {
    // Checking row elements
    for (int i = 0; i < SIZE; i++) {
        if (state[i][0] == element && state[i][1] == element && state[i][2] == element)
            return TRUE;
    }

    // Checking column elements
    for (int i = 0; i < SIZE; i++) {
        if (state[0][i] == element && state[1][i] == element && state[2][i] == element)
            return TRUE;
    }

    // Checking diagonal elements
    if (state[0][0] == element && state[1][1] == element && state[2][2] == element)
        return TRUE;

    if (state[2][0] == element && state[1][1] == element && state[0][2] == element)
        return TRUE;

    return FALSE;
}

int evaluateState(const board currentState) {
    if (checkForWin(currentState, PLAYER))
        return WIN;

    if (checkForWin(currentState, BOT))
        return LOSS;

    return DRAW;
}

// function to check if the game is over.
BOOLEAN isTerminalState(const board state) {
    if (checkForWin(state, PLAYER) || checkForWin(state, BOT) || isFull(state))
        return TRUE;
    return FALSE;
}

int minimax(board state, int depth, BOOLEAN isMaximizingPlayer) {
    BOOLEAN isMinimizingPlayer = !isMaximizingPlayer;

    if (depth == 0 || isTerminalState(state))
        return evaluateState(state);

    int maxScore; int evalScore;
    if (isMaximizingPlayer) {
        maxScore = INT_MIN;

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (state[i][j] == EMPTY) {
                    state[i][j] = BOT;
                    evalScore = minimax(state, depth - 1, FALSE);
                    state[i][j] = EMPTY;
                    maxScore = max(maxScore, evalScore);
                }
            }
        }
        return maxScore;
    }

    if (isMinimizingPlayer) {
        int minScore = INT_MAX;

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (state[i][j] == EMPTY) {
                    state[i][j] = PLAYER;
                    evalScore = minimax(state, depth - 1, TRUE);
                    state[i][j] = EMPTY;
                    minScore = min(minScore, evalScore);
                }
            }
        }
        return minScore;
    }
    return 0;
}


BOOLEAN isValidMove(const board state, Move move) {
    if (state[move.rowNum][move.colNum] != EMPTY)
        return FALSE;
    if (move.rowNum >= SIZE || move.colNum >= SIZE || move.rowNum < 0 || move.colNum < 0)
        return FALSE;
    return TRUE;
}

void makeMove(board state, Move move, gridElement element) {
    state[move.rowNum][move.colNum] = element;
}

void botMove(board state, difficultyLevel depth) {
    int bestMoveScore = INT_MIN;
    Move currentMove = {0, 0};
    Move bestMove = {0, 0};
    for (int i = 0; i < SIZE; i++) {
        currentMove.rowNum = i;
        for (int j = 0; j < SIZE; j++) {
            currentMove.colNum = j;
            if (isValidMove(state, currentMove)) {
                makeMove(state, currentMove, BOT);
                int currentMoveScore = -minimax(state, depth, FALSE);
                makeMove(state, currentMove, EMPTY);
                if (currentMoveScore > bestMoveScore) {
                    bestMoveScore = currentMoveScore;
                    bestMove = currentMove;
                }
            }
        }
    }
    printf("Game state after computer move\n");
    makeMove(state, bestMove, BOT);
}

void getPlayerMove(board state) {
    int inputGridNum;
    Move playerMove;
    BOOLEAN isValid = FALSE;
    do {
        printf("Enter your move ");
        scanf("%d", &inputGridNum);
        playerMove.rowNum = (inputGridNum - 1) / 3;
        playerMove.colNum = (inputGridNum - 1) % 3;
        if (isValidMove(state, playerMove)) {
            makeMove(state, playerMove, PLAYER);
            isValid = TRUE;
        }
    } while (!isValid);
}

int main() {
    board state = {
            {EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY}
    };

    int difficulty = IMPOSSIBLE;
    BOOLEAN retry;

    do {
        printBoard(state);

        while (!isTerminalState(state)) {
            getPlayerMove(state);
            printBoard(state);

            if (isTerminalState(state))
                break;

            botMove(state, difficulty);
            printBoard(state);
        }

        if (evaluateState(state) == WIN)
            printf("You win!");
        if (evaluateState(state) == LOSS)
            printf("You lose!");
        if (evaluateState(state) == DRAW)
            printf("The match is a tie!");

        BOOLEAN isValidResponse = FALSE;
        do {
            printf("Do you want to try again? (Y/N)");
            char response;
            scanf(" %c", &response);
            if (response == 'Y' || response == 'y') {
                retry = TRUE;
                isValidResponse = TRUE;
            } else if (response == 'N' || response == 'n') {
                retry = FALSE;
                isValidResponse = TRUE;
            } else {
                printf("Invalid reponse");
                isValidResponse = FALSE;
            }
        } while (!isValidResponse);
    } while (retry);

    return 0;
}
