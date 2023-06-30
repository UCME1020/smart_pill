
#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
RTC_DS3231 rtc; //RTC로 선언
LiquidCrystal_I2C lcd(0x27,16,2);

int slot[7]={1,1,1,1,1,1,1};
int alarm[7];
int takeMedicine[7]={1,1,1,1,1,1,1};
int alarm_onoff[7];
int ampm[7]={1,1,1,1,1,1,1}; //ampm

const int piezo = 13;
const int modebtn = 10; 
const int submodebtn = 9;
const int OKbtn = 8;
const int plusbtn = 7;

byte alarmChar[] = {B00100, B01110, B01110, B01110, B11111, B00000, B00100, B00000};


void setup() {
  //if (! rtc.begin()) {
  //  Serial.println("Couldn't find RTC");
  //  while (1);
  //}
  Serial.begin(9600);// 시리얼 프린트 시작
  delay(3000); // 콘솔을 열기를 기다립니다.

  if (! rtc.begin()) { 
//(!는 논리 부정연산자로 거짓을 참으로 참을 거짓으로 바꾸어 줍니다. 즉 RTC가 시작을 하지 않는다면 
//이라는 조건입니다.)
    Serial.println("RTC를 찾을 수 없습니다.");
    while (1); //1번 반복
  }
  
  if (rtc.lostPower()) { //RTC에 전원이 없다면
    Serial.println("RTC가 전원이 없습니다. 시간을 설정합니다."); //
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //시각을 설정 날짜와 시간을 세팅합니다.
    
    // 아래와 같은 코팅을 한다면 January 27 2017 at 12:56 
   //rtc.adjust(DateTime(2023, 6, 21, 21, 7, 0));
  }
  //rtc.adjust(DateTime(2023, 6, 21, 21, 7, 0));
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  lcd.init();                      // LCD 초기화
  lcd.backlight();
  int temp;
  pinMode(modebtn, INPUT);
  pinMode(submodebtn, INPUT);
  pinMode(OKbtn, INPUT);
  pinMode(plusbtn, INPUT);
  pinMode(piezo, OUTPUT);

  lcd.createChar(1, alarmChar);
}
void printslot(int i){
  lcd.clear();
  lcd.print("slot:");
  lcd.print(i+1);
  lcd.print("->");
  lcd.print(slot[i]);
}
void checkMedicine(){
  lcd.clear();
  lcd.setCursor(0, 0);
  for(int i=0;i<7;i++){
    lcd.print(i+1);
    lcd.print(" ");
  }

  lcd.setCursor(0, 1);
  for(int i=0;i<7;i++){
    if(takeMedicine[i]==1){
      lcd.print("O ");
    }
    else{
      lcd.print("X ");
    }
  }
  delay(3000);
  lcd.clear();
}
void checkTheAlarmTime() {
  DateTime now = rtc.now();
  for(int i=0;i<7;i++){
    if(alarm_onoff[i]==1){
     if(alarm[i]/100 == now.hour() && alarm[i]%100 == now.minute()&&now.second()==0) {
        lcd.clear();
        printslot(i);
       tone(piezo, 330);
    
        int sec=0;
        while(1){
          sec++;
          if(sec==120||digitalRead(OKbtn)){
           noTone(piezo);
           lcd.clear();
           takeMedicine[i]=0;
            break;
          }
          delay(500);
        }
      }
    }
  }
}

void printalarm(int i){
  lcd.clear();
  lcd.print("slot:");
  lcd.print(i+1);
  lcd.print("->");
  if(alarm_onoff[i]==1){
    lcd.print("on");
  }else{
    lcd.print("off");
  }
}
void setampm(int ampm){
  if(ampm==1){
    lcd.print("AM ");
  }else{
    lcd.print("PM ");
  }
}
void Set_AMPM(int hour) {
  if(hour >=12) 
    lcd.print("PM ");
  else 
    lcd.print("AM ");

  lcd.print(hour%12, DEC);     //시간 출력
}
void Set_lowThanTen(int time) {
 if(time < 10) {
    lcd.print("0");
    lcd.print(time%10);
  }
  else
    lcd.print(time);
}
void clock(){ //초기화면인 현재시간을 lcd에 출력
  DateTime now=rtc.now();
  lcd.setCursor(0,0);
  lcd.print("Date  ");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.day(), DEC);
  lcd.setCursor(0,1);
  Set_AMPM(now.hour()); //am/pm+시간
  lcd.print(":");
  Set_lowThanTen(now.minute());//분
  lcd.print(":");
  Set_lowThanTen(now.second());//초
  
  lcd.setCursor(11, 0);
  switch (now.dayOfTheWeek()) {
    case 0: lcd.print("Sun"); break;
    case 1: lcd.print("Mon"); break;
    case 2: lcd.print("Tue"); break;
    case 3: lcd.print("Wed"); break;
    case 4: lcd.print("Thu"); break;
    case 5: lcd.print("Fri"); break;
    case 6: lcd.print("Sat"); break;
  }
}

