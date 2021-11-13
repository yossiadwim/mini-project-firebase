
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include <NewPing.h>
#include <ESP32Servo.h>
#include <analogWrite.h>
#include <ESP32Tone.h>
#include <ESP32PWM.h>



//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "realme6pro"
#define WIFI_PASSWORD "1234abcd"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyBaS2GPljGKQ8Rsz_4MRAhCt2SDdq6Xd64"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://cobaiot-30fb6-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "yssdmhrdka@gmail.com"
#define USER_PASSWORD "yossiaDWI0208"

//Define Firebase Data object
FirebaseData fbData;

String path = "/esp32";
int jarak_lama;
int jarak_baru;


FirebaseAuth auth;
FirebaseConfig config;


#define TRIGGER_PIN  5  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     18
static const int servoPin = 13;
Servo servo;
NewPing sonar(TRIGGER_PIN, ECHO_PIN);

void setup()
{

  Serial.begin(115200);
  servo.attach(servoPin);

  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = "RLHxdGTuooEQskWKZjucc3GGjCPhwQBxDid4Dj9s";

  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  

  /** Timeout options, below is default config.

  //WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
  config.timeout.wifiReconnect = 10 * 1000;

  //Socket begin connection timeout (ESP32) or data transfer timeout (ESP8266) in ms (1 sec - 1 min).
  config.timeout.socketConnection = 30 * 1000;

  //ESP32 SSL handshake in ms (1 sec - 2 min). This option doesn't allow in ESP8266 core library.
  config.timeout.sslHandshake = 2 * 60 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  */
}

void loop()
{

  delay(1000);

  jarak_baru = sonar.ping_cm();
  Serial.println(jarak_baru);
  
  if(jarak_baru != jarak_lama){
      Firebase.setDouble(fbData,path + "/jarak",jarak_baru);
      jarak_lama = jarak_baru;
    }

  if(jarak_baru <= 100){
    Firebase.setString(fbData,path + "/status","objek mendekat");
    for(int degree = 0; degree <=180; degree++){
      servo.write(degree);
      delay(20);
     }
      servo.write(0);
  }
  else if(jarak_baru > 100){
    Firebase.setString(fbData,path + "/status","objek menjauh");
  }

}

/// PLEASE AVOID THIS ////

//Please avoid the following inappropriate and inefficient use cases
/**
 * 
 * 1. Call get repeatedly inside the loop without the appropriate timing for execution provided e.g. millis() or conditional checking,
 * where delay should be avoided.
 * 
 * Everytime get was called, the request header need to be sent to server which its size depends on the authentication method used, 
 * and costs your data usage.
 * 
 * Please use stream function instead for this use case.
 * 
 * 2. Using the single FirebaseData object to call different type functions as above example without the appropriate 
 * timing for execution provided in the loop i.e., repeatedly switching call between get and set functions.
 * 
 * In addition to costs the data usage, the delay will be involved as the session needs to be closed and opened too often
 * due to the HTTP method (GET, PUT, POST, PATCH and DELETE) was changed in the incoming request. 
 * 
 * 
 * Please reduce the use of swithing calls by store the multiple values to the JSON object and store it once on the database.
 * 
 * Or calling continuously "set" or "setAsync" functions without "get" called in between, and calling get continuously without set 
 * called in between.
 * 
 * If you needed to call arbitrary "get" and "set" based on condition or event, use another FirebaseData object to avoid the session 
 * closing and reopening.
 * 
 * 3. Use of delay or hidden delay or blocking operation to wait for hardware ready in the third party sensor libraries, together with stream functions e.g. Firebase.RTDB.readStream and fbdo.streamAvailable in the loop.
 * 
 * Please use non-blocking mode of sensor libraries (if available) or use millis instead of delay in your code.
 * 
 * 4. Blocking the token generation process.
 * 
 * Let the authentication token generation to run without blocking, the following code MUST BE AVOIDED.
 * 
 * while (!Firebase.ready()) <---- Don't do this in while loop
 * {
 *     delay(1000);
 * }
 * 
 */
