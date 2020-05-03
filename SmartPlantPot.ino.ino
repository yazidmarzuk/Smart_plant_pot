int sensor_light = A1;
int value_light;
int sensor_temp = A2;
int value_temp;
int sensor_water = A3;
int value_water;
int red = 9;
int green = 8;
int blue = 7;
int status;
int wifiConnected = 0;

#define DEBUG 1  // Please set DEBUG = 0 when USB not connected

#define _baudrate 9600
#define wifi_pin 13

#define SSID "yourwifiname"
#define PASS "yourwifipassword"
#define IP "184.106.153.149"

String GET = "GET /update?key=yourthingspeakAPIkey";

void setup() {
  pinMode (13, OUTPUT);
  digitalWrite (13, HIGH);
  delay(1000);

  pinMode (red, OUTPUT);
  pinMode (blue, OUTPUT); 
  pinMode (green, OUTPUT); 
  
  digitalWrite (red, HIGH);
  delay(200);
  digitalWrite (red, LOW);
  delay(200);
  digitalWrite (green, HIGH);
  delay(200);
  digitalWrite (green, LOW);
  delay(200);
  digitalWrite (blue, HIGH);
  delay(400);
  digitalWrite (blue, LOW);
  
  
  Serial.begin( _baudrate );
  Serial1.begin( _baudrate );
  
  if(DEBUG) { 
    while(!Serial);
  }

  //set mode needed for new boards
  Serial1.println("AT+RST");
  delay(3000);//delay after mode change       
  Serial1.println("AT+CWMODE=1");
  delay(300);
  Serial1.println("AT+RST");
  delay(500);

}

void loop() {
  
  if(!wifiConnected) {
    Serial1.println("AT");
    delay(1000);
    if(Serial1.find("OK")){
      Serial.println("Module Test: OK");
      connectWifi();

      if (wifiConnected) {
          String cmd = "AT+CIPMUX=0";
            digitalWrite (green, HIGH);
      delay(200);
        digitalWrite (green, LOW);
        delay(200);
        digitalWrite (green, HIGH);
        delay(200);
        digitalWrite (green, LOW);
        delay(200);
        digitalWrite (green, HIGH);
        delay(600);
        digitalWrite (green, LOW);
          sendDebug( cmd );
          if( Serial1.find( "Error") )
          {
               Serial.print( "RECEIVED: Error" );
               return;
          }
      }
    } 
  }

  if(!wifiConnected) {
    delay(500);
    return;
  }

  status=0;

  value_temp = analogRead(sensor_temp);
  Serial.print("temperature");
  Serial.println( value_temp );
  value_light = analogRead(sensor_light);
  Serial.print("light");
  Serial.println( value_light );
  value_water = analogRead(sensor_water);
  Serial.print("water");
  Serial.println( value_water );
  
  String temp=String(value_temp);// turn integer to string
  String light=String(value_light);// turn integer to string
  String water=String(value_water);// turn integer to string
  
  updateTS(temp,light, water);
  delay(5000); //added delay here

  if (value_temp < 800){
  digitalWrite (blue, LOW);
  digitalWrite (red, HIGH);
  digitalWrite (green, LOW);
status=1;
}else{
  digitalWrite (blue, LOW);
  digitalWrite (red, LOW);
  digitalWrite (green, LOW); // normal temp - red led off
}
  if (value_light > 600) {
  digitalWrite (blue, HIGH); // not enough light - yellow led on
  digitalWrite (red, LOW);
  digitalWrite (green, HIGH);
  delay(300);               // wait for a second
  digitalWrite(blue, LOW);  // turn the LED off by making the voltage LOW
  digitalWrite (red, LOW);
  digitalWrite (green, LOW);
  delay(300); 
status=1;  
}else{
  digitalWrite(blue, LOW);  // turn the LED off by making the voltage LOW
  digitalWrite (red, LOW);
  digitalWrite (green, LOW); // enough light - yellow led off
}
  if (value_water < 300) {  
  digitalWrite (blue, HIGH);
  digitalWrite (red, LOW);
  digitalWrite (green, LOW); // plant thirsty - blue led on
  delay(300);               // wait for a second
  digitalWrite(blue, LOW);  // turn the LED off by making the voltage LOW
  digitalWrite (red, LOW);
  digitalWrite (green, LOW);    // turn the LED off by making the voltage LOW
  delay(300); 
status=1;
}else{
  digitalWrite(blue, LOW);  // turn the LED off by making the voltage LOW
  digitalWrite (red, LOW);
  digitalWrite (green, LOW); // soil is moist - blue led off
}

if(status==0) {
  digitalWrite (blue, LOW);
  digitalWrite (red, LOW);
  digitalWrite (green, HIGH);
  delay(300);               // wait for a second
  digitalWrite (blue, LOW);  // turn the LED off by making the voltage LOW
  digitalWrite (red, LOW);
  digitalWrite (green, LOW);   // turn the LED off by making the voltage LOW
  delay(300); 
 }else{
  digitalWrite (blue, LOW);  // turn the LED off by making the voltage LOW
  digitalWrite (red, LOW);
  digitalWrite (green, LOW);
}

}

//----- update the Thingspeak string with 3 values
void updateTS( String T, String H , String W)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  sendDebug(cmd);
  delay(2000);
  if( Serial1.find( "Error" ) )
  {
    Serial1.print( "RECEIVED: Error\nExit1" );
    return;
  }

  cmd = GET + "&field1=" + T +"&field2=" + H +"&field3=" + W +"\r\n";
  Serial1.print( "AT+CIPSEND=" );
  Serial1.println( cmd.length() );

  //display command in serial monitor
  Serial.print("AT+CIPSEND=");
  Serial.println( cmd.length() );

  if(Serial1.find(">")) {
    // The line is useless
    //Serial1.print(">");
    Serial1.print(cmd);
    Serial.print(cmd);
    delay(1000);
    sendDebug( "AT+CIPCLOSE" );
  } else {
    sendDebug( "AT+CIPCLOSE" );
    return;
  }


  if( Serial1.find("OK") )
  {
    Serial1.println( "RECEIVED: OK" );
  }
  else
  {
    Serial1.println( "RECEIVED: Error\nExit2" );
  }
}

void sendDebug(String cmd)
{
  Serial.print("SEND: ");
  Serial.println(cmd);
  Serial1.println(cmd);
}

boolean connectWifi() {     
 String cmd="AT+CWJAP=\"";
 cmd+=SSID;
 cmd+="\",\"";
 cmd+=PASS;
 cmd+="\"";
 Serial.println(cmd);
 Serial1.println(cmd);
           
 for(int i = 0; i < 20; i++) {
   Serial.print(".");
   if(Serial1.find("OK"))
   {
     wifiConnected = 1;

     break;
   }
   
   delay(50);
 }
 
 Serial.println(
   wifiConnected ? 
   "OK, Connected to WiFi." :
   "Can not connect to the WiFi."
 );
 
 return wifiConnected;
}
