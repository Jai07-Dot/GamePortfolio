import java.util.Scanner;

public class BlackjackGame {
    
    // FIXED: Capitalized 'Card' to match your class name.
    // ALSO ADDED: Real Ace logic so Aces can count as 11 if it helps the player.
    public static int computeScore(Card[] hand, int index) {
        int score = 0;
        boolean hasAce = false;
        
        for (int i = 0; i < index; i++) {
            if (hand[i].getValue() == 1) {
                hasAce = true;
            }
            score += hand[i].getValue();
        }
        
        // If the hand has an Ace and adding 10 won't cause a bust, treat Ace as 11
        if (hasAce && (score + 10 <= 21)) {
            score += 10;
        }
        
        return score;
    }

    public static void main(String[] args) {
        Card[] playerHand = new Card[10];
        int playerIndex = 0;
        Card[] dealerHand = new Card[10];
        int dealerIndex = 0;
        
        String[] suits = {"Spades", "Hearts", "Clubs", "Diamonds"};
        String[] displayValues = {"Two", "Three", "Four", "Five", 
                                    "Six", "Seven", "Eight", "Nine",
                                    "Ten", "Jack", "Queen", "King",
                                    "Ace"};
        int[] values = {2, 3, 4, 5,
                        6, 7, 8, 9,
                        10, 10, 10, 10,
                        1}; // Ace defaults to 1, handled dynamically in computeScore
        
        Deck d = new Deck(suits, displayValues, values);
        d.shuffle(10000);
        
        // Deal initial cards
        playerHand[playerIndex++] = d.deal();
        dealerHand[dealerIndex++] = d.deal();
        playerHand[playerIndex++] = d.deal();
        dealerHand[dealerIndex++] = d.deal();
        
        System.out.println("--- Welcome to Blackjack ---");
        System.out.println("Player hand: ");
        for (int i = 0; i < playerIndex; i++) {
            System.out.println("  " + playerHand[i]);
        }
        
        System.out.println("\nDealer's visible card: ");
        System.out.println("  " + dealerHand[0]); // Only show the first card
        
        Scanner k = new Scanner(System.in);
        String userInput = "yes";
        
        // FIXED: Lowercase 'while', correct variable names, fixed loop mechanics
        while (userInput.toLowerCase().equals("yes") || userInput.toLowerCase().equals("y")) {
            int currentScore = computeScore(playerHand, playerIndex);
            
            if (currentScore >= 21) {
                break; // Stop asking to hit if they bust or hit 21
            }
            
            System.out.print("\nDo you want to hit? (yes/no): ");
            userInput = k.nextLine();
            
            if (userInput.toLowerCase().equals("yes") || userInput.toLowerCase().equals("y")) {
                playerHand[playerIndex++] = d.deal();
                
                System.out.println("\nYour hand now: ");
                for (int i = 0; i < playerIndex; i++) {
                    System.out.println("  " + playerHand[i]);
                }
            }
        }
        
        int playerScore = computeScore(playerHand, playerIndex);
        
        // Dealer AI: Houses usually hit until they reach at least 17 (if player didn't bust)
        if (playerScore <= 21) {
            System.out.println("\nDealer's turn...");
            while (computeScore(dealerHand, dealerIndex) < 17) {
                dealerHand[dealerIndex++] = d.deal();
            }
        }
        
        int dealerScore = computeScore(dealerHand, dealerIndex);
        
        // Print the dealer's complete final hand
        System.out.println("\nDealer's final hand: ");
        for (int i = 0; i < dealerIndex; i++) {
            System.out.println("  " + dealerHand[i]);
        }
        
        // FIXED: Typos in method names and incorrect parameter order
        System.out.println("\nPlayer Score: " + playerScore + " | Dealer Score: " + dealerScore);
        
        // Game outcome logic
        if (playerScore > 21) {
            System.out.println("You busted! Dealer Wins.");
        } else if (dealerScore > 21) {
            System.out.println("Dealer busted! Player wins!");
        } else if (playerScore > dealerScore) {
            System.out.println("Player wins!");
        } else if (dealerScore > playerScore) {
            System.out.println("Dealer Wins.");
        } else {
            System.out.println("It's a tie (Push)!");
        }
        
        k.close();
    }
}