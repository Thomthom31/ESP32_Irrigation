/**
 * @file Fonction_MQTT.cpp
 * @brief Fonction de l'interface MQTT.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions de l'interface MQTT
 *
 */

#include <PubSubClient.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "Fonctions_MQTT.h"
#include "capteurs.h"
#include "reseau_serveur.h"
#include "File_System.h"
#include "GPIO.h"
#include "global.h"



/**
 * @var WiFiClient espClient
 * @brief Objet WiFiClient pour la gestion de la connexion WiFi.
 *
 * Cet objet est utilisé pour établir et gérer la connexion WiFi de l'appareil ESP32.
 * Il est associé à la bibliothèque WiFi.h et est utilisé comme client pour les opérations de communication WiFi.
 */
WiFiClient espClient;

// Déclaration de l'objet PubSubClient pour la communication MQTT
/**
 * @var PubSubClient client(espClient)
 * @brief Objet PubSubClient pour la communication MQTT.
 *
 * Cet objet est utilisé pour la communication avec le serveur MQTT.
 * Il utilise un client WiFi (espClient) pour établir la connexion MQTT.
 */
PubSubClient client(espClient);

/**
 * @var String mqtt_server
 * @brief Adresse IP ou nom de domaine du serveur MQTT.
 *
 * Cette variable stocke l'adresse du serveur MQTT avec lequel l'appareil ESP32 doit se connecter.
 */
String mqtt_server;

/**
 * @var int mqtt_port
 * @brief Numéro de port pour la connexion MQTT.
 *
 * Cette variable stocke le numéro de port utilisé pour la connexion avec le serveur MQTT.
 */
int mqtt_port;

/**
 * @var String mqttUser
 * @brief Nom d'utilisateur pour l'authentification MQTT.
 *
 * Cette variable stocke le nom d'utilisateur utilisé lors de l'authentification auprès du serveur MQTT.
 */
String mqttUser;

/**
 * @var String mqttPassword
 * @brief Mot de passe pour l'authentification MQTT.
 *
 * Cette variable stocke le mot de passe utilisé lors de l'authentification auprès du serveur MQTT.
 */
String mqttPassword;

/**
 * @var String mqttClient
 * @brief Identifiant du client MQTT.
 *
 * Cette variable stocke l'identifiant du client MQTT utilisé lors de la connexion au serveur MQTT.
 */
String mqttClient;

/**
 * @var String mqttSubscribe1
 * @brief Sujet de souscription MQTT pour le canal 1.
 *
 * Cette variable stocke le sujet de souscription pour le canal 1 de la communication MQTT.
 */
String mqttSubscribe1;

/**
 * @var String mqttSubscribe1_full
 * @brief Sujet complet de souscription MQTT pour le canal 1.
 *
 * Cette variable stocke le sujet complet de souscription pour le canal 1, incluant tous les sous-sujets possibles.
 */
String mqttSubscribe1_full;

/**
 * @var String mqttSubscribe2
 * @brief Sujet de souscription MQTT pour le canal 2.
 *
 * Cette variable stocke le sujet de souscription pour le canal 2 de la communication MQTT.
 */
String mqttSubscribe2;

/**
 * @var String mqttPublish1
 * @brief Sujet de publication MQTT pour le canal 1.
 *
 * Cette variable stocke le sujet de publication pour le canal 1 de la communication MQTT.
 */
String mqttPublish1;

/**
 * @var String mqttPublish2
 * @brief Sujet de publication MQTT pour le canal 2.
 *
 * Cette variable stocke le sujet de publication pour le canal 2 de la communication MQTT.
 */
String mqttPublish2;

/**
 * @var String mqttPublish_s1_ext_out
 * @brief Sujet de publication MQTT pour les sorties externes du canal 1.
 *
 * Cette variable stocke le sujet de publication pour les sorties externes du canal 1 de la communication MQTT.
 */
String mqttPublish_s1_ext_out;

/**
 * @var String mqttPublish_s1_meteo
 * @brief Sujet de publication MQTT pour les données météo du canal 1.
 *
 * Cette variable stocke le sujet de publication pour les données météo du canal 1 de la communication MQTT.
 */
String mqttPublish_s1_meteo;

