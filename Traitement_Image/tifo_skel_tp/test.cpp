#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

std::vector<std::vector<int>> lireFichierImage(const std::string& nomFichier, int nbre_ligne) {
    std::ifstream fichier(nomFichier); // Ouvre le fichier pour la lecture
    std::vector<std::vector<int>> image; // Vecteur pour stocker l'image

    if (fichier) { // Vérifie si l'ouverture du fichier a réussi
        image.resize(nbre_ligne, std::vector<int>(3)); // Redimensionne le vecteur image avec le nombre de lignes attendu

        std::string ligne; // Variable pour stocker chaque ligne lue depuis le fichier
        int ligneIndex = 0; // Indice pour suivre la ligne actuelle de l'image

        while (std::getline(fichier, ligne) && ligneIndex < nbre_ligne) { // Lit chaque ligne du fichier et vérifie la limite du nombre de lignes
            std::istringstream iss(ligne); // Crée un flux de chaîne de caractères à partir de la ligne lue
            int valeur; // Variable pour stocker chaque valeur R, G, B du pixel

            for (int j = 0; j < 3 && iss >> valeur; ++j) { // Lit chaque valeur de la ligne
                image[ligneIndex][j] = valeur; // Stocke la valeur dans le vecteur image
            }

            if (!iss.eof()) { // Vérifie si une erreur s'est produite pendant la lecture
                std::cerr << "Erreur lors de la lecture du fichier." << std::endl; // Affiche un message d'erreur
                return std::vector<std::vector<int>>(); // Retourne un vecteur vide
            }

            ++ligneIndex; // Passe à la ligne suivante de l'image
        }
        fichier.close(); // Ferme le fichier après la lecture
    }
    else {
        std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl; // Affiche un message d'erreur si l'ouverture du fichier a échoué
    }

    // Vérifie si le nombre total de lignes lues correspond au nombre de lignes attendu de l'image
    if (image.size() != nbre_ligne) {
        std::cerr << "Le format de l'image n'est pas valide." << std::endl; // Affiche un message d'erreur si le format de l'image est incorrect
        return std::vector<std::vector<int>>(); // Retourne un vecteur vide
    }

    return image; // Retourne le vecteur image après la lecture réussie du fichier
}

int main() {
    const int nbre_ligne = 480 * 360; // Nombre de lignes de l'image
    std::vector<std::vector<int>> image = lireFichierImage("frame_rgb", nbre_ligne); // Utilisation de la fonction pour lire le fichier et stocker l'image dans un vecteur

    // Affichage des 5 premières lignes de l'image (juste pour vérification)
    for (int i = 0; i < 5; ++i) {
        std::cout << "Ligne " << i << " : ";
        for (int valeur : image[i]) {
            std::cout << valeur << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
