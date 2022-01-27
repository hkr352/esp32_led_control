/*
    WiFi LED ON/OFF TEST
      PWM Control
      slider control

    - hkr352 @miya - https://github.com/hkr352
     This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
 */

#include <WiFi.h>
 
#define DEBUG
 
const char* ssid = "SSID";
const char* password = "PASSWORD";
 
IPAddress ip(192, 168, 1, 32);           // for fixed IP Address
IPAddress gateway(192,168, 1, 1);        //
IPAddress subnet(255, 255, 255, 0);      //
IPAddress DNS(192, 168, 1, 90);          //
 
WiFiServer server(80);
 
byte led_brightness = 0 ;
 
void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);      // set the LED pin mode
     
    WiFi.config(ip, gateway, subnet, DNS);   // Set fixed IP address
    //WiFi.config(ip);
    delay(10);
 
    // We start by connecting to a WiFi network -----------------------------
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
 
    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
     
    server.begin();
 
    // for LED PWM Control ---------------------------------------------------
    //sigmaDeltaSetup(0, 312500);    // setup channel 0 with frequency 312500 Hz
    //sigmaDeltaAttachPin(2,0);      // attach pin 2 to channel 0
    //sigmaDeltaWrite(0, 0);         // initialize channel 0 to off     
}
 
void loop(){
  WiFiClient client = server.available();     // listen for incoming clients
  int pos ;
  int val ;
  int xhr ;
  String cmd = "" ;
   
  if (client) {                                               // if you get a client,
    # ifdef DEBUG
        Serial.println("***** Client access start *****");       // print a message out the serial port
    #endif
    xhr = 0 ;
    while (client.connected()) {                              // loop while the client's connected
      if (client.available()) {                               // if there's bytes to read from the client,
        String line = client.readStringUntil('\n');           // Get Line data until '\n'
        # ifdef DEBUG
            Serial.println(line);
        #endif
        if ((pos= line.indexOf("GET /?slid")) != -1) {
          pos += 11 ;
          while((line.charAt(pos) >='0') & (line.charAt(pos) <='9')) { 
            cmd += line.charAt(pos++) ;
          }
          val = cmd.toInt() ;
          if (val>256) val = 255 ;
          led_brightness = (byte)val ;
          xhr=1;
          # ifdef DEBUG
              Serial.print("led_brightness : ");
              Serial.println(led_brightness) ;
          #endif
        }
        if ((pos= line.indexOf("GET /?led_v")) != -1) {
          pos += 12 ;
          while((line.charAt(pos) >='0') & (line.charAt(pos) <='9')) { 
            cmd += line.charAt(pos++) ;
          }
          val = cmd.toInt() ;
          if (val>256) val = 255 ;
          led_brightness = (byte)val ;
          # ifdef DEBUG
              Serial.print("led_brightness : ");
              Serial.println(led_brightness) ;
          #endif
        }
        if ((pos=line.indexOf("GET /?on")) != -1) {                 // Client request was "GET /?on" 
          led_brightness += 1 ;
          # ifdef DEBUG
              Serial.print("led_brightness : ");
              Serial.println(led_brightness);
          #endif
        }
        if ((pos=line.indexOf("GET /?off")) != -1) {                // Client request was "GET /?off"
          led_brightness = 0 ;
          # ifdef DEBUG
              Serial.print("led_brightness : ");
              Serial.println(led_brightness);
          #endif
        }
        // sigmaDeltaWrite(0, led_brightness) ;                        // set PWM value to channel#0
 
        if (line.length() == 1 && line[0] == '\r'){         // end of HTTP request
          if (xhr == 0)
            send_response(client) ;                                 // send response to client
          else
            send_response2(client) ;                                // send response to client
          break;                                                    // break while loop
        }
      }
    }
    delay(1);                                        // give the web browser time to receive the data
    // close the connection:
    client.stop();
    # ifdef DEBUG
        Serial.println("Client Disconnected.");
        Serial.println("--------------------------------------------------");
    #endif
  }
}
 
