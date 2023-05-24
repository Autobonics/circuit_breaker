//LCD Display
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Current sensor
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance
double Irms = 0;
double ampPreset = 0;

//Relay
int relayPin = 32;

//Keypad
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = { 
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {13, 12, 14, 27}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 25, 33}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char enteredPassword[5];
String password = "1234";

//WiFi
#define wifiLedPin 2

//Firebase
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
/* 1. Define the WiFi credentials */
#define WIFI_SSID "ankur"
#define WIFI_PASSWORD "ankur@123"
// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino
/* 2. Define the API Key */
#define API_KEY "AIzaSyDa_etOGczVsI0JaHfRFFJfrAF3wuf4ftw"
/* 3. Define the RTDB URL */
#define DATABASE_URL "https://circuit-breaker-2de9d-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "device@autoboincs.com"
#define USER_PASSWORD "12345678"
// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
// Variable to save USER UID
String uid;
//Databse
String path;


unsigned long printDataPrevMillis = 0;

FirebaseData stream;
void streamCallback(StreamData data)
{
  Serial.println("NEW DATA!");

  String p = data.dataPath();

  Serial.println(p);
  printResult(data); // see addons/RTDBHelper.h

  // Serial.println();
  FirebaseJson jVal = data.jsonObject();
  FirebaseJsonData ampPresetFb;
  FirebaseJsonData passwordFb;

  jVal.get(ampPresetFb, "ampPreset");
  jVal.get(passwordFb, "password");

  if (ampPresetFb.success)
  {
    Serial.println("Success data ampPresetFb");
    int value = ampPresetFb.to<float>();   
    ampPreset = value;
    reset();
  }   

  if (passwordFb.success)
  {
    Serial.println("Success data passwordFb");
    Serial.println(passwordFb.type);
    int value = passwordFb.to<int>();   
    password = String(value);
    reset();
  } 
}


void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup() {  
  
  Serial.begin(115200);
  //Current
  emon1.current(35, 8);             // Current: input pin, calibration.

  pinMode(relayPin, OUTPUT);
  //LCD
  lcd.init();
  // lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WIFI.");

  //WIFI
  pinMode(wifiLedPin, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(wifiLedPin, LOW);
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  digitalWrite(wifiLedPin, HIGH);
  Serial.println(WiFi.localIP());
  Serial.println();

  //FIREBASE
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  path = "devices/" + uid + "/reading";

//Stream setup
  if (!Firebase.beginStream(stream, "devices/" + uid + "/data"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);

  lcd.setCursor(0, 0);
  lcd.print("Enter password:");
  Serial.println("Enter password  ");
  
}

bool isLoggedIn = false;

void loop() {
  updateData();
  char key = keypad.getKey();
  if (key != NO_KEY && strlen(enteredPassword) < 4)
  {
    enteredPassword[strlen(enteredPassword)] = key;
    lcd.setCursor(strlen(enteredPassword) - 1, 1);
    lcd.print("*");
    Serial.print(enteredPassword);
  }
  if (strlen(enteredPassword) == 4)
  {
    Serial.println("Password:");
    Serial.println(password);
    if (strcmp(enteredPassword, password.c_str()) == 0)
    {
      Serial.println("Correct password");
      lcd.setCursor(0, 0);
      lcd.print("Password correct");
      // enteredPassword[0] = '\0';
      memset(enteredPassword, 0, sizeof(enteredPassword));
      lcd.setCursor(0, 1);
      lcd.print("            ");
      isLoggedIn = true;
      checkCurrentAndOperate();
     }
       else
     {
      lcd.setCursor(0, 0);
      Serial.println("Password incorrect");
      lcd.print("Password incorrect");
      delay(2000);
      memset(enteredPassword, 0, sizeof(enteredPassword));
      lcd.clear();  
      lcd.setCursor(0, 0);
      lcd.print("Enter password: ");
     }
  }
}

int count = 0;

void checkCurrentAndOperate(){
  while(isLoggedIn)
  {
    Irms = emon1.calcIrms(600);  // Calculate Irms only
    updateData();
    if (count > 20  && Irms > ampPreset)
    {
      digitalWrite(relayPin, LOW);
      Serial.print("over loaded: ");
      Serial.println(Irms);
      overLoad();
      updateData();
      delay(5000);
      reset();
    }
    else 
    {
      Serial.print("Current value=");
      Serial.print(Irms);
      Serial.println("Amps");
      digitalWrite(relayPin, HIGH);
      count ++;
    } 
  }
}

void overLoad(){
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Overload");
}

void reset(){
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Enter password:");
  isLoggedIn = false;
  count = 0;
  Irms = 0;
}


void updateData(){
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    FirebaseJson json;
    json.set("isLoggedIn", isLoggedIn);
    json.set("amp", 0);
    json.set("maxAmp", Irms);
    json.set("volt", 2.51);
    json.set(F("ts/.sv"), F("timestamp"));
    Serial.printf("Set json... %s\n", Firebase.RTDB.set(&fbdo, path.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    Serial.println("");
  }
}
