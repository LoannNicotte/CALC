import java.util.Arrays;

public class Calculatrice {

    public static String valide_input(String operator, String operand1, String operand2) {

        if (!Arrays.asList("ADD", "SUB", "MUL", "DIV", "POW").contains(operator)) {
            return "Invalid operator: use one of ADD, SUB, MUL, DIV, POW.";
        }

        // Validate operands are integers
        // Parse operands to ensure they are valid integers
        int b;
        try {
            // We don't need the value of 'a' beyond validation; just ensure it parses
            Integer.parseInt(operand1);
            b = Integer.parseInt(operand2);
        } catch (NumberFormatException e) {
            return "Invalid input: Please enter valid integers.";
        }

        // Additional rule: division by zero is not allowed
        if (operator.equals("DIV") && b == 0) {
            return "Division by zero is not allowed.";
        }

        // All validations passed
        return "";
    }

    public static int calculate(String operator, String operand1, String operand2){
        int a = Integer.parseInt(operand1);
        int b = Integer.parseInt(operand2);
        switch (operator) {
            case "ADD":
                return a + b;
            case "SUB":
                return a - b;
            case "MUL":
                return a * b;
            case "DIV":
                return a / b;
            case "POW":
                return (int) Math.pow(a, b);
            default:
                return 0;
        }
    }
}
