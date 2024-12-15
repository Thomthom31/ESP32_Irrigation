/**
 * @file capteur.cpp
 * @brief Fonction des capteurs.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction pour les capteurs de la passerelle MQTT IOT.
 * Ce fichier traire les fonctions de capteurs de température/pression/météo et capteur d'impulsions
 *
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h>
#include "File_System.h"
#include "global.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10



/** @var Struct_GPIO Telemetre
 *  @brief Capteur de télémétrie.
 */
Struct_GPIO Telemetre;

/** @var Struct_GPIO Tab_PT100[4]
 *  @brief Tableau de 4 capteurs PT100.
 */
Struct_GPIO Tab_PT100[4];

/** @var Struct_GPIO Tab_Sonde[4]
 *  @brief Tableau de 4 capteurs de sonde.
 */
Struct_GPIO Tab_Sonde[4];

/** @var Struct_IMP Tab_Impulsion[2]
 *  @brief Tableau de 2 capteurs d'impulsions.
 */
Struct_IMP Tab_Impulsion[2];

/** @var float temperature
 *  @brief Variable globale pour stocker la température en degrés Celsius.
 */
float temperature = 0;

/** @var float pression
 *  @brief Variable globale pour stocker la pression en hPa.
 */
float pression = 0;

/** @var float humidite
 *  @brief Variable globale pour stocker l'humidité.
 */
float humidite = 0.5;

/** @var float point_de_rosee
 *  @brief Variable globale pour stocker le point de rosée.
 */
float point_de_rosee = 0;

/** @var float max_temperature
 *  @brief Variable globale pour stocker la température maximale enregistrée.
 */
float max_temperature = -100;

/** @var float min_temperature
 *  @brief Variable globale pour stocker la température minimale enregistrée.
 */
float min_temperature = 100;

// Déclaration de l'objet BME280
Adafruit_BME280 bme280;
Adafruit_BMP280 bmp280;

bool EnableImpulsion1=false;
bool EnableImpulsion2=false;

bool EnablePT100_1=false;
bool EnablePT100_2=false;
bool EnablePT100_3=false;
bool EnablePT100_4=false;

bool EnableSonde_1=false;
bool EnableSonde_2=false;
bool EnableSonde_3=false;
bool EnableSonde_4=false;

bool EnableBME280=false;
bool EnableBMP280=false;
bool EnableTelemetre=false;

bool EnablePFC8574_1=false;
bool EnablePFC8574_2=false;

/**
 * @fn void ConfigCapteur(void)
 * @brief Configuration des capteurs à partir du fichier de configuration.
 *
 * Cette fonction lit les paramètres de configuration des capteurs à partir du fichier "config.json"
 * et active/désactive les capteurs en conséquence.
 */
