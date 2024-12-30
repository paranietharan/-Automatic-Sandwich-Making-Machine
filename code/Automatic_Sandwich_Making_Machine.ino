// for i2c display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// for keypad
#include <Keypad.h>

// servo motor
#include <Servo.h>


// Specify the pin connected to the relay
#define RELAY_PIN 53

// Specify the pin connected to the IR sensor
#define IR_SENSOR_PIN 52

#define SERVO_MOTOR_PIN 22

// for 4 ultrasonic sensors
#define breadTrigPin 28
#define breadEchoPin 30

#define carrotTrigPin 32
#define carrotEchoPin 34

#define leaksTrigPin 36
#define leaksEchoPin 38

#define eggsTrigPin 40
#define eggsEchoPin 42
/************************* Servo Motor ***************************************/
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 90;  // variable to store the servo position

/************************* Keypad ********************************************/
// Set the LCD address and dimensions (change these to match your setup)
const int lcdColumns = 16;
const int lcdRows = 2;
const int lcdAddress = 0x27;

LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows);

// Define the keypad layout
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };  // Connect to the row pinouts of the keypad
byte colPins[COLS] = { 5, 4, 3, 2 };  // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define the toppings
const int numToppings = 4;
const char* toppings[numToppings] = {
  "1. Leaks",
  "2. Scrambled Egg",
  "3. Tomato Sauce",
  "4. Carrot"
};
/*************************************************************************************/

/************************ Stepper Motor *********************************************/

// Define the pins for motor control
const int carrotMotorPin1 = 23;
const int carrotMotorPin2 = 25;
const int carrotMotorPin3 = 27;
const int carrotMotorPin4 = 29;

const int leaksMotorPin1 = 31;
const int leaksMotorPin2 = 33;
const int leaksMotorPin3 = 35;
const int leaksMotorPin4 = 37;

const int eggsMotorPin1 = 39;
const int eggsMotorPin2 = 41;
const int eggsMotorPin3 = 43;
const int eggsMotorPin4 = 45;

// Define the steps per revolution for the stepper motors
const int stepsPerRevolution = 2048;

// Define the sequence of steps for one revolution (can be customized)
const int stepSequence[][4] = {
  { 1, 0, 0, 1 },
  { 1, 0, 0, 0 },
  { 1, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 1, 0 },
  { 0, 0, 1, 1 },
  { 0, 0, 0, 1 }
};

// Variable to keep track of the current step
int currentCarrotStep = 0;
int currentLeaksStep = 0;
int currentEggsStep = 0;

// Delay between steps (in microseconds)
int stepDelay = 1000;  // Adjust this value to control the speed

/***************************************************************************************/

void setup() {
  // Servo Motor
  myservo.attach(SERVO_MOTOR_PIN);  // attaches the servo on pin 9 to the servo object
  myservo.write(pos);

  // ultrasonic sensor
  pinMode(breadTrigPin, OUTPUT);
  pinMode(breadEchoPin, INPUT);
  pinMode(carrotTrigPin, OUTPUT);
  pinMode(carrotEchoPin, INPUT);
  pinMode(leaksTrigPin, OUTPUT);
  pinMode(leaksEchoPin, INPUT);
  pinMode(eggsTrigPin, OUTPUT);
  pinMode(eggsEchoPin, INPUT);

  // relay - solenoid valve
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // off relay

  // IR sensor
  pinMode(IR_SENSOR_PIN, INPUT);  // Set the IR sensor pin as an input

  // Initialize the I2C bus
  Wire.begin();

  // Initialize the LCD
  lcd.begin(lcdColumns, lcdRows);
  lcd.setBacklight(HIGH);  // Set backlight to a higher level

  /************************* For Stepper Motor ***************************************/
  // Set the motor control pins as outputs
  pinMode(carrotMotorPin1, OUTPUT);
  pinMode(carrotMotorPin2, OUTPUT);
  pinMode(carrotMotorPin3, OUTPUT);
  pinMode(carrotMotorPin4, OUTPUT);

  pinMode(leaksMotorPin1, OUTPUT);
  pinMode(leaksMotorPin2, OUTPUT);
  pinMode(leaksMotorPin3, OUTPUT);
  pinMode(leaksMotorPin4, OUTPUT);

  pinMode(eggsMotorPin1, OUTPUT);
  pinMode(eggsMotorPin2, OUTPUT);
  pinMode(eggsMotorPin3, OUTPUT);
  pinMode(eggsMotorPin4, OUTPUT);
}

