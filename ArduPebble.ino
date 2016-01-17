//ArduPebble.ino
//An HTTP webserver meant to accept various requests from a Pebble smartwatch
//This code is designed to run on a Wifi-Enabled Arduino microcontroller
//Chris Green - 1/17/2016

#include <ESP8266WiFi.h> //replace this with the library specific to your board/wifi shield
#include <Servo.h>

#define LEDPIN 2
#define LIGHTPIN 5
#define SERVOPIN 4
#define LOCKED_ANGLE 0
#define UNLOCKED_ANGLE 10

const char* ssid     = "iPhone";
const char* password = "guest123";

WiFiServer server(80); //server on port 80
Servo servo; //physical servo
bool lockStatus = 0; //0 = unlocked, 1 = locked
bool lightStatus = 0;
String request;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to a WiFi Network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  pinMode(LEDPIN, OUTPUT); //active when the servo is "on"
  digitalWrite(LEDPIN, LOW);
  pinMode(LIGHTPIN, OUTPUT);
  digitalWrite(LIGHTPIN, LOW);

  servo.attach(SERVOPIN); //initialize servo on digital pin 0
  servo.write(UNLOCKED_ANGLE); //set an initial angle, this should be the unlocked position
}

void loop() {
  WiFiClient client = server.available();  // try to get client
    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                request += c;
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // emulate a http response for debug use
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>Arduino Door Control</title>");
                    client.println("</head>");
                    client.println("<body>");
                    processRequest(client);
                    client.println("</form>");
                    client.println("</body>");
                    client.println("</html>");
                    Serial.println(lockStatus);
                    Serial.print(request);
                    request = "";    // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

void processRequest(WiFiClient cl){
    Serial.println(request.indexOf("Lock"));
    switch(request[6]){ //the first byte of the request, indicates target device
      case 'L':
        if(request[7] == 'N'){ //Indicates an "On" signal
          lockStatus = 1;
          lock(); //call a routine that "locks the door"
        }
        else{ //Assume an "Off" symbol
          lockStatus = 0;
          unlock();
        }
        break;
      case 'F':
        if(request[7] == 'N'){
          lightOn();
          lightStatus = 1;
        }
        else{
          lightOff();
          lightStatus = 0;
        }
        break;
      default: 
        Serial.print("Default Case Encountered!");
    }
}
//functions defined below called as a result of a specific incoming http command
void lightOn(){
  digitalWrite(LIGHTPIN, HIGH);
  Serial.print("Lights on!\n");
}
void lightOff(){
  digitalWrite(LIGHTPIN, LOW);
  Serial.print("Lights off!\n");
}
void lock(){ //lock routine
  digitalWrite(LEDPIN, LOW);
  Serial.print("Locking\n");
  servo.write(LOCKED_ANGLE);
  //delay(locktime);???
}
void unlock(){ //unlock routine
  digitalWrite(LEDPIN, HIGH);
  Serial.print("Unlocking\n");
  servo.write(UNLOCKED_ANGLE);
  //delay(locktime);???
}
