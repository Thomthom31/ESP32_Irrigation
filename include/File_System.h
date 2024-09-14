/**
 * @file File_System.h
 * @brief Fonction d'échange avec le file system.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions d'échanges avec le file system pour la lecture et l'écriture de fichiers
 *
 */

void init_file_system();
String getStringValueFromJsonFile(String filePath, String tag1, String tag2, String tag3);
int getIntValueFromJsonFile(String filePath, String tag1, String tag2, String tag3);
void saveDataToFile(float temperature_max, float temperature_min, float pression, int turbine);
void readMeteoFileToSerial();
void readFileToSerial(String filepath);
void modifFileToSerial(String filepath);
void delFileToSerial(String filepath);

void SaveValueToFile(void);
void RestoreValueFromFile(void);