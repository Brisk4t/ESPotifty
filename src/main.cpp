#include "secrets.h" // all constants are defined as const char* in this file
#include "NeoPixelImage.h"

#include "Arduino.h"
#include "WiFi.h"
#include "SpotifyEsp32.h"
#include "TJpg_decoder.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

Spotify spotify(SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, 8080);

//AsyncWebServer server(80);
NeoPixelImage npimage(1, 1);


// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE HTML><html>
// <head>
//   <meta name="viewport" content="width=device-width, initial-scale=1">
// </head>
// <body>
//   <h2>ESP Image Web Server</h2>
//   <img src="image">
// </body>  
// </html>)rawliteral";


void blinkOnce(int pin, int delayTime = 1000) {
  digitalWrite(pin, HIGH);
  delay(delayTime);
  digitalWrite(pin, LOW);
  delay(delayTime);
}

void connectToWifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    blinkOnce(LED_BUILTIN, 1000);
    delay(100);
  }
  
  Serial.print("\nConnected to the WiFi network ");
  Serial.print(WiFi.SSID());
  Serial.println("\nLocal ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void waitForSpotifyAuth() {

  while (!spotify.is_auth()) {
    spotify.handle_client();
    Serial.print(".");
    blinkOnce(LED_BUILTIN, 100);
    delay(100);
  }
  Serial.println("\nSpotify authentication successful!");
}

bool beginSPIFFS(){
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return 0;
  }
  Serial.println("SPIFFS mounted successfully");
  return 1;
}

bool saveToSPIFFS(const String& filename, const uint8_t* data, size_t length) {
  File file = SPIFFS.open(filename, "w");
  if (!file || !data || length == 0) {
    Serial.println("Failed to open file for writing or invalid data");
    Serial.println(filename);
    //Serial.println(data);
    Serial.println(length);
    return false;
  }

  size_t written = file.write(data, length);
  file.close();

  if (written != length) {
    Serial.println("Incomplete write to file");
    return false;
  }

  Serial.println("Data saved to " + filename);
  return true;
}

void setup(){
  Serial.begin(115200);
  
  // Initialize SPIFFS
  Serial.println("Mounting SPIFFS...");
  if(!beginSPIFFS()){
    Serial.println("Failed to mount SPIFFS, exiting setup...");
    return; // Exit setup if SPIFFS fails to mount
  } 
  Serial.println("SPIFFS mounted successfully");

  // Connect to Wi-Fi
  Serial.println("Initializing WiFi...");
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

  npimage.begin(); // Initialize the NeoPixelImage class

  connectToWifi(); // Connect to Wi-Fi network with SSID and password
  
  spotify.begin(); // Initialize Spotify API on ESP32 webserver
  waitForSpotifyAuth();



  //sendImageSerial("/current_cover.jpg"); // Send the image data to Serial for debugging

  // host webserver with album image (debug)

  // Route for root / web page
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(200, "text/html", index_html);
  // });
  
  // server.on("/image", HTTP_GET, [](AsyncWebServerRequest *request){
  //   File file = SPIFFS.open("/current_cover.jpg", "r");
  //   if (!file || file.isDirectory()) {
  //     request->send(404, "text/plain", "Image not found");
  //     return;
  //   }
  
  //   AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/current_cover.jpg", "image/jpeg");
  //   response->addHeader("Cache-Control", "no-cache");
  //   request->send(response);
  //   });

  // server.begin();



  digitalWrite(LED_BUILTIN, HIGH); // Solid led for setup complete

}

void loop()
{
  String track_name = spotify.current_track_name();
  String cover_url = spotify.get_current_album_image_url(0); // Get image 0 from the track
  uint8_t* image_data = nullptr;

  Serial.println("Current track: " + track_name);
  Serial.println("Cover URL: " + cover_url);

  spotify.save_current_album_image(cover_url, "/current_cover.jpg");

  Serial.println("Drawing image from SPIFFS...");
  npimage.drawImage("/current_cover.jpg"); // Draw the image on the LED matrix

  delay(10000);
}
