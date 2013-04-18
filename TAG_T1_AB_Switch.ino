/* Started with the WebDuidno Web_Demo.pde  */

/*
 * Transportation Agent Grid web enabled A/B switch for Cat5 cables.
 * This device is intended to switch our PRI T1s back and forth between the primary and failover asterisk servers in our data center
 *
 * http://host/
 *
 * This URL brings up a display of two Cat5 circuits that can be switched from primary to backup
 * 
 * 
 */

#define WEBDUINO_FAIL_MESSAGE "<h1>EPIC FAIL</h1>"

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"



//At this tim, IP addresses must be statically set in code.  Future revisions should allow for DHCP 
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint8_t ip[] = { 192, 168, 10, 56 };
static uint8_t gateway[] = { 192, 168, 60, 1};	
static uint8_t subnet[] = { 255, 255, 255, 0 };
const char* password = "polloloco";

#define PREFIX "/"
WebServer webserver(PREFIX, 80);



//Define the pins for this AB switch
int switchA = A0;
int switchB = A1;
int switchC = A2;
int switchD = A3;

int stateA = 0;
int stateB = 0;
int stateC = 0;
int stateD = 0;

int relayA = 2;
int relayB = 3;
int relayC = 4;
int relayD = 5;


// no-cost stream operator as described at 
// http://sundial.org/arduino/?page_id=119
template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }



void printPage(WebServer &server, WebServer::ConnectionType type)
{
  // We'll always print the JSON state of all switches 
  server << "{ ";
  server << "GarageLights: " + stateA + "; ";
  else
    server << "<font style=\"color: green;\">NORMAL</font>";
  
  server << "<form method='POST' action='/' id='A'><input type='hidden' name='flipA' value='true'>password:<input type='password' name='auth'><input type='submit' value='Toggle A' name='Toggle'></form>"; 
  
  server << "<li>Circuit B is: ";
  if (isBOn())
    server << "<font style=\"color: red;\">FAILOVER</font>";
  else
    server << "<font style=\"color: green;\">NORMAL</font>";
  
  server << "<form method='POST' action='/'id ='B'><input type='hidden' name='flipB' value='true'>password:<input type='password' name='auth'><input type='submit' value='Toggle B' name='Toggle'></form></body></html>"; 
  
  
}

void formCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  
 server.httpSeeOther(PREFIX "/");
    
  
  // Whether we do something or not, we still need to print the page
  printPage(server, type);
}

void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool authenticated = false;
    bool flipA = false;
    bool flipB = false;
    
    bool repeat;
    char name[16], value[16];
    do
    {
      repeat = server.readPOSTparam(name, 16, value, 16);
      if (strcmp(name, "auth") == 0)
      {
        if (strcmp(value, password) == 0)
        {
          authenticated = true;
        }
      }
      
      if (strcmp(name, "flipA") == 0)
        flipA = true;
        
      if (strcmp(name, "flipB") == 0)
        flipB = true;
      
    } while (repeat);
    
    if (authenticated && flipA)
      toggleA();
      
    if (authenticated && flipB)
      toggleB();
  }
      
  printPage(server, type); 
}

void setup()
{
  // set pins 0-8 for digital input
  pinMode(relayA0, OUTPUT);
  pinMode(relayA1, OUTPUT);
  pinMode(relayB0, OUTPUT);
  pinMode(relayB1, OUTPUT);
  
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);

  Ethernet.begin(mac, ip, gateway, subnet);
  webserver.begin();

  webserver.setDefaultCommand(&defaultCmd);
  webserver.addCommand("form", &formCmd);
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();
  
  checkButtons();
}


void checkButtons()
{
  if (analogRead(switchA) == 1023)
  {
    // Button should be held for 2 seconds
    delay(2000);
    if (analogRead(switchA) == 1023)
      toggleA();
    
  }
  
  if (analogRead(switchB) == 1023)
  {
    // Button should be held for 2 seconds
    delay(2000);
    if (analogRead(switchB) == 1023)
      toggleB();
    
  }
  
}

void flipSwitchA(boolean turnOn)
{
  if (turnOn)
  {
    digitalWrite(relayA0, HIGH);
    //delay(100);
    digitalWrite(relayA1, HIGH);
    //delay(100);
    digitalWrite(ledA, HIGH);
  }
  else
  {
    digitalWrite(relayA0, LOW);
    digitalWrite(relayA1, LOW);
    digitalWrite(ledA, LOW);
  }
}

void flipSwitchB(boolean turnOn)
{
  if (turnOn)
  {
    digitalWrite(relayB0, HIGH);
    //delay(100);
    digitalWrite(relayB1, HIGH);
    //delay(100);
    digitalWrite(ledB, HIGH);
  }
  else
  {
    digitalWrite(relayB0, LOW);
    digitalWrite(relayB1, LOW);
    digitalWrite(ledB, LOW);
  }
}

void toggleA()
{
  flipSwitchA(!isAOn());
}

void toggleB()
{
  flipSwitchB(!isBOn());
}

boolean isAOn()
{
  if (digitalRead(relayA0) == HIGH)
    return true;
  else
    return false;
}

boolean isBOn()
{
  if (digitalRead(relayB0) == HIGH)
    return true;
  else
    return false;
}

