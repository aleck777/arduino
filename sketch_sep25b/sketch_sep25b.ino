int vl = 0;
int _digits = 4;
const byte _symbols = 20;
boolean _isAnode = false;
const byte digit_order[4] = {13,12,11,10};
const byte pin_order[8] = {2,3,4,5,6,7,8,9};
const byte seven_seg_digits[ _symbols][7] = { 
                           { 0,0,0,0,0,0,1 },  // = 0
                           { 1,0,0,1,1,1,1 },  // = 1
                           { 0,0,1,0,0,1,0 },  // = 2
                           { 0,0,0,0,1,1,0 },  // = 3
                           { 1,0,0,1,1,0,0 },  // = 4
                           { 0,1,0,0,1,0,0 },  // = 5
                           { 0,1,0,0,0,0,0 },  // = 6
                           { 0,0,0,1,1,1,1 },  // = 7    1
                           { 0,0,0,0,0,0,0 },  // = 8   6 2
                           { 0,0,0,1,1,0,0 },  // = 9    7
                           { 0,1,1,0,0,0,0 },  // = E   5 3
                           { 0,0,0,1,0,0,0 },  // = A    4
                           { 1,0,0,1,0,0,0 },  // = H 
                           { 1,0,0,0,0,0,1 },  // = U 
                           { 1,1,0,0,0,0,0 },  // = b 
                           { 0,0,0,1,0,0,1 },  // = П 
                           { 1,1,0,0,0,1,0 },  // = o 
                           { 0,0,1,1,1,0,0 },  // = |o 
                           { 0,1,1,1,0,0,1 },  // = Г 
                           { 0,1,1,0,0,0,1 }   // = C 
                           };


void pickDigit(int x)
{
 // If only one digit (7-Segment 1 Digit) then no need to continue here
  if (_digits == 1)
    return;

  // Turn off ALL digits
  for(int i=0; i< _digits; i++)
  {
    digitalWrite(digit_order[i], _isAnode ? LOW : HIGH);
  }

  // Turn ON only the desired digit
  digitalWrite(digit_order[x-1], _isAnode ? HIGH : LOW);
}


void digitWrite(byte digit, byte number) {
  // If the desired digit does not exist, return
  if (digit > 4)
    return;

  // Activate the digit
  pickDigit(digit);

  // Activate each led in the digit
  int pinseq = 0;
  for (byte segCount = 0; segCount < 7; ++segCount) 
  {
    byte finalValue = seven_seg_digits[number % _symbols][segCount];
    if (!_isAnode)
      finalValue = finalValue == 1 ? 0 : 1; 

    digitalWrite(pin_order[pinseq], finalValue);
    pinseq++;
  }

  // Turn off the DOT
  // TODO: Make this optional
  if (number>=_symbols) {
    digitalWrite(pin_order[pinseq], _isAnode ? LOW : HIGH);
  } else {
    digitalWrite(pin_order[pinseq], _isAnode ? HIGH : LOW);
  }

  delay(5);  // DISPLAY_BRIGHTNESS
}

void valueWrite(int number) {
  if (number < 10)
  {
    digitWrite(4, 0);
    digitWrite(3, 0);
    digitWrite(2, 0);
    digitWrite(1, number);
  }
  else if (number < 100)
  {
    digitWrite(4, 0);
    digitWrite(3, 0);
    digitWrite(2, number / 10);
    digitWrite(1, number % 10);
  }
  else if (number < 1000)
  {
    digitWrite(4, 0);
    digitWrite(3, number / 100);
    digitWrite(2, (number % 100) / 10);
    digitWrite(1, number % 10);
  }
  else
  {
    digitWrite(4, number / 1000);
    digitWrite(3, (number % 1000) / 100);
    digitWrite(2, (number % 100) / 10);
    digitWrite(1, number % 10);
  }
}


void setup() {
  for(int i=0; i<8; i++)
  {
    pinMode(pin_order[i], OUTPUT);   
  }
  for(int i=0; i<_digits; i++)
  {
    pinMode(digit_order[i], OUTPUT);   
  }

}

void loop() {
  for (int i=0 ; i<10000; i++) {
    for (int j = 0 ; j<10; j++) {
      digitWrite(1, i % 10);
      digitWrite(2, (i % 100)/10);
      digitWrite(3, (i % 1000)/100 + _symbols);
      digitWrite(4, _symbols-2);
    }
  }
}
