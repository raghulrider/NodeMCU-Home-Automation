
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

String fan1state = "off";
String fan2state = "off";
String fan3state = "off";
String fan4state = "off";
String fan5state = "off";
String fan6state = "off";

const char* ssid = "HWjunction"; // your connection name
const char* password = "forged@forge"; // your connection password

ESP8266WebServer server(80);

int Fan1 = D2;
int Fan2 = D1;
int Fan3 = D6;
int Fan4 = D7;
int Fan5 = D5;
int Fan6 = D8;


//Check if header is present and correct
bool is_authentified() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "Titans" &&  server.arg("PASSWORD") == "veralevelprotosem" )
    {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;

    }

    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  String content = "<html><body style='background-color:darkgrey'><img align:center; src='data:image/png;base64,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' alt=''><form action='/login' method='POST'><p  align ='center' style='font-size:300%;color:red'><b>  Titans's Gate  </b></p><br>";
  content += " <p   align ='center' style='font-size:160%'><b> UserName: <input type='text' name='USERNAME' placeholder='              user name' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><b>Password: <input type='password' name='PASSWORD' placeholder='              password' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%,color:red'><input type='submit' name='SUBMIT' value='Submit'></form><strong>" + msg + "</strong></p><br></body></html>";
  content += " Contact : <a href='https://www.linkedin.com/in/raghul-prasad-v'>LinkedIn</a></body></html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String content = "<head><meta http-equiv='refresh' content='13'><link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<link rel=\"icon\" href=\"data:,\">";
  content += "<style> html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  content += ".fa {padding: 5px;font-size: 20px;width: 20px;text-align: center;text-decoration: none;}";
  content += ".fa-linkedin {background: #007bb5;color: white;}";
  content += ".fa-github{background: white;color: black}";
  content += ".button {display: inline-block;padding: 15px 25px;font-size: 20px;cursor: pointer;text-align: center;text-decoration: none;outline: none;color: #fff;background-color: #4CAF50;border: none;border-radius: 15px;box-shadow: 0 6px #999;}";
  content += ".button2 {display: inline-block;padding: 15px 25px;font-size: 20px;cursor: pointer;text-align: center;text-decoration: none;outline: none;color: #fff;background-color: red;border: none;border-radius: 15px;box-shadow: 0 6px #999;}</style></head>";
  content +=  "<body style='background: #414042'><h1 style = 'color:#c4161c'><strong>Titans's Den</strong></h1>";
  content += "<p style = 'color:white'><strong>All Fans</strong></p>";if (fan1state == "off"  || fan2state == "off"  || fan3state == "off"  || fan4state == "off"  || fan5state == "off"  || fan6state == "off"){
  content += "<p><a href=\"allOn\"><button class=\"button\">Turn ON all</button></a></p>";}
  else if (fan1state == "on"  || fan2state == "on"  || fan3state == "on"  || fan4state == "on"  || fan5state == "on"  || fan6state == "on"){content += "<p><a href=\"allOff\"><button class=\"button button2\">Turn OFF all</button></a></p>";}
  content += "<p style = 'color:white'><strong>Fan-1 (State : " + fan1state + ")</strong></p>";if (fan1state == "off"){
  content += "<p><a href=\"fan1On\"><button class=\"button\">ON</button></a></p>";}
  else if (fan1state == "on"){content += "<p><a href=\"fan1Off\"><button class=\"button button2\">OFF</button></a></p>";}
  content += "<p style = 'color:white'><strong>Fan-2 (State : " + fan2state + ")</strong></p>";if (fan2state == "off"){
  content += "<p><a href=\"fan2On\"><button class=\"button\">ON</button></a></p>";}
  else if (fan2state == "on"){content += "<p><a href=\"fan2Off\"><button class=\"button button2\">OFF</button></a></p>";}
  content += "<p style = 'color:white'><strong>Fan-3 (State : " + fan3state + ")</strong></p>";if (fan3state == "off"){
  content += "<p><a href=\"fan3On\"><button class=\"button\">ON</button></a></p>";}
  else if (fan3state == "on"){content += "<p><a href=\"fan3Off\"><button class=\"button button2\">OFF</button></a></p>";}
  content += "<p style = 'color:white'><strong>Fan-4 (State : " + fan4state + ")</strong></p>";if (fan4state == "off"){
  content += "<p><a href=\"fan4On\"><button class=\"button\">ON</button></a></p>";}
  else if (fan4state == "on"){content += "<p><a href=\"fan4Off\"><button class=\"button button2\">OFF</button></a></p>";}
  content += "<p style = 'color:white'><strong>Fan-5 (State : " + fan5state + ")</strong></p>";if (fan5state == "off"){
  content += "<p><a href=\"fan5On\"><button class=\"button\">ON</button></a></p>";}
  else if (fan5state == "on"){content += "<p><a href=\"fan5Off\"><button class=\"button button2\">OFF</button></a></p>";}
  content += "<p style = 'color:white'><strong>Fan-6 (State : " + fan6state + ")</strong></p>";if (fan6state == "off"){
  content += "<p><a href=\"fan6On\"><button class=\"button\">ON</button></a></p>";}
  else if (fan6state == "on"){content += "<p><a href=\"fan6Off\"><button class=\"button button2\">OFF</button></a></p>";}
  content += "<p1 style ='color:#c74a1b'><marquee direction='right'>Vera level Protosem</marquee></p1>";
  content += "<p1 style ='color:#c74a1b'><marquee direction='left'><strong> @FORGE</strong></marquee></p1>";
  content += "<a href=\"/login?DISCONNECT=YES\"><button class=\"button\">LOGOUT</button></a></body></html>";
  content += "<br><br> Contact : <a href='https://www.linkedin.com/in/raghul-prasad-v/' target='_blank' class='fa fa-linkedin'></a>";
   content += "<br><br> Source Code : <a href='https://github.com/raghulrider/NodeMCU-Home-Automation' target='_blank' class='fa fa-github'></a>";
  server.send(200, "text/html", content);

}