/**
 * @var String mqttPublish_s2
 * @brief Sujet de publication MQTT pour le canal 2.
 *
 * Cette variable stocke le sujet de publication pour le canal 2 de la communication MQTT.
 */
String mqttPublish_s2;

/**
 * @var const int numPorts[]
 * @brief Tableau des numéros de port de sortie numérique.
 *
 * Ce tableau stocke les numéros de port de sortie numérique correspondant à chaque voie.
 */
const int numPorts[] = {1, 2, 3, 4, 5, 6, 7, 8};

/**
 * @var unsigned long lastValPortUpdate[]
 * @brief Tableau des dernières mises à jour des valeurs de port.
 *
 * Ce tableau stocke les moments des dernières mises à jour des valeurs de port.
 */
unsigned long lastValPortUpdate[] = {0, 0, 0, 0, 0, 0, 0, 0};

/**
 * @var long timeOut[]
 * @brief Tableau des délais de timeout en secondes.
 *
 * Ce tableau stocke les délais de timeout en secondes pour chaque voie. La valeur -1 indique qu'il n'y a pas de timeout.
 */
long timeOut[] = {60, 60, 60, 60, 60, 60, 60, 60};

extern Struct_GPIO Telemetre;
extern Struct_GPIO Tab_PT100[4];
extern Struct_GPIO Tab_Sonde[4];
extern Struct_GPIO Tab_GPIO_OUT[8];
extern Struct_GPIO Tab_GPIO_IN[8];
extern Struct_GPIO Tab_GPIO_ANA[8];
extern Struct_IMP Tab_Impulsion[2];

/**
 * @fn void mqtt_service_setup()
 * @brief Configuration du service MQTT.
 *
 * Initialise et configure la connexion au serveur MQTT ainsi que les topics MQTT.
 */
void mqtt_service_setup(){
  if(!EnableMQTT){return;}
   Serial.println();
   Serial.println(F("============================================================================================"));
   Serial.println("Initialisation du serveur MQTT");
   Serial.println(F("============================================================================================")); 

   mqtt_server = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_serveur");
   mqtt_port = getIntValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_port");
   mqttUser = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_user");
   mqttPassword = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_password");
   mqttClient = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_client");
   mqttSubscribe1 = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_subscribe_1");
   mqttSubscribe1_full = mqttSubscribe1+"/#";
   mqttPublish_s1_ext_out = mqttSubscribe1+"_ext_out/#";
   mqttPublish_s1_meteo = mqttSubscribe1+"_meteo";
   mqttSubscribe2 = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_subscribe_2");
   mqttPublish_s2 = mqttSubscribe2+"_out/#";
   mqttPublish1 = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_publish_1");
   mqttPublish2 = getStringValueFromJsonFile("/MQTT.json", "MQTT", "General", "MQTT_publish_2");
   
  client.setServer(mqtt_server.c_str(), (uint16_t)mqtt_port);
  client.setCallback(callback);

  reconnect();
}

/**
 * @fn void reconnect()
 * @brief Fonction de reconnexion au serveur MQTT.
 *
 * Cette fonction tente de se reconnecter au serveur MQTT en cas de déconnexion.
 */
void reconnect() {
  if(!EnableMQTT){return;}
  while (!client.connected()) {
    Serial.print("Tentative de connexion au serveur MQTT " + mqtt_server + ":" + mqtt_port + " ...");
    if (client.connect(mqttClient.c_str())) {
      Serial.println("> Connecté au client " + mqttClient);

      client.subscribe(mqttSubscribe1_full.c_str());
      Serial.println("Souscription au canal " + mqttSubscribe1_full);

      client.subscribe(mqttSubscribe2.c_str());
      Serial.println("Souscription au canal " + mqttSubscribe2);

    } else {
      Serial.print("échec, code d'erreur = ");
      Serial.print(client.state());
      Serial.println("réessayer dans 5 secondes");
      delay(5000);
    }
  }
}

/**
 * @fn void callback(char *topic, byte *payload, unsigned int length)
 * @brief Fonction de rappel appelée lorsqu'un message MQTT est reçu.
 *
 * @param topic Topic MQTT sur lequel le message a été reçu.
 * @param payload Données du message.
 * @param length Longueur des données du message.
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if(!EnableMQTT){return;}

  Serial.print("Message reçu sur le topic: ");
  Serial.println(topic);
  
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("Message reçu : ");
  Serial.println(message);

  update_Subscribe1(mqttSubscribe1.c_str(), topic, (char*)payload, length);
}

/**
 * @fn void publish_1()
 * @brief Fonction de publication de données sur le canal 1 MQTT.
 *
 * Cette fonction construit et publie un message MQTT avec des données factices.
 */
