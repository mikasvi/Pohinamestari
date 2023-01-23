/*  
Pöhinämestari
*/
#include <OneWire.h>  //Lämpötila-anturi
#include <DallasTemperature.h>  //Lämpötila-anturi
#include <LiquidCrystal.h>  //LCD kirjasto

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);//lcd(RS,En,D4,D5,D6,D7)

int LED = 13;
int THRESHOLD =    10; // Threshold liiketunnistimelle
int KNOCK1 =        A0; //Piezekiekko
int GAS_SENSOR =    A1; //MQ-3
int ONE_WIRE_BUS =  A5; //DS18B20 lämpötilasensori
int BUZZER =        11; //Kaiutin

long int x; //x ja y on alkoholisensorin kalibrointiin käytetyt muuttujat
long int y;

int kupla = 0;  //Kuplien laskeminen
int liike = 0;  //Lukee piezon mittaaman tärähtelyn

OneWire oneWire(ONE_WIRE_BUS);  //Lämpötilasensori
DallasTemperature sensors(&oneWire);
int alarmtemp1 = 30; //Lämpötilojen raja-arvot hälytystä varten
int alarmtemp2 = 20;

unsigned long alkumillis;  //Liikesensorin ajastin, että saadaan laskettua kupla/min
unsigned long nykymillis;
const unsigned long minuutti = 60000;

unsigned long aloitus; //Ohjelman päättävä ajastin
unsigned long nykyinen;
const unsigned long lopetus = 259200000;

const long kaasutimer = 10000; //Lämmön ja kaasun tulostukselle ajastimet
const long lampotimer = 10000;
unsigned long aika1 = 0;
unsigned long aika2 = 0;

void setup() {

  Serial.begin(9600); //Serial datan siirto 9600baudilla
  sensors.begin();

  lcd.begin(16, 2); //Näytön aloitus
  lcd.clear();

  // Pinnien määrittely in ja outputeiksi
  for (int i = KNOCK1; i <= ONE_WIRE_BUS; i++)
  pinMode(i, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  sensors.setResolution(9); /* Lämpötila-anturin resoluution määritys.
    Tämä oli pakollinen, koska vakiona arvo oli liian korkea ja se hidasti
    ohjelmaa, niin tästä syystä liiketunnistimesta ei saada kirjattua
    kaikkia liikkeitä, joka tarkoittaa kelvotonta mittausta */

}

void loop() {

  //Lokeroidaan anturit omiin osioihin
  Kaasusensori();
  Lamposensori();
  Liikesensori();
}

void Kaasusensori() { //Kaasusensorin toiminta
  unsigned long nykyaika1 = millis();
    x = analogRead(GAS_SENSOR);

    y = (0.04 * x - 8); //Lineraarisen suoran yhtälö, että saadaan mitattu arvo näkymään prosentteina
    
    lcd.setCursor(0, 0);//Kursorin asetus näytön ekaan ruutuun
    lcd.print("ALK:");//Kaasusensori tulostuu tämän perään
    lcd.setCursor(4, 0);  //Kaasusensorin tulostus LCD-näytölle ja serial monitorille
    lcd.print(y);
    
    if ( nykyaika1 - aika1 >= kaasutimer) {
    Serial.print("Alkoholi:");
    Serial.print(y);
    Serial.print("%");
    Serial.print("\t");
    aika1 = nykyaika1;}
  
}

void Lamposensori() {  //Lämpöanturin toiminta
  unsigned long nykyaika2 = millis();

    sensors.requestTemperatures();  //Pyytää mitatut lämpötilat sensorilta
    
    lcd.setCursor(0, 1);//Kursorin asetus
    lcd.print("Asteet:");//Liiketunnistin tulostuu tämän perään
    lcd.setCursor(7, 10);  //Lämpötilan tulostus LCD-näytölle
    lcd.print(sensors.getTempCByIndex(0));
    lcd.print((char)223); //asteen tunnus (°)
    lcd.print("C");
  
  if ( nykyaika2 - aika2 >= lampotimer) { //Ajastetaan serial monitorin tulostus halutun ajan välein
    
    Serial.print("Celsius: ");  //Lämpötilan tulostus serial monitoriin
    Serial.println(sensors.getTempCByIndex(0));
    aika2 = nykyaika2;}
   
    if (sensors.getTempCByIndex(0) > alarmtemp1) {  //Hälytys päälle jos lämpötila ylittää halutun rajan
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
    }

    else if (sensors.getTempCByIndex(0) < alarmtemp1) { //Sammuttaa hälyttimen lämpötilan laskettua
      digitalWrite(11, LOW);
    }

    if (sensors.getTempCByIndex(0) < alarmtemp2) {  //Hälytys päälle jos lämpötila alittaa halutun rajan
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
      delay(250);
      tone(BUZZER, 1000, 250);
    }

    else if (sensors.getTempCByIndex(0) > alarmtemp2) { //Sammuttaa hälyttimen lämpötilan noustua
      digitalWrite(11, LOW);
    }
    
  
}

void Liikesensori() { //Liikeanturin toiminta
  unsigned long nykymillis = millis();
  unsigned long nykyinen = millis();
  liike = analogRead(KNOCK1);
  if (liike >= THRESHOLD) { //Kun sensorin arvo ylittää thresholdin, lisätään laskuriin kupla
    kupla++;
    
    //Laskee minuutin välein onko kuplien määrä alle 4per min, jos on niin hälytys alkaa soimaan
    if (nykymillis - alkumillis >= minuutti) {
      //Serial.print(kupla);
      if (kupla <= 4)
      { tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
        tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
        tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
        tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
        tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
        delay(250);
        tone(BUZZER, 1000, 250);
      }

      kupla = 0;
      alkumillis = nykymillis;
    }
    if (nykyinen - aloitus >=lopetus){ //Kun aikaa on kulunut 72h ja kuplia tulee alle 4per min, ohjelma sulkee itsensä
      if (kupla <=4){
      Serial.println("KEITTO VALMIS!");
      {exit(0);}
      }
  }
    delay(2000);
    lcd.setCursor(8, 0);  //LCD ja serial monitorin tulostus
    lcd.print("Kupla:");
    lcd.setCursor(14, 0);
    lcd.print(kupla);
    Serial.print("Kuplia:");
    Serial.print(kupla);
    Serial.println();
  }
}
