import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;
import javax.swing.*;

public class PacMan extends JPanel implements ActionListener, KeyListener {

    // ==========================
    // BLOCK CLASS
    // ==========================
    class Block {

        int x, y;
        int width, height;

        Image image;

        int startX, startY;

        char direction = 'R';

        int velocityX = 0;
        int velocityY = 0;

        Block(Image image, int x, int y, int width, int height) {
            this.image = image;
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;

            this.startX = x;
            this.startY = y;
        }

        void updateDirection(char direction) {
            this.direction = direction;
            updateVelocity();
        }

        void updateVelocity() {

            if (direction == 'U') {
                velocityX = 0;
                velocityY = -tileSize / 4;
            } else if (direction == 'D') {
                velocityX = 0;
                velocityY = tileSize / 4;
            } else if (direction == 'L') {
                velocityX = -tileSize / 4;
                velocityY = 0;
            } else if (direction == 'R') {
                velocityX = tileSize / 4;
                velocityY = 0;
            }
        }

        void reset() {
            x = startX;
            y = startY;
            velocityX = 0;
            velocityY = 0;
            direction = 'R';
        }
    }

    // ==========================
    // SETTINGS
    // ==========================
    private int rowCount = 21;
    private int columnCount = 19;
    private int tileSize = 32;

    private int boardWidth = columnCount * tileSize;
    private int boardHeight = rowCount * tileSize;

    // ==========================
    // IMAGES
    // ==========================
    private Image wallImage;
    private Image blueGhostImage;
    private Image orangeGhostImage;
    private Image pinkGhostImage;
    private Image redGhostImage;

    private Image pacmanUpImage;
    private Image pacmanDownImage;
    private Image pacmanLeftImage;
    private Image pacmanRightImage;

    // ==========================
    // MAP
    // ==========================
    private String[] tileMap = {
            "XXXXXXXXXXXXXXXXXXX",
            "X        X        X",
            "X XX XXX X XXX XX X",
            "X                 X",
            "X XX X XXXXX X XX X",
            "X    X       X    X",
            "XXXX XXXX XXXX XXXX",
            "OOOX X       X XOOO",
            "XXXX X XXrXX X XXXX",
            "O       bpo       O",
            "XXXX X XXXXX X XXXX",
            "OOOX X       X XOOO",
            "XXXX X XXXXX X XXXX",
            "X        X        X",
            "X XX XXX X XXX XX X",
            "X  X     P     X  X",
            "XX X X XXXXX X X XX",
            "X    X   X   X    X",
            "X XXXXXX X XXXXXX X",
            "X                 X",
            "XXXXXXXXXXXXXXXXXXX"
    };

    // ==========================
    // GAME OBJECTS
    // ==========================
    HashSet<Block> walls;
    HashSet<Block> foods;
    HashSet<Block> ghosts;

    Block pacman;

    Timer gameLoop;
    Random random = new Random();
    char[] directions = {'U', 'D', 'L', 'R'};

    int score = 0;
    int lives = 3;
    boolean gameOver = false;

    Block foodEaten = null;

    // ==========================
    // CONSTRUCTOR
    // ==========================
    public PacMan() {

        setPreferredSize(new Dimension(boardWidth, boardHeight));
        setBackground(Color.BLACK);
        setFocusable(true);
        addKeyListener(this);

        wallImage = new ImageIcon(getClass().getResource("/wall.png")).getImage();

        blueGhostImage = new ImageIcon(getClass().getResource("/blueGhost.png")).getImage();
        orangeGhostImage = new ImageIcon(getClass().getResource("/orangeGhost.png")).getImage();
        pinkGhostImage = new ImageIcon(getClass().getResource("/pinkGhost.png")).getImage();
        redGhostImage = new ImageIcon(getClass().getResource("/redGhost.png")).getImage();

        pacmanUpImage = new ImageIcon(getClass().getResource("/pacmanUp.png")).getImage();
        pacmanDownImage = new ImageIcon(getClass().getResource("/pacmanDown.png")).getImage();
        pacmanLeftImage = new ImageIcon(getClass().getResource("/pacmanLeft.png")).getImage();
        pacmanRightImage = new ImageIcon(getClass().getResource("/pacmanRight.png")).getImage();

        loadMap();
        resetGhostDirections();

        gameLoop = new Timer(50, this);
        gameLoop.start();
    }

    // ==========================
    // RESET
    // ==========================
    public void resetGame() {
        score = 0;
        lives = 3;
        gameOver = false;
        loadMap();
        resetGhostDirections();
    }

    public void resetPositions() {
        pacman.reset();
        for (Block ghost : ghosts) {
            ghost.reset();
            ghost.updateDirection(directions[random.nextInt(4)]);
        }
    }

    public void resetGhostDirections() {
        for (Block ghost : ghosts) {
            ghost.reset();
            ghost.updateDirection(directions[random.nextInt(4)]);
        }
    }

