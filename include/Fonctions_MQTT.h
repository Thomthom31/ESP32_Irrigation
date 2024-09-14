/**
 * @file Fonction_MQTT.h
 * @brief Fonction de l'interface MQTT.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions de l'interface MQTT
 *
 */

void reconnect();
void setup() ;
void callback(char* topic, byte* message, unsigned int length) ;
void publish_1();
void publish_2();
void publish_s1();
void publish_s2();
void update_Subscribe1(String mqttSubscribe, char* topic, char* payload, unsigned int length);
void update_Subscribe2(char* message, unsigned int length);
void update_vannes();
void mqtt_service_setup();
void loop_MQTT();



