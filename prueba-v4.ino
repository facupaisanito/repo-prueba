// aca pongo algo
#include <SPI.h>
#include <SD.h>
#include <Time.h>

#define led     13
#define relay1  8  
#define relay2  9
#define relay3  10
#define relay4  11
#define LISTA   63
#define TIEMPO_MED1   20000
#define TIEMPO_MED2   20000
#define DEBUG   0
// saraza algo

#define ledON digitalWrite(led,HIGH)
#define ledOFF digitalWrite(led,LOW)

const int chipSelect = 4;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  

  pinMode(led, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  ledON;
  delay(1000);
  ledOFF;

if(DEBUG)
{
//  see if the card is present and can be initialized:
 if (!SD.begin(chipSelect)) 
      {
        blink();// Serial.println("Card failed, or not present");
        return;
      }
}
else
{
  Serial.begin(9600);
  while (!Serial) ;
  Serial.print("Initializing DEBUG...");
}
  analogReadResolution(12);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int sensor, i=0;
unsigned int presion[LISTA+1];
unsigned long t0;

void loop()
{
  unsigned long t;
  carga();
    
  graba("ID\tTu\tTm\tADC\tAvg3\tAvg5\tdP");
  t0=millis();
  do{
      graba(mide());
      t=millis();
     } while ( (t-t0) <= TIEMPO_MED1);
  descarga();
  
  ledON;
  digitalWrite(relay4, LOW); //empieza a perder
  delay(2000);
  ledOFF;

  carga();
  i=0;
  t0=millis();
  do{
      graba(mide());
      t=millis();
    } while ( (t-t0) <= TIEMPO_MED2);

  descarga();
  digitalWrite(relay4, HIGH); //empieza a perder

  while(1)
  {
    blink();
  };
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void carga()      
{                             // 2Seg de duracion de carga
  delay(1000);
  digitalWrite(relay1, LOW);  //Principal de 3 vias
  delay(200);
  digitalWrite(relay2, LOW);  //Carga el DUT
  delay(500);
  digitalWrite(relay2, HIGH);  //Deja presurizado el DUT
  delay(200);
  digitalWrite(relay1, HIGH);  //Ventea la caÃƒÂ±eria
  delay(200);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void descarga()
{
  delay(200);
  digitalWrite(relay2, LOW); //Ventea todo el DUT
  delay(200);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void blink()
{
  while(1)
  {
    delay(300); 
    ledON;
    delay(300);
    ledOFF;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
String mide()
{   long  mov_avg;
    long dpdt;
    unsigned long tm;
  String  dataString = "";

  // read three sensors and append to the string:
  dataString += String(i);      //ID
  dataString += "\t";
  tm=micros();
  dataString += String(tm);    //Time
  dataString += "\t";
  tm=millis();
  dataString += String(tm-t0); //Time
  dataString += "\t";
  unsigned int sensor = analogRead(0);
  dataString += String(sensor);

  
  if(i <= LISTA)              //La ultima medicion es presion[LISTA]
    {presion[i]=sensor;}
  else
  {
    for(int j=0;j < LISTA;j++)
      {
        presion[j]=presion[j+1];
      }
    presion[LISTA]=sensor;
  }
  

  if(i>LISTA)              // Moving average de 8 elementos y descentrado
    { 
      /*mov_avg= (presion[0]/9) + (presion[LISTA*1/7]/9) + (presion[LISTA*2/7]/9) + 
                  (presion[LISTA*1/5]/9) + (presion[LISTA*3/7]/9) + 
                  (presion[LISTA*4/7]/9) + (presion[LISTA*5/7]/9) + 
                  (presion[LISTA*6/7]/9) + (presion[LISTA]    /9);*/
/*      dataString += "\t";
      dataString += String((presion[0]));
      dataString += "\t";
      dataString += String((presion[LISTA/2]));
      dataString += "\t";
      dataString += String((presion[LISTA]));
*/
      mov_avg = (presion[0]/3) + (presion[LISTA/2]/3) + (presion[LISTA]/3);
      dataString += "\t";
      dataString += String(mov_avg);
      mov_avg = (presion[0]/5) + (presion[LISTA/4]/5) + (presion[LISTA/2]/5) + (presion[(LISTA*3)/4]/5) + (presion[LISTA]/5);
      dataString += "\t";
      dataString += String(mov_avg);
      
    }
  else
  {
    dataString += "\t";
    dataString += '0';
    dataString += "\t";
    dataString += '0';

  }

  if(i>LISTA)           // Derivada numerica
  {
    long dp_y = presion[LISTA]-presion[0];
    long dp_x = LISTA;//presion[LISTA]-presion[0];
    dpdt = dp_y / dp_x;
    dataString += "\t";
    dataString += String(dp_y);

  }
  else
  {
    dataString += "\t";
    dataString += '0';
  }

  i++;
  return dataString;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void graba(String dataString)
{
  if(DEBUG)
  {
    File  dataFile = SD.open("datalog.txt", FILE_WRITE);

    if (dataFile) 
    {
      dataFile.println(dataString);
      dataFile.close();
    }
    else
    {
      blink();
    }
  }
  else 
  {
    //Serial.println("error opening datalog.txt");
    Serial.println(dataString);
  }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////







