#include <Arduino.h>
#include <Wire.h>
#include <ds3231.h>

struct ts t;

//primeiro painel
#define sensor_umidade_1 A1
#define rele_bomba_1 12 
#define pino_5v_bomba 13
#define nivel_agua_r1_vazio 24
#define nivel_agua_r1_baixo 26
#define nivel_agua_r1_medio 28
#define nivel_agua_r1_cheio 30
#define led_vermelho_r1 8
#define led_verde_r1 22
#define led_amarelo_r1 6
#define sensor_luz_1 A0               

//segundo painel

#define rele_bomba_2 11
//#define nivel_agua_r2_vazio 32
//#define nivel_agua_r2_baixo 34
//#define nivel_agua_r2_medio 36
//#define nivel_agua_r2_cheio 38
//#define led_vermelho_r2 40
//#define led_verde_r2 42
//#define led_amarelo_r2 44

//determina se o led liga, desliga ou pisca de acordo com a altura do reservatório
void ledsReservatorio(int estado_cheio, int estado_medio, int estado_baixo, int estado_vazio, int led_vermelho, int led_amarelo, int led_verde);
//liga ou desliga a bomba de acordo com a umidade do solo
void ligaDesligaBomba(int rele_bomba, int valor_umidade, int min_solo_seco, int max_solo_seco, int min_solo_umido, int max_solo_umido);
void piscaLed(int led, int tempo);

int valor_umidade_1,
    valor_luz_1 = 0;

void setup(){
  //taxa de transferencia de dados
  Serial.begin(9600);
  
  //declara os pinos como entrada e saida
  //reservatório 1
  pinMode(valor_umidade_1, INPUT);
  pinMode(nivel_agua_r1_vazio, INPUT);
  pinMode(nivel_agua_r1_baixo, INPUT);
  pinMode(nivel_agua_r1_medio, INPUT);
  pinMode(nivel_agua_r1_cheio, INPUT);
  pinMode(rele_bomba_1, OUTPUT); 
  pinMode(led_vermelho_r1, OUTPUT);
  pinMode(led_amarelo_r1, OUTPUT);
  pinMode(led_verde_r1, OUTPUT);
  digitalWrite(rele_bomba_1, HIGH);

  //reservatório 2
//  pinMode(valor_umidade_2, INPUT);
//  pinMode(nivel_agua_r2_vazio, INPUT);
//  pinMode(nivel_agua_r2_baixo, INPUT);
//  pinMode(nivel_agua_r2_medio, INPUT);
//  pinMode(nivel_agua_r2_cheio, INPUT);
//  pinMode(rele_bomba_2, OUTPUT); 
//  pinMode(led_vermelho_r2, OUTPUT);
//  pinMode(led_amarelo_r2, OUTPUT);
//  pinMode(led_verde_r2, OUTPUT);
//  digitalWrite(rele_bomba_2, HIGH);

  pinMode(pino_5v_bomba, OUTPUT);
  digitalWrite(pino_5v_bomba, HIGH);
  
  //seta as configurações do módulo de tempo
  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);
  t.hour=19; 
  t.min=22;
  t.sec=0;
  t.mday=8;
  t.mon=7;
  t.year=2021;
 
  DS3231_set(t);
}
 
