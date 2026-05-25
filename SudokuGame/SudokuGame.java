import java.util.Scanner;
import java.util.Random;

public class SudokuGame {
    private static final int GRID_SIZE = 9;
    private static int[][] board = new int[GRID_SIZE][GRID_SIZE];
    
    private static final int[][][] ALL_PUZZLES = {
        // ... (Keep the same 3D array from the last step here!) ...
        {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
        },
        {
            {0, 0, 0, 2, 6, 0, 7, 0, 1},
            {6, 8, 0, 0, 7, 0, 0, 9, 0},
            {1, 9, 0, 0, 0, 4, 5, 0, 0},
            {8, 2, 0, 1, 0, 0, 0, 4, 0},
            {0, 0, 4, 6, 0, 2, 9, 0, 0},
            {0, 5, 0, 0, 0, 3, 0, 2, 8},
            {0, 0, 9, 3, 0, 0, 0, 7, 4},
            {0, 4, 0, 0, 5, 0, 0, 3, 6},
            {7, 0, 3, 0, 1, 8, 0, 0, 0}
        },
        {
            {1, 0, 0, 4, 8, 9, 0, 0, 6},
            {7, 3, 0, 0, 0, 0, 0, 4, 0},
            {0, 0, 0, 0, 0, 1, 2, 9, 5},
            {0, 0, 7, 1, 2, 0, 6, 0, 0},
            {5, 0, 0, 7, 0, 3, 0, 0, 8},
            {0, 0, 6, 0, 9, 5, 7, 0, 0},
            {9, 1, 4, 6, 0, 0, 0, 0, 0},
            {0, 2, 0, 0, 0, 0, 0, 3, 7},
            {8, 0, 0, 5, 1, 2, 0, 0, 4}
        }
    };

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Welcome to Sudoku!");
        
        loadRandomBoard();
        
        // --- THE GAME LOOP ---
        while (!isBoardFull()) {
            printBoard();
            
            System.out.println("\nEnter your move (or type -1 to quit).");
            
            System.out.print("Row (1-9): ");
            int inputRow = scanner.nextInt();
            if (inputRow == -1) break; // Quit option
            
            System.out.print("Column (1-9): ");
            int inputCol = scanner.nextInt();
            
            System.out.print("Number (1-9): ");
            int num = scanner.nextInt();
            
            // Convert user 1-9 input to array 0-8 indices
            int row = inputRow - 1;
            int col = inputCol - 1;
            
            // Basic boundaries check
            if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE || num < 1 || num > 9) {
                System.out.println("Invalid input! Rows/Cols/Numbers must be between 1 and 9.");
                continue; // Skips back to the top of the loop
            }
            
            // Check if the cell is already occupied
            if (board[row][col] != 0) {
                System.out.println("That spot is already filled! Try an empty spot.");
                continue;
            }
            
            // Check Sudoku rules
            if (isValidMove(row, col, num)) {
                board[row][col] = num;
                System.out.println("Nice move!");
            } else {
                System.out.println("Invalid move! That violates Sudoku rules (check row, column, or 3x3 box).");
            }
        }
        
        // Game Over evaluation
        if (isBoardFull()) {
            printBoard();
            System.out.println("\nCongratulations! You solved the Sudoku puzzle!");
        } else {
            System.out.println("\nThanks for playing!");
        }
        
        scanner.close();
    }

    // Checks if there are any remaining empty cells (0s)
    private static boolean isBoardFull() {
        for (int r = 0; r < GRID_SIZE; r++) {
            for (int c = 0; c < GRID_SIZE; c++) {
                if (board[r][c] == 0) {
                    return false; // Found an empty slot, so it's not full
                }
            }
        }
        return true; 
    }

    // Keep your loadRandomBoard(), printBoard(), and isValidMove() methods from before!
    private static void loadRandomBoard() {
        Random rand = new Random();
        int puzzleIndex = rand.nextInt(ALL_PUZZLES.length);
        int[][] selectedPuzzle = ALL_PUZZLES[puzzleIndex];
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                board[i][j] = selectedPuzzle[i][j];
            }
        }
        System.out.println("Loaded Puzzle Layout #" + (puzzleIndex + 1));
    }

    private static void printBoard() {
        for (int row = 0; row < GRID_SIZE; row++) {
            if (row % 3 == 0 && row != 0) System.out.println("---------------------");
            for (int col = 0; col < GRID_SIZE; col++) {
                if (col % 3 == 0 && col != 0) System.out.print("| ");
                if (board[row][col] == 0) System.out.print(". ");
                else System.out.print(board[row][col] + " ");
            }
            System.out.println();
        }
    }

    private static boolean isValidMove(int row, int col, int number) {
        for (int i = 0; i < GRID_SIZE; i++) {
            if (board[row][i] == number) return false;
            if (board[i][col] == number) return false;
        }
        int boxRowStart = row - row % 3;
        int boxColStart = col - col % 3;
        for (int i = boxRowStart; i < boxRowStart + 3; i++) {
            for (int j = boxColStart; j < boxColStart + 3; j++) {
                if (board[i][j] == number) return false;
            }
        }
        return true;
    }
}