#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define RELAY 4
#define Switch 7
#define PIN_RED 11
#define PIN_GREEN 10
#define PIN_BLUE 9
#define MAX_TEMPLATE_COUNT 127
byte timerRunning;

int buttonState      = 0;
int lastButtonState  = 0;
unsigned long startTime     = 0;
unsigned long endTime       = 0;
unsigned long holdTime      = 0;
unsigned long idleTime      = 0; 
int counter = 0;

SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
void setup()
{
  pinMode(Switch,INPUT);
  delay(100);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  pinMode(PIN_RED,  OUTPUT);
  pinMode(PIN_GREEN,OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  Serial.begin(9600);
  while(!Serial);
  if (finger.verifyPassword())
    Colour(240,240,240);
  else
  {
    Serial.println("Circuit wrong");
    while (1)
      Blink(255,0,0,500);
  }

  finger.getTemplateCount();
  if(finger.templateCount >= MAX_TEMPLATE_COUNT)
    while(1)
    {
      Serial.println("Something very wrong");
      Blink(240,240,240,500);
    }
}

void loop() // run over and over again
{
  Colour(240,240,240);
  Serial.println(finger.templateCount);
  delay(500);
  buttonState = digitalRead(Switch);
  holdTime = 0;
  if(buttonState == HIGH && lastButtonState == LOW)
  {
    startTime = millis();
    idleTime = startTime - endTime;
  }
  else if(buttonState == LOW && lastButtonState == HIGH)
  {
    endTime = millis();
    holdTime = endTime - startTime;
  }
    //updateState();
    
  lastButtonState = buttonState;
  Serial.print("HoldTime = ");
  Serial.println(holdTime);
  if((holdTime>=0)&&(holdTime<5000))
  {
    Serial.println("check finger to unlock");
    if(getFingerprintIDez()!=-1)
    {
      Unlock();
      delay(50); // yet to check if needed
    }
  }

  else if((holdTime>=5000)&&(holdTime<10000))
  {
    Serial.println("add finger function invoked");
    Colour(0,0,240);
    while(getFingerprintIDez()==-1)
    {
      Colour(240,0,0);
      delay(250);
      Colour(0,0,240);
    }
  // enrollment authorized
  Colour(0,240,0);
  delay(500);
  id = finger.templateCount + 1;
  Colour(0,0,240);
  while(!getFingerprintEnroll(id));
  }

  else
  {
    Serial.println("deletion invoked");
    Colour(160,0,0);
    while(getFingerprintIDez()==-1)
    {
      Colour(240,0,0);
      delay(250);
      Colour(160,0,0);
    }
    // deletion authorized
    Colour(0,240,0);
    delay(500);
    finger.emptyDatabase();
    //for(id=finger.templateCount; id>1; id--)
    //{
    // deleteFingerprint(id);                     //deletion call function
    //}
    
    Serial.print("deletion done! ");
    getFingerprintEnroll(1);
  }
}

uint8_t deleteFingerprint(uint8_t id)
{
  Serial.print("deletion invoked");
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK)
  {
    Colour(0,240,0);
    delay(500);
    Colour(240,240,240);
  }
  else
  {
    Colour(240,0,0);
    delay(500);
    Colour(0,0,0);
    return p;
  }
}

