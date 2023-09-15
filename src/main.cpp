#include <Arduino.h>

const int Yellow = 11;
const int Red = 10;
const int Green = 9;
const int Zvuk = 3;
const int Button = 2;

const int YRG[3]  = {11, 10, 9}; //порты по цветам лампочек

const int D = 7; //кол-во выходов, подключенных к индикатору
const int R = 7; //кол-во режимов мигания(включая выкл.)

const int dashes[D] = {4, 5, 6, 7, 8, 12, 13
}; // семисегментный индикатор, порты A, B, C, D, E, F, G
const int images[R+1][D]={ //что показывать для каждого режима на семисегментном индикаторе
  {0,0,0,0,0,0,0}, //обнулить
  {0,0,0,0,0,0,1}, //-
  {0,1,1,0,0,0,0}, //1
  {1,1,0,1,1,0,1}, //2
  {1,1,1,1,0,0,1}, //3
  {0,1,1,0,0,1,1}, //4
  {1,0,1,1,0,1,1}, //5
  {1,0,1,1,1,1,1} //6
}; 


int light_mode=0; //режим мигания от 0 до 6
boolean lastButton = LOW; //для устранения дребезга контактов
boolean currentButton = LOW; // /\ тоже


void change_img(int n){ //меняет изображение на семисегментном индикаторе в зависимости от n (номера режима), n=-1 всё обнуляет
    for(int i=0; i<D; i++) digitalWrite(dashes[i], (images[n+1][i]==1)?(HIGH):(LOW));
}

boolean debounce(boolean last){ //устранение дребезга контактов кнопки (задержка на 5мс)
  boolean current = digitalRead(Button);
  if(last!=current){
    delay(5);
    current = digitalRead(Button);
  }
  return current;
}

int stop(){ // если не проверять нажатие кнопки на каждом изменении яркости светодиода, то придется, зажимая кнопку, ждать, пока цикл изменения яркости закончится 
  int flag = 0; //менять ли режим мигания
  currentButton = debounce(lastButton);
  if(lastButton == LOW && currentButton==HIGH){
    flag = 1;
  }
  lastButton = currentButton;
  return flag;
}

int range_light(int my_delay, int plus){
  int i=0, j0, j;
  for(j0=0;j0<3;j0++){
      j = YRG[j0];
      for(;i<256;i+=plus){
        analogWrite(j,i);
        if(stop()==1) return 1;
        delay(my_delay);
      }
      for(i=255;i>0;i-=plus){
        analogWrite(j,i);
        if(stop()==1) return 1;
        delay(my_delay);
      }
  }
  return 0;
}

int stay_light(){
  int i=0, j0, j;
  for(;i<256;i++) for(j0=0;j0<3;j0++){
    j = YRG[j0];
    analogWrite(j,i);
    if(stop()==1) return 1;
    delay(1);
  }
  for(i=255;i>0;i--) for(j0=0;j0<3;j0++){
    j = YRG[j0];
    analogWrite(j,i);
    if(stop()==1) return 1;
    delay(1);
  }
  return 0;
}

int stay_long_light(){
  int i=0, j0, j;
  for(;i<256;i++) for(j0=0;j0<3;j0++){
    j = YRG[j0];
    analogWrite(j,i);
    if(stop()==1) return 1;
    delay((i>50)?(4):(3));
  }
  for(i=255;i>0;i--) for(j0=0;j0<3;j0++){
    j = YRG[j0];
    analogWrite(j,i);
    if(stop()==1) return 1;
    delay((i>50)?(4):(3));
  }
  return 0;
}

int light(){
  int j;
  for(int j0=0;j0<3;j0++){
    j = YRG[j0];
    if(stop()==1) return 1;
    digitalWrite(j,HIGH);
  }
  return 0;
}

int unlight(){
  int j;
  for(int j0=0;j0<3;j0++){
    j = YRG[j0];
    if(stop()==1) return 1;
    digitalWrite(j,LOW);
  }
  return 0;
}


void setup() {
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(Yellow, OUTPUT);
  pinMode(Zvuk, OUTPUT);
  pinMode(Button, INPUT);
  for(int i=0; i<D; i++) pinMode(dashes[i], OUTPUT);
    change_img(0); 
}


void loop() {
  int push = 0;
  switch(light_mode){
    case(0): push = unlight(); break; //все огоньки не горят
    case(1): push = light(); break; //все огоньки горят
    case(2): push = stay_long_light(); break; //все вместе огоньки подолгу горят и потухают
    case(3): push = stay_light(); break; //все вместе огоньки быстро мигают
    case(4): push = range_light(1,5); break; //огоньки поочереди быстро мигают
    case(5): push = range_light(1,1); break; //огоньки поочереди средне быстро мигают
    case(6): push = range_light(5,1); break; //огоньки поочереди медленно мигают
  } 
  if(push == 1){
    if(light_mode<6) light_mode++; else light_mode=0;
    unlight();
    tone(Zvuk,2000);
    delay(200);
    noTone(Zvuk); //выдали звук частотой 2К ГЦ на 0,2 с.
    change_img(-1); 
    change_img(light_mode); //поменяли вывод на семисегментном индикаторе
  }
}
