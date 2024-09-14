/**
 * @file reseau_serveur.cpp
 * @brief Fonction de serices réseau.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions des services réseaux : Wifi, NTP, WEB
 *
 */

#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include <eztime.h>
#include "Fonctions_MQTT.h"
#include "capteurs.h"
#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "File_System.h"
#include "string.h"
#include "global.h"
#include "user_function.h"


/**
 * @var bool EnableWIFI
 * @brief Indique si le module WiFi est activé ou non.
 */
bool EnableWIFI=false;

/**
 * @var bool EnableNTP
 * @brief Indique si le module NTP (Network Time Protocol) est activé ou non.
 */
bool EnableNTP=false;

/**
 * @var bool EnableMQTT
 * @brief Indique si le module MQTT est activé ou non.
 */
bool EnableMQTT=false;

/**
 * @var bool EnableWEB
 * @brief Indique si le module serveur web est activé ou non.
 */
bool EnableWEB=false;

/**
 * @var AsyncWebServer server
 * @brief Instance du serveur web asynchrone.
 */
AsyncWebServer server(80);

/**
 * @var Timezone timeZone
 * @brief Objet pour la gestion du fuseau horaire.
 */
Timezone timeZone;

/**
 * @var time_t previousHour
 * @brief Heure précédente.
 */
time_t previousHour = 0;

/**
 * @var time_t previousMinute
 * @brief Minute précédente.
 */
time_t previousMinute = 0;

/**
 * @var time_t currentTime
 * @brief Temps actuel.
 */
time_t currentTime ;

/**
 * @var String currentTimeStr
 * @brief Chaîne de caractères représentant l'heure actuelle.
 */
String currentTimeStr ;

/**
 * @var String currentDayStr
 * @brief Chaîne de caractères représentant la date actuelle.
 */
String currentDayStr ;

/**
 * @var const char *configFilePath
 * @brief Chemin du fichier de configuration.
 */
const char *configFilePath = "/config.json";

/**
 * @fn void ConfigReseau()
 * @brief Lecture du fichier de configuration pour la partie réseau.
 * @return void
 */
void ConfigReseau (void){
  String config_json;

  Serial.println("");
  Serial.println("Lecture du fichier de configuration partie RESEAU :");

  Serial.print("    WIFI = ");
  if(getStringValueFromJsonFile("/config.json", "RESEAU", "WIFI", "Enable")=="true"){EnableWIFI=true;};
  Serial.println(EnableWIFI);

  Serial.print("    NTP = ");
  if(getStringValueFromJsonFile("/config.json", "RESEAU", "NTP", "Enable")=="true"){EnableNTP=true;};
  Serial.println(EnableNTP);

  Serial.print("    MQTT = ");
  if(getStringValueFromJsonFile("/config.json", "RESEAU", "MQTT", "Enable")=="true"){EnableMQTT=true;};
  Serial.println(EnableMQTT); 

  Serial.print("    WEB = ");
  if(getStringValueFromJsonFile("/config.json", "RESEAU", "WEB", "Enable")=="true"){EnableWEB=true;};
  Serial.println(EnableWEB); 

}


//*************************************************************************************************************
//************************************************** WIFI *****************************************************
//*************************************************************************************************************

/**
 * @fn void setup_wifi()
 * @brief Configuration de la connexion WiFi.
 * @return void
 */
