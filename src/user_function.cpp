/**
 * @file user_function.cpp
 * @author Thomas GAUTIER
 * @brief Fichier des fonctions utilisateur
 * @version 0.1
 * @date 2023-11-11
 * 
 * @copyright Copyright (c) 2023
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

/**
 * @fn Fonction_Utilisateur(void)
 * @brief Fonction utilisateur lancée à chaque boucle.
 * Cette fonction va cummuler les impulsions en fonction des bits qui sont actifs sur l'extention OUT.
 * @param void
 * @return void
 */
void Fonction_Utilisateur(void){
    // Partie du programme Spécifique utilisateur
  long delta_turbine=Tab_Impulsion[0].Valeur_Cumul-Tab_Impulsion[0].Valeur_Cumul_back;

  for(int i=0; i<8; i++){
    if(Tab_PCF8574_OUT_1[i]){
      Tab_Info_USER[i].Val_LONG += delta_turbine;
      Tab_Info_USER[i].Val_FLOAT = val_impulsion1_ps();
    }
  }

 Tab_Impulsion[0].Valeur_Cumul_back=Tab_Impulsion[0].Valeur_Cumul;
 return;
}

/**
 * @fn Fonction_Utilisateur_minute(void)
 * @brief Fonction executé automatiquement chaque nouvelle minute si NTP est activé
 * 
 */
void Fonction_Utilisateur_minute(void){
    return;
}

/**
 * @fn Fonction_Utilisateur_heure(void)
 * @brief Fonction executé automatiquement chaque nouvelle heure si NTP est activé
 * 
 */
void Fonction_Utilisateur_heure(void){
    return;
}

/**
 * @fn Fonction_Utilisateur_jour(void)
 * @brief Fonction executé automatiquement chaque nouvelle jour si NTP est activé
 * 
 */
void Fonction_Utilisateur_jour(void){
    return;
}