
import javax.swing.JFrame;

public class App {

    // Main method = starts the game
    public static void main(String[] args) {

        // Create window
        JFrame frame = new JFrame("Pac Man");

        // Create game panel
        PacMan pacmanGame = new PacMan();

        // Add game panel into window
        frame.add(pacmanGame);

        // Resize window to fit panel
        frame.pack();

        // Center window
        frame.setLocationRelativeTo(null);

        // Prevent resizing
        frame.setResizable(false);

        // Close app when X pressed
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // Show window
        frame.setVisible(true);

        // Allow keyboard input
        pacmanGame.requestFocusInWindow();
    }
}