void ConfigCapteur(void){
  String config_json;

  Serial.println("");
  Serial.println("Lecture du fichier de configuration partie CAPTEUR :");

  Serial.print("   BME_280 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "BME280", "Enable")=="true"){EnableBME280=true;};
  Serial.println(EnableBME280);

  Serial.print("   BMP_280 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "BMP280", "Enable")=="true"){EnableBMP280=true;};
  Serial.println(EnableBMP280);

  Serial.print("   Impulsion_1 = ");
  Tab_Impulsion[0].Enable=false;
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_1", "Enable")=="true"){
    Tab_Impulsion[0].Enable=true;
    Serial.println(Tab_Impulsion[0].Enable);
    Tab_Impulsion[0].PIN_compteur=getIntValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_1", "PIN_impulsion");
    Tab_Impulsion[0].PIN_quadratique=getIntValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_1", "PIN_quadratique");
    Tab_Impulsion[0].Temps_integration=getIntValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_1", "Temps_integration");
    Tab_Impulsion[0].Quadratique=false;
    if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_1", "Quadratique")=="true"){Tab_Impulsion[0].Quadratique=true;}
     Serial.printf("      PIN compteur : %d, Activation Quadratique : %d, PIN quadratique : %d, Temps d'integration : %d \n", Tab_Impulsion[0].PIN_compteur, Tab_Impulsion[0].Quadratique, Tab_Impulsion[0].PIN_quadratique, Tab_Impulsion[0].Temps_integration);
  }
  else{
    Serial.println(Tab_Impulsion[1].Enable);
  };
  
  Serial.print("   Impulsion_2 = ");
  Tab_Impulsion[1].Enable=false;
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_2", "Enable")=="true"){
    Tab_Impulsion[1].Enable=true;
    Serial.println(Tab_Impulsion[1].Enable);
    Tab_Impulsion[1].Enable=true;
    Tab_Impulsion[1].PIN_compteur=getIntValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_2", "PIN_impulsion");
    Tab_Impulsion[1].PIN_quadratique=getIntValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_2", "PIN_quadratique");
    Tab_Impulsion[1].Temps_integration=getIntValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_2", "Temps_integration");
    Tab_Impulsion[0].Quadratique=false;
    if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "Impulsion_2", "Quadratique")=="true"){Tab_Impulsion[1].Quadratique=true;}
     Serial.printf("      PIN compteur : %d, Activation Quadratique : %d, PIN quadratique : %d, Temps d'integration : %d \n", Tab_Impulsion[1].PIN_compteur, Tab_Impulsion[1].Quadratique, Tab_Impulsion[1].PIN_quadratique, Tab_Impulsion[1].Temps_integration);
  }
  else{
    Serial.println(Tab_Impulsion[1].Enable);
  };

  Serial.print("   PT100_1 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_1", "Enable")=="true"){EnablePT100_1=true;};
  Serial.println(EnablePT100_1);

  Serial.print("   PT100_2 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_2", "Enable")=="true"){EnablePT100_2=true;};
  Serial.println(EnablePT100_2);

  Serial.print("   PT100_3 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_3", "Enable")=="true"){EnablePT100_2=true;};
  Serial.println(EnablePT100_1);

  Serial.print("   PT100_4 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_4", "Enable")=="true"){EnablePT100_3=true;};
  Serial.println(EnablePT100_2);

  Serial.print("   Sonde_1 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_1", "Enable")=="true"){EnableSonde_1=true;};
  Serial.println(EnableSonde_1);

  Serial.print("   Sonde_2 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_2", "Enable")=="true"){EnableSonde_2=true;};
  Serial.println(EnableSonde_2);

  Serial.print("   Sonde_3 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_3", "Enable")=="true"){EnableSonde_3=true;};
  Serial.println(EnableSonde_3);

  Serial.print("   Sonde_4 = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "PT100_4", "Enable")=="true"){EnableSonde_4=true;};
  Serial.println(EnableSonde_4);

  Serial.print("   Telemetre = ");
  if(getStringValueFromJsonFile("/config.json", "CAPTEUR", "Telemetre", "Enable")=="true"){EnableTelemetre=true;};
  Serial.println(EnableTelemetre);
}

/**
 * @fn void Config_BMx280()
 * @brief Initialisation du capteur de température et de pression.
 *
 * Cette fonction initialise le capteur BME280 pour la mesure de la température et de la pression.
 * Elle vérifie également si le capteur est disponible et fonctionnel.
 */
void Config_BMx280() {
  unsigned status;
  Serial.println();
  Serial.println(F("============================================================================================"));
  Serial.println(F("Initialisation du capteur BME280 "));
  Serial.println(F("============================================================================================"));
  
  if(EnableBME280){status = bme280.begin(0x76);} //0x76
  if(EnableBMP280){status = bmp280.begin(0x56);} //0x56

  if (!status) {
    Serial.println("Impossible de trouver un capteur BME280 valide, vérifiez le câblage, l'adresse, l'ID du capteur!");
    Serial.print("L'ID du capteur était : 0x"); Serial.println(bme280.sensorID(), 16);
    Serial.print("Un ID de 0xFF signifie probablement une mauvaise adresse, un BMP 180 ou BMP 085\n");
    Serial.print("Un ID de 0x56-0x58 représente un BMP 280,\n");
    Serial.print("Un ID de 0x60 représente un BME 280.\n");
    Serial.print("Un ID de 0x61 représente un BME 680.\n");
    while (1) delay(10);
  }
  else {
    Serial.println("> Capteur Initialisé");
  }
}

/**
 * @fn void Read_BMx280()
 * @brief Lecture des capteurs.
 *
 * Cette fonction lit les capteurs de température, pression et humidité.
 * Les valeurs sont stockées dans les variables globales correspondantes.
 */
