#include <EEPROM.h>

int DataRec[1]; //Vector auxiliar para leer los bytes desde el celular.
float tempRecibida = 0.0; //Temperatura recibida desde el celular como temperatura límite.
float tempC; //Temperatura leída instantaneamente desde el sensor LM35.
float tempProm = 0.0; //Temperatura promedio de 50 muestras.
int sensor; //variable auxiliar
float guardarTemperaturas[2]; // Vector auxiliar para guardar los valores de TempRecibida
float tempAux = 0.0; // Variable para guardar la temperatura en la EEPROM

int pinLM35 = 0; // Variable del pin de entrada del sensor (A0) 
int enPos=15; //Pin en donde está conectado el LED
int BUZZ=14; //Pin en donde está conectado el BUZZER
int enNeg=16; //Pin en donde está conectado el Optoacoplador

int flag1 = 0;
int flag2 = 0;
String StringRec = "";
int tempR1, tempR2;


void setup() {
  DataRec[0]=0;
  DataRec[1]=0;
  Serial1.begin(38400); //Comunicación Serial con BT a 38400 baudios 
  pinMode(enPos,OUTPUT);  //Pin de salida digital para el LED (positiva)
  pinMode(BUZZ,OUTPUT);  //Pin de salida digital para el BUZZER
  pinMode(enNeg,OUTPUT);  //Pin de salida digital negativa
  EEPROM.get(0, tempAux); // Obtenemos el valor que tiene guardado la EEPROM
  guardarTemperaturas[1] = tempAux; // Almacenamos el valor de la EEPROM en el index 1 del vector 
  tempRecibida = tempAux;
}

 
void loop() {
  float temp = 0.0;  //Se inicia cada loop 
  temp = Sensar();
  //Serial1.println(temp);
  //Serial1.println(tempRecibida);
  if(Serial1.available()>0){
      StringRec = Serial1.readString();
  }
  if (StringRec == "Con"){
    flag1 = 1;
    flag2 = 1;
    Serial1.begin(38400); //Comunicación Serial con BT a 38400 baudios
    do{
      if(Serial1.available()>0){
        StringRec=Serial1.readString();
      }
      Serial1.println(tempAux);
      delay(100);
    }while(StringRec != "gotit");
  }
  while(flag1 == 1){
    temp = Sensar();
    Serial1.println(temp);//Se manda la temperatura promedio de ese loop  
    //Si hay datos del BT para recibir
    if(Serial1.available()>0){
      DataRec[0] = Serial1.read();  //Lee la parte entera del número
      DataRec[1] = Serial1.read();  //Lee la parte decimal
      if(DataRec[0]==13 && DataRec[1]==13){
        if(Serial1.available()>0){
        StringRec=Serial1.readString();
        }
        if(StringRec == "Dis"){
          flag2=0;
          flag1=0;
          //Serial1.end();
          break;
        }
      }else{
          tempR1 = DataRec[0];
          tempR2 = DataRec[1];
      }
    }
    tempRecibida = tempR1 + (tempR2*.01); //Forma un flotante con las dos partes
    if(tempRecibida == 0.00){
      tempRecibida = tempAux;
    }
    //Serial1.println(tempRecibida);
    //Serial1.println(tempAux);
    //EEPROM.get(0, tempAux);
    guardarTemperaturas[0] = tempRecibida;
    if(guardarTemperaturas[0] != guardarTemperaturas[1] && guardarTemperaturas[0]>0 && guardarTemperaturas[0] != 68.11 
    && guardarTemperaturas[0] != 12.99 && guardarTemperaturas[0] != 114.99 && guardarTemperaturas[0] != 69.05 && guardarTemperaturas[0] != 115.99){
      EEPROM.put(0, guardarTemperaturas[0]);
      guardarTemperaturas[1] = guardarTemperaturas[0];
    }
    comparar();
    delay(200);
   }
  comparar();
  // Esperamos un tiempo para repetir el loop
  delay(200);
}


//FUNCIÓN PARA COMPARAR LAS TEMPERATURAS
void comparar(){
      //Si ya llegó un límite de temperatura
      tempAux = guardarTemperaturas[1];
      if(tempAux!=0.0){
        //Revisar si la temperatura promedio es mayor al límite permitido
        if(tempProm>=tempAux+0.5){
            digitalWrite(enPos, HIGH);
            digitalWrite(BUZZ, HIGH);
            digitalWrite(enNeg, LOW);
        }else if(tempProm>=tempAux && tempProm<tempAux+0.5){
            digitalWrite(enNeg, HIGH);
            digitalWrite(enPos, HIGH);
            digitalWrite(BUZZ, HIGH); 
            delay(100);
            digitalWrite(enPos, LOW);
            digitalWrite(BUZZ, LOW); 
       
        }else if(tempProm>=tempAux-1 && tempProm<tempAux){
            digitalWrite(enNeg, HIGH);
            digitalWrite(enPos, HIGH);
            digitalWrite(BUZZ, HIGH); 
            delay(1250);
            digitalWrite(enPos, LOW);
            digitalWrite(BUZZ, LOW);
       
        }
            else{
                digitalWrite(enPos, LOW);
                digitalWrite(BUZZ, LOW);
                digitalWrite(enNeg, HIGH);
            }
       }
       else{
          digitalWrite(enPos, LOW);
          digitalWrite(BUZZ, LOW);
          digitalWrite(enNeg, LOW);
       }
}


//FUNCIÓN PARA SENSAR LA TEMPERATURA
float Sensar(){
  tempProm = 0.0; //Cada loop se inicializa el promedio en cero para sacar uno nuevo con 50 nuevas muestras.
  for (int i=0;i<50;i++){
      // Con analogRead leemos el sensor, es un valor de 0 a 1023
      //tempC = analogRead(pinLM35); esta era la linea original
      sensor = analogRead(pinLM35);
      /* Calculamos la temperatura con la fórmula, 
      o con un mapeo ya que la relación es lineal*/
      //tempC = map(tempC, 0, 1024, 0.0, 472.0); //Mapeo
      //tempC = (4.72 * sensor * 100.0)/1024.0;   //Fórmula
      tempC = (5.00 * sensor * 100.0)/1024.0;   //Fórmula
      tempProm = tempProm + tempC;  //Se suma cada iteración para sacar el promedio al final
  }
  tempProm = tempProm/50; //Se calcula el promedio después del ciclo for
  return tempProm;
}
