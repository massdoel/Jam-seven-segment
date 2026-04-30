//
//
//
//        0. jam menit
//        1. jam menit
//        2. jam menit
//      
//      setting
//        3. tahun (2000-2049)
//        4. bulan (1-12)
//        5. tanggal (1-31)
//        6. hari (1-7 , ahad = 1)
//        7. jam (0-23)
//        8. menit (0-59)
//        9. detik (0-59)
//        10. TimerLampuOn0_Jamstart0 (0-23)
//        11. TimerLampuOn0_Menitstart0 (0-59)
//        12. TimerLampuOn0_Jamend0 (0-23)
//        13. TimerLampuOn0_Menitend0 (0-59)
//        14. TimerLampuOn1_Jamstart1 (0-23)
//        15. TimerLampuOn1_Menitstart1 (0-59)
//        16. TimerLampuOn1_Jamend1 (0-23)
//        17. TimerLampuOn1_Menitend1 (0-59)
//
//
//        3-9 rtc ds1307
//        10-17 eeprom arduino
//
//

//----------------------------- cek 2017.10.15 10.21
//----------------------------- maju satu hari 3 detik
//----------------------------- adjust accuracy, 3 detik per hari

#include <EEPROM.h>
#include <Wire.h>
#include <DS1307.h>

int tahun;
byte nilai,i,bulan,tgl,tahunx,hari,jam,menit,detik;
byte detikx;

byte done,doneSetAcc;

//common anoda
byte seven_seg_digits[11][7] = { { 0,0,0,0,0,0,1 },  // = 0
                                 { 1,0,0,1,1,1,1 },  // = 1
                                 { 0,0,1,0,0,1,0 },  // = 2
                                 { 0,0,0,0,1,1,0 },  // = 3
                                 { 1,0,0,1,1,0,0 },  // = 4
                                 { 0,1,0,0,1,0,0 },  // = 5
                                 { 0,1,0,0,0,0,0 },  // = 6
                                 { 0,0,0,1,1,1,1 },  // = 7
                                 { 0,0,0,0,0,0,0 },  // = 8
                                 { 0,0,0,0,1,0,0 },  // = 9
                                 { 1,1,1,1,1,1,1 }   // = 10
                               };
 
#define relay0 2  //?
#define relay1 12  //?

#define PBSET A2
#define PBINC A3
#define PBDEC A6

#define dig1 10
#define dig2 11
#define dig3 A0
#define dig4 A1

#define dp 13

byte regset ;
long unsigned incset;
long unsigned udset, back0;
boolean doneset;
boolean doneudset, back0x;

byte jamON,menitON,jamOFF,menitOFF;
byte jamON1,menitON1,jamOFF1,menitOFF1;

int aStart , aStop, rtcJamMenit;
int aStart1 , aStop1;
unsigned long lastmillis,millisdetik ;
//long unsigned milsec = millis();


