/**
 * @file GPIO.cpp
 * @brief Fonction des ports d'entrée sorties.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions d'entrée sortie'
 *
 */
#include <Wire.h>
#include <PCF8574.h>
#include <ESP32Servo.h>
#include <ESP32PWM.h>
#include "File_System.h"
#include "global.h"


// Déclaration des broches pour l'extension de port parallèle I2C PCF8574
#define PCF8574_ADDRESS 0x20
#define PCF8574_GPIO0 0
#define PCF8574_GPIO1 1
#define PCF8574_GPIO2 2
#define PCF8574_GPIO3 3
#define PCF8574_GPIO4 4
#define PCF8574_GPIO5 5
#define PCF8574_GPIO6 6
#define PCF8574_GPIO7 7



Struct_GPIO Tab_GPIO_OUT[8];
Struct_GPIO Tab_GPIO_IN[8];
Struct_GPIO Tab_GPIO_ANA[8];

// Structure pour stocker les informations sur un servomoteur
struct ServoMoteurConfig {
    bool Enable;
    int Defaut;
    int Angle_min;
    int Angle_max;
    int PIN_OUT;
};

// Tableau pour stocker les configurations des servomoteurs
ServoMoteurConfig Tab_ServoMoteur[4];

// Déclaration des objets Servo
Servo servo[4];

struct PWMConfig {
    bool Enabled;
    uint8_t PIN_OUT;
    uint32_t Frequence;
    uint32_t DutyCycle; // En pourcentage
};

// Tableau pour stocker les configurations des PWM
PWMConfig Tab_PWM[4]; // Ajuster le nombre de canaux PWM si nécessaire


bool Tab_PCF8574_OUT_1[8];
bool Tab_PCF8574_OUT_2[8];

int Periode=1000;

// Déclaration de l'objet PCF8574
PCF8574 pcf8574(PCF8574_ADDRESS);

/**
 * @fn Config_PCF8574_OUT_1()
 * @brief Configuration de la première extension PCF8574 en sortie.
 *
 * Cette fonction configure la première extension PCF8574 en tant que sortie
 * et initialise le tableau de sortie. Elle lit également les valeurs de
 * configuration depuis le fichier JSON.
 */
void Config_PCF8574_OUT_1(){
    if(!EnablePFC8574_1){return;};

  for(int i=0;i<8;i++){Tab_PCF8574_OUT_1[i]=false;} // Initialisation du tableau de sortie

  int i=0;
    // Initialisation du PCF8574 pour la gestion des sorties booléennes
  Serial.println();
  Serial.println(F("============================================================================================"));
  Serial.println(F("Initialisation du composant PCF8574 "));
  Serial.println(F("============================================================================================"));

  for(int i=0;i<8;i++){
    pcf8574.pinMode(i, OUTPUT, HIGH);
  }
  
  if (pcf8574.begin()){
		Serial.println("> PC8574 Initialisé ");
	}else{
		Serial.println("Initialisation KO ");
	}

  Serial.println("=OK");
}

/**
 * @fn PCF8574_OUT_1_maj()
 * @brief Mise à jour des sorties de la première extension PCF8574.
 *
 * Cette fonction met à jour les sorties de la première extension PCF8574
 * en fonction du tableau de sortie.
 */
void PCF8574_OUT_1_maj(){ 
  for(int i=0;i<8;i++){
    pcf8574.digitalWrite(i, !Tab_PCF8574_OUT_1[i]);
  }
}

/**
 * @fn PCF8574_OUT_1_out(int num_port, bool val)
 * @brief Configuration d'une sortie spécifique de la première extension PCF8574.
 *
 * Cette fonction configure une sortie spécifique de la première extension PCF8574
 * en fonction du numéro de port et de la valeur.
 *
 * @param num_port Numéro du port de sortie
 * @param val Valeur de sortie
 */
int PCF8574_OUT_1_out(int num_port, bool val){ 

  Tab_PCF8574_OUT_1[num_port]=val;
  pcf8574.digitalWrite(num_port, !Tab_PCF8574_OUT_1[num_port]);
  return 1;
}

/**
 * @fn ConfigGPIO(void)
 * @brief Configuration des ports GPIO.
 *
 * Cette fonction lit les configurations GPIO depuis le fichier JSON et
 * initialise les ports en conséquence.
 */
