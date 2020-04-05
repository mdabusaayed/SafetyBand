#include <TinyGPS++.h>
#include <SoftwareSerial.h>
SoftwareSerial GPS_SoftSerial(4, 5);
SoftwareSerial sms(8, 6);
TinyGPSPlus gps;
int Button = A0;
int Button_indicator = 7;
char link[] = "Google map link: http://maps.google.com/maps?q=";

volatile float minutes, seconds;
volatile int degree, secs, mins;
int button_cnt = 0;
unsigned int time_counter = 0;
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    digitalWrite(Button_indicator, LOW);
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
    /* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
  digitalWrite(Button_indicator, HIGH);
}

void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
}

void setup()
{
  Serial.begin(9600);
  GPS_SoftSerial.begin(9600);
  smartDelay(1000);
  pinMode(Button, INPUT);
  pinMode(Button_indicator, INPUT);
  digitalWrite(Button_indicator, LOW);
}

void loop()
{
  smartDelay(1000);
  unsigned long start;
  double lat_val, lng_val, alt_m_val;
  uint8_t hr_val, min_val, sec_val;
  bool loc_valid, alt_valid, time_valid;
  lat_val = gps.location.lat(); /* Get latitude data */
  loc_valid = gps.location.isValid(); /* Check if valid location data is available */
  lng_val = gps.location.lng(); /* Get longtitude data */
  alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
  alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
  hr_val = gps.time.hour(); /* Get hour */
  min_val = gps.time.minute();  /* Get minutes */
  sec_val = gps.time.second();  /* Get seconds */
  time_valid = gps.time.isValid();  /* Check if valid time data is available */
  if (!loc_valid)
  {
    Serial.print("Latitude : ");
    Serial.println("*****");
    Serial.print("Longitude : ");
    Serial.println("*****");
  }
  else
  {
    DegMinSec(lat_val);
    Serial.print("Latitude in Decimal Degrees : ");
    Serial.println(lat_val, 6);
    Serial.print("Latitude in Degrees Minutes Seconds : ");
    Serial.print(degree);
    Serial.print("\t");
    Serial.print(mins);
    Serial.print("\t");
    Serial.println(secs);
    DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
    Serial.print("Longitude in Decimal Degrees : ");
    Serial.println(lng_val, 6);
    Serial.print("Longitude in Degrees Minutes Seconds : ");
    Serial.print(degree);
    Serial.print("\t");
    Serial.print(mins);
    Serial.print("\t");
    Serial.println(secs);
  }
  if (!alt_valid)
  {
    Serial.print("Altitude : ");
    Serial.println("*****");
  }
  else
  {
    Serial.print("Altitude : ");
    Serial.println(alt_m_val, 6);
  }
  if (!time_valid)
  {
    Serial.print("Time : ");
    Serial.println("*****");
  }
  else
  {
    char time_string[32];
    sprintf(time_string, "Time : %02d/%02d/%02d \n", hr_val, min_val, sec_val);
    Serial.print(time_string);
  }

  GPS_SoftSerial.end(); delay(10);
  sms.begin(9600); delay(10);
  sms.println("AT"); delay(10);
  sms.end(); delay(10);
  GPS_SoftSerial.begin(9600); delay(10);

  time_counter++;
  if (time_counter > 300)
  {
    digitalWrite(Button_indicator, HIGH); delay(2000);
    digitalWrite(Button_indicator, LOW);
    delay(200);
    digitalWrite(Button_indicator, HIGH);
    Serial.println(F("Sending SMS"));
    GPS_SoftSerial.end();delay(10);    
    sms.begin(9600);delay(10);
    sms.println("AT"); delay(1500);
    sms.println("AT+CMGF=1"); delay(1500);
    sms.println("AT+CMGS=\"01762033107\""); delay(1500);
    sms.print("Lat:");
    sms.print(lat_val, 6);
    sms.print("  Lon:");
    sms.print(lng_val, 6);
    sms.print(" Google map:");
    sms.print(link);
    sms.print(lat_val, 6);
    sms.print(",");
    sms.print(lng_val, 6);
    sms.print(".");
    sms.println((char)26); 
    delay(3000);
    sms.end();
    GPS_SoftSerial.begin(9600);
    time_counter = 0;//clear
  }

  //Button function
  if (digitalRead(Button) == LOW)
  {
    Pulse();
    Pulse();
    Serial.println(F("Sending SMS "));
    GPS_SoftSerial.end();delay(10);
    sms.begin(9600);delay(10);
    sms.println("AT"); delay(1500);
    sms.println("AT+CMGF=1"); delay(1500);
    sms.println("AT+CMGS=\"01762033107\""); delay(1500);
    sms.print("Lat:");
    sms.print(lat_val, 6);
    sms.print("  Lon:");
    sms.print(lng_val, 6);
    sms.print(" Google map:");
    sms.print(link);
    sms.print(lat_val, 6);
    sms.print(",");
    sms.print(lng_val, 6);
    sms.print(".");
    sms.println((char)26); delay(10000);
    sms.flush();
    sms.begin(9600);delay(1000);
    Serial.println(F("Calling ..."));
    sms.println("AT"); delay(1500);
    sms.println("ATD01762033107;");
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    sms.flush(); delay(100);
    sms.end();delay(10);
    GPS_SoftSerial.begin(9600);delay(10);
    Pulse(); Pulse(); Pulse(); Pulse();
  }
}

void Pulse()
{
  digitalWrite(Button_indicator, LOW);
  for (int k = 0; k < 4; k++)
  {
    digitalWrite(Button_indicator, HIGH);
    delay(100);
    digitalWrite(Button_indicator, LOW);
    delay(100);
  }
  digitalWrite(Button_indicator, LOW);
}



