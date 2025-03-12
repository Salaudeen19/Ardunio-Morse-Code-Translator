#include <LiquidCrystal.h>

/* the duration of a dot is the reference unit of time
 * dash is three times the duration of a dot
 * time between dots and dashes within a character is the duration of a dot
 * time between characters is three times the duration of a dot 
 * time between words must be seven times the duration of a dot 
 */

const char CLEAR = 0;
const char DOT = 1;
const char DASH = 2;
const char alphabet[26][6] = {
  { 'A', DOT, DASH, CLEAR, CLEAR, CLEAR},
  { 'B', DASH, DOT, DOT, DOT, CLEAR},
  { 'C', DASH, DOT, DASH, DOT, CLEAR},
  { 'D', DASH, DOT, DOT, CLEAR, CLEAR},
  { 'E', DOT, CLEAR, CLEAR, CLEAR, CLEAR},
  { 'F', DOT, DOT, DASH, DOT, CLEAR},
  { 'G', DASH, DASH, DOT, CLEAR, CLEAR},
  { 'H', DOT, DOT, DOT, DOT, CLEAR},
  { 'I', DOT, DOT, CLEAR, CLEAR, CLEAR},
  { 'J', DOT, DASH, DASH, DASH, CLEAR},
  { 'K', DASH, DOT, DASH, CLEAR, CLEAR},
  { 'L', DOT, DASH, DOT, DOT, CLEAR},
  { 'M', DASH, DASH, CLEAR, CLEAR, CLEAR},
  { 'N', DASH, DOT, CLEAR, CLEAR, CLEAR},
  { 'O', DASH, DASH, DASH, CLEAR, CLEAR},
  { 'P', DOT, DASH, DASH, DOT, CLEAR},
  { 'Q', DASH, DASH, DOT, DASH, CLEAR},
  { 'R', DOT, DASH, DOT, CLEAR, CLEAR},
  { 'S', DOT, DOT, DOT, CLEAR, CLEAR},
  { 'T', DASH, CLEAR, CLEAR, CLEAR, CLEAR},
  { 'U', DOT, DOT, DASH, CLEAR, CLEAR},
  { 'V', DOT, DOT, DOT, DASH, CLEAR},
  { 'W', DOT, DASH, DASH, CLEAR, CLEAR},
  { 'X', DASH, DOT, DOT, DASH, CLEAR},
  { 'Y', DASH, DOT, DASH, DASH, CLEAR},
  { 'Z', DASH, DASH, DOT, DOT, CLEAR}
};

const unsigned long dotDuration = 200; // duration [ms] of a dot; basic time unit
const unsigned long tolerance = 200; // duration that dash, dot or gap may deviate from the ideal length
const unsigned long dashDuration = 2.2 * dotDuration;
const unsigned long shortGap = dotDuration;
const unsigned long mediumGap = 3 * dotDuration;
const unsigned long longGap = 5 * dotDuration;

// current state of the button
enum State { 
  UP = 0, 
  DOWN = 1 
} state;

// time index of the last button state change
unsigned long lastChange;

// duration of the last button press
unsigned long downDuration;

// dash-dot-sequence of the current character
char character[5];

// index of the next dot/dash in the current character
int characterIndex;

// the next thing to do
enum Action {
  START = 0,
  READ_DASHDOT = 1,
  READ_CHARACTER = 2
} action;

// pin to which the button is connected
int buttonPin = 6;
// pin to which the buzzer is connected
int buzzerPin = 5;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int LCD_COLS = 16;  // Number of columns on the LCD
const int LCD_ROWS = 2;    // Number of rows on the LCD

int currentCol = 0;  // Track the current column
int currentRow = 0;  // Track the current row

// Define the unique space sequence (e.g., "....")
const char spaceSequence[5] = {DOT, DOT, DOT, DOT, DOT}; // Four dots

// Define the delete sequence (e.g., ".....")
const char deleteSequence[5] = {DASH, DOT, DOT, DOT, DOT}; // Five dots for deletion

void setup() {
  Serial.begin(9600);
  
  lcd.begin(LCD_COLS, LCD_ROWS);  // Initialize the LCD
  pinMode(buttonPin, INPUT_PULLUP);

  state = UP;
  characterIndex = 0;
  downDuration = 0;
  lastChange = 0;
  action = START;
}