void Read_BMx280() {
  float alpha=0;
  temperature = -100;
  pression = 0;
  humidite = -1;

  if(EnableBME280){
    temperature = bme280.readTemperature(); // Lire la température en degrés Celsius
    pression = bme280.readPressure() / 100.0F; // Lire la pression en hPa
    humidite = bme280.readHumidity();
  }
  if(EnableBMP280){
    temperature = bmp280.readTemperature(); // Lire la température en degrés Celsius
    pression = bmp280.readPressure() / 100.0F; // Lire la pression en hPa
  }  

  if(temperature>max_temperature){max_temperature=temperature;}
  if(temperature<min_temperature){min_temperature=temperature;}

    // calcul du point de rosée  (formule de Heinrich Gustav Magnus-Tetens)
  alpha = log(humidite / 100) + (17.27 * temperature) / (237.3 + temperature);
  point_de_rosee = (237.3 * alpha) / (17.27 - alpha);
}

/**
 * @fn void reset_min_max()
 * @brief Réinitialisation des valeurs maximales et minimales de la température.
 */
void reset_min_max() {
  max_temperature=temperature;
  min_temperature=temperature;
}

/**
 * @fn float Temperature(void)
 * @brief Obtenez la valeur de la température.
 * @return La valeur de la température en degrés Celsius.
 */
float Temperature(void) {
  return temperature;
}

/**
 * @fn float Temperature_max(void)
 * @brief Obtenez la valeur de la température max.
 * @return La valeur de la température en degrés Celsius.
 */
float Temperature_max(void) {
  return max_temperature;
}

/**
 * @fn float Temperature_min(void)
 * @brief Obtenez la valeur de la température min.
 * @return La valeur de la température en degrés Celsius.
 */
float Temperature_min(void) {
  return min_temperature;
}

/**
 * @fn float Humidite(void)
 * @brief Obtenez la valeur de l'humidité.
 * @return La valeur de l'humidité.
 */
float Humidite(void) {
  return humidite;
}

/**
 * @fn float Pression(void)
 * @brief Obtenez la valeur de la pression.
 * @return La valeur de la pression en hPa.
 */
float Pression(void) {
  return pression;
}

/**
 * @fn float Point_rosee(void)
 * @brief Obtenez la valeur du point de rosée.
 * @return La valeur du point de rosée.
 */
float Point_rosee(void) {
  return point_de_rosee;
}

/**
 * @fn void test_i2c_capteur()
 * @brief Teste la présence de périphériques sur le bus I2C.
 */
void test_i2c_capteur(){
  byte nombreDePeripheriquesTrouves = 0;
  Serial.println();
  Serial.println(F("============================================================================================"));
  Serial.println(F("                                    ~~ SCANNER I2C ~~                                       "));
  Serial.println(F("Scanne toutes les adresses i2c, afin de repérer tous les périphériques connectés à l'arduino"));
  Serial.println(F("============================================================================================"));
  Serial.println();
   // Boucle de parcous des 127 adresses i2c possibles
  for (byte adresseI2C = 0; adresseI2C < 127; adresseI2C++)
  {
    Wire.beginTransmission(adresseI2C);             // Interrogation de l'adresse i2c ciblée
    if (Wire.endTransmission () == 0)               // Si cela s'est bien passé, c'est qu'il y a un périphérique connecté à cette adresse
    {
      Serial.print(F("Périphérique i2c trouvé à l'adresse : "));
      Serial.print(adresseI2C, DEC);                // On affiche son adresse au format décimal
      Serial.print(F(" (0x"));
      Serial.print(adresseI2C, HEX);                // … ainsi qu'au format hexadécimal (0x..)
      Serial.println(F(")"));
      
      nombreDePeripheriquesTrouves++;
      delay(1);                                     // Temporisation, avant de passer au scan de l'adresse suivante
    }
  }

  // Affichage final, indiquant le nombre total de périphériques trouvés sur le port I2C de l'arduino
  if (nombreDePeripheriquesTrouves == 0) {
    Serial.println(F("Aucun périphérique I2C trouvé…"));
  }
  else if (nombreDePeripheriquesTrouves == 1) {
    Serial.println();
    Serial.println(F("1 périphérique trouvé !"));
  }
  else {
    Serial.println();
    Serial.print(nombreDePeripheriquesTrouves);
    Serial.println(F(" périphériques trouvés !"));
  }
  Serial.println(F("Scan terminé."));  
}

long Impulsion1Count=0;
long lastImpulsion1Time=0;
long lastImpulsion1=0;

