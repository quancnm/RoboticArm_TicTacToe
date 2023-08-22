#include <Arduino.h>

// Define the player identifiers
enum PLAYER {
    none,
    x,
    y
};

// Define the game result possibilities
enum RESULT {
    TIE,
    WIN,
    CONTINUE
};

// Define the MOVE structure
struct MOVE {
    int row;
    int col;
    int score;
};

// Function to get the AI's best move using MiniMax algorithm
MOVE getAIsBestMove(PLAYER board[BOARD_SIZE][BOARD_SIZE], bool currentPlayerX) {
    MOVE availableMovesAndScores[BOARD_SIZE * BOARD_SIZE];
    int count = 0;

    // Iterate through each available move
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == none) {
                // Copy the board to a new instance
                PLAYER tempBoard[BOARD_SIZE][BOARD_SIZE];
                copyBoard(board, tempBoard);

                // Make the move on the new board
                if (currentPlayerX) {
                    tempBoard[i][j] = x;
                } else {
                    tempBoard[i][j] = y;
                }

                // Check the game result
                RESULT result = checkResult(tempBoard);
                int score = 0;
                if (result == TIE) {
                    score = 0;
                } else if (result == WIN) {
                    score = 1;
                } else {
                    MOVE nextMove = getAIsBestMove(tempBoard, !currentPlayerX);
                    score = -nextMove.score;
                }

                // Store the move and its score
                availableMovesAndScores[count].row = i;
                availableMovesAndScores[count].col = j;
                availableMovesAndScores[count].score = score;
                if (score == 1) {
                    return availableMovesAndScores[count];
                }
                count++;
            }
        }
    }

    // Shuffle the availableMovesAndScores array
    for (int i = count - 1; i > 0; i--) {
        int j = random(i + 1);
        MOVE tempMove = availableMovesAndScores[i];
        availableMovesAndScores[i] = availableMovesAndScores[j];
        availableMovesAndScores[j] = tempMove;
    }

    // Sort the array from high to low
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (availableMovesAndScores[i].score < availableMovesAndScores[j].score) {
                MOVE tempMove = availableMovesAndScores[i];
                availableMovesAndScores[i] = availableMovesAndScores[j];
                availableMovesAndScores[j] = tempMove;
            }
        }
    }

    // Return the best move
    return availableMovesAndScores[0];
}

void setup() {
  
}

void loop() {
   
}