uint8_t getFingerprintEnroll(uint8_t inputId)
{
  Serial.println("enrollment called");
  int p = -1;
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
      Colour(0,240,0);
      delay(250);
      break;
      case FINGERPRINT_NOFINGER:
      Blink(0,0,240,250);
      break;
      case FINGERPRINT_PACKETRECIEVEERR:
      Colour(240,0,0);
      delay(500);
      break;
      case FINGERPRINT_IMAGEFAIL:
      Colour(240,0,0);
      delay(500);
      break;
      default:
      Colour(240,0,0);
      delay(500);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p)
  {
    case FINGERPRINT_OK:
    Colour(0,240,0);
    delay(250);
    break;
    case FINGERPRINT_IMAGEMESS:
    Colour(240,0,0);
    delay(250);
    return p;
    case FINGERPRINT_PACKETRECIEVEERR:
    Colour(240,0,0);
    delay(250);
    return p;
    case FINGERPRINT_FEATUREFAIL:
    Colour(240,0,0);
    delay(250);
    return p;
    case FINGERPRINT_INVALIDIMAGE:
    Colour(240,0,0);
    delay(250);
    return p;
    default:
    Colour(240,0,0);
    delay(250);
    return p;
  }
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER)
    p = finger.getImage();
  p = -1;
  Colour(0,0,240);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
      Colour(0,240,0);
      delay(250);
      break;
      case FINGERPRINT_NOFINGER:
      Blink(0,0,240,250);
      break;
      case FINGERPRINT_PACKETRECIEVEERR:
      Colour(240,0,0);
      delay(500);
      break;
      case FINGERPRINT_IMAGEFAIL:
      Colour(240,0,0);
      delay(500);
      break;
      default:
      Colour(240,0,0);
      delay(500);
      break;
    }
  }

  // OK success!
  Serial.println("First sample done");
  p = finger.image2Tz(2);
  switch (p)
  {
    case FINGERPRINT_OK:
    Colour(0,240,0);
    delay(250);
    break;
    case FINGERPRINT_IMAGEMESS:
    Colour(240,0,0);
    delay(250);
    return p;
    case FINGERPRINT_PACKETRECIEVEERR:
    Colour(240,0,0);
    delay(250);
    return p;
    case FINGERPRINT_FEATUREFAIL:
    Colour(240,0,0);
    delay(250);
    return p;
    case FINGERPRINT_INVALIDIMAGE:
    Colour(240,0,0);
    delay(250);
    return p;
    default:
    Colour(240,0,0);
    delay(250);
    return p;
  }
  Serial.println("second sample added");
  // OK converted!
  //Serial.print("Creating model for #"); Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    Colour(0,0,0);
    delay(250);
    Colour(0,240,0);
  }
  else
  {
    Colour(240,0,0);
    delay(500);
    Colour(0,0,0);
    return p;
  }

  p = finger.storeModel(inputId);
  if (p == FINGERPRINT_OK)
  {
    Colour(0,240,0);
    delay(250);
    Colour(0,0,0);
  }
  else
  {
    Colour(240,0,0);
    delay(500);
    Colour(0,0,0);
    return p;
  }
  Serial.println("addition done");
  return 1;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez()
{
  //if(finger.templateCount==0)
  //{
  //  Colour(0,240,0);
  //  delay(500);
  //  Colour(240,240,240);
  //  return 0;
  //}
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)
  {
    Colour(120,120,120);
    delay(250);
    Colour(240,240,240);
    return -1;
  }
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
  {
    Colour(120,120,120);
    delay(250);
    Colour(240,240,240);
    return -1;
  }
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    Colour(240,0,0);
    delay(500);
    Colour(240,240,240);
    return -1;
  }

  // found a match!
  Colour(0,240,0);
  delay(500);
  Colour(240,240,240);

  return finger.fingerID;
}
void Colour(int R,int G,int B)
{ 
  analogWrite(PIN_RED,  R);
  analogWrite(PIN_GREEN,G);
  analogWrite(PIN_BLUE, B);
}
void updateState()
{
  Serial.println("Function called");
  if(buttonState==HIGH){
    startTime = millis();
    idleTime = startTime-endTime;
  }
  else{
    endTime = millis();
    holdTime=endTime-startTime;
  }
}
void Blink(int R, int G, int B, int del)
{
  Colour(R,G,B);
  delay(del);
  Colour(0,0,0);
  delay(del);
}
void Unlock()
{
  digitalWrite(RELAY, LOW);
  Colour(0,240,0);
  delay(3000);
  digitalWrite(RELAY, HIGH);
  Colour(240,240,240);
}