void publish_1() {
  if(!EnableMQTT){return;}
  // Construction du message MQTT
  char messageBuffer[200];
  DynamicJsonDocument jsonDoc(200); // Ajustez la taille en fonction de vos besoins

  jsonDoc["variable1"] = 0;
  jsonDoc["variable2"] = 0;
  jsonDoc["variable3"] = 0;

  serializeJson(jsonDoc, messageBuffer);

  // Publication
  client.publish(mqttPublish1.c_str(), messageBuffer);
}

/**
 * @fn void publish_2()
 * @brief Fonction de publication de données sur le canal 2 MQTT.
 *
 * Cette fonction construit et publie un message MQTT avec des données factices.
 */
void publish_2() {
  if(!EnableMQTT){return;}
  // Construction du message MQTT
  char messageBuffer[200];
  DynamicJsonDocument jsonDoc(200); // Ajustez la taille en fonction de vos besoins

  jsonDoc["variable1"] = 0;
  jsonDoc["variable2"] = 0;
  jsonDoc["variable3"] = 0;

  serializeJson(jsonDoc, messageBuffer);

  // Publication
  client.publish(mqttPublish2.c_str(), messageBuffer);
}

/**
 * @fn void publish_s1()
 * @brief Fonction de publication des variables de l'ESP sur le canal 1 MQTT.
 * Cette fonction publie les données suivantes ;
 * - La 8 voies de l'extention PCF8574 sur    _out/PCF8574_OUT_1_
 * - La valeur des 8 GPIO OUT sur             _out/GPIO_OUT_x/Valeur
 * - La valeur des 8 GPIO IN sur              _out/GPIO_IN_x/Valeur
 * - La valeur des 8 GPIO ANA sur             _out/GPIO_ANA_x/Valeur
 * - La valeur des 4 PT100 sur                _out/PT100_x/Valeur
 * - La valeur des 4 Sondes sur               _out/Sonde_x/Valeur
 * - La valeur des 2 capteurs d'impulsion sur _out/Impulsion_x/{Cumul,Imp_par_sec,Imp_par_min}
 * - La valeur du télémètre sur               _out/Telemetre/Valeur
 * - Les valeurs des données météo   sur      _out/Telemetre/{temperature,temperature max,temperature min,pressure,humidity}
 * - La valeur des User sur                   _out/User/{INT,LONG,FLOAT}
 * 
 * @param void
 * @return void
 */
