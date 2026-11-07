#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

const bool moteurActif = false;

const int moteurA = 13;
const int moteurB = 12;
const int boutonJoystick = 4;
const int interrupteur = 18;
const int buzzer = 19;
const int servoPin = 25;
const int clignotant = 26;
const int phare = 27;

const int adresseADS = 0x4B;
const int adresseLCD = 0x27;

LiquidCrystal_I2C lcd(adresseLCD, 16, 2);
Servo monServo;

int x = 128;
int y = 128;
int lumiere = 128;

float vitesse = 0;

bool phareAllume = false;
bool lcdPresent = false;
bool adsPresent = false;
bool clignoAllume = false;
bool ancienInterrupteur = false;
bool buzzerAllume = false;

unsigned long tempsCapteurs = 0;
unsigned long tempsVitesse = 0;
unsigned long tempsServo = 0;
unsigned long tempsLCD = 0;
unsigned long tempsSerie = 0;
unsigned long tempsCligno = 0;

bool detecterI2C(int adresse) {
  Wire.beginTransmission(adresse);
  return Wire.endTransmission() == 0;
}

int lireADS(int canal) {
  byte commande = 0x84 | ((((canal << 2) | (canal >> 1)) & 7) << 4);

  Wire.beginTransmission(adresseADS);
  Wire.write(commande);

  if (Wire.endTransmission() != 0) {
    return -1;
  }

  if (Wire.requestFrom(adresseADS, 1) != 1) {
    return -1;
  }

  return Wire.read();
}

void lireCapteurs() {
  int nouveauX = lireADS(2);
  int nouveauY = lireADS(0);
  int nouvelleLumiere = lireADS(4);

  adsPresent = (nouveauX >= 0 && nouveauY >= 0);

  if (nouveauX >= 0) {
    x = nouveauX;
  }

  if (nouveauY >= 0) {
    y = nouveauY;
  }

  if (nouvelleLumiere >= 0) {
    lumiere = nouvelleLumiere;

    if (lumiere < 85) {
      phareAllume = true;
    }

    if (lumiere > 145) {
      phareAllume = false;
    }

    digitalWrite(phare, phareAllume ? HIGH : LOW);
  }
}

float lireAxe(int valeur) {
  float position = (valeur - 128) / 127.0f;

  if (abs(position) < 0.10f) {
    return 0;
  }

  return constrain(position, -1.0f, 1.0f);
}

void gererVitesse(float secondes) {
  float demande = adsPresent ? lireAxe(y) : 0;

  if (demande > 0) {
    vitesse += 5 * demande * secondes;
  }

  if (demande < 0) {
    vitesse += 3 * demande * secondes;
  }

  if (vitesse > 0) {
    vitesse = max(0.0f, vitesse - secondes);
  }

  if (vitesse < 0) {
    vitesse = min(0.0f, vitesse + secondes);
  }

  vitesse = constrain(vitesse, -50.0f, 100.0f);

  if (!moteurActif) {
    return;
  }

  int puissance = (int)(abs(vitesse) * 255 / 100);

  if (vitesse > 0) {
    analogWrite(moteurA, puissance);
    analogWrite(moteurB, 0);
  } else if (vitesse < 0) {
    analogWrite(moteurA, 0);
    analogWrite(moteurB, puissance);
  } else {
    analogWrite(moteurA, 0);
    analogWrite(moteurB, 0);
  }
}

void gererClignotant(unsigned long maintenant) {
  bool actif = digitalRead(interrupteur) == LOW;

  if (!actif) {
    clignoAllume = false;
    ancienInterrupteur = false;
    digitalWrite(clignotant, LOW);
    return;
  }

  if (!ancienInterrupteur) {
    ancienInterrupteur = true;
    clignoAllume = false;
    tempsCligno = maintenant;
  }

  int duree = clignoAllume ? 500 : 250;

  if (maintenant - tempsCligno >= duree) {
    clignoAllume = !clignoAllume;
    tempsCligno = maintenant;
  }

  digitalWrite(clignotant, clignoAllume);
}

void gererServo() {
  if (adsPresent) {
    int angle = (int)((lireAxe(x) + 1) * 90);
    angle = constrain(angle, 0, 180);
    monServo.write(angle);
  } else {
    monServo.write(90);
  }
}

void gererBuzzer() {
  bool appui = digitalRead(boutonJoystick) == LOW;

  if (appui != buzzerAllume) {
    buzzerAllume = appui;

    if (appui) {
      tone(buzzer, 1000);
    } else {
      noTone(buzzer);
    }
  }
}

void afficherLCD() {
  if (!lcdPresent) {
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Vitesse:        ");

  lcd.setCursor(9, 0);
  lcd.print((int)abs(vitesse));

  lcd.setCursor(15, 0);

  if (vitesse > 0.01) {
    lcd.print('>');
  } else if (vitesse < -0.01) {
    lcd.print('<');
  } else {
    lcd.print(' ');
  }

  lcd.setCursor(0, 1);

  if (phareAllume) {
    lcd.print("Phare:ON        ");
  } else {
    lcd.print("Phare:OFF       ");
  }

  lcd.setCursor(15, 1);
  lcd.print(ancienInterrupteur ? "*" : " ");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(boutonJoystick, INPUT_PULLUP);
  pinMode(interrupteur, INPUT_PULLUP);

  pinMode(phare, OUTPUT);
  pinMode(clignotant, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(moteurA, OUTPUT);
  pinMode(moteurB, OUTPUT);

  digitalWrite(phare, LOW);
  digitalWrite(clignotant, LOW);
  digitalWrite(moteurA, LOW);
  digitalWrite(moteurB, LOW);
  noTone(buzzer);

  monServo.setPeriodHertz(50);
  monServo.attach(servoPin, 500, 2400);
  monServo.write(90);

  lcdPresent = detecterI2C(adresseLCD);

  if (lcdPresent) {
    lcd.init();
    lcd.backlight();
    lcd.print("TP2 Dragula");
  }

  Serial.println("TP2 Dragula");

  Serial.print("LCD: ");
  Serial.println(lcdPresent ? "OK" : "absent");

  Serial.print("ADS7830: ");
  Serial.println(detecterI2C(adresseADS) ? "OK" : "absent");
}

void loop() {
  unsigned long maintenant = millis();

  if (maintenant - tempsCapteurs >= 50) {
    tempsCapteurs = maintenant;
    lireCapteurs();
  }

  if (maintenant - tempsVitesse >= 100) {
    float secondes = (maintenant - tempsVitesse) / 1000.0f;
    tempsVitesse = maintenant;
    gererVitesse(secondes);
  }

  if (maintenant - tempsServo >= 250) {
    tempsServo = maintenant;
    gererServo();
  }

  gererClignotant(maintenant);
  gererBuzzer();

  if (maintenant - tempsLCD >= 150) {
    tempsLCD = maintenant;
    afficherLCD();
  }

  if (maintenant - tempsSerie >= 1000) {
    tempsSerie = maintenant;

    Serial.print("X=");
    Serial.print(x);

    Serial.print(" Y=");
    Serial.print(y);

    Serial.print(" LDR=");
    Serial.print(lumiere);

    Serial.print(" Vitesse=");
    Serial.println(vitesse);
  }
}