void ConfigGPIO(void){
  String config_json;
  String json_pin_config;
  String json_pin_mode;
  unsigned char json_pin_number;

  Serial.println("");
  Serial.println("Lecture du fichier de configuration partie GPIO :");

  Serial.println("   GPIO en sortie :");
  for(int i=1;i<9;i++){
    config_json = String(("GPIO_OUT_" + std::to_string(i)).c_str());
    Tab_GPIO_OUT[i-1].Enable=false; //Mise de la valeur par défaut à false
    Tab_GPIO_OUT[i-1].PIN=0;
    Tab_GPIO_OUT[i-1].Valeur=0;
    if(getStringValueFromJsonFile("/config.json", "GPIO", config_json, "Enable")=="true"){
      json_pin_number=getIntValueFromJsonFile("/config.json", "GPIO", config_json, "PIN_OUT");
      if(json_pin_number>0){pinMode(json_pin_number, OUTPUT);};
      Serial.printf("    %s Enable sur PIN %d \n",config_json, json_pin_number);
      Tab_GPIO_OUT[i-1].Enable=true;
      Tab_GPIO_OUT[i-1].Valeur=0;
      Tab_GPIO_OUT[i-1].PIN=json_pin_number;
    }
  }

  Serial.println("   GPIO en entree :");
  for(int i=1;i<9;i++){
    config_json = String(("GPIO_IN_" + std::to_string(i)).c_str());
    Tab_GPIO_IN[i-1].Enable=false; //Mise de la valeur par défaut à false
    Tab_GPIO_IN[i-1].PIN=0;
    Tab_GPIO_IN[i-1].Valeur=0;
    if(getStringValueFromJsonFile("/config.json", "GPIO", config_json, "Enable")=="true"){
      json_pin_number=getIntValueFromJsonFile("/config.json", "GPIO", config_json, "PIN_IN");
      if(getStringValueFromJsonFile("/config.json", "GPIO", config_json, "Pull_up")=="true"){
        if(json_pin_number>0){pinMode(json_pin_number, INPUT_PULLUP);};
        Serial.printf("    %s Enable sur PIN %d Pull up\n",config_json, json_pin_number);
        Tab_GPIO_IN[i-1].Enable=true;
        Tab_GPIO_IN[i-1].Valeur=0;
        Tab_GPIO_IN[i-1].PIN=json_pin_number;
      }
      else
      {
        pinMode(json_pin_number, INPUT);
        Serial.printf("    %s Enable sur PIN %d \n",config_json, json_pin_number);
        Tab_GPIO_IN[i-1].Enable=true;
        Tab_GPIO_IN[i-1].Valeur=0;
        Tab_GPIO_IN[i-1].PIN=json_pin_number;
      }
    }
  }

  for(int i=1;i<9;i++){
    config_json = String(("GPIO_ANA_" + std::to_string(i)).c_str());
    Tab_GPIO_ANA[i-1].Enable=false; //Mise de la valeur par défaut à false
    Tab_GPIO_ANA[i-1].PIN=0;
    Tab_GPIO_ANA[i-1].Valeur=0;
    if(getStringValueFromJsonFile("/config.json", "GPIO", config_json, "Enable")=="true"){
      json_pin_number=getIntValueFromJsonFile("/config.json", "GPIO", config_json, "PIN_IN");
      if(json_pin_number>0){pinMode(json_pin_number, INPUT);};
      Serial.printf("    %s Enable sur PIN %d en mode analogique\n",config_json, json_pin_number);
      Tab_GPIO_ANA[i-1].Enable=true;
      Tab_GPIO_ANA[i-1].Valeur=0;
      Tab_GPIO_ANA[i-1].PIN=json_pin_number;
    }
  }

  Serial.print("   EXT_OUT_PCF8574_1 = ");
  EnablePFC8574_1=false;
  if(getStringValueFromJsonFile("/config.json", "GPIO", "EXT_OUT_PCF8574_1", "Enable")=="true"){EnablePFC8574_1=true;};
  Serial.println(EnablePFC8574_1);

  Serial.print("   EXT_OUT_PCF8574_2 = ");
  EnablePFC8574_2=false;
  if(getStringValueFromJsonFile("/config.json", "GPIO", "EXT_OUT_PCF8574_2", "Enable")=="true"){EnablePFC8574_2=true;};
  Serial.println(EnablePFC8574_2);

  Serial.println("");
  Serial.println("Lecture du fichier de configuration partie GENERAL :");

  Serial.print("   Boucle = ");
  Periode=getIntValueFromJsonFile("/config.json", "GENERAL", "Boucle", "Periode");
  Serial.println(Periode);
}


