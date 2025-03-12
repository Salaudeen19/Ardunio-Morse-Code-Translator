# Arduino Morse Code Translator

## Overview
The **Arduino Morse Code Translator** is an embedded system that converts Morse code input from a button into English text displayed on an LED screen. This project is designed to demonstrate basic embedded systems, signal processing, and hardware-software integration using Arduino.

## Features
- Converts real-time Morse code input (dot/dash) into English characters.
- Uses a push button for Morse code input.
- Displays translated text on an LED screen.
- Written in C++ using the Arduino framework.

## Components Used
- **Arduino Board** (Uno, Mega, or compatible)
- **Push Button** (for Morse code input)
- **LED Display** (16x2 LCD or OLED for output)
- **Resistors and Wires** (for circuit connections)
- **Breadboard** (optional for prototyping)

## Circuit Diagram
*(Attach a circuit diagram image here or provide a Fritzing schematic.)*

## How It Works
1. **Input**: Pressing the button for a short duration represents a dot (`.`), and a long press represents a dash (`-`).
2. **Processing**: The Arduino measures the press duration, converts it into Morse code, and maps it to an English character.
3. **Output**: The translated English letter is displayed on the LED screen.

## Installation and Usage
### Hardware Setup
1. Connect the push button to the Arduino with a pull-down resistor.
2. Connect the LED screen to the Arduino following the circuit diagram.
3. Upload the Arduino code to the board.

### Software Setup
1. Install the **Arduino IDE** (if not already installed).
2. Clone this repository:
   ```sh
   git clone https://github.com/your-username/Arduino-Morse-Code-Translator.git
   ```
3. Open `morse_code_translator.ino` in the Arduino IDE.
4. Select the correct board and COM port.
5. Upload the code to your Arduino.

## Sample Morse Code Translations
| Morse Code | Letter |
|------------|--------|
| .-         | A      |
| -...       | B      |
| -.-.       | C      |
| ...        | S      |
| ---        | O      |

## Future Improvements
- Adding audio feedback (buzzer) for real-time Morse code sound.
- Implementing a serial monitor version for computer display.
- Enhancing accuracy with debouncing techniques.

## Contributing
Feel free to fork this repository and submit pull requests to enhance the project!

## License
This project is open-source and licensed under the MIT License.

## Acknowledgments
- Inspired by classic Morse code communication methods.
- Thanks to the Arduino community for helpful resources!