void setup_wifi() {
    if(!EnableWIFI){return;}
    delay(10);
    // Connexion au réseau WiFi
    Serial.println();
    Serial.println(F("============================================================================================"));
    Serial.println("Connexion au réseau WiFi ");
    Serial.println(F("============================================================================================"));
    // Charger le fichier de configuration JSON
    String ssid;
    String password;
    String ssid_json;
    int i;
    boolean connecte = false;
    
    while (!connecte) {
      for(i=1;i<4&& !connecte;i++){
        ssid_json = String(("WIFI_" + std::to_string(i)).c_str());
        ssid=getStringValueFromJsonFile("/wifi.json", ssid_json, "Configuration", "SSID");
        password=getStringValueFromJsonFile("/wifi.json", ssid_json, "Configuration","password");
        

        WiFi.begin(ssid.c_str(), password.c_str());

        Serial.println("Tentative de connexion à WiFi " + ssid + "( " + password + ")");

        for (int i = 0; i < 20 && !connecte; i++) {
          if (WiFi.status() == WL_CONNECTED) {
            connecte=true;
          }
          delay(1000);
          Serial.print(".");
        }
      delay(1000);  
      Serial.println("");
      }
    }

  Serial.println("> Wifi initialisé");
  Serial.println("Connexion WiFi établie");
  Serial.println("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

/**
 * @fn void test_connect_wifi()
 * @brief Teste et configure la connexion WiFi si elle n'est pas établie.
 * @return void
 */
void test_connect_wifi(void){
  if(!EnableWIFI){return;}
  if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
  }
}

//*************************************************************************************************************
//************************************************** Temps NTP ************************************************
//*************************************************************************************************************

/**
 * @fn void daylyRoutine()
 * @brief Routine quotidienne.
 * @return void
 */
void daylyRoutine() {
  // Votre code pour la routine d'une jounée
  saveDataToFile(Temperature_max(), Temperature_min(), Pression(), val_impulsion1());
  reset_min_max();
  Fonction_Utilisateur_jour();
  waitForSync();
}

/**
 * @fn void hourlyRoutine()
 * @brief Routine horaire.
 * @return void
 */
void hourlyRoutine() {
  // Votre code pour la routine d'une heure
  SaveValueToFile();
  Fonction_Utilisateur_heure();
  waitForSync();
}

/**
 * @fn void minutlyRoutine()
 * @brief Routine minute.
 * @return void
 */
void minutlyRoutine() {
  // Votre code pour la routine d'une heure
  Fonction_Utilisateur_minute();
  waitForSync();
}

/**
 * @fn void setup_temps()
 * @brief Initialisation de l'horloge NTP.
 * @return void
 */
void setup_temps(){
  if(!EnableNTP){return;}
  Serial.println();
  Serial.println(F("============================================================================================"));
  Serial.println("Initialisation de l'horloge NTP");
  Serial.println(F("============================================================================================"));
  // Initialisation d'eZTime
  Serial.print(".");
  waitForSync();
  Serial.print(".");
  // Configuration du fuseau horaire
  timeZone.setLocation("Europe/Paris"); 
  Serial.print(".");
  // Obtention de l'heure actuelle
  currentTime = timeZone.now();
  currentTimeStr = String(hour(currentTime)) + ":" + String(minute(currentTime)) + ":" + String(second(currentTime));
  Serial.println(".");
  Serial.print("> Heure initialisée, il est : ");
  Serial.println(currentTimeStr);
}

/**
 * @fn void maj_temps()
 * @brief Mise à jour de l'heure.
 * @return void
 */
void maj_temps(){
  if(!EnableNTP){return;}
  // Obtention de l'heure actuelle
  currentTime = timeZone.now();
  currentTimeStr = String(hour(currentTime)) + ":" + String(minute(currentTime)) + ":" + String(second(currentTime));
  currentDayStr = String(day(currentTime)) + "/" + String(month(currentTime)) + "/" + String(year(currentTime));
  
  // Vérification si une heure s'est écoulée
  if (day(currentTime) != day(previousHour)) {
    previousHour = currentTime;
    daylyRoutine();
  }

  // Vérification si une heure s'est écoulée
  if (hour(currentTime) != hour(previousHour)) {
    previousHour = currentTime;
    hourlyRoutine();
  }

    // Vérification si une heure s'est écoulée
  if (minute(currentTime) != minute(previousMinute)) {
    previousMinute = currentTime;
    minutlyRoutine();
  }
}



//*************************************************************************************************************
//************************************************** WEB *****************************************************
//*************************************************************************************************************

/**
 * @fn void setup_web()
 * @brief Initialisation du serveur web.
 * @return void
 */
void setup_web(){
  if(!EnableWEB){return;}
    Serial.println();
    Serial.println(F("============================================================================================"));
    Serial.println("Initialisation du serveur web");
    Serial.println(F("============================================================================================")); 
    // Définissez les routes du serveur web
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><body>";
    html += "<h1>Données du capteur</h1>";
    html += "<p>Température: " + String(Temperature()) + " &deg;C</p>";
    html += "<p>Pression: " + String(Pression() / 100.0F) + " hPa</p>";
    html += "<p>Humidité: " + String(Humidite()) + " %</p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Démarrez le serveur web
  server.begin();
  Serial.println("> Serveur web initialisé");
}




