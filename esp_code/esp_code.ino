//LCD Display
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Current sensor
int currentSensorPin = 35;
const int avgval = 400;
long int currentReadignValue = 0;
float volts;
float amps = 0;
float ampPreset = 10;

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
const char *password = "1234";


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
#define WIFI_SSID "arjunkichu"
#define WIFI_PASSWORD "arjun@123"
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
    int value = ampPresetFb.to<int>();   
    ampPreset = value;
  }   

  if (passwordFb.success)
  {
    Serial.println("Success data passwordFb");
    Serial.println(passwordFb.type);
    int value = passwordFb.to<int>();   
    String str = String(value);
    // password = str.c_str();
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
  pinMode(relayPin, OUTPUT);
  //LCD
  lcd.init();
  // lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting wifi.");

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
  Serial.println("Enter password");
}

bool isLoggedIn = false;

void loop() {
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
    if (strcmp(enteredPassword, password) == 0)
    {
      Serial.println("Correct password");
      lcd.setCursor(0, 0);
      lcd.print("Password correct");
      digitalWrite(13, HIGH);
      enteredPassword[0] = '\0';
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
      digitalWrite(13, LOW);
      delay(2000);
      memset(enteredPassword, 0, sizeof(enteredPassword));
      lcd.clear();  
      lcd.setCursor(0, 0);
      lcd.print("Enter password: ");
     }
    }
}

void checkCurrentAndOperate(){
  while(isLoggedIn)
  {
    updateData();
    for (int t = 0; t < avgval; t++)
    {
      currentReadignValue += analogRead(currentSensorPin);
      delay(4);
    }
    currentReadignValue = currentReadignValue / avgval;
    volts = currentReadignValue * 5.0 / 4095.0;
    amps = (volts - 2.5) / 0.100;
    if (amps > ampPreset)
    {
      Serial.println("over loaded");
      overLoad();
      delay(2000);
      digitalWrite(relayPin, LOW);
      reset();
    }
    else 
    {
      Serial.print("current value=");
      Serial.print(amps);
      Serial.println("Amps");
      digitalWrite(relayPin, HIGH);
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
}


void updateData(){
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    FirebaseJson json;
    json.set("amp", amps);
    json.set("volt", volts);
    json.set(F("ts/.sv"), F("timestamp"));
    Serial.printf("Set json... %s\n", Firebase.RTDB.set(&fbdo, path.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    Serial.println("");
  }
}