void loop() {
  // Give time start machine
  lcd.print("Initalizing");// Print in first row
  lcd.setCursor(0, 1);  // Move cursor to the second row
  lcd.print("Machine.....");// Print in Second Row
  delay(1000);

  lcd.clear();
  lcd.print("Checking process");// Print in first row
  lcd.setCursor(0, 1);  // Move cursor to the second row
  lcd.print("Started.....");// Print in Second Row
  delay(700);

  int numberOfSelectedToppings = 0;                   //to store how many topping slelected while begining
  char selectedToppings[4] = { '0', '0', '0', '0' };  // to store the selected toppings

  //check all chambers' status
  while (true) {
    if (checkCarrotStatus() == 1 && checkBreadStatus() == 1 && checkLeaksStatus() == 1 && checkEggsStatus() == 1) {
      lcd.clear();
      break;
    }

    lcd.clear();
    delay(1000);

    if (checkCarrotStatus() == 0) {
      lcd.clear();
      lcd.print("Carrot empty!");
    }
    delay(1000);
    if (checkEggsStatus() == 0) {
      lcd.clear();
      lcd.print("Egg empty!");
    }
    delay(1000);
    if (checkBreadStatus() == 0) {
      lcd.clear();
      lcd.print("Bread empty!");
    }
    delay(1000);
    if (checkLeaksStatus() == 0) {
      lcd.clear();
      lcd.print("Leaks empty!");
    }
    delay(1000);
    if (checkCarrotStatus() == 1 && checkBreadStatus() == 1 && checkLeaksStatus() == 1 && checkEggsStatus() == 1) {
      lcd.clear();
      break;
    }

    delay(1000);  // wait for 1 seconds
  }

  // if all toppings are avilable print a success message
  if (checkCarrotStatus() == 0 && checkBreadStatus() == 0 && checkLeaksStatus() == 0 && checkEggsStatus() == 0) {
    lcd.clear();
    lcd.print("Machine ready!");
    delay(2000);
  }

// initalizing beginning Point of Getting toppings point
beginningPoint:

  lcd.clear();
  // Print the welcome message on the LCD
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);  // Move cursor to the second row
  lcd.print("Sandwich Maker");

  delay(2000);  // Wait for 2 seconds

  lcd.clear();  // Clear the LCD display

  // Show available toppings
  lcd.print("Available");
  lcd.setCursor(0, 1);  // Move cursor to the second row
  lcd.print("Toppings:");
  delay(1000);  // Delay to ensure proper display

  for (int i = 0; i < numToppings; i++) {
    lcd.clear();  // Clear the LCD display
    lcd.print(toppings[i]);
    delay(2000);  // Delay to display each topping for 2 seconds
  }

  lcd.clear();  // Clear the LCD display

  // put your main code here, to run repeatedly:
  lcd.print("Select Toppings:");
  char key;
  while (true) {
startPoint:                 // start point
    key = keypad.getKey();  // get keyboard input

    if (key == 'A' && numberOfSelectedToppings == 3) {
      break;
    }
    //check input
    if (isValidToppingChoice(key)) {
      lcd.clear();
      lcd.print(toppings[key - '1']);

      //check if already selected topping is selected
      for (int i = 0; i < 4; i++) {
        if (key == selectedToppings[i]) {
          lcd.clear();
          lcd.print("Invalid Selection!");
          delay(2000);
          lcd.clear();
          goto startPoint;  // if invalid selection it goes to start point
        }
      }

      // add selected toppings
      selectedToppings[numberOfSelectedToppings] = key;
      numberOfSelectedToppings++;  // increment selected toppings count
    }
    if (numberOfSelectedToppings == 4) {
      lcd.clear();
      lcd.print("Confirm");
      lcd.setCursor(0, 1);
      lcd.print("Press A");
      delay(500);

      char confirmKey = keypad.getKey();
      if (confirmKey == 'A') {
        break;
      } else if (confirmKey == 'B') {

        // Set all topping to '0'
        for(int i=0; i<4; i++)
        {
          selectedToppings[i] = '0';
        }

        // Set number of selected toppings to zero
        int numberOfSelectedToppings = 0;

        goto beginningPoint;
      }
      //delay(1000);
    }
  }

  lcd.clear();
  lcd.print("Thank you!");
  delay(10000);

  lcd.clear();

  // adding first bread slice
  lcd.print("Adding bread...!");
  AddBreadSlice();
  lcd.clear();

  for (int i = 0; i < 4; i++) {
    //lcd.print(selectedToppings[i]);
    if (selectedToppings[i] == '1') {
      lcd.print("Adding Leaks..");
      rotateLeaksStepperMotor(stepsPerRevolution / 3, false);
      delay(250);
      rotateLeaksStepperMotor(stepsPerRevolution / 3, true);
      delay(1000);
      lcd.clear();
    } else if (selectedToppings[i] == '2') {
      lcd.print("Adding Scrambled Egg...");
      rotateScrambledEggsStepperMotor(stepsPerRevolution / 3, false);
      delay(250);
      rotateScrambledEggsStepperMotor(stepsPerRevolution / 3, true);
      delay(1000);
      lcd.clear();
    } else if (selectedToppings[i] == '3') {
      lcd.print("Adding sauce...");
      putSauce();
      delay(1000);
      lcd.clear();
    } else if (selectedToppings[i] == '4') {
      lcd.print("Adding carrots...");
      rotateCarrotStepperMotor(stepsPerRevolution / 3, false);
      delay(250);
      rotateCarrotStepperMotor(stepsPerRevolution / 3, true);
      delay(1000);
      lcd.clear();
    }
  }

  // adding second bread slice
  lcd.print("Adding bread...!");
  AddBreadSlice();
  lcd.clear();
  delay(1000);

  // print a success msg after work done
  lcd.print("Sandwich ready!");
  delay(1000);
  lcd.clear();

  while (true) {
    // check with IR sensor
    if (detectSandwich() == 1) {
      lcd.clear();
      lcd.print("Take your");
      lcd.setCursor(0, 1);  // Move cursor to the second row
      lcd.print("sandwich");
      delay(1000);
    } else {
      break;
    }
  }
  lcd.clear();
}

