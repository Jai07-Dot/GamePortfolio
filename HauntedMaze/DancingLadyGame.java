
// By Jaira Settles
import java.util.Scanner;

public class DancingLadyGame {

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);// Creates a Scanner Called sc
		System.out.println("You see a creepy dancing lady in the middle of the street");
		System.out.println("On a scale of 1-10, How brave are you");
		int courage = sc.nextInt();// reads number from the keyboard
		sc.nextLine();// Clears out put
		// First decision
		// High Courage path
		if (courage >= 5) {
			System.out.println("She notices you. Do you want to watch her or approach?");
			String action = sc.nextLine();

			if (action.equalsIgnoreCase("watch")) {
				System.out.println("she spins beautifully. Do you want to clap to her dance or stay quiet?");
				String reaction = sc.nextLine();

				if (reaction.equalsIgnoreCase("clap")) {
					System.out.println("She bows due to the kindess and then vanishes. Ending 1 : The Polite Witness");
					// First Ending 
				} else {
					System.out.println("Do you evenually hum along to her dance? (yes/No)");
					String hum = sc.nextLine();
					if (hum.equalsIgnoreCase("yes")) {
						// Ending 2
						System.out.println(
								"She joins your song, then drags you away in the night, leaving only silence. (Ending 2: The last Note)");
					} else {
						System.out.println(
								"She fades away silently, leaving behind a terrifying feeling  (Ending 3: Silence is the Answer)");

					}

				}
				
			} else if (action.equalsIgnoreCase("approach")) {
				System.out.println("She extends her hand to you. Do you dance or refuse?");
				String offer = sc.nextLine();
				if (offer.equalsIgnoreCase("dance")) {
					if (courage >= 8) {
						System.out.println("You dance gracefully and free her spirit. Ending 4 : The liberator");

					} else {
						System.out.println("She twirls faster. Do you keep dancing or break away?");
						String spin = sc.nextLine();
						if (spin.equalsIgnoreCase("keep dancing")) {
							System.out.println("You vanish with her into the night. Ending 5: Eternal Waltz");
						} else {
							System.out.println(
									"You break away and survive, but are for evercursed. Ending 6: The Escaped and Scarred");
						}
					}
				} else {
					System.out.println("You refuse. She glares and disappears in rage. Ending 7: The Rejection");

				}
			}//closes High courage path

			}
			// starts low courage path
			else {
				System.out.println(" She Notices you, Do you Scream or Hide");
				String choice = sc.nextLine();

				if (choice.equalsIgnoreCase("Scream")) {
					System.out.println("Do you run after screaming? (Yes/No)");
					String run = sc.nextLine();

					if (run.equalsIgnoreCase("yes") && courage < 5) {
						System.out
								.println("You escape,but sounds of her dances still haunts you. Ending 8: The Runner");
					} else {
						System.out.println("she covers your mouth then drags you away. Ending 9: Silent Whimpers");
					}
				} else if (choice.equalsIgnoreCase("Hide")) {
					System.out.println(
							"You try to hide but she finds you, and takes you away with her. Ending 10: The Hiding soul"); // Last Ending
				}
			}
			sc.close();
		}
	}
