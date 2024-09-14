/**
 * @file File_System.cpp
 * @brief Fonction d'échange avec le file system.
 * @author Thomas GAUTIER
 * @version 1
 * @date 22/11/2023
 *
 * Fichier de fonction de passerelle MQTT IOT.
 * Le fichier traite les fonctions d'échanges avec le file system pour la lecture et l'écriture de fichiers
 *
 */


#include "ArduinoJson.h"
#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "capteurs.h"
#include "global.h"




/**
 * @fn void init_file_system()
 * @brief Initialisation du système de fichiers SPIFFS
 * 
 * @return void
 */
void init_file_system() {
// Initialisation du système de fichiers SPIFFS
    Serial.println();
    Serial.println(F("============================================================================================"));
    Serial.println("Initialisation du système de fichier");
    Serial.println(F("============================================================================================")); 
    if (!SPIFFS.begin(true)) {
        Serial.println("Erreur lors de l'initialisation du système de fichiers SPIFFS");
        return;
    }
    Serial.println("> File système initialisé");
}

/**
 * @fn String getStringValueFromJsonFile(String filePath, String tag1, String tag2, String tag3)
 * @brief Lecture d'une valeur de type chaîne de caractères depuis un fichier JSON
 * 
 * @param filePath Chemin du fichier JSON
 * @param tag1 Tag de niveau 1 dans la structure JSON
 * @param tag2 Tag de niveau 2 dans la structure JSON
 * @param tag3 Tag de niveau 3 dans la structure JSON
 * 
 * @return Valeur de la clé spécifiée dans le fichier JSON
 */
String getStringValueFromJsonFile(String filePath, String tag1, String tag2, String tag3) {
    //Serial.printf("            Lecture JSON %s %s %s %s => ", filePath, tag1, tag2, tag3);
  File file = SPIFFS.open(filePath, "r");
  if (!file) {
    Serial.println("Erreur lors de l'ouverture du fichier");
    return "Null";
  }

  size_t size = file.size();
  //Serial.println(size);
  std::unique_ptr<char[]> buf(new char[size]);
  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Erreur lors de la désérialisation du fichier JSON");
    return "Null";
  }

  String a = doc[tag1][tag2][tag3].as<String>();
  //Serial.printf("%s \n",a);
  return a.c_str();
}

/**
 * @fn int getIntValueFromJsonFile(String filePath, String tag1, String tag2, String tag3)
 * @brief Lecture d'une valeur de type entier depuis un fichier JSON
 * 
 * @param filePath Chemin du fichier JSON
 * @param tag1 Tag de niveau 1 dans la structure JSON
 * @param tag2 Tag de niveau 2 dans la structure JSON
 * @param tag3 Tag de niveau 3 dans la structure JSON
 * 
 * @return Valeur de la clé spécifiée dans le fichier JSON
 */
int getIntValueFromJsonFile(String filePath, String tag1, String tag2, String tag3) {
  //Serial.printf("            Lecture JSON %s %s %s %s => ", filePath, tag1, tag2, tag3);
  File file = SPIFFS.open(filePath, "r");
  if (!file) {
    Serial.println("Erreur lors de l'ouverture du fichier");
    return 0;
  }

  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);
  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Erreur lors de la désérialisation du fichier JSON");
    return 0;
  }
  String a = doc[tag1][tag2][tag3].as<String>();
  //Serial.printf("%s \n",a);
  return atoi(a.c_str());
}

/**
 * @fn void saveDataToFile(float temperature_max, float temperature_min, float pression, int turbine)
 * @brief Sauvegarde des données dans un fichier CSV
 * 
 * @param temperature_max Température maximale
 * @param temperature_min Température minimale
 * @param pression Pression atmosphérique
 * @param turbine Valeur de la turbine
 * 
 * @return void
 */
void saveDataToFile(float temperature_max, float temperature_min, float pression, int turbine) {
    
    File file = SPIFFS.open("/data.csv", "a");
    if (!file) {
        // Le fichier n'existe pas, essayez de le créer
        Serial.println("Création du fichier data.cvs");
        file = SPIFFS.open("/data.csv", "w");
        file.printf("Date;Tmax;Tmin;Pression;Turbine\n");
        if (!file) {
            Serial.println("Failed to create the file");
            return;
        }
    }
    
    file.printf("%s;%.2f;%.2f;%.2f;%d\n", currentDayStr, temperature_max, temperature_min, pression);
    file.close();
}

/**
 * @fn void readMeteoFileToSerial()
 * @brief Lecture du fichier météo et affichage dans le moniteur série
 * 
 * @return void
 */
void readMeteoFileToSerial() {
  File file = SPIFFS.open("/meteo.csv", "r");
  if (!file) {
    Serial.println("Impossible d'ouvrir le fichier meteo.csv");
    return;
  }

  while (file.available()) {
    char c = file.read();
    Serial.print(c);
  }

  file.close();
}

/**
 * @fn void readFileToSerial(String filepath)
 * @brief Lecture d'un fichier et affichage dans le moniteur série
 * 
 * @param filepath Chemin du fichier à lire
 * 
 * @return void
 */
void readFileToSerial(String filepath) {
  File file = SPIFFS.open(filepath, "r");
  if (!file) {
    Serial.println("Impossible d'ouvrir le fichier " + filepath);
    return;
  }
  Serial.println("Lecture du fichier " + filepath);

  int lineNumber = 1;

  // Afficher le contenu du fichier avec des numéros de ligne
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial.print(lineNumber);
    Serial.print(". ");
    Serial.println(line);
    lineNumber++;
  }

  file.close();
}

/**
 * @fn void modifFileToSerial(String filepath)
 * @brief Modification d'une ligne spécifique dans un fichier
 * 
 * @param filepath Chemin du fichier à modifier
 * 
 * @return void
 */
