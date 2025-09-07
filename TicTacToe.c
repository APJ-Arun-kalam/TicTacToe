#include <stdio.h>
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

char gridChar(gridElement gridValue) {
    switch (gridValue) {
        case EMPTY: return '-';
        case PLAYER: return 'X';
        case BOT: return 'O';
        default: return '-';
    }
}

void printBoard(const board state) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            printf("%c ", gridChar(state[i][j]));
        printf("\n");
    }
}

BOOLEAN isFull(const board state) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (state[i][j] == EMPTY)
                return FALSE;
    return TRUE;
}

BOOLEAN checkForWin(const board state, gridElement element) {
    for (int i = 0; i < SIZE; i++) {
        if (state[i][0] == element && state[i][1] == element && state[i][2] == element)
            return TRUE;
        if (state[0][i] == element && state[1][i] == element && state[2][i] == element)
            return TRUE;
    }
    if (state[0][0] == element && state[1][1] == element && state[2][2] == element)
        return TRUE;
    if (state[2][0] == element && state[1][1] == element && state[0][2] == element)
        return TRUE;
    return FALSE;
}

int evaluateState(const board currentState) {
    if (checkForWin(currentState, BOT))
        return 10;
    if (checkForWin(currentState, PLAYER))
        return -10;
    return 0;
}

BOOLEAN isTerminalState(const board state) {
    if (checkForWin(state, PLAYER) || checkForWin(state, BOT) || isFull(state))
        return TRUE;
    return FALSE;
}

int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a < b) ? a : b; }

int minimax(board state, int depth, BOOLEAN isMaximizing) {
    int score = evaluateState(state);
    if (score == 10 || score == -10 || isFull(state) || depth == 0)
        return score;

    if (isMaximizing) {
        int best = INT_MIN;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (state[i][j] == EMPTY) {
                    state[i][j] = BOT;
                    best = max(best, minimax(state, depth - 1, FALSE));
                    state[i][j] = EMPTY;
                }
            }
        }
        return best;
    } else {
        int best = INT_MAX;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (state[i][j] == EMPTY) {
                    state[i][j] = PLAYER;
                    best = min(best, minimax(state, depth - 1, TRUE));
                    state[i][j] = EMPTY;
                }
            }
        }
        return best;
    }
}

BOOLEAN isValidMove(const board state, Move move) {
    if (move.rowNum < 0 || move.rowNum >= SIZE || move.colNum < 0 || move.colNum >= SIZE)
        return FALSE;
    if (state[move.rowNum][move.colNum] != EMPTY)
        return FALSE;
    return TRUE;
}

void makeMove(board state, Move move, gridElement element) {
    state[move.rowNum][move.colNum] = element;
}

void botMove(board state, difficultyLevel depth) {
    int bestMoveScore = INT_MIN;
    Move bestMove = {0, 0};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Move currentMove = {i, j};
            if (isValidMove(state, currentMove)) {
                makeMove(state, currentMove, BOT);
                int currentMoveScore = minimax(state, depth - 1, FALSE);
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
        printf("Enter your move (1-9): ");
        if (scanf("%d", &inputGridNum) != 1) {
            int c;
            while ((c = getchar()) != EOF && c != '\n') {}
            printf("Invalid input. Please enter a number between 1 and 9.\n");
            continue;
        }
        if (inputGridNum < 1 || inputGridNum > 9) {
            printf("Please enter a number between 1 and 9.\n");
            continue;
        }
        playerMove.rowNum = (inputGridNum - 1) / SIZE;
        playerMove.colNum = (inputGridNum - 1) % SIZE;
        if (isValidMove(state, playerMove)) {
            makeMove(state, playerMove, PLAYER);
            isValid = TRUE;
        } else {
            printf("Cell already occupied or invalid. Try again.\n");
        }
    } while (!isValid);
}

int main() {
    board state;
    int difficulty = IMPOSSIBLE;
    BOOLEAN retry;

    do {
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                state[i][j] = EMPTY;

        printf("Board positions mapping:\n1 2 3\n4 5 6\n7 8 9\n");

        printBoard(state);

        while (!isTerminalState(state)) {
            getPlayerMove(state);
            printBoard(state);

            if (isTerminalState(state))
                break;

            botMove(state, difficulty);
            printBoard(state);
        }

        int final = evaluateState(state);
        if (final == -10)
            printf("You win!\n");
        else if (final == 10)
            printf("You lose!\n");
        else
            printf("The match is a tie!\n");

        BOOLEAN isValidResponse = FALSE;
        do {
            printf("Do you want to try again? (Y/N): ");
            char response;
            scanf(" %c", &response);
            if (response == 'Y' || response == 'y') {
                retry = TRUE;
                isValidResponse = TRUE;
            } else if (response == 'N' || response == 'n') {
                retry = FALSE;
                isValidResponse = TRUE;
            } else {
                printf("Invalid response\n");
                isValidResponse = FALSE;
                int c;
                while ((c = getchar()) != EOF && c != '\n') {}
            }
        } while (!isValidResponse);
    } while (retry);

    return 0;
}