void setpill(int slot[]){
  int num=0;
  lcd.clear();
  int i=0;
  printslot(i);
  delay(500);
  while(1){
    if(digitalRead(OKbtn)&&num==0){//슬롯 정하기
      num=1;
      delay(500);
      lcd.clear();
      lcd.print("slot:");
      lcd.print(i+1);
      lcd.print(" is checked!");
      delay(500);
      if(takeMedicine[i]==0){
        takeMedicine[i]=1;
      }
      printslot(i);
    }else if(digitalRead(plusbtn)&&num==0){//슬롯 고르기
      i++;
      lcd.clear();
      printslot(i);
      delay(500);
      if(i==6)i=-1;
    }else if(digitalRead(OKbtn)&&num==1){//함수종료
      delay(500);
      lcd.clear();
      lcd.print("check!");
      delay(500);
      lcd.clear();
      break;
    }else if(digitalRead(plusbtn)&&num==1){//약갯수 더하기
      slot[i]++;
      lcd.clear();
      printslot(i);
      delay(500);
    }else if(digitalRead(submodebtn)&&num==1){//약갯수 빼기 
      slot[i]--;
      if(slot[i]<0){
        lcd.clear();
        lcd.print("error!");
        delay(500);
        slot[i]++;
      }
      lcd.clear();
      printslot(i);
      delay(500);
    }
   
    
    
  }
}
void selecthour(int i,int ampm){
  alarm[i]=0;
  int hour=0;
  lcd.clear();
  lcd.print("hour:");
  setampm(ampm);
  Set_lowThanTen(hour);
  while(1){
    if(digitalRead(plusbtn)){
              
      hour++;
      if(hour==12){
        hour=0;
      }
      lcd.clear();
      lcd.print("hour:");
      setampm(ampm);          
      Set_lowThanTen(hour);
      delay(500);
    }else if(digitalRead(OKbtn)){
      if(ampm==-1){
        hour=hour+12;
      }
      hour=hour*100;
      
      alarm[i]=hour;
      lcd.print("check!");
      break;
    }
  }
}
void selectmin(int i){
  delay(500);
  int min=0;
  lcd.clear();
  lcd.print("min:");
  Set_lowThanTen(min);
  while(1){
        
    if(digitalRead(plusbtn)){
      min=min+5;
      if(min==60){
        min=0;
      }
      lcd.clear();
      lcd.print("min:");
      Set_lowThanTen(min);
      delay(500);
    }else if(digitalRead(OKbtn)){
          
      alarm[i]=alarm[i]+min;
      lcd.print("check!");
      
      delay(500);
      break;
    }
  }
}
void setAlarm(){
  int num=0;
  lcd.clear();
  int i=0;
  printalarm(i);
  lcd.setCursor(0,1);
  setampm(ampm[i]);
  Set_lowThanTen(alarm[i]/100);
  lcd.print(":");
  Set_lowThanTen(alarm[i]%100);
  lcd.setCursor(0,0);
  delay(500);
  while(1){
    if(digitalRead(modebtn)){
      lcd.clear();
      lcd.print("back");
      delay(500);
      lcd.clear();
      break;
    }
    if(digitalRead(submodebtn)){ //알람끄기// 켜기
      if(alarm_onoff[i]==0){
        alarm_onoff[i]=1;
      }else{
        alarm_onoff[i]=0;
      }
      printalarm(i);
      lcd.setCursor(0,1);
      setampm(ampm[i]);
      Set_lowThanTen(alarm[i]/100);
      lcd.print(":");
      Set_lowThanTen(alarm[i]%100);
      lcd.setCursor(0,0);
      delay(500);
    }
    if(digitalRead(OKbtn)&&num==0){//슬롯 정하기
      num=1;
      delay(500);
      lcd.clear();
      lcd.print("slot:");
      lcd.print(i+1);
      lcd.print(" is checked!");
      delay(500);
      
    }else if(digitalRead(plusbtn)&&num==0){//슬롯 고르기
      i++;
      lcd.clear();
      printalarm(i);
      lcd.setCursor(0,1);
      setampm(ampm[i]);
      Set_lowThanTen(alarm[i]/100);
      lcd.print(":");
      Set_lowThanTen(alarm[i]%100);
      lcd.setCursor(0,0);
      delay(500);
      if(i==6)i=-1;
    }else if(num==1){//시간변경
      lcd.clear();
      
      setampm(ampm[i]);
      while(1){
        if(digitalRead(OKbtn)){
          lcd.print("check!!");
          delay(700);
          break;
        }
        if(digitalRead(submodebtn)){
          ampm[i]=ampm[i]*-1;
          lcd.clear();
          setampm(ampm[i]);
          delay(500);
        }
      }
      selecthour(i,ampm[i]);
      selectmin(i);
      lcd.clear();
      setampm(ampm[i]);
      Set_lowThanTen(alarm[i]/100);
      lcd.print(":");
      Set_lowThanTen(alarm[i]%100);
      delay(2000);
      lcd.clear();
      break;

      
    }
  }
}


void loop(){
  int mode=0;
  checkTheAlarmTime();
  lcd.setCursor(0,0);
  if(digitalRead(modebtn)){
    lcd.clear();
    mode++;
    lcd.print("mode:");
    lcd.print(mode);
    delay(500);
    if(mode==3){
      mode=0;
    }
    while(1){
      if(digitalRead(modebtn)){
        lcd.clear();
        mode++;
        if(mode==4){
          mode=1;
        }
        lcd.print("mode:");
        lcd.print(mode);
        delay(500);
      }else if(digitalRead(OKbtn)){
        lcd.clear();
        break;
      }
    }
  }
  
  if(mode==0)clock();
  if(mode==1)checkMedicine();
  if(mode==2)setpill(slot);
  if(mode==3)setAlarm();
  
  for (int x = 0; x < 7; x++) {
    if( (alarm_onoff[x]) == 1) {
      lcd.setCursor(15,0);
      lcd.write(1);
    }
  }
}