void printCharacterToLCD(char c) {
  lcd.setCursor(currentCol, currentRow);  // Set cursor to current position
  lcd.print(c);  // Print the character

  // Move to the next column
  currentCol++;
  
  // If we reach the end of the current line, move to the next line
  if (currentCol >= LCD_COLS) {
    currentCol = 0;  // Reset to first column
    currentRow++;    // Move to the next row
    
    // If we reach the last row, wrap back to the top or clear the display
    if (currentRow >= LCD_ROWS) {
      currentRow = 0;    // Wrap back to the first row
      lcd.clear();       // Optionally clear the display
    }
  }
}

// Remove the last character from the LCD
void deleteLastCharacter() {
  if (currentCol == 0 && currentRow == 0) {
    // No characters to delete, LCD is empty
    return;
  }

  // Move back one column (or row if needed)
  if (currentCol == 0) {
    currentRow--;
    currentCol = LCD_COLS - 1;  // Move to last column of previous row
  } else {
    currentCol--;
  }

  lcd.setCursor(currentCol, currentRow);  // Set cursor to position of the last character
  lcd.print(" ");  // Overwrite with a space
  lcd.setCursor(currentCol, currentRow);  // Set cursor back to this position
}

// reset the dash-dot-sequence of the current character
void clearCharacter() {
  characterIndex = 0;
  for (int i = 0; i < 5; ++i) {
    character[i] = CLEAR;
  }
}

// Check if the current character matches the space sequence
bool isSpaceSequence() {
  for (int i = 0; i < 5; ++i) {
    if (character[i] != spaceSequence[i]) {
      return false;
    }
  }
  return true;
}

// Check if the current sequence is the delete sequence
bool isDeleteSequence() {
  for (int i = 0; i < 5; ++i) {
    if (character[i] != deleteSequence[i]) {
      return false;
    }
  }
  return true;
}

// determine the next dash or dot in the dash-dot sequence of the current character
void readDashDot() {
  Serial.print("Down duration was: ");
  Serial.println(downDuration);
  
  if (downDuration >= dashDuration - tolerance && downDuration < dashDuration + tolerance) {
    character[characterIndex] = DASH;
    Serial.println("DASH");
    characterIndex++;
  } else if (downDuration >= dotDuration - tolerance && downDuration < dotDuration + tolerance) {
    character[characterIndex] = DOT;
    Serial.println("DOT");
    characterIndex++;
  } else {
    Serial.println("Down duration unrecognized");
  }
}

// interpret the current dash-dot-sequence as a character
char readCharacter() {
  // Check if the sequence matches the space sequence
  if (isSpaceSequence()) {
    return ' ';  // Return space if sequence matches
  }

  // Check if the sequence matches the delete sequence
  if (isDeleteSequence()) {
    deleteLastCharacter();  // Call delete function
    return 0;  // Return 0 since no character should be printed
  }

  bool found;
  for (int i = 0; i < 26; ++i) {
    found = true;
    for (int j = 0; found && j < 5; ++j) {
      if (character[j] != alphabet[i][j + 1]) {
        found = false;
      }
    }
    if (found) {
      return alphabet[i][0];
    }
  }
  return 0;
}

void loop() {
  State newState = digitalRead(buttonPin) ? UP : DOWN;

  // If the button is pressed, play a tone
  if (newState == DOWN) {
    tone(buzzerPin, 800);
  } else {
    noTone(buzzerPin);
  }

  if (newState == state) {
    if (newState == UP) {
      unsigned long upDuration = (millis() - lastChange);
     
      if (action == READ_DASHDOT && upDuration >= shortGap - tolerance && upDuration < mediumGap) {
        readDashDot();
        action = READ_CHARACTER;
      } 
      else if (action == READ_CHARACTER && upDuration >= mediumGap && upDuration < longGap) {
        char c = readCharacter();
        if (c != 0) {
          Serial.print("Read character: ");
          Serial.println(c);
          printCharacterToLCD(c);  // Use the new function to print the character to the correct line
        } else {
          Serial.println("Unrecognized character");
        }
        clearCharacter();
        action = READ_DASHDOT;
      }
    } else {
      downDuration = (millis() - lastChange);
      if (downDuration > longGap) {
        // Clear LCD when button is pressed for a long duration
        lcd.clear();
        Serial.println("LCD cleared due to long button press");
        currentCol = 0;
        currentRow = 0;
      }
    }
  } else {
    if (state == UP && newState == DOWN) {
      downDuration = 0;
    }
    lastChange = millis();
    state = newState;
    action = READ_DASHDOT;
  }
}