/**
 * @fn GPIO_maj(void)
 * @brief Mise à jour des ports GPIO.
 *
 * Cette fonction met à jour les ports GPIO en fonction des valeurs actuelles.
 */
void GPIO_maj(void){
  for(int i=0;i<8;i++){
    if(Tab_GPIO_OUT[i].Enable){digitalWrite(Tab_GPIO_OUT[i].PIN, Tab_GPIO_OUT[i].Valeur);}
    if(Tab_GPIO_IN[i].Enable){Tab_GPIO_IN[i].Valeur=digitalRead(Tab_GPIO_IN[i].PIN);}
    if(Tab_GPIO_ANA[i].Enable){Tab_GPIO_ANA[i].Valeur=analogRead(Tab_GPIO_ANA[i].PIN);}
  }
}

/**
 * @fn GPIO_OUT(int i, int val)
 * @brief Configuration d'une sortie GPIO spécifique.
 *
 * Cette fonction configure une sortie GPIO spécifique en fonction du
 * numéro de port et de la valeur.
 *
 * @param i Numéro du port GPIO
 * @param val Valeur de sortie
 */
int GPIO_OUT(int i, int val){

  i--;
  Tab_GPIO_OUT[i].Valeur=val;
  if(Tab_GPIO_OUT[i].Enable){
    digitalWrite(Tab_GPIO_OUT[i].PIN, Tab_GPIO_OUT[i].Valeur);
    return 1;
  }
  else{
    return 0;
  }
}

/**
 * @fn GPIO_IN(int i, int val)
 * @brief Lecture d'une entrée GPIO spécifique.
 *
 * Cette fonction lit la valeur d'une entrée GPIO spécifique en fonction
 * du numéro de port.
 *
 * @param i Numéro du port GPIO
 * @param val Valeur à retourner
 * @return int
 */
int GPIO_IN(int i, int val){
  i--;
  return Tab_GPIO_IN[i].Valeur;
}

/**
 * @fn GPIO_ANA(int i, int val)
 * @brief Lecture d'une entrée analogique GPIO spécifique.
 *
 * Cette fonction lit la valeur d'une entrée analogique GPIO spécifique en
 * fonction du numéro de port.
 *
 * @param i Numéro du port GPIO
 * @param val Valeur à retourner
 * @return int
 */
int GPIO_ANA(int i, int val){
  i--;
  return Tab_GPIO_ANA[i].Valeur;
}

/**
 * @fn ConfigTIMER(void)
 * @brief Configure les TIMER pour l'utilisation PWM ou Servo.
 *
 */
void ConfigTIMER(void) {
  Serial.println("   Configuration des timers :");
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
}

/**
 * @fn ConfigServoMoteur(void)
 * @brief Configure les sorties des servomoteurs en fonction des paramètres du fichier JSON.
 *
 * Cette fonction lit les paramètres des servomoteurs du fichier JSON et initialise les
 * objets Servo correspondants avec leur valeur par défaut, en prenant en compte
 * les valeurs minimales et maximales des angles.
 */
void ConfigServoMoteur(void) {
  String config_json;
  String json_servo_config;
  unsigned char json_pin_number;

  Serial.println("   ServoMoteurs :");
  for (int i = 0; i < 4; i++) {
    config_json = String(("ServoMoteur_OUT_" + std::to_string(i + 1)).c_str());
    Tab_ServoMoteur[i].Enable = false;
    Tab_ServoMoteur[i].Defaut = 90;
    Tab_ServoMoteur[i].Angle_min = 544;
    Tab_ServoMoteur[i].Angle_max = 2400;
    Tab_ServoMoteur[i].PIN_OUT = 0;

    if (getStringValueFromJsonFile("/config.json", "ServoMoteur", config_json, "Enable") == "true") {
      Tab_ServoMoteur[i].Enable = true;
      Tab_ServoMoteur[i].Defaut = getIntValueFromJsonFile("/config.json", "ServoMoteur", config_json, "Defaut");
      Tab_ServoMoteur[i].Angle_min = getIntValueFromJsonFile("/config.json", "ServoMoteur", config_json, "Angle_min");
      Tab_ServoMoteur[i].Angle_max = getIntValueFromJsonFile("/config.json", "ServoMoteur", config_json, "Angle_max");
      Tab_ServoMoteur[i].PIN_OUT = getIntValueFromJsonFile("/config.json", "ServoMoteur", config_json, "PIN_OUT");

      // Initialisation du servo
      Serial.print("     Voie ");
      Serial.print(i);
      Serial.print(" angle par defaut : ");
      Serial.println(Tab_ServoMoteur[i].Defaut);      
      servo[i].write(Tab_ServoMoteur[i].Defaut);
      servo[i].attach(Tab_ServoMoteur[i].PIN_OUT, Tab_ServoMoteur[i].Angle_min, Tab_ServoMoteur[i].Angle_max);

    }
  }
}

