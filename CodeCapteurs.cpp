#include "mbed.h"
#include "DS1820.h"
#include "DHT.h"
#include "DavisAnemometer.h"
#include "HX711.h"

DS1820 ds1820(D2); 
Serial pc(USBTX, USBRX); 
Serial sigfox(D1,D0); 
static DavisAnemometer anemometer(A1 /* wind direction */,D4 /* wind speed */); 
HX711 Balance(D5,D6);     
AnalogIn batterie(A3);

DHT dht22(D3, DHT22);

int main(){
    
    float temp = 0; //temperature de la sonde
    int temp_form;
    int temp_sonde_form;
    int humid_form;
    int vitesse_form;
    int direction;
    int poids_form;
    int resultat = 0;
    int erreur=0;
    int trame1;
    int trame2;
    int trame3;
    int flag;
    long valeurHx;
    long valeurTare;
    float poids;
    float vitesse;
    int charge; //de 0 à 100% (batterie va de 0 a 2.1V)
    
    float humidite = 0, tempDHT22 = 0;
    anemometer.enable();
    valeurTare = Balance.getValue();    
    pc.printf("\r\n--Commencer--\r\n");
    
    while(1){
       
        erreur = dht22.readData(); 
            
        if(!(ds1820.begin() && erreur == 0)){
            pc.printf("ERREUR DS1820 ou DHT22\r\n");
        }
        ds1820.startConversion();
  
        resultat = ds1820.read(temp);
        temp_sonde_form=(int)((temp*10)+400); // température de la sonde formalisée pour etre envoyée sur Sigfox
       
        
        tempDHT22 = dht22.ReadTemperature(CELCIUS); // lire temperature par DHT22
         temp_form=(int)((tempDHT22*10)+400);// température du DHT22 formalisée pour etre envoyée sur Sigfox
        humidite= dht22.ReadHumidity(); // lire humidite par DHT22
        humid_form=(int) humidite;
        
        switch (resultat) {
                  case 0:                 // pas d'erreur -> 'temp' comporte valeur de temperature
                      //pc.printf("tempSonde = %3.1f%cC ,tempDHT22: %f%cC , humidite: %f \r\n", temp, 176,tempDHT22,176,humidite);
                     //  pc.printf("tempSonde = %d%cC ,tempDHT22: %d%cC , humidite: %d \r\n", temp_sonde_form, 176,temp_form,176,humid_form);
                     // sigfox.printf("AT$SF=%02x%02x%02x\r\n",tempDHT22,humidite,(int)temp); //envoyer le message a backend sigfox

                    break;
  
                  case 1:                 // pas de capteur present -> 'temp' n'est pas mis a jour
                      pc.printf("pas de capteur present\n\r");
                      break;
  
                  case 2:                 // erreur de CRC  -> 'temp' n'est pas mis a jour
                      pc.printf("erreur de CRC\r\n");
        }
        pc.printf("tempSonde = %3.1f%cC ,tempDHT22: %3.1f%cC , humidite: %f \r\n tempSondeForm=%d, tempDHT22form=%d\n", temp, 176,tempDHT22,176,humidite,temp_sonde_form,temp_form);
        
        
        vitesse=anemometer.readWindSpeed();
        if(anemometer.readWindDirection() <=309 && anemometer.readWindDirection() >=306){
                direction=0;   
                pc.printf("La direction est Nord\r\n numero envoye sur Sigfox: %d\r\n [speed] %.2f km/h\r\n",direction,vitesse); 
                

            }
        else if(anemometer.readWindDirection() <=328 && anemometer.readWindDirection() >=326){
                direction=1;
                pc.printf("La direction est Sud\r\n [speed] %.2f km/h\r\n",vitesse); 
               
            }
        else if(anemometer.readWindDirection() <=295 && anemometer.readWindDirection() >=291){
                direction=2;
                pc.printf("La direction est Ouest\r\n [speed] %.2f km/h\r\n",vitesse); 
                
            }
            
         else if(anemometer.readWindDirection() <=324 && anemometer.readWindDirection() >=321){
                direction=3;
                pc.printf("La direction est Est\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
         else if(anemometer.readWindDirection() <=262 && anemometer.readWindDirection() >=259){
                direction=4;
                pc.printf("La direction est Nord-Ouest\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
         else if(anemometer.readWindDirection() <=277 && anemometer.readWindDirection() >=274){
                direction=5;
                pc.printf("La direction est Sud-Ouest\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
            
         else if(anemometer.readWindDirection() <=302 && anemometer.readWindDirection() >=298){
                direction=6;
                pc.printf("La direction est Sud-Est\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
         else if(anemometer.readWindDirection() <=285 && anemometer.readWindDirection() >=283){
                direction=7;
                pc.printf("La direction est Nord-Est\r\n [speed] %.2f km/h\r\n",vitesse); 
            }
        
        valeurHx= Balance.getValue();   
        poids = 2*((double)valeurHx-(double)valeurTare)/11500;  // Conversion de la valeur de l'ADC en grammes
        poids = 10*((-0.50)*(poids/0.875)+0.02)-350; // ajuster en kg
       //poids_form= (int)((poids*10));
       
        pc.printf("weight is %4.2f \r\n",poids);
        
        
        
        //calcul du pourcentage de batterie
        charge=(int)(batterie.read()*100/2.1); //tension max a 2.1
        pc.printf("charge batterie: %d %c",charge,25);
        pc.printf("----------------------------\r\n");
        
         trame1=(trame1 & 0x0)| // trame a 0
        ((int)charge<<8)| // temperature 8 bits
        ((int)humidite<<16)| // humidite sur 8 bits
        ((int)vitesse<<24); //vitesse sur 8 bits

        
        trame2=(trame2 & 0x0)|
        (direction<<8)|((int)poids<<16);//poids sur 16bits
        
        trame3=(trame3 & 0x0)|(temp_sonde_form<<16)|(temp_form);//temperature sur 16bits
        
        
       //sigfox.printf("AT$SF=%02x%02x%02x%02x%02x\r\n",(int)tempDHT22,(int)humidite,(int)temp,(int)anemometer.readWindSpeed(),(int) poids);
        sigfox.printf("AT$SF=%08x%08x%08x\r\n",trame1,trame2,trame3);

        //humidite= dht22.ReadHumidity();
        //tempDHT22 = dht22.ReadTemperature(CELCIUS); 
       // pc.printf("tempDHT22=%3.1f /humidite= %3.1f \r\n",tempDHT22,humidite);                
        
        wait(10);
        
    }
}
