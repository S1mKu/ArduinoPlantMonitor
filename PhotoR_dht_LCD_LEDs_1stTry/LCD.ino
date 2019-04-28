#include <LiquidCrystal.h>

//check the pins
LiquidCrystal lcd(0, 1, 2, 3, 4, 5);

void lcdBegin() {
  lcd.begin(16, 2);
}

// set position of the cursor. possible arguments (0,0), (0,1)
void LCDsetCursor(int digit, int line) {
  lcd.setCursor(digit, line);  
}

void LCDprintString(String str) {
  lcd.print(str);
}

void LCDprintInt (int i) {
  lcd.print(i);
}

void LCDprintChar(char ch){
  lcd.print((char) ch);
}

void LCDclear(){
  lcd.clear();
}

//lcd.print((char)223);
