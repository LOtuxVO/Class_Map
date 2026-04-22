#ifndef POINTH
#define POINTH

// ©EdgarPullès

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>
#include <string.h>

typedef struct Eleve {
    char prenom[50];
    char nom[50];

} Eleve;

int lireFichierEleves(const char *nom_fichier, Eleve liste_eleves[], int max_eleves);

void melangerEleves(Eleve liste_eleves[], int nb_eleves);

void initTab(int rangee, int place, int** tableau);

void placerEleve(int rangee, int place, int** tableau, Eleve liste_eleves[], int nb_eleves);

/* Cette fonction est destinée à une application console et n'est pas utilisée par l'interface graphique. */
// void afficherPlanDeClasse(int rangee, int place, int** plan, Eleve liste_eleves[]);

void enregistrerPlanDeClasse(const char *nom_fichier, int rangee, int place, int** plan, Eleve liste_eleves[], int nb_eleves);

#endif