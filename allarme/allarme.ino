//librerie
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <secTimer.h>

//VARIABILI PER FUNZIONAMENTO TASTIERINO
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'#', '0', '*', 'D'}
};

//variabili per connessione pin tastierino
byte rowPins[ROWS] = {7, 6, 5, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {3, 2, 10, 11}; //connect to the column pinouts of the keypad

//CODICE INSERITO
char keyInsert[6];
//CODICE DA TROVARE
char code[7] = "112233";

//variabili necessarie per verificare se codice inserito corretto
int i = 0; int j = 0; int s = 0; int x = 0;
int a = 0; int b = 0; int c = 0; int d = 0;
int zonei = 0;

int chk;
bool stato, hasuonato;
int RETROILLUMINAZIONE = 5, duratasirena=30;
unsigned long ritardoinserimento,RITARDORILEVAMENTO;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//conversione analogica digitale pin
int LED = 15;
int BUZZER = 16;
int SIRENA = 9; //COLLEGATO A LED GIALLO E RELE'
int VOL = 14; //VAR VOLUMETRICO
int PER = 8; //VAR PERIMETRALE
int PORTA = 12; //VAT PORTONE INGRESSO
int TAP = 13; //PERIMETRALE TAPPARELLE
int LEDRILEVAZIONE = 17; //UTILIZZATA PER CONTROLLO MANCANZA ALIMENTAZIONE
char zone;

secTimer myTimer;
unsigned long inizio = 0, tempopassato; //NECESSARIE PER FAR PARTIRE CONTATORE

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SIRENA, OUTPUT);
  pinMode(VOL, INPUT);
  pinMode(PER, INPUT);
  pinMode(PORTA, INPUT);
  pinMode(TAP, INPUT);
  pinMode( LEDRILEVAZIONE, OUTPUT);
  ritardoinserimento = 1;
  RITARDORILEVAMENTO=20;
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Allarm 2.0");
  lcd.setCursor(0, 1);
  lcd.print("Luca Antonello");
  delay(1000);
  stato = 0;
  hasuonato = 0;
  myTimer.startTimer();
  lcd.clear();
}

void loop() {
  int chk = inserisci();
  if (chk == 6) {
    stato = !stato;
    if (!stato) {
      lcd.backlight();
      Serial.println("DISINSERITO");
      lcd.setCursor(0, 1);
      lcd.print("DISINSERITO");
      digitalWrite(LED, LOW);
      delay(1500);
      lcd.noBacklight();
      lcd.clear();
      x = 0;
      i = 0;
      j = 0;
    }
    else {
      lcd.clear();
      lcd.backlight();
      lcd.print("RITARDO IN CORSO");
      Serial.println("RITARDO IN CORSO");
      digitalWrite(BUZZER, HIGH);
      Serial.println(ritardoinserimento*1000);
      delay(ritardoinserimento * 1000);
      lcd.clear();
      lcd.print("INSERITO");
      Serial.println("INSERITO");
      digitalWrite(BUZZER, LOW);
      delay(1500);
      lcd.noBacklight();
      if (!movimento()) {
        stato = !stato;
        digitalWrite(LED, LOW);
        digitalWrite(SIRENA, LOW);
        digitalWrite(LEDRILEVAZIONE, LOW);
        lcd.clear();
      }
      if (hasuonato == 1) stato = 0;
      digitalWrite(LED, LOW);
      digitalWrite(BUZZER, LOW);
      zonei = 0;
      zone = ' ';
      x = 0;
      i = 0;
      j = 0;
      c = 0;
      a = 0;
      b = 0;
      lcd.clear();
    }

  }
}

int inserisci() {
  if (zonei == 0) zona();
  char key = keypad.getKey();
  if (i == 0) {
    if (zonei == 0) {
      lcd.backlight();
      lcd.print("INSERT ZONE A-C");
      lcd.setCursor(0, 1);
      lcd.print("O IMPOSTAZIONI D");
      Serial.println("Inserire la ZONA");
      delay(1500);
      lcd.noBacklight();
      i++;
    }
    if (zonei == 1) {
      lcd.backlight();
      lcd.clear();
      lcd.print("Inserire PIN:");
      Serial.println("Inserire PIN:");
      delay(1500);
      lcd.noBacklight();
      i++;
    }
  }
  lcd.setCursor(0, 1);
  if (key != NO_KEY && j < 6) {
    lcd.backlight();
    Serial.print("*");
    lcd.setCursor(j, 1);
    lcd.print("*");
    //Serial.println(key);
    keyInsert[j] = key;
    j++;
  }
  if (key == '*') {
    lcd.clear();
    lcd.print("Verifica PIN");
    Serial.println();
    Serial.println("Verifica PIN");
    delay(1000);
    for (s = 0; s < 6; s++) {
      if (keyInsert[s] == code[s]) {
        x++;
      }
    }
    if (x == 6) {
      lcd.clear();
      lcd.backlight();
      lcd.print("PIN Corretto");
      Serial.println("PIN Corretto");
      digitalWrite(LED, HIGH);
      lcd.noBacklight();
      return x;

    } else {
      lcd.clear();
      lcd.print("PIN errato");
      Serial.println("PIN errato");
      delay(2000);
      lcd.clear();
      x = 0;
      i = 0;
      j = 0;
    }
  }
  if (key == '#') {
    lcd.clear();
    x = 0;
    i = 0;
    j = 0;
  }
}


