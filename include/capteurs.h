/**
 * @file capteur.h
 * @brief Fonction des capteurs.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction pour les capteurs de la passerelle MQTT IOT.
 * Ce fichier traire les fonctions de capteurs de température/pression/météo et capteur d'impulsions
 *
 */

void Config_BMx280(void);
void Read_BMx280(void);
void reset_min_max(void);
float Temperature(void);
float Temperature_max(void);
float Temperature_min(void);
float Humidite(void);
float Pression(void);

void test_i2c_capteur(void);

void setup_impulsion1(void);
long val_impulsion1(void);
float val_impulsion1_ps(void);
float val_impulsion1_pmin(void);
void maj_impulsion1(unsigned long val);

void ConfigCapteur (void);