//----------------------------------------------------------              
void setup()
{
  
  pinMode(3, OUTPUT);//a 
  pinMode(4, OUTPUT);//b
  pinMode(5, OUTPUT);//c
  pinMode(6, OUTPUT);//d
  pinMode(7, OUTPUT);//e
  pinMode(8, OUTPUT);//f
  pinMode(9, OUTPUT);//g 
  
  pinMode(dp, OUTPUT);//dp detik
  
  pinMode(relay0, OUTPUT);//relay0 
  pinMode(relay1, OUTPUT);//relay1
  
  pinMode(dig1, OUTPUT);//dig 1 
  pinMode(dig2,OUTPUT);//dig 2 
  pinMode(dig3,OUTPUT);//dig 3 
  pinMode(dig4,OUTPUT);//dig 4
  delay(100);
  nilai=100;   
  delay(100);

  digitalWrite(dp,HIGH);
  
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);  
  delay(1000);
  
  //RTC.stop();  
  /*
  RTC.set(DS1307_SEC,0);      //set the seconds
  RTC.set(DS1307_MIN,15);     //set the minutes
  RTC.set(DS1307_HR,8);      //set the hours
  RTC.set(DS1307_DOW,5);      //set the day of the week
  RTC.set(DS1307_DATE,9);     //set the date
  RTC.set(DS1307_MTH,6);      //set the month    
  RTC.set(DS1307_YR,17);       //set the year
  */
  delay(1000);
  //RTC.start();
  lastmillis = millis();
  millisdetik = millis();
  
  regset = 0;
  incset = millis();
  udset = millis();
  doneset = LOW ;
  doneudset = LOW ;

  done = 0;
  doneSetAcc==0;
 
  jamON = EEPROM.read(1);
  delay(500);
  menitON = EEPROM.read(2);
  delay(500);
  jamOFF = EEPROM.read(3);
  delay(500);
  menitOFF = EEPROM.read(4);
  delay(500);
  
  if (jamON>23)
  EEPROM.write(1,23);
  delay(500);
  if (menitON>59)
  EEPROM.write(2,59);
  delay(500);
  if (jamOFF>23)
  EEPROM.write(3,23);
  delay(500);
  if (menitOFF>59)
  EEPROM.write(4,59);
  delay(500);

  
  jamON1 = EEPROM.read(5);
  delay(500);
  menitON1 = EEPROM.read(6);
  delay(500);
  jamOFF1 = EEPROM.read(7);
  delay(500);
  menitOFF1 = EEPROM.read(8);
  delay(500);
  
  if (jamON1>23)
  EEPROM.write(5,23);
  delay(500);
  if (menitON1>59)
  EEPROM.write(6,59);
  delay(500);
  if (jamOFF1>23)
  EEPROM.write(7,23);
  delay(500);
  if (menitOFF1>59)
  EEPROM.write(8,59);
  delay(500);

  delay(1000);
  
  for(int su=0;su<9;su++)
  {  
    if(digitalRead(dp)== LOW)
    {
      digitalWrite(dp,HIGH);
    }
    else
    {
      digitalWrite(dp,LOW);
    }
    delay(1000);
  }

}

void sevenSegWrite(byte segment) {
  byte pin = 3;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    digitalWrite(pin, seven_seg_digits[segment][segCount]);
    ++pin;
  }
}