void publish_s1() {
  if(!EnableMQTT){return;}

  DEBUG_PRINT_MQTT("Fonction publish_s1");

  char messageBuffer[400];
  DynamicJsonDocument jsonDoc(512); // Ajustez la taille en fonction de vos besoins
  String Adress_Publication;

  /// @brief Construction du message MQTT vers PCF8574_OUT_1_x (x compris entre 1 et 8)
  for(int i=0; i<7; i++){
    /// @brief Ajoutez des données au JSON
    jsonDoc["port_status"] = Tab_PCF8574_OUT_1[i];

    serializeJson(jsonDoc, messageBuffer);

    /// @brief Publication du message sur le topic _out/PCF8574_OUT_1_x (x compris entre 1 et 8)
    Adress_Publication = mqttSubscribe1+"_out/PCF8574_OUT_1_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 

  /// @brief  Balayage des sorties GPIO digital
  for(int i=0; i<7; i++){
    jsonDoc["Valeur"] = Tab_GPIO_OUT[i].Valeur;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/GPIO_OUT_x (x compris entre 1 et 8)
    Adress_Publication = mqttSubscribe1+"_out/GPIO_OUT_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  }

   /// @brief  Balayage des entrées GPIO digital
  for(int i=0; i<7; i++){
    jsonDoc["Valeur"] = Tab_GPIO_IN[i].Valeur;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/GPIO_IN_x (x compris entre 1 et 8)
    Adress_Publication = mqttSubscribe1+"_out/GPIO_IN_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 

  /// @brief  Balayage des entrées GPIO Analog
  for(int i=0; i<7; i++){
    jsonDoc["Valeur"] = Tab_GPIO_ANA[i].Valeur;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/GPIO_ANA_x (x compris entre 1 et 8)
    Adress_Publication = mqttSubscribe1+"_out/GPIO_ANA_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 


  /// @brief  Balayage des PT100
  for(int i=0; i<4; i++){
    jsonDoc["Valeur"] = Tab_PT100[i].Valeur;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/PT100_x (x compris entre 1 et 4)
    Adress_Publication = mqttSubscribe1+"_out/PT100_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 

    /// @brief  Balayage des Sondes
  for(int i=0; i<4; i++){
    jsonDoc["Valeur"] = Tab_Sonde[i].Valeur;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/Sonde_x (x compris entre 1 et 4)
    Adress_Publication = mqttSubscribe1+"_out/Sonde_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 

    /// @brief  Balayage des Impulsions
  for(int i=0; i<2; i++){
    jsonDoc["Cumul"] = Tab_Impulsion[i].Valeur_Cumul;
    jsonDoc["Imp_par_sec"] = Tab_Impulsion[i].Valeur_ps;
    jsonDoc["Imp_par_min"] = Tab_Impulsion[i].Valeur_pmin;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/Impulsion_x (x compris entre 1 et 2)
    Adress_Publication = mqttSubscribe1+"_out/Impulsion_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 

  /// @brief  Balayage du Télémetre
  jsonDoc["Valeur"] = Telemetre.Valeur;
  serializeJson(jsonDoc, messageBuffer);

  /// @brief  Publication du message sur le topic _out/Telemetre
  Adress_Publication = mqttSubscribe1+"_out/Telemetre";
  client.publish(Adress_Publication.c_str(), messageBuffer);

  jsonDoc.clear();
  
  /// @brief  Publication des informations météo 
  jsonDoc["temperature"] = Temperature();
  jsonDoc["temperature_max"] = Temperature_max();
  jsonDoc["temperature_min"] = Temperature_min();
  jsonDoc["pressure"] = Pression();
  jsonDoc["humidity"] = Humidite();

  serializeJson(jsonDoc, messageBuffer);

  /// @brief  Publication du message sur le topic _out/Meteo
  Adress_Publication = mqttSubscribe1+"_out/Meteo";
  client.publish(Adress_Publication.c_str(), messageBuffer);

  DEBUG_PRINT_MQTT(Adress_Publication);
  DEBUG_PRINT_MQTT(messageBuffer);

  jsonDoc.clear();

  /// @brief  Balayage des User
  for(int i=0; i<16; i++){
    jsonDoc["INT"] = Tab_Info_USER[i].Val_INT;
    jsonDoc["LONG"] = Tab_Info_USER[i].Val_LONG;
    jsonDoc["FLOAT"] = Tab_Info_USER[i].Val_FLOAT;
    serializeJson(jsonDoc, messageBuffer);

    /// @brief  Publication du message sur le topic _out/User_x (x compris entre 1 et 16)
    Adress_Publication = mqttSubscribe1+"_out/User_"+(i+1);
    client.publish(Adress_Publication.c_str(), messageBuffer);

    DEBUG_PRINT_MQTT(Adress_Publication);
    DEBUG_PRINT_MQTT(messageBuffer);

    jsonDoc.clear();
  } 
}

/**
 * @fn void update_Subscribe1(String mqttSubscribe, char *topic, char *payload, unsigned int length)
 * @brief Mise à jour des données en fonction du message MQTT reçu sur le canal 1.
 *
 * Cette fonction analyse le topic et le payload du message MQTT reçu sur le canal 1,
 * puis met à jour les données en conséquence.
 *
 * @param mqttSubscribe Topic principal du canal MQTT.
 * @param topic Topic du message MQTT reçu.
 * @param payload Données du message MQTT.
 * @param length Longueur des données du message MQTT.
 */
void update_Subscribe1(String mqttSubscribe, char* topic, char* payload, unsigned int length) {
  if(!EnableMQTT){return;}
  char topicBuffer[300];
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.println(message);

  //Recherche de la voie pour un topic voie
  for (int i = 0; i < 8; i++) {
    
    //Pour un topic PCF8574_OUT
    sprintf(topicBuffer, "%s/PCF8574_OUT_1_%d",mqttSubscribe, i + 1);
    Serial.println(topicBuffer);
    if (strcmp(topic, topicBuffer) == 0) {

      StaticJsonDocument<200> jsonDoc; // Ajustez la taille en fonction de votre message JSON
      DeserializationError error = deserializeJson(jsonDoc, message);
        if (error) {
          Serial.print("Erreur lors de la désérialisation JSON: ");
          Serial.println(error.c_str());
          return;
        }
      
      bool valPort = jsonDoc["val_port"];
 
      if (valPort) {
        Serial.println("PCF8574_OUT_1_" + String(i) + " = TRUE!!!");
        PCF8574_OUT_1_out(i, true);
      }
      else{
        Serial.println("PCF8574_OUT_1_" + String(i) + " = LOW!!!");
        PCF8574_OUT_1_out(i, false);
      }
      break;
    }
  }

  //Pour un topic GPIO_OUT
  sprintf(topicBuffer, "%s/GPIO_OUT",mqttSubscribe);
  Serial.println(topicBuffer);
    if (strcmp(topic, topicBuffer) == 0) {
      StaticJsonDocument<200> jsonDoc; // Ajustez la taille en fonction de votre message JSON
      DeserializationError error = deserializeJson(jsonDoc, message);
        if (error) {
          Serial.print("Erreur lors de la désérialisation JSON: ");
          Serial.println(error.c_str());
          return;
        }
      
      uint8_t ValPort = jsonDoc["val_port"]; 
      uint8_t NumPort = jsonDoc["num_port"];
      Serial.println("Changement Etat de Sortie Bit " + String(NumPort) + " Valeur : " + String(ValPort)) ;
      GPIO_OUT(NumPort, ValPort);
    }

  //Pour un topic ServoMoteur
  sprintf(topicBuffer, "%s/ServoMoteur",mqttSubscribe);
  Serial.println(topicBuffer);
    if (strcmp(topic, topicBuffer) == 0) {
      StaticJsonDocument<200> jsonDoc; // Ajustez la taille en fonction de votre message JSON
      DeserializationError error = deserializeJson(jsonDoc, message);
        if (error) {
          Serial.print("Erreur lors de la désérialisation JSON: ");
          Serial.println(error.c_str());
          return;
        }
      
      uint8_t ValPort = jsonDoc["val_servo"]; 
      uint8_t NumPort = jsonDoc["num_servo"];
      Serial.println("Changement Etat de Sortie servo " + String(NumPort) + " Valeur : " + String(ValPort)) ;
      ServoMoteur_OUT(NumPort, ValPort);
    }

  //Pour un topic PWM
  sprintf(topicBuffer, "%s/PWM",mqttSubscribe);
  Serial.println(topicBuffer);
    if (strcmp(topic, topicBuffer) == 0) {
      StaticJsonDocument<200> jsonDoc; // Ajustez la taille en fonction de votre message JSON
      DeserializationError error = deserializeJson(jsonDoc, message);
        if (error) {
          Serial.print("Erreur lors de la désérialisation JSON: ");
          Serial.println(error.c_str());
          return;
        }
      
      uint8_t ValPort = jsonDoc["val_servo"]; 
      uint8_t NumPort = jsonDoc["num_servo"];
      Serial.println("Changement Etat de Sortie servo " + String(NumPort) + " Valeur : " + String(ValPort)) ;
      PWM_OUT(NumPort, ValPort);
    }

}

/**
 * @fn void update_Subscribe2(char *message, unsigned int length)
 * @brief Mise à jour des données en fonction du message MQTT reçu sur le canal 2.
 *
 * Cette fonction analyse le message MQTT reçu sur le canal 2 et met à jour les données en conséquence.
 *
 * @param message Message MQTT reçu sur le canal 2.
 * @param length Longueur des données du message MQTT.
 */
void update_Subscribe2(char* message, unsigned int length) {
  if(!EnableMQTT){return;}
}

/**
 * @fn void loop_MQTT()
 * @brief Boucle de gestion MQTT dans la fonction loop().
 *
 * Cette fonction est appelée dans la boucle principale loop() et gère les opérations MQTT
 * telles que la reconnexion au serveur MQTT et la mise à jour des messages.
 */
void loop_MQTT(){
  if(!EnableMQTT){return;}
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


