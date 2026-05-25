
import java.util.Random;

public class Deck {
	//Card objects
	//Deal, Shuffle 

	private Card[] cards;
	private int deckPos;

	public Deck()
	{
		cards = new Card[52];
		deckPos = 0;
	}

	public Deck(String[] suits, String[] displayValues, int[] values)
	{
		cards = new Card[suits.length * values.length];

		deckPos = 0;

		for(int i = 0; i < suits.length; i++)
		{
			for(int j = 0; j < values.length; j++)
			{
				Card c = new Card(suits[i], displayValues[j], values[j]);
				cards[deckPos] = c;
				deckPos++;
			}
		}
		deckPos = 0;
	}

	public Card[] getCards()
	{
		return cards;
	}

	public String toString()
	{
		String returnString = "";
		for(int i = 0; i < getCards().length; i++)
		{
			returnString += getCards()[i] + "\n";
		}
		return returnString;
	}

	public Card deal()
	{
		//TODO: Fix out of bounds issue here
		Card returnCard = getCards()[deckPos];
		deckPos++;
		return returnCard;
	}

	public void shuffle(int swaps)
	{
		Random r = new Random();
		//		System.out.println(r.nextInt(cards.length));
		for(int i = 0; i < swaps; i++)
		{
			int card1 = r.nextInt(cards.length);
			int card2 = r.nextInt(cards.length);

			Card temp = cards[card1];
			cards[card1] = cards[card2];
			cards[card2] = temp;
		}
	}
}












