/**
 * @file com_serie.cpp
 * @brief Fonction de la liaison série.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions de la laison série pour la communication avec un terminal
 *
 */

#include <Wire.h>
#include <PubSubClient.h>
#include <PCF8574.h>
#include "capteurs.h"
#include "Fonctions_MQTT.h"
#include "reseau_serveur.h"
#include "com_serie.h"
#include "File_System.h"
#include "global.h"
#include "GPIO.h"



/// @var isMenuVisible
/// @brief Permet de basculer du mode Menu au mode Défilement
bool isMenuVisible=true;
 
void affiche_menu(void)
{
      Serial.println("");
      Serial.println("**************");
      Serial.println("* Menu ESP32 *");
      Serial.println("**************");
      Serial.println("");
      Serial.println("1. Flux de valeur");
      Serial.println("2. Modification du fichier de configuration réseau");
      Serial.println("3. Modification du fichier de configuration MQTT");
      Serial.println("4. Modification du fichier de configuration générale");
      Serial.println("5. Lecture des données stockées");
      Serial.println("6. Effacer les données stockées");
      Serial.println("7. Effacer les valeurs saugardées");
}

void menu_serie(void)
{
    int val1=0, val2=0, val3=0, val4=0;
  if (Serial.available() > 0) {
    char inputChar = Serial.read();
    
    if (inputChar == 'm' && !isMenuVisible) {
      // Effacer le terminal série
      Serial.flush();
      for (int i = 0; i < 50; i++) {
        Serial.println();
      }

      isMenuVisible = true;
    }
    
    if (isMenuVisible) {
      Serial.println("");  
      switch (inputChar) {
        case '1':
          Serial.println("Option 1 sélectionnée : Flux de valeur");
          // Mettez le code de votre option 1 ici
          isMenuVisible = false;
          break;
        case '2':
          Serial.println("Option 2 sélectionnée : Modification du fichier de configuration réseau");
          modifFileToSerial("/wifi.json");
          break;
        case '3':
          Serial.println("Option 3 sélectionnée : Modification du fichier de configuration MQTT");
          modifFileToSerial("/MQTT.json");
          break;
        case '4':
          Serial.println("Option 4 sélectionnée : Modification du fichier de configuration générale");
          modifFileToSerial("/config.json");
          break;
        case '5':
          Serial.println("Option 5 sélectionnée : Lecture du fichier météo");
          readFileToSerial("/data.csv");
          break;          
        case '6':
          Serial.println("Option 6 sélectionnée : Effacer les données stockées");
          delFileToSerial("/data.csv");
          // Mettez le code de votre option 3 ici
          break;
        case '7':
          Serial.println("Option 7 sélectionnée : Effacer les valeurs saugardées");
          maj_impulsion1(0);
            
          for(int n=1;n<7;n++){
            Tab_Info_USER[n].Val_INT=0;
            Tab_Info_USER[n].Val_LONG=0;
            Tab_Info_USER[n].Val_FLOAT=0;
            }

          // Mettez le code de votre option 3 ici
          break;

        default:
          if (isMenuVisible) {

          }
          break;
      }
      affiche_menu();
      Serial.println("Sélectionnez une option (1/2/3/4/5/6/7) :");
    }
  }
}




String incomingData = "";  // Variable pour stocker les données reçues
boolean isProcessing = false;  // Indicateur pour savoir si on est en train de traiter une trame
char device;
int deviceNumber;
int value;

void print_ack(String rep, int devicenumber, int value){
  Serial.print(rep); // Réponse de succès
  Serial.print(devicenumber); 
  Serial.print(" "); 
  Serial.println(value);  
}

void print_ack_f(String rep, int devicenumber, float value){
  Serial.print(rep); // Réponse de succès
  Serial.print(devicenumber); 
  Serial.print(" "); 
  Serial.println(value);  
}

void serialEvent() {

  if(isMenuVisible) {
    menu_serie();
    return;
    }

  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();

    if (inChar == 'm') {
      menu_serie();  // Appel de la fonction de menu
      return;
      
    } else if (isProcessing) {
      incomingData += inChar;
      if ((inChar == '!')||(inChar == '\n')) {
        // Traitement de la trame complète
        isProcessing = false;
        // Extraire les informations de la trame 
        device = incomingData[1];
        deviceNumber = incomingData.substring(2, 4).toInt();
        value = incomingData.substring(5).toInt();

        // Traitement selon le type de périphérique
        switch (device) {
          case 'S':
            // Commande pour un servo
            if(ServoMoteur_OUT(deviceNumber, value)==1){
              Serial.print(deviceNumber); 
              print_ack("#ACK S",deviceNumber,value);          
              }
            else{
              print_ack("#ERR S",deviceNumber,value);
              }
            break;

          case 'W':
            // Commande pour un PWM
            if(PWM_OUT(deviceNumber, value)==1){
              print_ack("#ACK W",deviceNumber,value);             
              }
            else{
              print_ack("#ERR W",deviceNumber,value); 
            }
            break;   

          case 'O':
            // Commande pour une sortie GPIO
            if(GPIO_OUT(deviceNumber, value)==1){
              print_ack("#ACK O",deviceNumber,value);             
              }
            else{
              print_ack("#ERR O",deviceNumber,value); 
            }
            break;  

          case 'E':
            // Commande pour une sortie PCF8574_OUT_1
            if(PCF8574_OUT_1_out(deviceNumber, value)==1){
              print_ack("#ACK E",deviceNumber,value);              
              }
            else{
              print_ack("#ERR E",deviceNumber,value);
            }
            break; 

          case 'I':
            // Commande pour une entreé numérique GPIO
            print_ack("#ACK I",deviceNumber,GPIO_IN(deviceNumber,0));
            break;  

          case 'A':
            // Commande pour une entreé numérique GPIO
            print_ack_f("#ACK A",deviceNumber,GPIO_ANA(deviceNumber,0));
            break;  

          case 'T':
            // Commande pour une entreé numérique GPIO
            print_ack_f("#ACK T",deviceNumber,Temperature());
            break;  

          case 'P':
            // Commande pour une entreé numérique GPIO
            print_ack_f("#ACK P",deviceNumber,Pression());
            break;  

          case 'H':
            // Commande pour une entreé numérique GPIO
            print_ack_f("#ACK H",deviceNumber,Humidite());
            break;  

          case 'R':
            // Commande pour une entreé numérique GPIO
            print_ack_f("#ACK R",deviceNumber,Point_rosee());
            break;  


          default:
            Serial.println("#ACK 000!!!"); // Réponse d'échec
        }

        incomingData = "";
      }
    } else if (inChar == '#') {
      isProcessing = true;
      incomingData = "#";
    }
  }
}