/*
  IoT-Based Fire and Smoke Detection System

  Hardware:
  - Arduino UNO / UNO R4 WiFi
  - MQ-2 Smoke/Gas Sensor
  - IR Flame Sensor
  - Buzzer
  - Red LED
  - Green LED
  - GSM Module (SIM800L/SIM900)

  Working:
  MQ-2 and flame sensors continuously monitor the environment.
  If smoke/gas or flame is detected:
      1. Buzzer is activated
      2. Red LED turns ON
      3. GSM module sends an SMS alert

  Otherwise:
      Green LED remains ON and monitoring continues.
*/

// ---------------- PIN DEFINITIONS ----------------

const int MQ2_PIN = A0;
const int FLAME_PIN = 2;

const int BUZZER_PIN = 8;
const int RED_LED = 9;
const int GREEN_LED = 10;

// Smoke threshold
// Adjust this value after calibrating your MQ-2 sensor.
const int SMOKE_THRESHOLD = 400;

// Change this to the mobile number that should receive SMS
const char PHONE_NUMBER[] = "+91XXXXXXXXXX";

// Prevent sending SMS continuously
bool alertSent = false;


// ---------------- SETUP ----------------

void setup() {

  Serial.begin(9600);

  // GSM communication
  Serial1.begin(9600);

  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  Serial.println("================================");
  Serial.println("Fire & Smoke Detection System");
  Serial.println("System Started");
  Serial.println("================================");

  delay(2000);

  // Initialize GSM
  Serial1.println("AT");
  delay(1000);

  Serial1.println("AT+CMGF=1");
  delay(1000);

  Serial.println("GSM Initialized");
}


// ---------------- MAIN LOOP ----------------

void loop() {

  // Read sensor values
  int smokeValue = analogRead(MQ2_PIN);
  int flameValue = digitalRead(FLAME_PIN);

  Serial.print("MQ-2 Value: ");
  Serial.print(smokeValue);

  Serial.print(" | Flame Sensor: ");
  Serial.println(flameValue);


  // ------------------------------------------------
  // FIRE / SMOKE DETECTION
  // ------------------------------------------------

  // Many flame sensor modules give LOW when flame
  // is detected. Change LOW to HIGH if your module
  // works in the opposite way.

  bool smokeDetected = smokeValue > SMOKE_THRESHOLD;
  bool flameDetected = (flameValue == LOW);

  if (smokeDetected || flameDetected) {

    // Activate local alarm
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    Serial.println("!!! WARNING !!!");

    if (smokeDetected) {
      Serial.println("Smoke/Gas Detected!");
    }

    if (flameDetected) {
      Serial.println("Flame Detected!");
    }


    // Send SMS only once for this event
    if (!alertSent) {

      sendSMS();

      alertSent = true;
    }

  }

  // ------------------------------------------------
  // SAFE CONDITION
  // ------------------------------------------------

  else {

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    Serial.println("Environment Safe");

    // Allow another SMS if a new emergency occurs
    alertSent = false;
  }


  Serial.println("--------------------------------");

  delay(1000);
}


// ---------------- GSM SMS FUNCTION ----------------

void sendSMS() {

  Serial.println("Sending SMS Alert...");

  // Set SMS text mode
  Serial1.println("AT+CMGF=1");
  delay(1000);

  // Select recipient
  Serial1.print("AT+CMGS=\"");
  Serial1.print(PHONE_NUMBER);
  Serial1.println("\"");

  delay(1000);

  // SMS content
  Serial1.println("FIRE EMERGENCY! Smoke or flame detected. Please check the location immediately.");

  delay(500);

  // CTRL+Z to send SMS
  Serial1.write(26);

  delay(5000);

  Serial.println("SMS Alert Sent.");
}
