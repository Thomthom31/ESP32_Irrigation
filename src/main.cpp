/**
 * @file main.cpp
 * @brief Programme principal.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Programme principal de passerelle MQTT IOT.
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
#include "user_function.h"
#include "global.h"

// Variables globales
extern bool isMenuVisible;
extern bool Tab_PCF8574_OUT_1[8];
extern Struct_IMP Tab_Impulsion[2];
extern int Periode;
Struct_USER Tab_Info_USER[16];

//Variables locales

// Fonctions locales
void setup();
void loop();

/**
 * @fn setup(void)
 * @brief Initialisation des composants de l'ESP32.
 * Cette fonction initialise tous les composants nécessaires au fonctionnement du programme.
 * Elle effectue l'initialisation de la communication série, du système de fichiers,
 * de la liaison i2C, des capteurs, des entrées/sorties, de la connexion WiFi, du temps,
 * du client MQTT, et d'autres composants.
 * @param void
 * @return void
 */
void setup() { 
  // Initialisation de la communication série avec une vitesse de 115200 bauds
  isMenuVisible = false;
  Serial.begin(115200);
  Serial.println();
  Serial.println("============================================================================================");
  Serial.println("============================================================================================");
  Serial.println("====================   Démarage du programme   =============================================");
  Serial.println("============================================================================================");
  Serial.println("============================================================================================");

  /// @brief Initialisation du système de fichiers
  init_file_system();

  /// @brief  Initialisation de la liaison i2C
  Wire.begin();

  /// @brief  Rechargement des valeurs sauvegardées
  RestoreValueFromFile();

  /// @brief  Recherche des information de configuration
  ConfigGPIO();
  
  ConfigReseau();

  ConfigCapteur();

  setup_impulsion1();

  /// @brief  Recherche d'éléments i2c
  test_i2c_capteur();

  /// @brief  Initialisation du capteur BMx280
  Config_BMx280();
  Read_BMx280();

  
  /// @brief  Initialisation de l'extension des entrées/sorties
  Config_PCF8574_OUT_1();

  /// @brief  Initialisation de la connexion WiFi
  setup_wifi();

  /// @brief  Initialisation de la gestion du temps
  setup_temps();

  /// @brief  Initialisation du client MQTT
  mqtt_service_setup();

  /// @brief  Initialisation du client WEB
  setup_web();
}



/**
 * @fn loop(void)
 * @brief Boucle principale du programme.
 * La boucle principale exécute les tâches principales du programme en continu.
 * Elle teste la connexion WiFi, met à jour le temps, lit les capteurs,
 * gère MQTT, publie des données, et effectue d'autres opérations périodiques
 * @param void
 * @return void
 */
void loop() {
  long currentTime = micros(); // Temps actuel en microsecondes

  test_connect_wifi();
  maj_temps();

  /// @brief MAJ des valeurs provenant des périphériques
  Read_BMx280();
  GPIO_maj();
  PCF8574_OUT_1_maj();

  /// @brief Publication MQTT
  publish_1();
  //publish_2();
  publish_s1();
  //publish_s2();

  /// @brief Execution des fonctions spécifiques utilisateur
  Fonction_Utilisateur();
 
  /// @brief Gestion des menus sur la liaison série
  menu_serie();
  if(!isMenuVisible){
    Serial.print(" Temp : ");
    Serial.print(Temperature(),1);
    Serial.print(" T max: ");
    Serial.print(Temperature_max(),1);
    Serial.print(" T min: ");
    Serial.print(Temperature_min(),1);
    Serial.print("°C, Pression : ");
    Serial.print(Pression(),1);
    Serial.print("hPa, Humidite : ");
    Serial.print(Humidite());
    Serial.print("% ** ");
    Serial.print("Turbine : ");
    Serial.print(val_impulsion1());
    Serial.print(", Turbine par sec : ");
    Serial.println(val_impulsion1_ps(),2);
  }

  /// @brief Temporisation de boucle
  while(micros()<(currentTime + Periode*1000)){
    /// @brief Vérification de l'arrivée d'un message MQTT
    loop_MQTT();
  }
  
}