/**
 * @fn ServoMoteur_Desactive(int i)
 * @brief Désactive le servomoteur spécifié.
 *
 * Cette fonction détache le servomoteur de la broche correspondante.
 *
 * @param i Index du servomoteur à désactiver
 */
void ServoMoteur_Desactive(int i) {
    if (i >= 0 && i < 4 && servo[i].attached()) {
        servo[i].detach();
    }
}



/**
 * @fn ServoMoteur_OUT(int i, int val)
 * @brief Définit la position d'un servomoteur.
 *
 * Cette fonction définit la position d'un servomoteur en fonction de son index
 * et de la valeur souhaitée. Si le servomoteur n'est pas attaché, il ests
 * attaché avant de définir la position.
 *
 * @param i Index du servomoteur (0 à 3)
 * @param val Nouvelle position du servomoteur (entre 0 et 180)
 */
int ServoMoteur_OUT(int i, int val) {

    if (i >= 0 && i < 4 && Tab_ServoMoteur[i].Enable) {
        servo[i].write(val);
        if (!servo[i].attached()) {
            servo[i].attach(Tab_ServoMoteur[i].PIN_OUT, Tab_ServoMoteur[i].Angle_min, Tab_ServoMoteur[i].Angle_max);
        }
        return 1;
    }
    else{
      return 0;
    }
}

/**
 * @brief Configure les sorties PWM en fonction des paramètres du fichier JSON.
 *
 * Cette fonction lit les paramètres des sorties PWM depuis un fichier JSON et initialise 
 * les canaux PWM correspondants avec les fréquences et les cycles de service spécifiés.
 * Les informations sur les broches et les états d'activation sont également lues depuis le fichier JSON.
 *
 * **Note:** Assurez-vous que la bibliothèque ESP32PWM est incluse et que le fichier JSON est 
 * correctement formaté.
 */
void ConfigurePWM() {
    String config_json;
    unsigned char json_pin_number;

    Serial.println("   PWM :");
    for (int i = 0; i < 4; i++) {
        config_json = String(("PWM_OUT_" + std::to_string(i + 1)).c_str());
        Tab_PWM[i].Enabled = false;
        Tab_PWM[i].Frequence = 5000; // Valeur par défaut
        Tab_PWM[i].DutyCycle = 0;
        Tab_PWM[i].PIN_OUT = 0;

        if (getStringValueFromJsonFile("/config.json", "PWM", config_json, "enabled") == "true") {
            Tab_PWM[i].Enabled = true;
            Tab_PWM[i].Frequence = getIntValueFromJsonFile("/config.json", "PWM", config_json, "Frequency");
            Tab_PWM[i].DutyCycle = getIntValueFromJsonFile("/config.json", "PWM", config_json, "DutyCycle");
            Tab_PWM[i].PIN_OUT = getIntValueFromJsonFile("/config.json", "PWM", config_json, "PIN_OUT");
            

            // Initialisation du PWM
            ledcSetup(i, Tab_PWM[i].Frequence, 8);
            ledcAttachPin(Tab_PWM[i].PIN_OUT, i);
            ledcWrite(i, map(Tab_PWM[i].DutyCycle, 0, 100, 0, 255));
        }
    }
}

/**
 * @fn PWM_OUT(int i, int val)
 * @brief Définit le cycle de service d'une sortie PWM.
 *
 * Cette fonction définit le cycle de service d'une sortie PWM en fonction de son index
 * et de la valeur souhaitée. Si le PWM n'est pas attaché, il est attaché avant de définir le cycle de service.
 *
 * @param i Index du PWM (0 à 3)
 * @param val Nouveau cycle de service (entre 0 et 100)
 */
int PWM_OUT(int i, int val) {
    if (i >= 0 && i < 4 && Tab_PWM[i].Enabled) {
        ledcWrite(i, map(val, 0, 100, 0, 255)); // Mappe la valeur de 0 à 100 en une valeur de 0 à 255 pour le PWM
        return 1;
    }
    else{
      return 0;
    }
}