//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



void setup(void) {
  // preparing GPIOs
  pinMode(Fan1, OUTPUT);
  digitalWrite(Fan1, HIGH);
  pinMode(Fan2, OUTPUT);
  digitalWrite(Fan2, HIGH);
  pinMode(Fan3, OUTPUT);
  digitalWrite(Fan3, HIGH);
  pinMode(Fan4, OUTPUT);
  digitalWrite(Fan4, HIGH);
  pinMode(Fan5, OUTPUT);
  digitalWrite(Fan5, HIGH);
  pinMode(Fan6, OUTPUT);
  digitalWrite(Fan6, HIGH);


  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }


  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/", []() {
    server.send(200, "text/plain", "Inga onnum illa kelambu");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );


  server.on("/", []() {
    //
  });
  server.on("/fan1On", []() {
    digitalWrite(Fan1, LOW);
    Serial.println("Fan one ON");
    fan1state = "on";
    handleRoot();
  });
  server.on("/fan1Off", []() {
    //
    digitalWrite(Fan1, HIGH);
    Serial.println("Fan one OFF");
    fan1state = "off";
    handleRoot();
  });
  server.on("/fan2On", []() {
    //
    digitalWrite(Fan2, LOW);
    Serial.println("Fan two ON");
    fan2state = "on";
    handleRoot();
  });
  server.on("/fan2Off", []() {
    //
    digitalWrite(Fan2, HIGH);
    Serial.println("Fan two OFF");
    fan2state = "off";
    handleRoot();
  });
  server.on("/fan3On", []() {
    //
    digitalWrite(Fan3, LOW);
    Serial.println("Fan three ON");
    fan3state = "on";
    handleRoot();
  });
  server.on("/fan3Off", []() {
    //
    digitalWrite(Fan3, HIGH);
    Serial.println("Fan three OFF");
   fan3state = "off";
    handleRoot();
  });
  server.on("/fan4On", []() {
    //
    digitalWrite(Fan4, LOW);
    Serial.println("Fan four ON");
    fan4state = "on";
    handleRoot();
  });
  server.on("/fan4Off", []() {
    //
    digitalWrite(Fan4, HIGH);
    Serial.println("Fan four OFF");
   fan4state = "off";
    handleRoot();
  });
  server.on("/fan5On", []() {
    //
    digitalWrite(Fan5, LOW);
    Serial.println("Fan five ON");
    fan5state = "on";
    handleRoot();
  });
  server.on("/fan5Off", []() {
    //
    digitalWrite(Fan5, HIGH);
    Serial.println("Fan five OFF");
    fan5state = "off";
    handleRoot();
  });
  server.on("/fan6On", []() {
    //
    digitalWrite(Fan6, LOW);
    Serial.println("Fan 6 ON");
    fan6state = "on";
    handleRoot();
  });
  server.on("/fan6Off", []() {
    //
    digitalWrite(Fan6, LOW);
    Serial.println("Fan HIGH OFF");
    fan6state = "off";
    handleRoot();
  });
  server.on("/allOn", []() {
    //
     digitalWrite(Fan1, LOW);
     digitalWrite(Fan2, LOW);
     digitalWrite(Fan3, LOW);
     digitalWrite(Fan4, LOW);
     digitalWrite(Fan5, LOW);
     digitalWrite(Fan6, LOW);
     Serial.println("All Fans On");
    fan1state = "on";
    fan2state = "on";
    fan3state = "on";
    fan4state = "on";
    fan5state = "on";
    fan6state = "on";
    handleRoot();
  });
  server.on("/allOff", []() {
    //
     digitalWrite(Fan1, HIGH);
     digitalWrite(Fan2, HIGH);
     digitalWrite(Fan3, HIGH);
     digitalWrite(Fan4, HIGH);
     digitalWrite(Fan5, HIGH);
     digitalWrite(Fan6, HIGH);
     Serial.println("All Fans OFF");
    fan1state = "off";
    fan2state = "off";
    fan3state = "off";
    fan4state = "off";
    fan5state = "off";
    fan6state = "off";
    handleRoot();
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
