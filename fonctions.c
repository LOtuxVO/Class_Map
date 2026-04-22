#include "pointH.h"

// ©EdgarPullès

int lireFichierEleves(const char *nom_fichier, Eleve liste_eleves[], int max_eleves) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        return 0;
    }

    int nb_eleves = 0;
    while (nb_eleves < max_eleves && fscanf(fichier, "%49s %49s", liste_eleves[nb_eleves].prenom, liste_eleves[nb_eleves].nom) == 2) { // NOLINT
        nb_eleves++;
    }

    fclose(fichier);
    return nb_eleves;
}

void melangerEleves(Eleve liste_eleves[], int nb_eleves) {
    if (nb_eleves > 1) {
        for (int i = nb_eleves - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Eleve temp = liste_eleves[i];
            liste_eleves[i] = liste_eleves[j];
            liste_eleves[j] = temp;
        }
    }
}

void initTab(int rangee, int place, int** tableau) {
    for (int i = 0; i < rangee; i++) {
        for (int j = 0; j < place; j++) {
            tableau[i][j] = -1;
        }
    }
}

void placerEleve(int rangee, int place, int** tableau, Eleve liste_eleves[], int nb_eleves) {
    if (nb_eleves > rangee * place) {
        // Message d'avertissement géré par l'interface graphique
    }
    int eleves_a_placer = (nb_eleves > rangee * place) ? (rangee * place) : nb_eleves;

    if (eleves_a_placer == 0) {
        return;
    }

    // Allocation dynamique pour les eleves
    int (*primary_candidates)[2] = (int (*)[2])malloc(rangee * place * sizeof(*primary_candidates));
    int (*final_candidates)[2] = (int (*)[2])malloc(rangee * place * sizeof(*final_candidates));
    if (!primary_candidates || !final_candidates) {
        // Gere l'erreur d'allocation mémoire
        if (primary_candidates) free(primary_candidates);
        if (final_candidates) free(final_candidates);
        return;
    }

    for (int k = 0; k < eleves_a_placer; k++) {
        int num_primary_candidates = 0;
        int max_min_dist_sq = -1;
        for (int r = 0; r < rangee; r++) {
            for (int c = 0; c < place; c++) {
                if (tableau[r][c] != -1) continue;

                int current_min_dist_sq = 2000000;

                if (k == 0) {
                    current_min_dist_sq = 1; // Le premier élève peut aller n'importe où, on initialise pour qu'il soit choisi
                } 
                else {
                    for (int r2 = 0; r2 < rangee; r2++) {
                        for (int c2 = 0; c2 < place; c2++) {
                            if (tableau[r2][c2] != -1) {
                                int d_sq = (r - r2) * (r - r2) + (c - c2) * (c - c2);
                                if (d_sq < current_min_dist_sq) {
                                    current_min_dist_sq = d_sq;
                                }
                            }
                        }
                    }
                }

                if (current_min_dist_sq > max_min_dist_sq) {
                    max_min_dist_sq = current_min_dist_sq;
                    num_primary_candidates = 0;
                    primary_candidates[num_primary_candidates][0] = r;
                    primary_candidates[num_primary_candidates][1] = c;
                    num_primary_candidates++;
                } else if (current_min_dist_sq == max_min_dist_sq) {
                    primary_candidates[num_primary_candidates][0] = r;
                    primary_candidates[num_primary_candidates][1] = c;
                    num_primary_candidates++;
                }
            }
        }

        if (num_primary_candidates > 0) {
            int min_diagonal_neighbors = 2000000;
            int num_final_candidates = 0;

            for (int i = 0; i < num_primary_candidates; i++) {
                int r_cand = primary_candidates[i][0];
                int c_cand = primary_candidates[i][1];
                int current_diagonal_neighbors = 0;

                for (int r2 = 0; r2 < rangee; r2++) {
                    for (int c2 = 0; c2 < place; c2++) {
                        if (tableau[r2][c2] != -1) {
                            if ((r_cand - r2) * (r_cand - r2) == 1 && (c_cand - c2) * (c_cand - c2) == 1) {
                                current_diagonal_neighbors++;
                            }
                        }
                    }
                }

                if (current_diagonal_neighbors < min_diagonal_neighbors) {
                    min_diagonal_neighbors = current_diagonal_neighbors;
                    num_final_candidates = 0;
                    final_candidates[num_final_candidates][0] = r_cand;
                    final_candidates[num_final_candidates][1] = c_cand;
                    num_final_candidates++;
                } else if (current_diagonal_neighbors == min_diagonal_neighbors) {
                    final_candidates[num_final_candidates][0] = r_cand;
                    final_candidates[num_final_candidates][1] = c_cand;
                    num_final_candidates++;
                }
            }

            int r_final = final_candidates[0][0];
            int c_final = final_candidates[0][1];

            tableau[r_final][c_final] = k;
            // La sortie est maintenant gérée par enregistrerPlanDeClasse
        }
    }
    free(primary_candidates);
    free(final_candidates);
}

/* La fonction afficherPlanDeClasse est supprimée car elle est destinée à une application console
   et n'est pas utilisée par l'interface graphique. */
// void afficherPlanDeClasse(...) { ... }

void enregistrerPlanDeClasse(const char *nom_fichier, int rangee, int place, int** plan, Eleve liste_eleves[], int nb_eleves) {
    FILE *fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        // Dans une application GUI, on ne doit pas utiliser perror ou printf.
        // L'erreur sera gérée par l'appelant (interface.c) via MessageBox.
        return;
    }

    int eleves_a_placer = (nb_eleves > rangee * place) ? (rangee * place) : nb_eleves;

    fprintf(fichier, "--- Placement des eleves ---\n");

    if (eleves_a_placer > 0) {
        typedef struct {
            int r, c;
        } Position;
        Position positions[eleves_a_placer];

        for (int r = 0; r < rangee; r++) {
            for (int c = 0; c < place; c++) {
                if (plan[r][c] != -1) {
                    int eleve_index = plan[r][c];
                    if (eleve_index < eleves_a_placer) {
                        positions[eleve_index].r = r;
                        positions[eleve_index].c = c;
                    }
                }
            }
        }

        for (int k = 0; k < eleves_a_placer; k++) {
            fprintf(fichier, "Eleve : %s %s - rangee : %d - siege : %d\n", 
                    liste_eleves[k].prenom, liste_eleves[k].nom, positions[k].r + 1, positions[k].c + 1);
        }
    }

    for (int i = 0; i < rangee; i++) {
        for (int k = 0; k < place; k++) {
            fprintf(fichier, "+--------------------");
        }
        fprintf(fichier, "+\n");
        for (int j = 0; j < place; j++) {
            int eleve_index = plan[i][j];
            if (eleve_index != -1) {
                char nom_complet[102];
                snprintf(nom_complet, sizeof(nom_complet), "%s %s", liste_eleves[eleve_index].prenom,
                         liste_eleves[eleve_index].nom);
                fprintf(fichier, "| %-18.18s ", nom_complet);
            } else {
                fprintf(fichier, "| %-18s ", "Vide");
            }
        }
        fprintf(fichier, "|\n");
    }
    for (int k = 0; k < place; k++) {
        fprintf(fichier, "+--------------------");
    }
    fprintf(fichier, "+\n");

    fclose(fichier); // NOLINT
}
