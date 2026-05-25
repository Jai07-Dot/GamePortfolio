
public class Card {
	//Step 1: Attributes
	//They should be private so we don't access them incorrectly
	private String suit;
	private String displayValue;
	private int value;
	
	//Step 2: Constructors
	//These are like the "printer" for the card
	//They make the object exist
	
	//Default
	//Set values to something "default"
	public Card()
	{
		setSuit("Spades");
		setDisplayValue("2");
		setValue(2);
	}
	
	//Parameterized
	//Set values to whatever we want
	public Card(String aS, String aD, int aV)
	{
		setSuit(aS);
		setDisplayValue(aD);
		setValue(aV);
	}
	
	//Step 3: Getters and Setters
	
	public void setSuit(String aS)
	{
		this.suit = aS;
	}
	
	public String getSuit()
	{
		return this.suit;
	}

	public String getDisplayValue() 
	{
		return displayValue;
	}

	public void setDisplayValue(String displayValue) 
	{
		this.displayValue = displayValue;
	}

	public int getValue() 
	{
		return value;
	}

	public void setValue(int value) 
	{
		this.value = value;
	}
	
	public String toString() {
	    // Get the visual symbol for the suit
	    String symbol = "";
	    switch (this.suit.toLowerCase()) {
	        case "spades":   symbol = "♠"; break;
	        case "hearts":   symbol = "♥"; break;
	        case "diamonds": symbol = "♦"; break;
	        case "clubs":    symbol = "♣"; break;
	        default:         symbol = "?"; break;
	    }

	    // Get a short 1 or 2 letter display value (e.g., "Ace" -> "A", "Ten" -> "10")
	    String shortRank = "";
	    if (this.displayValue.equals("Ace")) shortRank = "A ";
	    else if (this.displayValue.equals("King")) shortRank = "K ";
	    else if (this.displayValue.equals("Queen")) shortRank = "Q ";
	    else if (this.displayValue.equals("Jack")) shortRank = "J ";
	    else if (this.displayValue.equals("Ten")) shortRank = "10"; // 2 characters wide
	    else {
	        // For numbers Two through Nine, we can just use the numerical value
	        shortRank = this.value + " "; 
	    }

	    // Draw the card using standard text lines
	    return "\n" +
	           " ┌─────────┐\n" +
	           " │ " + shortRank + "      │\n" +
	           " │         │\n" +
	           " │    " + symbol + "    │\n" +
	           " │         │\n" +
	           " │      " + shortRank + " │\n" +
	           " └─────────┘";
	}
}