    // ==========================
    // LOAD MAP
    // ==========================
    public void loadMap() {

        walls = new HashSet<>();
        foods = new HashSet<>();
        ghosts = new HashSet<>();

        for (int r = 0; r < rowCount; r++) {
            String row = tileMap[r];

            for (int c = 0; c < columnCount; c++) {

                char tile = row.charAt(c);

                int x = c * tileSize;
                int y = r * tileSize;

                if (tile == 'X') walls.add(new Block(wallImage, x, y, tileSize, tileSize));
                else if (tile == 'b') ghosts.add(new Block(blueGhostImage, x, y, tileSize, tileSize));
                else if (tile == 'o') ghosts.add(new Block(orangeGhostImage, x, y, tileSize, tileSize));
                else if (tile == 'p') ghosts.add(new Block(pinkGhostImage, x, y, tileSize, tileSize));
                else if (tile == 'r') ghosts.add(new Block(redGhostImage, x, y, tileSize, tileSize));
                else if (tile == 'P') pacman = new Block(pacmanRightImage, x, y, tileSize, tileSize);
                else if (tile == ' ') foods.add(new Block(null, x + 14, y + 14, 4, 4));
            }
        }
    }

    // ==========================
    // GAME LOOP
    // ==========================
    @Override
    public void actionPerformed(ActionEvent e) {
        if (!gameOver) move();
        repaint();
    }

    // ==========================
    // MOVE LOGIC
    // ==========================
    public void move() {

        // PACMAN
        pacman.x += pacman.velocityX;
        pacman.y += pacman.velocityY;

        for (Block wall : walls) {
            if (collision(pacman, wall)) {
                pacman.x -= pacman.velocityX;
                pacman.y -= pacman.velocityY;
                break;
            }
        }

        // FOOD
        foodEaten = null;
        for (Block food : foods) {
            if (collision(pacman, food)) {
                foodEaten = food;
                score += 10;
                break;
            }
        }
        if (foodEaten != null) foods.remove(foodEaten);

        // ==========================
        // GHOST MOVEMENT
        // ==========================
        for (Block ghost : ghosts) {

            ghost.x += ghost.velocityX;
            ghost.y += ghost.velocityY;

            boolean blocked = false;

            for (Block wall : walls) {
                if (collision(ghost, wall)) {
                    blocked = true;
                    break;
                }
            }

            if (ghost.x < 0 || ghost.x > boardWidth - tileSize ||
                ghost.y < 0 || ghost.y > boardHeight - tileSize) {
                blocked = true;
            }

            // CRITICAL FIX: If they aren't blocked, give them a small 10% chance to 
            // naturally try a random turn every time they are perfectly centered on a tile.
            if (!blocked && ghost.x % tileSize == 0 && ghost.y % tileSize == 0) {
                if (random.nextInt(10) == 0) { 
                    char randomDir = directions[random.nextInt(4)];
                    ghost.updateDirection(randomDir);
                }
            }

            if (blocked) {
                ghost.x -= ghost.velocityX;
                ghost.y -= ghost.velocityY;

                // Pick a new direction that isn't the one we just crashed into
                ArrayList<Character> alternatives = new ArrayList<>();
                for (char d : directions) {
                    if (d != ghost.direction) {
                        alternatives.add(d);
                    }
                }
                char newDir = alternatives.get(random.nextInt(alternatives.size()));
                ghost.updateDirection(newDir);
            }

            if (collision(pacman, ghost)) {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                } else {
                    resetPositions();
                }
            }
        }
    }

    // ==========================
    // COLLISION
    // ==========================
    public boolean collision(Block a, Block b) {
        return a.x < b.x + b.width &&
               a.x + a.width > b.x &&
               a.y < b.y + b.height &&
               a.y + a.height > b.y;
    }

    // ==========================
    // DRAW
    // ==========================
    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        g.drawImage(pacman.image, pacman.x, pacman.y, pacman.width, pacman.height, null);

        for (Block ghost : ghosts)
            g.drawImage(ghost.image, ghost.x, ghost.y, ghost.width, ghost.height, null);

        for (Block wall : walls)
            g.drawImage(wall.image, wall.x, wall.y, wall.width, wall.height, null);

        g.setColor(Color.WHITE);
        for (Block food : foods)
            g.fillRect(food.x, food.y, food.width, food.height);

        g.setFont(new Font("Arial", Font.PLAIN, 18));

        if (gameOver)
            g.drawString("Game Over! Score: " + score, 20, 20);
        else
            g.drawString("Lives: " + lives + " Score: " + score, 20, 20);
    }

    // ==========================
    // CONTROLS
    // ==========================
    @Override
    public void keyPressed(KeyEvent e) {

        if (e.getKeyCode() == KeyEvent.VK_UP) {
            pacman.updateDirection('U');
            pacman.image = pacmanUpImage;
        }
        else if (e.getKeyCode() == KeyEvent.VK_DOWN) {
            pacman.updateDirection('D');
            pacman.image = pacmanDownImage;
        }
        else if (e.getKeyCode() == KeyEvent.VK_LEFT) {
            pacman.updateDirection('L');
            pacman.image = pacmanLeftImage;
        }
        else if (e.getKeyCode() == KeyEvent.VK_RIGHT) {
            pacman.updateDirection('R');
            pacman.image = pacmanRightImage;
        }
        else if (e.getKeyCode() == KeyEvent.VK_R) {
            resetGame();
        }
    }

    @Override public void keyReleased(KeyEvent e) {}
    @Override public void keyTyped(KeyEvent e) {}
}