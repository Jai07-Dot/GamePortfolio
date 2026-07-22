/*
 * =========================================================================
 * AUTHOR: Jaira Settles
 * PROJECT: The Haunted Waltz 5.0 (2D Grid Maze Engine)
 * COPYRIGHT: (c) 2026 Jaira Settles. All Rights Reserved.
 * =========================================================================
 */

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Random;
import javax.swing.Timer;

public class MazeGame extends JPanel implements KeyListener {

    // AUTHOR & VERSION CONSTANTS
    public static final String AUTHOR = "Jaira Settles";
    public static final String VERSION = "5.0.0";

    // 1. GAME SETTINGS
    private final int TILE_SIZE = 40;
    private final int ROWS = 12;
    private final int COLS = 15;
    
    // PLAYER DATA/GHOST
    private int playerX = 1, playerY = 1; 
    private int ghostX = 13, ghostY = 10;
    private int courage = 0; 
    private int lives = 3;
    private int ghostDelay = 700;
    private boolean isGameOver = false;
    private String storyText = "You see a creepy dancing lady... Use arrow keys to evade or approach.";
    private int[][] currentMaze;
    private Timer ghostTimer;
    private Random rand = new Random();
    
    // Variable used for cosmetic pulsing effects of the ghost
    private double pulseTime = 0;