// Function to check if the user's topping choice is valid
bool isValidToppingChoice(char choice) {
  return (choice >= '1' && choice <= '4');
}

/*********************** Ultra sonic sensor ******************************************/
//checkBreadStatus()checkLeaksStatus()checkEggsStatus()
int checkBreadStatus() {
  float duration, distance;
  // Write a pulse to the bread sensor's trigger pin

  digitalWrite(breadTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(breadTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(breadTrigPin, LOW);

  // Measure the response from the bread sensor's echo pin

  duration = pulseIn(breadEchoPin, HIGH);

  // Determine distance from duration
  // Use 343 meters per second as the speed of sound

  distance = (duration / 2) * 0.0343;

  if (distance >= 9) {
    return 0;
  } else {
    return 1;
  }
}

int checkCarrotStatus() {
  float duration, distance;
  // Write a pulse to the carrot sensor's trigger pin

  digitalWrite(carrotTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(carrotTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(carrotTrigPin, LOW);

  // Measure the response from the carrot sensor's echo pin

  duration = pulseIn(carrotEchoPin, HIGH);

  // Determine distance from duration
  // Use 343 meters per second as the speed of sound
  distance = (duration / 2) * 0.0343;

  if (distance >= 9) {
    return 0;
  } else {
    return 1;
  }
}

int checkLeaksStatus() {
  float duration, distance;
  // Write a pulse to the carrot sensor's trigger pin

  digitalWrite(leaksTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(leaksTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(leaksTrigPin, LOW);

  // Measure the response from the carrot sensor's echo pin

  duration = pulseIn(leaksEchoPin, HIGH);

  // Determine distance from duration
  // Use 343 meters per second as the speed of sound
  distance = (duration / 2) * 0.0343;

  if (distance >= 9) {
    return 0;
  } else {
    return 1;
  }
}

int checkEggsStatus() {
  float duration, distance;
  // Write a pulse to the carrot sensor's trigger pin

  digitalWrite(eggsTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(eggsTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(eggsTrigPin, LOW);

  // Measure the response from the carrot sensor's echo pin

  duration = pulseIn(eggsEchoPin, HIGH);

  // Determine distance from duration
  // Use 343 meters per second as the speed of sound
  distance = (duration / 2) * 0.0343;

  if (distance >= 9) {
    return 0;
  } else {
    return 1;
  }
}


/*********************************** Stepper Motor *************************************/
// Function to rotate the Carrot stepper motor
void rotateCarrotStepperMotor(int steps, bool clockwise) {
  // Determine the direction of rotation
  int direction = clockwise ? 1 : -1;

  // Perform the specified number of steps
  for (int i = 0; i < steps; i++) {
    // Update the current step based on the direction
    currentCarrotStep += direction;

    // Wrap around the current step if it exceeds the step sequence length
    if (currentCarrotStep >= 8) {
      currentCarrotStep = 0;
    } else if (currentCarrotStep < 0) {
      currentCarrotStep = 7;
    }

    // Set the motor control pins according to the step sequence
    digitalWrite(carrotMotorPin1, stepSequence[currentCarrotStep][0]);

    digitalWrite(carrotMotorPin2, stepSequence[currentCarrotStep][1]);
    digitalWrite(carrotMotorPin3, stepSequence[currentCarrotStep][2]);
    digitalWrite(carrotMotorPin4, stepSequence[currentCarrotStep][3]);

    // Delay between steps
    delayMicroseconds(stepDelay);
  }
}

// Function to rotate the Leaks stepper motor
void rotateLeaksStepperMotor(int steps, bool clockwise) {
  // Determine the direction of rotation
  int direction = clockwise ? 1 : -1;

  // Perform the specified number of steps
  for (int i = 0; i < steps; i++) {
    // Update the current step based on the direction
    currentLeaksStep += direction;

    // Wrap around the current step if it exceeds the step sequence length
    if (currentLeaksStep >= 8) {
      currentLeaksStep = 0;
    } else if (currentLeaksStep < 0) {
      currentLeaksStep = 7;
    }

    // Set the motor control pins according to the step sequence
    digitalWrite(leaksMotorPin1, stepSequence[currentLeaksStep][0]);
    digitalWrite(leaksMotorPin2, stepSequence[currentLeaksStep][1]);
    digitalWrite(leaksMotorPin3, stepSequence[currentLeaksStep][2]);
    digitalWrite(leaksMotorPin4, stepSequence[currentLeaksStep][3]);

    // Delay between steps
    delayMicroseconds(stepDelay);
  }
}

// Function to rotate the Scrambled Eggs stepper motor
void rotateScrambledEggsStepperMotor(int steps, bool clockwise) {
  // Determine the direction of rotation
  int direction = clockwise ? 1 : -1;

  // Perform the specified number of steps
  for (int i = 0; i < steps; i++) {
    // Update the current step based on the direction
    currentEggsStep += direction;

    // Wrap around the current step if it exceeds the step sequence length
    if (currentEggsStep >= 8) {
      currentEggsStep = 0;
    } else if (currentEggsStep < 0) {
      currentEggsStep = 7;
    }

    // Set the motor control pins according to the step sequence
    digitalWrite(eggsMotorPin1, stepSequence[currentEggsStep][0]);
    digitalWrite(eggsMotorPin2, stepSequence[currentEggsStep][1]);
    digitalWrite(eggsMotorPin3, stepSequence[currentEggsStep][2]);
    digitalWrite(eggsMotorPin4, stepSequence[currentEggsStep][3]);

    // Delay between steps
    delayMicroseconds(stepDelay);
  }
}

/************************************ Servo Motor ******************************************/
void AddBreadSlice() {
  for (pos = 0; pos <= 90; pos += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);  // tell servo to go to position in variable 'pos'
    delay(5);            // waits 5ms for the servo to reach the position
  }
  delay(1000);
  for (pos = 90; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
    myservo.write(pos);                 // tell servo to go to position in variable 'pos'
    delay(5);                           // waits 5ms for the servo to reach the position
  }
  delay(1000);
}

/************************************ Relay - solenoid valve *******************************/
void putSauce() {
  // Turn on the relay
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);  // Wait for 1 second

  // Turn off the relay
  digitalWrite(RELAY_PIN, LOW);
}

/************************************* IR sensor ******************************************/

int detectSandwich() {
  int sensorValue = digitalRead(IR_SENSOR_PIN);  // Read the value from the IR sensor
  if (sensorValue == LOW) {
    return 1;
  } else {
    return 0;
  }
}