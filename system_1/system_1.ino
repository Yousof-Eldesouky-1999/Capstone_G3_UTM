
// Include required libraries
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>

// Create instances
MFRC522 mfrc522(10, 9); // MFRC522 mfrc522(SS_PIN, RST_PIN)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo sg90;

// Initialize Pins for led's, servo and buzzer
// Blue LED is connected to 5V
constexpr uint8_t greenLed = 7;
constexpr uint8_t redLed = 6;
constexpr uint8_t servoPin = 8;
constexpr uint8_t buzzerPin = 5;

char initial_password[4] = {'1', '2', '3', '4'};  // Variable to store initial password
String tagUID = "0A 6C A4 7F";  // String to store UID of tag. Change it with your tag's UID
char password[4];   // Variable to store users password
boolean RFIDMode = false; // boolean to change modes
boolean NormalMode = true; // boolean to change modes
char key_pressed = 0; // Variable to store incoming keys
uint8_t i = 0;  // Variable used for counter

// defining how many rows and columns our keypad have
const byte rows = 4;
const byte columns = 4;

// Keypad pin map
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Initializing pins for keypad
byte row_pins[rows] = {A0, A1, A2, A3};
byte column_pins[columns] = {2, 1, 0};

// Create instance for keypad
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);
int outPin = 4;                // output to start camera
void setup() {
  // Arduino Pin configuration
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(outPin, OUTPUT); 

  sg90.attach(servoPin);  //Declare pin 8 for servo
  sg90.write(0); // Set initial position at 0 degrees

  lcd.begin();   // LCD screen
  lcd.backlight();
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522


  lcd.clear(); // Clear LCD screen
  
}

void loop() {
  

  if (NormalMode == true) {
    // System will first look for mode
    if (RFIDMode == false) {
      key_pressed = keypad_key.getKey(); // Storing keys
      if (key_pressed)
      {
        password[i++] = key_pressed; // Storing in password variable
        lcd.print("*");
      }
      if (i == 4) // If 4 keys are completed
      {
        delay(200);
        if (!(strncmp(password, initial_password, 4))) // If password is matched
        {
          lcd.clear();
          lcd.print("System Initialized");
          digitalWrite(greenLed, HIGH);
          delay(3000);
          digitalWrite(greenLed, LOW);
          sg90.write(0);// Door is unlocked
          lcd.clear();
          i = 0;
          RFIDMode = true; // Make RFID mode true
        }
        else    // If password is not matched
        {
          lcd.clear();
          lcd.print("Wrong Password");
          digitalWrite(buzzerPin, HIGH);
          digitalWrite(redLed, HIGH);
          delay(3000);
          digitalWrite(buzzerPin, LOW);
          digitalWrite(redLed, LOW);
          lcd.clear();
          i = 0;
          RFIDMode = false;  
        }
      }
    }
    if (RFIDMode == true) {
    
    while (RFIDMode == true){
      lcd.setCursor(0, 0);
      lcd.print("   System is scanning");
      lcd.setCursor(0, 1);
      lcd.print(" Scaning Your Tag ");

      // Look for new cards
      if ( ! mfrc522.PICC_IsNewCardPresent()) {
        
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }

      //Reading from the card
      String tag = "";
      for (byte j = 0; j < mfrc522.uid.size; j++)
      {
        tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
        tag.concat(String(mfrc522.uid.uidByte[j], HEX));
      }
      tag.toUpperCase();

      //Checking the card
      if (tag.substring(1) == tagUID)
      {
        // If UID of tag is matched.
        lcd.clear();
        lcd.print("Tag Matched");
        digitalWrite(greenLed, HIGH);
        delay(3000);
        digitalWrite(greenLed, LOW);

        lcd.clear();
        lcd.print("Asset in place");
        lcd.setCursor(0, 1);
     
      }
        
        
      }


      else
      {
        // If UID of tag is not found.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Tag is not found");
        lcd.setCursor(0, 1);
        lcd.print("Asset stolen");
        sg90.write(180); // Door is locked
        RFIDMode = false;
        NormalMode = false;
        while(NormalMode == false){
        digitalWrite(buzzerPin, HIGH);
        digitalWrite(redLed, HIGH);
        delay(3000);
        digitalWrite(buzzerPin, LOW);
        digitalWrite(redLed, LOW);
        lcd.print("Asset stolen");
        lcd.clear();
        digitalWrite(outPin, HIGH);
        if (RFIDMode == false) {
      key_pressed = keypad_key.getKey(); // Storing keys
      if (key_pressed)
      {
        password[i++] = key_pressed; // Storing in password variable
        lcd.print("*");
      }
      if (i == 4) // If 4 keys are completed
      {
        delay(200);
        if (!(strncmp(password, initial_password, 4))) // If password is matched
        {
          lcd.clear();
          lcd.print("System shutdown");
          digitalWrite(greenLed, HIGH);
          delay(3000);
          digitalWrite(greenLed, LOW);
          sg90.write(-180);// Door is unlocked
          lcd.clear();
          i = 0;
          NormalMode = true;
          digitalWrite(outPin, LOW);
        }
         
      }
      
      }
    
    }
   }
  }   
 }   
}
}  