    // MAZE DATA (3D Array Storage)
    private final int[][][] mazeBank = {
        {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,0,1,0,1,1,1,1,1,1,1,0,1},
            {1,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
            {1,0,1,1,1,1,1,1,1,1,1,0,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        },
        {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
            {1,0,0,0,1,0,1,0,1,0,0,0,1,0,1},
            {1,0,1,0,1,0,0,0,1,0,1,0,1,0,1},
            {1,0,1,0,1,1,1,1,1,0,1,0,0,0,1},
            {1,0,1,0,0,0,0,0,0,0,1,1,1,1,1},
            {1,0,1,1,1,1,1,1,1,0,1,0,0,0,1},
            {1,0,0,0,0,0,0,0,1,0,1,0,1,0,1},
            {1,1,1,1,1,1,1,0,1,0,1,0,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        },
        {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
            {1,0,1,0,1,1,1,1,1,1,1,0,1,0,1},
            {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
            {1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
            {1,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
            {1,0,1,1,1,0,1,0,1,1,1,1,1,0,1},
            {1,0,1,0,1,0,0,0,0,0,0,0,1,0,1},
            {1,0,1,0,1,1,1,1,1,1,1,0,1,0,1},
            {1,0,0,0,1,0,0,0,0,0,1,0,1,0,1},
            {1,0,1,0,0,0,1,0,1,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        }
    };

    public MazeGame() {
        setPreferredSize(new Dimension(COLS * TILE_SIZE, (ROWS * TILE_SIZE) + 100));
        setBackground(new Color(10, 10, 15)); // Darker aesthetic background
        setFocusable(true);
        addKeyListener(this);
        
        currentMaze = mazeBank[0];
        
        // Ghost movement timer loop
        ghostTimer = new Timer(ghostDelay, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (!isGameOver) {
                    moveGhostTowardPlayer();
                    pulseTime += 0.3; // Increment pulse color animation
                    repaint();
                }
            }
        });
        ghostTimer.start();
    }

    private void moveGhostTowardPlayer() {
        int nextX = ghostX;
        int nextY = ghostY;
        
        if (ghostX < playerX) {
            nextX++;
        } else if (ghostX > playerX) {
            nextX--;
        }
         
        // Collision safety backup check
        if (currentMaze[ghostY][nextX] == 1) {
            nextX = ghostX; 
            if (ghostY < playerY) nextY++;
            else if (ghostY > playerY) nextY--;
        } else if (ghostX != playerX) {
            ghostX = nextX;
            checkEncounter();
            return;
        }

        if (currentMaze[nextY][ghostX] == 0){
            ghostY = nextY;
        }
        checkEncounter();
    }

    // SINGLE UNIFIED MOVEMENT LISTENER
    @Override
    public void keyPressed(KeyEvent e) {
        if (isGameOver) return;

        int dx = 0, dy = 0;
        if (e.getKeyCode() == KeyEvent.VK_UP) dy = -1;
        if (e.getKeyCode() == KeyEvent.VK_DOWN) dy = 1;
        if (e.getKeyCode() == KeyEvent.VK_LEFT) dx = -1;
        if (e.getKeyCode() == KeyEvent.VK_RIGHT) dx = 1;

        // Verify bounds & check structural barriers (0 = valid path)
        if (currentMaze[playerY + dy][playerX + dx] == 0) {
            playerX += dx;
            playerY += dy;
        }

        checkEncounter();
        repaint();
    }

    @Override public void keyTyped(KeyEvent e) {}
    @Override public void keyReleased(KeyEvent e) {}

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // Draw Maze Grid Layout
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                if (currentMaze[y][x] == 1) {
                    g2d.setColor(new Color(25, 25, 45)); // Gothic deep blue brick style
                    g2d.fillRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                    g2d.setColor(new Color(40, 40, 70)); // Subtle brick borders
                    g2d.drawRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                }
            }
        }

        // Draw Ghost with an animated, pulsing red aura
        int pulseOffset = (int) (Math.sin(pulseTime) * 4);
        g2d.setColor(new Color(220, 40, 40, 100)); // Transparent red outer aura
        g2d.fillOval(ghostX * TILE_SIZE + 2 - pulseOffset, ghostY * TILE_SIZE + 2 - pulseOffset, 
                     TILE_SIZE - 4 + (pulseOffset * 2), TILE_SIZE - 4 + (pulseOffset * 2));
        g2d.setColor(new Color(160, 0, 0)); // Core solid color
        g2d.fillOval(ghostX * TILE_SIZE + 6, ghostY * TILE_SIZE + 6, TILE_SIZE - 12, TILE_SIZE - 12);

        // Draw Player character with modern look
        g2d.setColor(Color.WHITE);
        g2d.fillOval(playerX * TILE_SIZE + 6, playerY * TILE_SIZE + 6, TILE_SIZE - 12, TILE_SIZE - 12);
        g2d.setColor(new Color(0, 180, 255)); // Inner glowing core ring
        g2d.drawOval(playerX * TILE_SIZE + 6, playerY * TILE_SIZE + 6, TILE_SIZE - 12, TILE_SIZE - 12);

        // UI Dashboard Panel Footer
        g2d.setColor(new Color(15, 15, 22));
        g2d.fillRect(0, ROWS * TILE_SIZE, getWidth(), 100);
        g2d.setColor(new Color(50, 50, 70));
        g2d.drawRect(0, ROWS * TILE_SIZE, getWidth(), 100);
        
        // Render UI Text Content
        g2d.setColor(new Color(220, 220, 220));
        g2d.setFont(new Font("Monospaced", Font.PLAIN, 13));
        g2d.drawString(storyText, 20, (ROWS * TILE_SIZE) + 40);
        g2d.drawString("Lives left: " + lives + "  |  Developer: " + AUTHOR, 20, (ROWS * TILE_SIZE) + 70);
    }

    private void checkEncounter() {
        if (playerX == ghostX && playerY == ghostY && !isGameOver) {
            ghostTimer.stop(); 

            String input = JOptionPane.showInputDialog(this, 
                    "You stand face-to-face with the Dancing Lady...\nOn a scale of 1-10, how brave do you feel?", 
                    "The Bravery Check", JOptionPane.QUESTION_MESSAGE);

            try {
                courage = Integer.parseInt(input);
            } catch (NumberFormatException e) {
                courage = 5; 
            }

            // High Courage Logic Tree
            if (courage >= 5) {
                String[] options = {"Watch her dance", "Approach her"};
                int action = JOptionPane.showOptionDialog(this,
                        "She notices you. Do you want to watch her or approach?",
                        "The Dancing Lady", JOptionPane.DEFAULT_OPTION, 
                        JOptionPane.WARNING_MESSAGE, null, options, options[0]);

                if (action == 0) { 
                    String[] watchOptions = {"Clap politely", "Stay silent"};
                    int reaction = JOptionPane.showOptionDialog(this,
                            "She spins beautifully. Do you want to clap to her dance or stay quiet?",
                            "Watching...", JOptionPane.DEFAULT_OPTION, 
                            JOptionPane.QUESTION_MESSAGE, null, watchOptions, watchOptions[0]);

                    if (reaction == 0) {
                        JOptionPane.showMessageDialog(this, "She bows gracefully and vanishes.\nEnding 1: The Polite Witness");
                    } else {
                        String[] humOptions = {"Yes", "No"};
                        int hum = JOptionPane.showOptionDialog(this, "Do you eventually hum along to her rhythm?", 
                                "Silence...", JOptionPane.DEFAULT_OPTION, 
                                JOptionPane.QUESTION_MESSAGE, null, humOptions, humOptions[0]);
                        if (hum == 0) {
                            JOptionPane.showMessageDialog(this, "She joins your song, then drags you away into the dark...\nEnding 2: The Last Note");
                        } else {
                            JOptionPane.showMessageDialog(this, "She fades away silently, leaving a cold shiver behind.\nEnding 3: Silence is the Answer");
                        }
                    }
                } else { 
                    String[] offerOptions = {"Dance with her", "Refuse her hand"};
                    int offer = JOptionPane.showOptionDialog(this, "She extends her hand to you. Do you dance or refuse?", 
                            "Approaching...", JOptionPane.DEFAULT_OPTION, 
                            JOptionPane.QUESTION_MESSAGE, null, offerOptions, offerOptions[0]);

                    if (offer == 0) {
                        if (courage >= 8) {
                            JOptionPane.showMessageDialog(this, "You waltz gracefully and set her bound spirit free!\nEnding 4: The Liberator");
                        } else {
                            String[] spinOptions = {"Keep dancing", "Break away!"};
                            int spin = JOptionPane.showOptionDialog(this, "She twirls drastically faster. Do you keep dancing or break away?", 
                                    "The Waltz", JOptionPane.DEFAULT_OPTION, 
                                    JOptionPane.QUESTION_MESSAGE, null, spinOptions, spinOptions[0]);
                            if (spin == 0) {
                                JOptionPane.showMessageDialog(this, "You vanish forever with her into the night map.\nEnding 5: Eternal Waltz");
                            } else {
                                JOptionPane.showMessageDialog(this, "You break away and survive, but carry an eternal curse.\nEnding 6: The Escaped and Scarred");
                            }
                        }
                    } else {
                        JOptionPane.showMessageDialog(this, "You refuse. She glares at you with burning rage and vanishes.\nEnding 7: The Rejection");
                    }
                }
            } else { // Low Courage Logic Tree (< 5)
                String[] lowOptions = {"Scream for help", "Hide in the shadows"};
                int choice = JOptionPane.showOptionDialog(this, "She spots you freezing! Do you Scream or Hide?", 
                        "Panic!", JOptionPane.DEFAULT_OPTION, 
                        JOptionPane.WARNING_MESSAGE, null, lowOptions, lowOptions[0]);

                if (choice == 0) {
                    String[] runOptions = {"Yes", "No"};
                    int run = JOptionPane.showOptionDialog(this, "Do you run as fast as you can after screaming?", 
                            "Screaming...", JOptionPane.DEFAULT_OPTION, 
                            JOptionPane.QUESTION_MESSAGE, null, runOptions, runOptions[0]);
                    if (run == 0) {
                        JOptionPane.showMessageDialog(this, "You run out, but the echo of her music follows you home.\nEnding 8: The Runner");
                    } else {
                        JOptionPane.showMessageDialog(this, "She instantly silences your screams and whisks you away.\nEnding 9: Silent Whimpers");
                    }
                } else {
                    JOptionPane.showMessageDialog(this, "You try to blend into the shadows, but she finds you instantly.\nEnding 10: The Hiding Soul");
                }
            }

            handleLifeDeduction();
        }
    }

    private void handleLifeDeduction() {
        lives--;
        if (lives > 0) {
            JOptionPane.showMessageDialog(this, "You survived the encounter but lost a portion of your sanity! The nightmare shifts...");
            resetLevel();
        } else {
            storyText = "GAME OVER. Your soul was claimed completely by the Dancing Lady.";
            isGameOver = true;
            ghostTimer.stop();
            repaint();
        }
    }

    private void resetLevel() {
        playerX = 1;
        playerY = 1;
        ghostX = 13;
        ghostY = 10;
        
        // Random layout pick from 3D storage deck
        currentMaze = mazeBank[rand.nextInt(mazeBank.length)];
        
        // Adjust and normalize game difficulty scaling based on previous choices
        if (courage < 5 && courage > 0) {
            ghostDelay = 300; // Fast tracking if panic was detected
            storyText = "Fear paralyzes you... She senses it and starts much closer!";
        } else {
            ghostDelay = Math.max(250, ghostDelay - 150); // Steady scaling drop down to a floor of 250ms
            storyText = "The rooms have shifted configurations... Keep your composure.";
        }
        
        ghostTimer.setDelay(ghostDelay);
        ghostTimer.start(); 
        repaint();
    }

    public static void main(String[] args) {
        System.out.println("==================================================");
        System.out.println(" Launching The Haunted Waltz " + VERSION);
        System.out.println(" Developer: " + AUTHOR);
        System.out.println(" Copyright (c) 2026 " + AUTHOR + ". All Rights Reserved.");
        System.out.println("==================================================");

        JFrame frame = new JFrame("The Haunted Waltz 5.0 - [Dev: " + AUTHOR + "]");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(new MazeGame());
        frame.pack();
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);
    }
}