/**
 * @fn void IRAM_ATTR handleInterrupt_imp1()
 * @brief Gestionnaire d'interruption pour le capteur d'impulsions 1.
 */
void IRAM_ATTR handleInterrupt_imp1() {
  if(!Tab_Impulsion[0].Enable){return;}

  if(Tab_Impulsion[0].Quadratique){
    // Dans le cas d'un capteur quadratique
    int a = digitalRead(Tab_Impulsion[0].PIN_compteur);
    int b = digitalRead(Tab_Impulsion[0].PIN_quadratique);

    // Logique pour détecter la direction du compteur
    if ((a == HIGH && b == LOW) || (a == LOW && b == HIGH)) {
      // Dans le sens horaire (augmentation)
      Tab_Impulsion[0].Valeur_Cumul++;
    } else {
      // Dans le sens antihoraire (diminution)
      Tab_Impulsion[0].Valeur_Cumul--;
    }
  }
  else{
  Tab_Impulsion[0].Valeur_Cumul++;
  }
}


/**
 * @fn void setup_impulsion1()
 * @brief Configuration du capteur d'impulsions 1.
 */
void setup_impulsion1() {
  if(!Tab_Impulsion[0].Enable){return;}

  Serial.println();
  Serial.println(F("============================================================================================"));
  Serial.println(F("Initialisation du capteur d'impulsion"));
  Serial.println(F("============================================================================================"));
  Serial.println();

  pinMode(Tab_Impulsion[0].PIN_compteur, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Tab_Impulsion[0].PIN_compteur), handleInterrupt_imp1, RISING);
  Serial.printf("Capteur initialisé sur PIN %d, activation de l'interruption sur font montant\n", Tab_Impulsion[0].PIN_compteur);

  // cas d'une capteur quadratique
  if(Tab_Impulsion[0].Quadratique){
    pinMode(Tab_Impulsion[0].PIN_quadratique, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Tab_Impulsion[0].PIN_quadratique), handleInterrupt_imp1, RISING);
    Serial.printf("Activation du mode quadratique sur PIN %d, activation de l'interruption sur font montant\n", Tab_Impulsion[0].PIN_quadratique);
  }
}

/**
 * @fn long val_impulsion1()
 * @brief Obtenez la valeur cumulée du capteur d'impulsions 1.
 * @return La valeur cumulée du capteur d'impulsions 1.
 */
long val_impulsion1() {
  if(!Tab_Impulsion[0].Enable){return 0;}

  return Tab_Impulsion[0].Valeur_Cumul;
}

/**
 * @fn void maj_impulsion1(unsigned long val)
 * @brief Met à jour la valeur cumulée du capteur d'impulsions 1.
 * @param val La nouvelle valeur cumulée.
 */
void maj_impulsion1(unsigned long val){
  Tab_Impulsion[0].Valeur_Cumul=val;
}

/**
 * @fn float val_impulsion1_ps()
 * @brief Obtenez la vitesse en impulsions par seconde du capteur d'impulsions 1.
 * @return La vitesse en impulsions par seconde.
 */
float val_impulsion1_ps() {
  if(!Tab_Impulsion[0].Enable){return 0;}
  long currentTime = micros(); // Temps actuel en microsecondes
  float delta_time = (currentTime-lastImpulsion1Time);
  delta_time=delta_time/1000000;
  long turbine=val_impulsion1();
  float PulsePS = (turbine-lastImpulsion1);
  PulsePS = PulsePS/(delta_time);
  lastImpulsion1Time = currentTime;
  lastImpulsion1 = turbine;
  Tab_Impulsion[0].Valeur_ps=PulsePS;
  return Tab_Impulsion[0].Valeur_ps;
}

/**
 * @fn float val_impulsion1_pmin()
 * @brief Obtenez la vitesse en impulsions par minute du capteur d'impulsions 1.
 * @return La vitesse en impulsions par minute.
 */
float val_impulsion1_pmin() {
  if(!Tab_Impulsion[0].Enable){return 0;}
  long currentTime = micros(); // Temps actuel en microsecondes
  float delta_time = (currentTime-lastImpulsion1Time);
  delta_time=delta_time/1000000;
  long turbine=val_impulsion1();
  float PulsePS = (turbine-lastImpulsion1);
  PulsePS = PulsePS/(delta_time);
  lastImpulsion1Time = currentTime;
  lastImpulsion1 = turbine;
  return PulsePS;
}