//-------------------------------------------------------------
void loop()
{  
    if(millis()>lastmillis)
    {
    
    jam=RTC.get(DS1307_HR,true); //
    menit=RTC.get(DS1307_MIN,false);//
    detik=RTC.get(DS1307_SEC,false);//
    
    hari=RTC.get(DS1307_DOW,false); //
    //delay(5);
    tgl=RTC.get(DS1307_DATE,false); //
    bulan=RTC.get(DS1307_MTH,false); //
    tahun=RTC.get(DS1307_YR,false); //  tahun int 2000, tahunx byte 00
    lastmillis = millis()+1000;
    }

//reset detik , adjust accuracy, 3 detik per hari
        if(jam == 0 && menit==0 && detik>2 && doneSetAcc==0)
      { 
        RTC.stop();  
        RTC.set(DS1307_SEC,0); 
        delay(1000);
        RTC.start(); 
        doneSetAcc = 1;
      }
      else
        {if (menit > 10 && doneSetAcc > 0)
        {doneSetAcc = 0;}
        }
//-------------------------------------------------------------


    //------------buzzer jam dan menit -------------------
        if(menit!=00 && detik<5 && done==0)
      { 
        digitalWrite(12,HIGH);
        delay(50);
        digitalWrite(12,LOW);
        done=1;
      }
    if(menit==00 && detik<5 && done==0)
      {
        digitalWrite(12,HIGH);
        delay(100);
        digitalWrite(12,LOW);
        delay(50);
        digitalWrite(12,HIGH);
        delay(100);
        digitalWrite(12,LOW);
        done=1;
      }    

    if (detik>5)
    done=0;
    //------------------------------------------------------

    
  
  if(doneudset == HIGH && millis() > udset)
  {doneudset = LOW;}
    
  if(doneset == HIGH && millis() > incset)
  {doneset = LOW;} 
  
  if(analogRead(PBSET) < 500 && doneset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    incset = millis()+1000;
    doneset = HIGH;
    regset = regset + 1;   
               // 3tahun, 4bulan, 5tanggal 
               // 6hari, 
               // 7jam, 8menit, 9detik, 
               // 10 jamON---------eeprom1     
               // 11 menitON-------eeprom2 
               // 12 jamOFF--------eeprom3     
               // 13 menitOFF------eeprom4
               // 14 jamON1--------eeprom5     
               // 15 menitON1------eeprom6 
               // 16 jamOFF1-------eeprom7     
               // 17 menitOFF1-----eeprom8
               
    if(regset>17)
      {regset=0;}       
  }

//------
  switch (regset) {
    case 0:
      normalshow();
      break;   
    case 1:
      normalshow();
      break;
    case 2:
      normalshow();
      break;      
    case 3:
      S_tahun();
      break;
    case 4:
      S_bulan();
      break;
    case 5:
      S_tanggal();
      break;
    case 6:
      S_hari();
      break;
    case 7:
      S_jam();
      break;
    case 8:
      S_menit();
      break;
    case 9:
      S_detik();
      break;
    case 10:
      S_jamON();
      break;
    case 11:
      S_menitON();
      break;
    case 12:
      S_jamOFF();
      break;
    case 13:
      S_menitOFF();
      break;
    case 14:
      S_jamON1();
      break;
    case 15:
      S_menitON1();
      break;
    case 16:
      S_jamOFF1();
      break;
    case 17:
      S_menitOFF1();
      break;      
    }



//timer0
  aStart = jamON + (menitON * 60);
  aStop = jamOFF + (menitOFF * 60);
  rtcJamMenit = jam + (menit * 60);
  
  if ((rtcJamMenit > aStart) && (rtcJamMenit < aStop))
  {digitalWrite(relay0,HIGH);}  //cek output relay
  else
  {digitalWrite(relay0,LOW);}
//timer0

//timer1
  aStart1 = jamON1 + (menitON1 * 60);
  aStop1 = jamOFF1 + (menitOFF1 * 60);
  
  
  if ((rtcJamMenit > aStart1) && (rtcJamMenit < aStop1))
  {digitalWrite(relay1,HIGH);}  //cek output relay
  else
  {digitalWrite(relay1,LOW);}
//timer1
   
  if(back0x == LOW && millis() > back0)
  {
  back0 = millis()+10000;
  regset = 0;
  back0x = HIGH;
  }  

if(regset<=2)
{
  if (millis() > millisdetik)
  {
  //-------------------- kedip

    if(digitalRead(dp) == HIGH)
      {digitalWrite(dp,LOW);
      }
      else
      {
      digitalWrite(dp,HIGH);
      }
    //-------------------- kedip
    millisdetik = millis()+500;
  }
}
  else
{
  digitalWrite(dp,HIGH); 
}
  
  
}  // end loop

//--------------------------

void normalshow()
{
  //-------------- normal show  
  sevenSegWrite(10);
  sevenSegWrite(10);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(10);
  sevenSegWrite(menit%10);
  delay(3);

  sevenSegWrite(10);
  sevenSegWrite(10);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(10);
  sevenSegWrite(menit/10);
  delay(3);

  sevenSegWrite(10);
  sevenSegWrite(10);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(10);
  sevenSegWrite(jam%10);
  delay(3);

  sevenSegWrite(10);
  sevenSegWrite(10);
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(10);
  sevenSegWrite(jam/10);
  delay(3);
}