// ------------------------------------------------------------------
 
void send_response(WiFiClient client) {
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    // the content of the HTTP response follows the header:
    
    client.println("<!DOCTYPE html><html lang='ja'><head><meta charset='UTF-8'>" ) ;
    client.println("<style>input.button  {margin:8px;width:100px;}" ) ;
    client.println("       input.button2 {margin-left:8px; width:40px;}" ) ;
    client.println("       input.text    {margin-left:8px; width:25px;}" ) ;
    client.println("       input.slid    {width:230px;}" ) ;
    client.println("       div   {font-size:16pt;text-align:center;width:250px;border:solid 4px #93ff93;} " ) ;
    client.println("       </style>" ) ;
    client.println("<title>Color LED Controller</title></head>" ) ;
    client.println("" ) ;
    client.println("<body>" ) ;
    client.println("<div><p>LED ON/OFF</p>" ) ;
    client.println("  <form method='get' style='text-align:left' > " ) ;
    client.println("    <span style='padding-left:15pt; font-size:8pt ;text-align:left'> LED brightness (0-255)</span> " ) ;
    client.println("    <input class='text'  type='text' name='led_v' id='led_v' value=" ) ;
    client.println(led_brightness ) ;
    client.println("    >" ) ;
    client.println("    <input class='button2' type='submit' name='set' value='SET'>" ) ;
    client.println("  </form> " ) ;
    client.println("" ) ;
    client.println("  <form name='slidform' method='get' style='text-align:left'> " ) ;
    client.print("    <input class='slid' type='range' name='led_s' value=" ) ;
    client.print(led_brightness ) ;
    client.println(" min='80' max='255' step='1' onchange='setval(this.value)' oninput='setval(this.value)' >" ) ;
    client.println("  </form> " ) ;
    client.println("" ) ;
    client.println("  <form method='get'>" ) ;
    client.println("    <input class='button' type='submit' name='on' value='ON'><input class='button' type='submit' name='off' value='OFF'><br>" ) ;
    client.println("  </form>" ) ;
    client.println("" ) ;
    client.println("  </div>" ) ;
    client.println("" ) ;
    client.println("" ) ;
    client.println("<script>" ) ;
    client.println("function setval(ledval){" ) ;
    client.println("    var xhr = new XMLHttpRequest();" ) ;
    client.println("    xhr.open('get', '?slid='+ledval );" ) ;
    client.println("    xhr.timeout = 1000 ;" ) ;
    client.println("    xhr.setRequestHeader('Cache-Control', 'no-cache');" ) ;
    client.println("    xhr.setRequestHeader('If-Modified-Since', 'Thu, 01 Jun 1970 00:00:00 GMT');" ) ;
    client.println("    xhr.responseType = 'document' ;" ) ;
    client.println("" ) ;
    client.println("    xhr.onreadystatechange = function() {" ) ;
    client.println("        if( (xhr.readyState == 4) && (xhr.status == 200) ) {" ) ;
    client.println("            document.getElementById('led_v').value = xhr.response.getElementById('output1').innerHTML;" ) ;
    client.println("        }" ) ;
    client.println("    }" ) ;
    client.println("    xhr.ontimeout = function(e) {" ) ;
    client.println("        xhr.abort() ;" ) ;
    client.println("    }" ) ;
    client.println("    xhr.send();" ) ;
    client.println("}" ) ;
    client.println("" ) ;
    client.println("</script>" ) ;
    client.println("</body>" ) ;
    client.println("</html>" ) ;
    client.println("") ;
    Serial.println( " --- send responce --- ");
}
 
void send_response2(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE html><html lang='ja'>" ) ;
    client.println(" <head> <title>Color LED Controller</title></head>") ;
    client.println("<body>") ;
    client.print("<output id='output1'>") ;
    client.print(led_brightness ) ;
    client.println(" </output> ") ;
    client.println("</body>") ;
    client.println("</html>") ;
    # ifdef DEBUG
        Serial.println( " --- send responce2 --- ");
    #endif
}
