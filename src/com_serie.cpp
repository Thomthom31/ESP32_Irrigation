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