void modifFileToSerial(String filepath) {
  File file = SPIFFS.open(filepath, "r");
  if (!file) {
    Serial.println("Impossible d'ouvrir le fichier " + filepath);
    return;
  }
  Serial.println("");
  Serial.println("Lecture du fichier " + filepath);

  int lineNumber = 1;

  // Afficher le contenu du fichier avec des numéros de ligne
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial.print(lineNumber);
    Serial.print(". ");
    Serial.println(line);
    lineNumber++;
  }

  file.close();
  Serial.println("");
  Serial.println("Entrez le numéro de ligne à modifier :");
  int lineNumberToEdit = -1;

  while (lineNumberToEdit < 1 || lineNumberToEdit >= lineNumber) {
    if (Serial.available() > 0) {
      lineNumberToEdit = Serial.parseInt();
      if (lineNumberToEdit ==0) {
        Serial.println("Pas de modification de fichier");
        return;
      }
      if (lineNumberToEdit < 1 || lineNumberToEdit >= lineNumber) {
        Serial.println("Numéro de ligne invalide. Entrez un numéro de ligne valide.");
      }
    }
  }

  // Ouvrir le fichier en mode lecture pour afficher la ligne correspondante
  File readLineFile = SPIFFS.open(filepath, "r");
  if (!readLineFile) {
    Serial.println("Impossible d'ouvrir le fichier " + filepath);
    return;
  }

  lineNumber = 1;
  String lineToEdit;

  // Trouver la ligne correspondante
  while (readLineFile.available()) {
    String line = readLineFile.readStringUntil('\n');
    if (lineNumber == lineNumberToEdit) {
      lineToEdit = line;
      break;
    }
    lineNumber++;
  }
  
  readLineFile.close();

  Serial.println("La ligne à modifier est :");
  Serial.println(lineToEdit);

  Serial.println("Entrez la nouvelle ligne (terminez par '\\n') :");
  String newLine = "";

  while (true) {
    if (Serial.available() > 0) {
      char c = Serial.read(); // Lire le caractère entré par l'opérateur
      newLine += c;
      Serial.print(c); // Renvoyer le caractère
      if (c == '\n') {
        break; // Sortir de la boucle lorsqu'un retour à la ligne est saisi
      }
    }
  }

  // Ouvrir le fichier en mode écriture
  File editFile = SPIFFS.open(filepath, "r");
  File newFile = SPIFFS.open(filepath + ".new", "w");
  
  if (!editFile || !newFile) {
    Serial.println("Erreur lors de l'ouverture des fichiers");
    return;
  }

  lineNumber = 1;

  // Écrire la nouvelle ligne dans le fichier
  while (editFile.available()) {
    String line = editFile.readStringUntil('\n');
    if (lineNumber == lineNumberToEdit) {
      newFile.print(newLine); // Notez l'utilisation de print au lieu de println pour ne pas ajouter de caractère de nouvelle ligne supplémentaire
    } else {
      newFile.println(line);
    }
    lineNumber++;
  }

  editFile.close();
  newFile.close();

  // Supprimer l'ancien fichier et renommer le nouveau fichier
  SPIFFS.remove(filepath);
  SPIFFS.rename(filepath + ".new", filepath);

  Serial.println("Ligne modifiée avec succès.");

  file = SPIFFS.open(filepath, "r");
  if (!file) {
    Serial.println("Impossible d'ouvrir le fichier " + filepath);
    return;
  }
  Serial.println("Lecture du fichier " + filepath);

  lineNumber = 1;

  // Afficher le contenu du fichier avec des numéros de ligne
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial.print(lineNumber);
    Serial.print(". ");
    Serial.println(line);
    lineNumber++;
  }

  file.close();

}

/**
 * @fn void delFileToSerial(String filepath)
 * @brief Suppression d'un fichier
 * 
 * @param filepath Chemin du fichier à supprimer
 * 
 * @return void
 */
void delFileToSerial(String filepath) {
Serial.println("Suppression du fichier " + filepath);
SPIFFS.remove(filepath);
}

/**
 * @fn void SaveValueToFile()
 * @brief Sauvegarde des valeurs actuelles dans un fichier texte
 * 
 * @return void
 */
void SaveValueToFile(void) {
  Serial.println("Sauvegarde des données dans /save_data.txt");
    File file = SPIFFS.open("/save_data.txt", "w");
      if (!file) {
          Serial.println("Failed to create the file");
          return;
      }
    
    file.printf("%lu\n",val_impulsion1());
    
    // Balayage des 8 bits
    for(int i=0; i<15; i++){
      file.printf("%d,%lu,%f\n", Tab_Info_USER[i].Val_INT, Tab_Info_USER[i].Val_LONG, Tab_Info_USER[i].Val_FLOAT);
    }
    file.close();
}

/**
 * @fn void RestoreValueFromFile()
 * @brief Restauration des valeurs depuis un fichier texte
 * 
 * @return void
 */
void RestoreValueFromFile() {
  File file = SPIFFS.open("/save_data.txt", "r");

  if (!file) {
    Serial.println("Failed to open data.txt");
    return;
  }

  // Lire les valeurs du fichier
  String line = file.readStringUntil('\n');

  // Convertir la ligne en valeurs de température, pression et humidité
  unsigned long val;
  sscanf(line.c_str(), "%ld", &val);
  maj_impulsion1(val);

  int val1=0, val2=0, val3=0, val4=0;
  for(int i=1;i<15;i++){
    line = file.readStringUntil('\n');
    sscanf(line.c_str(), "%d,%lu,%f\n", &Tab_Info_USER[i].Val_INT, &Tab_Info_USER[i].Val_LONG, &Tab_Info_USER[i].Val_FLOAT);
  }

  file.close();
}