void S_tahun()
{
//-------------- regset = 3  setting tahun
  tahunx = tahun - 2000;   
  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    tahunx++;
    if(tahunx >49)
      {tahunx = 1;} 
    RTC.stop();     
    RTC.set(DS1307_YR,tahunx);       //set the year 
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if (tahunx > 0)
    {tahunx--;}
    else
    {tahunx = 49;} 
    RTC.stop();     
    RTC.set(DS1307_YR,tahunx);       //set the year 
    RTC.start();   
  }  
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  //sevenSegWrite(0);
  //delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(3);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(tahunx/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(tahunx%10);
  delay(3); 
}

void S_bulan()
{
//-------------- regset = 4  setting bulan
   
  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    bulan++;
    if(bulan >12)
      {bulan = 1;} 
    RTC.stop();     
    RTC.set(DS1307_MTH,bulan);       //set the month 
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    bulan--;
    if(bulan <1)
      {bulan = 12;} 
    RTC.stop();     
    RTC.set(DS1307_MTH,bulan);       //set the month 
    RTC.start();   
  } 
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  //sevenSegWrite(0);
  //delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(4);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(bulan/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(bulan%10);
  delay(3);  
}

void S_tanggal()
{
//-------------- regset = 5  setting tanggal
   
  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    tgl++;
    if(tgl >31)
      {tgl = 1;} 
    RTC.stop();     
    RTC.set(DS1307_DATE,tgl);       //set the date 
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    tgl--;
    if(tgl <1)
      {tgl = 31;} 
    RTC.stop();     
    RTC.set(DS1307_DATE,tgl);       //set the date 
    RTC.start();   
  }  
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(0);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(5);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(tgl/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(tgl%10);
  delay(3);  
}


void S_hari()
{
//-------------- regset = 6  setting hari
   
  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    hari++;
    if(hari >7)
      {hari = 1;} 
    RTC.stop();     
    RTC.set(DS1307_DOW,hari);       //set the day of week  
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    hari--;
    if(hari <1)
      {hari = 7;} 
    RTC.stop();     
    RTC.set(DS1307_DOW,hari);       //set the day of week 
    RTC.start();   
  }  
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(0);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(6);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(hari/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(hari%10);
  delay(3);  
}


void S_jam()
{
//-------------- regset = 7  setting jam

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    jam++;
    if(jam >23)
      {jam = 0;} 
    RTC.stop();     
    RTC.set(DS1307_HR,jam);       //set the hour 
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(jam > 0)
    {jam--;}
    else   
    {jam = 23;} 
    RTC.stop();     
    RTC.set(DS1307_HR,jam);       //set the hour 
    RTC.start();   
  } 
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(0);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(7);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(jam/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(jam%10);
  delay(3);  
}


void S_menit()
{
//-------------- regset = 8  setting menit

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    menit++;
    if(menit >59)
      {menit = 0;} 
    RTC.stop();     
    RTC.set(DS1307_MIN,menit);       //set the minute 
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(menit > 0)
    {menit--;}
    else    
    {menit = 59;} 
    RTC.stop();     
    RTC.set(DS1307_MIN,menit);       //set the minute 
    RTC.start();   
  } 
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(0);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(8);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(menit/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(menit%10);
  delay(3);
  detikx = detik;  
}


void S_detik()
{
//-------------- regset = 9  setting detik
  
  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    detikx++;
    if(detikx >59)
      {detikx = 0;} 
    RTC.stop();     
    RTC.set(DS1307_SEC,detikx);       //set the second 
    RTC.start();   
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(detikx > 0 )
    {detikx--;}
    else    
    {detikx = 59;} 
    
    RTC.stop();     
    RTC.set(DS1307_SEC,detikx);       //set the second 
    RTC.start();   
  }   
  
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(0);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(9);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(detikx/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(detikx%10);
  delay(3);  
}

