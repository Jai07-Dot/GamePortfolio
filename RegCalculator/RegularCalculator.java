import java.util.Scanner;

public class RegularCalculator {
    public static void main(String[] args) {
        // Scanner reads user input from the console
        Scanner scanner = new Scanner(System.in);
        boolean running = true;

        System.out.println("=== Welcome to the Java Console Calculator ===");
        System.out.println("Available operations: +, -, *, /");
        System.out.println("Type 'exit' at any time to quit.\n");

        while (running) {
            // 1. Get the first number or exit command
            System.out.print("Enter first number (or 'exit'): ");
            String input1 = scanner.next();
            
            if (input1.equalsIgnoreCase("exit")) {
                running = false;
                break;
            }

            // 2. Get the mathematical operator
            System.out.print("Enter operator (+, -, *, /): ");
            String operator = scanner.next();

            // 3. Get the second number
            System.out.print("Enter second number: ");
            double num2 = scanner.nextDouble();

            // Convert our first input from text to a number now that we know we aren't exiting
            double num1 = Double.parseDouble(input1);
            double result = 0;
            boolean validOperation = true;

            // 4. Perform calculation using a switch expression
            switch (operator) {
                case "+":
                    result = num1 + num2;
                    break;
                case "-":
                    result = num1 - num2;
                    break;
                case "*":
                    result = num1 * num2;
                    break;
                case "/":
                    if (num2 == 0) {
                        System.out.println("Error: Cannot divide by zero!");
                        validOperation = false;
                    } else {
                        result = num1 / num2;
                    }
                    break;
                default:
                    System.out.println("Error: Invalid operator chosen.");
                    validOperation = false;
                    break;
            }

            // 5. Display the result
            if (validOperation) {
                System.out.println("Result: " + result);
            }
            
            System.out.println("-----------------------------------");
        }

        System.out.println("Calculator closed. Goodbye!");
        scanner.close(); // Clean up the scanner resource
    }
}