//SELEZIONA LA ZONA O LE IMPOSTAZIONI
void zona() {
  char key3 = keypad.getKey();
  if (key3 != NO_KEY && zone != 'A' && zone != 'B'/* && zone!='C'*/) {
    //SELEZIONE VOLUMETRICO
    if (key3 == 'A') {
      lcd.backlight();
      zone = 'A'; Serial.println(zone);
      zonei = 1;
      lcd.clear();
      lcd.print("VOL-PER - ZONA:");
      lcd.setCursor(0, 1);
      lcd.print(zone);
      delay(1500);
      lcd.noBacklight();
      x = 0;
      i = 0;
      j = 0;
    }
    //SELEZIONE PERIMETRALE
    if (key3 == 'B') {
      lcd.backlight();
      zone = 'B'; Serial.println(zone);
      zonei = 1;
      lcd.clear();
      lcd.print("PERIMET. ON-ZONA:");
      lcd.setCursor(0, 1);
      lcd.print(zone);
      delay(1500);
      lcd.noBacklight();
      x = 0;
      i = 0;
      j = 0;
    }
    /*if(key3=='C'){
      lcd.backlight();
      zone='C';Serial.println(zone);
      zonei=1;
      lcd.clear();
      lcd.print("PER. EST. -ZONA:");
      lcd.setCursor(0, 1);
      lcd.print(zone);
      delay(1500);
      lcd.noBacklight();
      x = 0;
      i = 0;
      j = 0;
    }*/
    //SELEZIONE IMPOSTAZIONI
    if (key3 != NO_KEY && zone != 'D') {
      if (key3 == 'D') {
        lcd.backlight();
        zone = 'D'; Serial.println(zone);
        impostazioni();
        lcd.clear();
        delay(1500);
        lcd.noBacklight();
        x = 0;
        i = 0;
        j = 0;
      }
    }

  }

}//zona


//IMPOSTA I TEMPI DI RITARDO USCITA E INGRESSO
int impostazioni() {
  char secInsert[6] = "", lettura = keypad.getKey();
  int c1 = 0;
  bool riting = false, newPin = false, rilev= false;
  lcd.backlight();
  ritardoinserimento = 0;
  lcd.clear();
  lcd.print("rit ingresso (s):");
  while (!riting) {
    char lettura = keypad.getKey();
    if (lettura != NO_KEY & c1 < 3) {
      lcd.clear();
      lcd.print("rit ingresso (s):");
      lcd.setCursor(0, 1);
      if (lettura == 'A' || lettura == 'B' || lettura == 'C' || lettura == 'D' || lettura == '#') {
        lcd.clear();
        lcd.print("inserire un");
        lcd.setCursor(0, 1);
        lcd.print("numero");
      }
      else {
        secInsert[c1] = lettura;
        lcd.print(secInsert);
        if (lettura == '*' || c1 == 2) {
          sscanf(secInsert, "%i", &ritardoinserimento);
          riting = true;
        }
        c1++;
      }


    }
  }

  c1 = 0;
  lcd.clear();
  for (int i = 1; i <= 6; i++) secInsert[i] = ' ';
  lcd.print("rit rilev (s):");
  while (!rilev) {
    char lettura = keypad.getKey();
    if (lettura != NO_KEY & c1 < 3) {
      lcd.clear();
      lcd.print("rit rilev (s):");
      lcd.setCursor(0, 1);
      if (lettura == 'A' || lettura == 'B' || lettura == 'C' || lettura == 'D' || lettura == '#') {
        lcd.clear();
        lcd.print("inserire un");
        lcd.setCursor(0, 1);
        lcd.print("numero");
      }
      else {
        secInsert[c1] = lettura;
        lcd.setCursor(0, 1);
        lcd.print(secInsert);
        lcd.setCursor(7, 1);
        lcd.print(" ");
        if (lettura == '*' || c1 == 2) {
          sscanf(secInsert, "%i", &RITARDORILEVAMENTO);
          rilev = true;
        }
        c1++;
      }


    }
  }



  c1 = 0;
  lcd.clear();
  for (int i = 1; i <= 6; i++) secInsert[i] = ' ';
  lcd.print("nuovo PIN:");
  while (!newPin) {
    char lettura = keypad.getKey();
    if (lettura != NO_KEY & c1 < 6) {
      lcd.clear();
      lcd.print("nuovo Pin");
      lcd.setCursor(0, 1);
      if (lettura == 'A' || lettura == 'B' || lettura == 'C' || lettura == 'D' || lettura == '#') {
        lcd.clear();
        lcd.print("inserire un");
        lcd.setCursor(0, 1);
        lcd.print("numero");
      }
      else {
        secInsert[c1] = lettura;
        lcd.clear();
        lcd.print("nuovo Pin");
        lcd.setCursor(0, 1);
        lcd.print(secInsert);
        lcd.setCursor(7, 1);
        lcd.print(" ");
        if (lettura == '*' || c1 == 5) {
          for (int i = 0; i < 7; i++) code[i] = secInsert[i];
          newPin = true;
        }
        c1++;
      }


    }
  }
  

}





