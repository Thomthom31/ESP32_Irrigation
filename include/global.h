/**
 * @file global.h
 * @brief Déclaration des variables globales et des structures.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier d'en-tête contenant la déclaration des variables globales et des structures utilisées dans le programme.
 */







//#define DEBUG_MODE_MQTT  // Commentez ou décommentez cette ligne pour activer ou désactiver le mode de débogage
//#define DEBUG_MODE_GPIO  // Commentez ou décommentez cette ligne pour activer ou désactiver le mode de débogage
//#define DEBUG_MODE_CAPTEUR  // Commentez ou décommentez cette ligne pour activer ou désactiver le mode de débogage
//#define DEBUG_MODE_FS  // Commentez ou décommentez cette ligne pour activer ou désactiver le mode de débogage

// Variable globale d'activation
extern bool EnableLED;               ///< Activation de la LED.
extern bool EnableBUZZER;            ///< Activation du buzzer.

extern bool EnableWIFI;              ///< Activation du module WiFi.
extern bool EnableNTP;               ///< Activation du module NTP.
extern bool EnableMQTT;              ///< Activation du module MQTT.
extern bool EnableWEB;               ///< Activation du serveur web.

extern bool EnableImpulsion1;        ///< Activation de l'impulsion 1.
extern bool EnableImpulsion2;        ///< Activation de l'impulsion 2.

extern bool EnablePT100_1;           ///< Activation du capteur PT100_1.
extern bool EnablePT100_2;           ///< Activation du capteur PT100_2.
extern bool EnablePT100_3;           ///< Activation du capteur PT100_3.
extern bool EnablePT100_4;           ///< Activation du capteur PT100_4.

extern bool EnableSonde_1;           ///< Activation de la sonde 1.
extern bool EnableSonde_2;           ///< Activation de la sonde 2.
extern bool EnableSonde_3;           ///< Activation de la sonde 3.
extern bool EnableSonde_4;           ///< Activation de la sonde 4.

extern bool EnableBME280;            ///< Activation du capteur BME280.
extern bool EnableBMP280;            ///< Activation du capteur BMP280.
extern bool EnableTelemetre;         ///< Activation du télémètre.

extern bool EnablePFC8574_1;         ///< Activation de l'extension PFC8574_1.
extern bool EnablePFC8574_2;         ///< Activation de l'extension PFC8574_2.

extern time_t currentTime ;          ///< Temps actuel (en secondes depuis l'époque).
extern String currentTimeStr ;       ///< Temps actuel sous forme de chaîne de caractères.
extern String currentDayStr ;        ///< Date actuelle sous forme de chaîne de caractères.

/**
 * @struct Struct_GPIO
 * @brief Structure représentant une GPIO.
 */
struct Struct_GPIO {
  bool Enable = false;               ///< Activation de la GPIO.
  int Valeur = 0;                    ///< Valeur de la GPIO.
  int PIN = 0;                       ///< Numéro de broche de la GPIO.
  float Valeur_F = 0;
  float A = 1;
  float B = 0;
};

extern Struct_GPIO Tab_GPIO_OUT[8];  ///< Tableau des GPIO de sortie.
extern Struct_GPIO Tab_GPIO_IN[8];   ///< Tableau des GPIO d'entrée.
extern Struct_GPIO Tab_GPIO_ANA[8];  ///< Tableau des GPIO analogiques.
extern Struct_GPIO Telemetre;        ///< GPIO pour le télémètre.
extern Struct_GPIO Tab_PT100[4];     ///< Tableau des GPIO pour les capteurs PT100.
extern Struct_GPIO Tab_Sonde[4];     ///< Tableau des GPIO pour les sondes.

/**
 * @struct Struct_IMP
 * @brief Structure représentant une impulsion.
 */
struct Struct_IMP {
  bool Enable = false;               ///< Activation de l'impulsion.
  long Valeur_Cumul = 0;             ///< Valeur cumulée de l'impulsion.
  long Valeur_Cumul_back = 0;        ///< Valeur cumulée de l'impulsion (backup).
  float Valeur_ps = 0;               ///< Valeur par seconde de l'impulsion.
  float Valeur_pmin = 0;             ///< Valeur par minute de l'impulsion.
  bool Quadratique = false;          ///< Activation du mode quadratique de l'impulsion.
  int PIN_compteur = 0;              ///< Numéro de broche du compteur d'impulsions.
  int PIN_quadratique = 0;           ///< Numéro de broche pour le mode quadratique.
  int Temps_integration = 10;        ///< Temps d'intégration de l'impulsion.
};

extern Struct_IMP Tab_Impulsion[2];  ///< Tableau des structures d'impulsions.

/**
 * @struct Struct_USER
 * @brief Structure représentant une variable utilisateur.
 */
struct Struct_USER {
  int Val_INT = 0;                   ///< Valeur entière pour l'utilisateur.
  long Val_LONG = 0;                 ///< Valeur longue pour l'utilisateur.
  float Val_FLOAT = 0;               ///< Valeur flottante pour l'utilisateur.
};

extern Struct_USER Tab_Info_USER[16]; ///< Tableau des structures pour les variables utilisateur.

// Tableau pour les extensions de sorties
extern bool Tab_PCF8574_OUT_1[8];    ///< Tableau des sorties pour l'extension PFC8574_1.
extern bool Tab_PCF8574_OUT_2[8];    ///< Tableau des sorties pour l'extension PFC8574_2.

// Autres variables globales
extern int Periode;                  ///< Période globale du système.

/**
 * @brief Macro fonction pour le débug MQTT
 * 
 */
#ifdef DEBUG_MODE_MQTT
#define DEBUG_PRINT_MQTT(x) Serial.println(x)
#else
#define DEBUG_PRINT_MQTT(x)
#endif

/**
 * @brief Macro fonction pour le débug GPIO
 * 
 */
#ifdef DEBUG_MODE_GPIO
#define DEBUG_PRINT_GPIO(x) Serial.println(x)
#else
#define DEBUG_PRINT_GPIO(x)
#endif

/**
 * @brief Macro fonction pour le débug CAPTEUR
 * 
 */
#ifdef DEBUG_MODE_CAPTEUR
#define DEBUG_PRINT_CAPTEUR(x) Serial.println(x)
#else
#define DEBUG_PRINT_CAPTEUR(x)
#endif

/**
 * @brief Macro fonction pour le débug FS
 * 
 */
#ifdef DEBUG_MODE_FS
#define DEBUG_PRINT_FS(x) Serial.println(x)
#else
#define DEBUG_PRINT_FS(x)
#endif