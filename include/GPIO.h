/**
 * @file GPIO.h
 * @brief Fonction des ports d'entrée sorties.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions d'entrée sortie'
 *
 */

void Config_PCF8574_OUT_1();
void PCF8574_OUT_1_maj();
void PCF8574_OUT_1_out(int num_port, bool val);
void ConfigGPIO(void);
void GPIO_maj(void);
void GPIO_OUT(int i, int val);
int GPIO_IN(int i, int val);
int GPIO_ANA(int i, int val);
 