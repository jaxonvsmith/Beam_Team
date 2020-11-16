#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid     = "Jax";
const char* password = "Jaxon1515";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String Hour;
String Minute;
String Second;
String Year;
String Month;
String Day;


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);
}
void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.print("FIRSTOUTPUT");
  Serial.print(formattedDate);
  Serial.print("\n");


  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  // Set individual Values
  Year = formattedDate.substring(0, 3);
  Month = formattedDate.substring(5, 6);
  Day = formattedDate.substring(8, 9);
  Hour = formattedDate.substring(11, 12);
  Minute = formattedDate.substring(14, 15);
  Second = formattedDate.substring(17, 18);
  //Serial.print("Year: %d Month: %d Day: %d Hour: %d Minute: %d Second: %d \n",Year.toInt(), Month.toInt(), Day.toInt(), Hour.toInt(), Minute.toInt(), Second.toInt());
  
  delay(1000);
}