void loop(){

DS3231_get(&t);
  
  while(t.sec < 60){
    Serial.print("Date : ");
    Serial.print(t.mday);
    Serial.print("/");
    Serial.print(t.mon);
    Serial.print("/");
    Serial.print(t.year);
    Serial.print("\t Hour : ");
    Serial.print(t.hour);
    Serial.print(":");
    Serial.print(t.min);
    Serial.print(".");
    Serial.println(t.sec);
    t.sec++;
    delay(1000);
    if(t.sec == 60){
      t.sec = 0; 
      t.min++;
      if(t.min == 60){
        t.min = 0;
        t.hour++;
        if(t.hour == 24){
          t.hour = 0;
          t.mday++;
          if(t.mon == 1 || t.mon == 3 || t.mon == 5 || t.mon == 7 || t.mon == 8 || t.mon == 10 || t.mon == 12){
            if(t.mday > 31){
              t.mday = 1;
              t.mon++;
            }
          }
          else if(t.mon == 4 || t.mon == 6 || t.mon == 9 || t.mon == 11){
            if(t.mday > 30){
              t.mday = 1;
              t.mon++;
            }
          }
          else if(t.mon == 2){
            if(t.mday > 29){
              t.mday = 1;
              t.mon++;
            }
          }
          if(t.mon > 12){
            t.mday = 1;
            t.mon = 1;
            t.year++;
          }
        }
      }
    }
    //  int valor_luz_1 = analogRead(sensor_luz_1);
  
  //Le o valor dos sensores
  int valor_umidade_1 = analogRead(sensor_umidade_1);
//  int valor_umidade_2 = analogRead(sensor_umidade_2);

//  ligaDesligaBomba(rele_bomba_1, valor_umidade_1, 800, 1024, 0, 400);
//  ligaDesligaBomba(rele_bomba_2, valor_umidade_2, 800, 1024, 0, 400);

  //verifica quais sensores de profundidade estão ativados e liga ou desliga os leds correspondentes
  //reservatório 1
  int estado_cheio_r1 = digitalRead(nivel_agua_r1_cheio);
  int estado_medio_r1 = digitalRead(nivel_agua_r1_medio);
  int estado_baixo_r1 = digitalRead(nivel_agua_r1_baixo);
  int estado_vazio_r1 = digitalRead(nivel_agua_r1_vazio);
  ledsReservatorio(estado_cheio_r1, estado_medio_r1, estado_baixo_r1, estado_vazio_r1, led_vermelho_r1, led_amarelo_r1, led_verde_r1);
  
  //reservatório 2
//  int estado_cheio_r2 = digitalRead(nivel_agua_r2_cheio);
//  int estado_medio_r2 = digitalRead(nivel_agua_r2_medio);
//  int estado_baixo_r2 = digitalRead(nivel_agua_r2_baixo);
//  int estado_vazio_r2 = digitalRead(nivel_agua_r2_vazio);
//  ledsReservatorio(estado_cheio_r2, estado_medio_r2, estado_baixo_r2, estado_vazio_r2, led_vermelho_r2, led_amarelo_r2, led_verde_r2);
  }
}

void ligaDesligaBomba(int rele_bomba, int valor_umidade, int min_solo_seco, int max_solo_seco, int min_solo_umido, int max_solo_umido){
   if (valor_umidade > min_solo_seco && valor_umidade < max_solo_seco){
     digitalWrite(rele_bomba, HIGH);//liga a bomba
   }
 
   //Solo umido, desliga a bomba
   if (valor_umidade > min_solo_umido && valor_umidade < max_solo_umido){
   digitalWrite(rele_bomba, LOW); //desliga a bomba
   }
}

void piscaLed(int led, int tempo){
    digitalWrite(led, HIGH);
    delay(tempo);
    digitalWrite(led, LOW);
    delay(tempo);
}

void ledsReservatorio(int estado_cheio, int estado_medio, int estado_baixo, int estado_vazio, int led_vermelho, int led_amarelo, int led_verde){
  if(estado_cheio == 0){
    piscaLed(led_verde, 500); 
  }if(estado_cheio == 1){
    digitalWrite(led_verde, HIGH);
  }
  
  if(estado_medio == 0 ){
    digitalWrite(led_verde, HIGH);
  }if(estado_medio == 1){
   digitalWrite(led_verde, LOW);
  }
  
  if(estado_baixo == 0){
    digitalWrite(led_amarelo, HIGH); 
  } if(estado_baixo==1){
    digitalWrite(led_amarelo, LOW);
  }

  if(estado_vazio == 0){
    digitalWrite(led_vermelho, HIGH);
  }if(estado_vazio == 1){
    piscaLed(led_vermelho, 500);
  }
}
