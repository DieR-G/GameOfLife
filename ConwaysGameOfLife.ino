#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <Arduino.h>
#include <U8g2lib.h>

const int cs_pin = 4;
const int dc_pin = 3;
const int rst_pin = 5;
const int play_pin = 7;

U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ cs_pin, /* dc=*/ dc_pin, /* reset=*/ rst_pin);     // Nokia 5110 Display

const byte filas PROGMEM = 28;
const byte columnas PROGMEM = 16;

bool inicial[filas][columnas] = {false};
bool (*pInicial)[filas][columnas] = &inicial;

bool siguiente[filas][columnas] = {false};
bool (*pSiguiente)[filas][columnas] = &siguiente;

volatile bool jugar = false;

int contar_vecinos(int x, int y){
  int valores_x[] = {-1,1,0,0,-1,-1,1,1};
  int valores_y[] = {0,0,1,-1,-1,1,1,-1};
  int total = 0;
  int rr, cc;
  for(int i = 0; i < 8; i++){
    rr = x + valores_x[i];
    cc = y + valores_y[i];
    if( (rr < 0) || (cc < 0) )
      continue;
    if( (rr >= filas) || (cc >= columnas) )
      continue;
    if((*pInicial)[rr][cc]){
      total++;
    }  
  }
  
  return total;
}

void calcular_siguiente(){
  for(int i = 0; i < 28; i++){
    for(int j = 0; j < 16; j++){
      (*pSiguiente)[i][j] = false;
    }
  }
  int vecinos;
  for(int i = 0; i < 28; i++){
    for(int j = 0; j < 16; j++){
      vecinos = contar_vecinos(i,j);
      if( (*pInicial)[i][j] && ((vecinos < 2) || (vecinos > 3)) )
        (*pSiguiente)[i][j] = false;
      else if( !((*pInicial)[i][j]) && (vecinos == 3) )
        (*pSiguiente)[i][j] = true;
      else
        (*pSiguiente)[i][j] = (*pInicial)[i][j];
    }
  }
  bool (*t)[filas][columnas] = pSiguiente;
  pSiguiente = pInicial;
  pInicial = t;
}

void iniciar_juego(){
  jugar = !jugar;
}

void setup() {
  u8g2.begin();
  u8g2.setDrawColor(1);
  (*pInicial)[12][6] = true;
  (*pInicial)[13][7] = true;
  (*pInicial)[14][7] = true;
  (*pInicial)[13][6] = true;
  (*pInicial)[13][8] = true;

  (*pInicial)[2][2] = true;
  (*pInicial)[2][3] = true;
  (*pInicial)[2][4] = true;

  (*pInicial)[12][0] = true;
  (*pInicial)[13][1] = true;
  (*pInicial)[14][1] = true;
  (*pInicial)[13][0] = true;
  (*pInicial)[13][2] = true;
  
  attachInterrupt(digitalPinToInterrupt(play_pin),iniciar_juego,LOW);
}

void loop() {
  u8g2.firstPage();
  do{
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
          if((*pInicial)[i][j])
            u8g2.drawBox(i*3,j*3,3,3);
          }
        }
    if(jugar){
      calcular_siguiente();
    }
    delay(300);
  }while(u8g2.nextPage());
}