//FA SCATTARE ALLARME SE SENSORI ECCITATI

int movimento() {
  int ritingresso = 30000;
  int volumentrico, perimetrale, portaingresso, tapparelle, p = 0, ing = 0, t = 0;
  bool movimento = 0;
  volumentrico = digitalRead(VOL);           //lettura valore del sensore
  perimetrale = digitalRead(PER);
  portaingresso = digitalRead(PORTA);
  tapparelle = digitalRead(TAP);
  while (stato) {
    lcd.clear();
    //DISINSERIMENTO PRIMA CHE IL SENSORE CAPTI
    while (c != 6 && !movimento) {
      portaingresso = digitalRead(PORTA);
      perimetrale = digitalRead(PER);
      volumentrico = digitalRead(VOL);            //lettura valore del sensore
      tapparelle = digitalRead(TAP);

      if (zone == 'A')
        if (volumentrico == HIGH || perimetrale == HIGH || portaingresso == HIGH || tapparelle == HIGH) {
          movimento = 1;
          if (perimetrale == HIGH) p = 1;
          if (portaingresso == HIGH) ing = 1;
          if (tapparelle == HIGH) t = 1;
        }
      if (zone == 'B')
        if (perimetrale == HIGH || portaingresso == HIGH || tapparelle == HIGH)
          movimento = 1;
      if (zone == 'C')
        if (portaingresso == HIGH || tapparelle == HIGH)
          movimento = 1;

      Serial.println(movimento);
      Serial.println(zone);
      Serial.print("volumentrico");
      Serial.println(volumentrico);
      Serial.print("perimetrale");
      Serial.println(perimetrale);
      Serial.print("portaingresso");
      Serial.println(portaingresso);
      Serial.print("tapparelle");
      Serial.println(tapparelle);
      char key2 = keypad.getKey();
      if (a == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Inserire PIN:");
        delay(1000);
        lcd.noBacklight();
        a++;
      }
      if (key2 != NO_KEY && b < 6) {
        lcd.backlight();
        lcd.setCursor(b, 1);
        lcd.print("*");
        //Serial.println(key2);
        keyInsert[b] = key2;
        b++;
      }
      if (key2 == '*') {
        lcd.clear();
        lcd.print("Verifica PIN");
        delay(1000);
        for (d = 0; d < 6; d++) {
          if (keyInsert[d] == code[d]) {
            c++;
          }
        }
        if (c == 6) {
          lcd.clear();
          lcd.backlight();
          lcd.print("PIN Corretto");
          //stato = 0;
          delay(1500);
          lcd.noBacklight();
          return 0;
        }
        else {
          lcd.clear();
          lcd.print("PIN errato");
          delay(2000);
          lcd.clear();
          c = 0;
          a = 0;
          b = 0;
        }
      }
      if (key2 == '#') {
        lcd.clear();
        c = 0;
        a = 0;
        b = 0;
      }
      x = 0;
      i = 0;
      j = 0;
      zonei = 0;
      //zone=' ';

    }
    //DISINSERIMENTO PRIMA CHE IL SENSORE CAPTI



    if (movimento) {
      lcd.backlight();
      inizio = myTimer.readTimer();
      digitalWrite(BUZZER, HIGH);
      //FA SCATTARE SUBITO L'ALLARME PERIMETRALE SE I SENSORI ECCITATI SONO I PERIMETRALI O TAPPARELLE
      if (movimento && (zone == 'B' || zone == 'C')) {
        digitalWrite(SIRENA, HIGH);
        digitalWrite(LEDRILEVAZIONE, LOW);
        digitalWrite(BUZZER, LOW);
        Serial.println("if b e c");
      }
      //FA SCATTARE SUBITO L'ALLARME VOLUMETRICO E PERIMETRALE SE I SENSORI ECCITATI SONO I PERIMETRALI O TAPPARELLE
      if ((movimento && zone == 'A') && (p == 1 || t == 1)) {
        digitalWrite(SIRENA, HIGH);
        digitalWrite(LEDRILEVAZIONE, LOW);
        digitalWrite(BUZZER, LOW);
        Serial.println("if a");
        Serial.print(p); Serial.print(t);
      }
      //FA SCATTARE DOPO UN RITARDO RITARDORILEVAMENTO L'ALLARME IN CASO ATTICO VOLUMETRICO PERIMETRALE
      while (c != 6) {
        tempopassato = myTimer.readTimer();
        int timer=tempopassato - inizio;
        Serial.print("timer: "); Serial.print(timer); Serial.print(" -- "); Serial.print("ritardo"); Serial.print("ritardo uscita "); Serial.print(" -- "); Serial.print("ritardo rilevamento: "); Serial.println(RITARDORILEVAMENTO);//20
        if (timer>=RITARDORILEVAMENTO) {
          //ACCENDERE LA SIRENA
          digitalWrite(SIRENA, HIGH);
          digitalWrite(LEDRILEVAZIONE, LOW);
          digitalWrite(BUZZER, LOW);
          if (timer==(RITARDORILEVAMENTO+duratasirena)) return 0;
          //DISINSERIMENTO SISTEMA QUANDO SIRENA STA SUONANDO
          if (zone=='B'&&timer==duratasirena) return 0;
          if (zone=='A'&&timer==duratasirena&&p==1) return 0;
          char key2 = keypad.getKey();
          if (a == 0) {
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Inserire PIN:");
            a++;
          }
          if (key2 != NO_KEY && b < 6) {
            lcd.backlight();
            lcd.setCursor(b, 1);
            lcd.print("*");
            //Serial.println(key2);
            keyInsert[b] = key2;
            b++;
          }
          if (key2 == '*') {
            lcd.clear();
            lcd.print("Verifica PIN");
            delay(1000);
            for (d = 0; d < 6; d++) {
              if (keyInsert[d] == code[d]) {
                c++;
              }
            }
            if (c == 6) {
              lcd.backlight();
              lcd.clear();
              lcd.print("PIN Corretto");
              //stato = 0;
              delay(1500);
              lcd.noBacklight();
              return 0;
            }
            else {
              lcd.clear();
              lcd.print("PIN errato");
              delay(2000);
              lcd.clear();
              c = 0;
              a = 0;
              b = 0;
            }
          }
          if (key2 == '#') {
            lcd.clear();
            c = 0;
            a = 0;
            b = 0;
          }
          x = 0;
          i = 0;
          j = 0;
          zonei = 0;
          //zone=' ';
          //DISINSERIMENTO SISTEMA QUANDO SIRENA STA SUONANDO

         
        }
        else {
          //DISINSERIMENTO SISTEMA QUANDO SENSORE ECCITATO PRIMA SUONO SIRENA
          char key2 = keypad.getKey();
          if (a == 0) {
            lcd.backlight();
            lcd.print("Inserire PIN:");
            delay(1000);
            lcd.noBacklight();
            a++;
          }
          if (key2 != NO_KEY && b < 6) {
            lcd.backlight();
            lcd.setCursor(b, 1);
            lcd.print("*");
            Serial.print("*");
            //Serial.println(key2);
            keyInsert[b] = key2;
            b++;
          }
          if (key2 == '*') {
            lcd.clear();
            lcd.print("Verifica PIN");
            delay(1000);
            for (d = 0; d < 6; d++) {
              if (keyInsert[d] == code[d]) {
                c++;
              }
            }
            if (c == 6) {
              lcd.clear();
              lcd.backlight();
              lcd.print("PIN Corretto");
              digitalWrite(LED, HIGH);
              delay(1500);
              lcd.noBacklight();
              //stato = 0;
              //ritingresso = 0;
              return 0;
            }
            else {
              lcd.clear();
              lcd.print("PIN errato");
              delay(2000);
              lcd.clear();
              c = 0;
              a = 0;
              b = 0;
            }
          }
          if (key2 == '#') {
            lcd.clear();
            c = 0;
            a = 0;
            b = 0;
          }
          x = 0;
          i = 0;
          j = 0;
          zonei = 0;
          //zone=' ';
        }//else
        //DISINSERIMENTO SISTEMA QUANDO SENSORE ECCITATO PRIMA SUONO SIRENA
      }//while

      delay(ritingresso);

      digitalWrite(BUZZER, LOW);
      //digitalWrite(RELAY, LOW);
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }


  //aspetta 1 secondo prima di ripetere
}