//timer0
void S_jamON()
{
//-------------- regset = 10  setting jamON

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    jamON++;
    if(jamON >23)
      {jamON = 0;} 
    EEPROM.write(1,jamON);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(jamON > 0)
    {jamON--;}
    else   
    {jamON = 23;} 
    EEPROM.write(1,jamON);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(0);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(jamON/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(jamON%10);
  delay(3);  
}


void S_menitON()
{
//-------------- regset = 11  setting menitON

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    menitON++;
    if(menitON >59)
      {menitON = 0;} 
    EEPROM.write(2,menitON);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(menitON > 0)
    {menitON--;}
    else    
    {menitON = 59;} 
    EEPROM.write(2,menitON);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(menitON/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(menitON%10);
  delay(3);  
}

void S_jamOFF()
{
//-------------- regset = 12  setting jamOFF

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    jamOFF++;
    if(jamOFF >23)
      {jamOFF = 0;} 
    EEPROM.write(3,jamOFF);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(jamOFF > 0)
    {jamOFF--;}
    else   
    {jamOFF = 23;} 
    EEPROM.write(3,jamOFF);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(2);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(jamOFF/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(jamOFF%10);
  delay(3);  
}


void S_menitOFF()
{
//-------------- regset = 13  setting menitOFF

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    menitOFF++;
    if(menitOFF >23)
      {menitOFF = 0;} 
    EEPROM.write(4,menitOFF);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(menitOFF > 0)
    {menitOFF--;}
    else    
    {menitOFF = 59;} 
    EEPROM.write(4,menitOFF);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(3);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(menitOFF/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(menitOFF%10);
  delay(3);  
}



//timer1
void S_jamON1()
{
//-------------- regset = 14  setting jamON1

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    jamON1++;
    if(jamON1 >23)
      {jamON1 = 0;} 
    EEPROM.write(5,jamON1);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(jamON1 > 0)
    {jamON1--;}
    else   
    {jamON1 = 23;} 
    EEPROM.write(5,jamON1);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(4);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(jamON1/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(jamON1%10);
  delay(3);  
}


void S_menitON1()
{
//-------------- regset = 15  setting menitON1

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    menitON1++;
    if(menitON1 >59)
      {menitON1 = 0;} 
    EEPROM.write(6,menitON1);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(menitON1 > 0)
    {menitON1--;}
    else    
    {menitON1 = 59;} 
    EEPROM.write(6,menitON1);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(5);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(menitON1/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(menitON1%10);
  delay(3);  
}

void S_jamOFF1()
{
//-------------- regset = 16  setting jamOFF1

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    jamOFF1++;
    if(jamOFF1 >23)
      {jamOFF1 = 0;} 
    EEPROM.write(7,jamOFF1);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(jamOFF1 > 0)
    {jamOFF1--;}
    else   
    {jamOFF1 = 23;} 
    EEPROM.write(7,jamOFF1);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(6);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(jamOFF1/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(jamOFF1%10);
  delay(3);  
}


void S_menitOFF1()
{
//-------------- regset = 17  setting menitOFF1

  if(analogRead(PBINC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    menitOFF1++;
    if(menitOFF1 >23)
      {menitOFF1 = 0;} 
    EEPROM.write(8,menitOFF1);  
  }
  if(analogRead(PBDEC) < 500 && doneudset == LOW)
  {
    back0x = LOW;
    back0 = millis()+15000;
    udset = millis()+1000;
    doneudset = HIGH;
    if(menitOFF1 > 0)
    {menitOFF1--;}
    else    
    {menitOFF1 = 59;} 
    EEPROM.write(8,menitOFF1);  
  }   
  
  digitalWrite(dig1,LOW);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(1);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,LOW);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(7);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,LOW);
  digitalWrite(dig4,HIGH);
  sevenSegWrite(menitOFF1/10);
  delay(3);
  digitalWrite(dig1,HIGH);
  digitalWrite(dig2,HIGH);
  digitalWrite(dig3,HIGH);
  digitalWrite(dig4,LOW);
  sevenSegWrite(menitOFF1%10);
  delay(3);  
}



