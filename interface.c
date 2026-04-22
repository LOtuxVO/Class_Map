#include "pointH.h"

// ©EdgarPullès
// gcc interface.c fonctions.c -o interface.exe -mwindows

#define ID_BTN_FILE 101
#define ID_EDIT_ROWS 102
#define ID_EDIT_SEATS 103
#define ID_BTN_GEN 104
#define ID_LBL_FILE 105 
#define ID_BTN_SAVE 106 // Bouton pour enregistrer le fichier de sortie
#define ID_LBL_OUTPUT_FILE 107

HWND hEditRows, hEditSeats, hLabelFile, hLabelOutputFile;
char szInputFileName[MAX_PATH] = ""; 
char szOutputFileName[MAX_PATH] = "sortie.txt"; // Nom du fichier de sortie par défaut modifiable a la sauvegarde

void OnGenerate(HWND hwnd) {
    char buffer[256];
    int rangee, place;
    char *endptr;
    
    GetWindowText(hEditRows, buffer, 256);
    rangee = (int)strtol(buffer, &endptr, 10);
    if (endptr == buffer || *endptr != '\0' || rangee <= 0) {
        MessageBox(hwnd, "Veuillez entrer un nombre valide et positif pour les rangees.", "Erreur de saisie", MB_ICONERROR);
        return;
    }
    
    GetWindowText(hEditSeats, buffer, 256);
    place = (int)strtol(buffer, &endptr, 10);
    if (endptr == buffer || *endptr != '\0' || place <= 0) {
        MessageBox(hwnd, "Veuillez entrer un nombre valide et positif pour les places.", "Erreur de saisie", MB_ICONERROR);
        return;
    }

    if (rangee > 100 || place > 100) { // limites
        MessageBox(hwnd, "Les dimensions de la salle sont trop grandes (max 100x100).", "Erreur de saisie", MB_ICONERROR);
        return;
    }

    if (strlen(szInputFileName) == 0) {
        MessageBox(hwnd, "Veuillez selectionner un fichier liste (.txt).", "Erreur", MB_ICONERROR);
        return;
    }

    int MAX_ELEVES = rangee * place;

    Eleve *liste_eleves = (Eleve *)malloc(sizeof(Eleve) * MAX_ELEVES);
    if (!liste_eleves) {
        MessageBox(hwnd, "Erreur d'allocation memoire.", "Erreur", MB_ICONERROR);
        return;
    }

    int nb_eleves = lireFichierEleves(szInputFileName, liste_eleves, MAX_ELEVES);

    if (nb_eleves == 0) {
        MessageBox(hwnd, "Aucun eleve lu ou erreur lors de l'ouverture du fichier.", "Erreur", MB_ICONERROR);
        free(liste_eleves);
        return;
    }
    if (nb_eleves == MAX_ELEVES) {
        MessageBox(hwnd, "Attention: Le nombre d'eleves lus atteint la capacite maximale de la salle. Certains eleves pourraient ne pas avoir ete lus si le fichier en contient plus.", "Avertissement", MB_ICONWARNING);
    }

    melangerEleves(liste_eleves, nb_eleves);

    // Allocation dynamique du tableau 2D
    int** tableau = (int**)malloc(rangee * sizeof(int*));
    if (!tableau) {
        MessageBox(hwnd, "Erreur d'allocation memoire pour le tableau de classe.", "Erreur", MB_ICONERROR);
        free(liste_eleves);
        return;
    }
    for (int i = 0; i < rangee; i++) {
        tableau[i] = (int*)malloc(place * sizeof(int));
        if (!tableau[i]) {
            MessageBox(hwnd, "Erreur d'allocation memoire pour le tableau de classe.", "Erreur", MB_ICONERROR);
            for (int j = 0; j < i; j++) free(tableau[j]);
            free(tableau);
            free(liste_eleves);
            return;
        }
    }

    initTab(rangee, place, tableau);
    placerEleve(rangee, place, tableau, liste_eleves, nb_eleves);
    
    enregistrerPlanDeClasse(szOutputFileName, rangee, place, tableau, liste_eleves, nb_eleves);
    free(liste_eleves);

    // Libération de la mémoire du tableau
    for (int i = 0; i < rangee; i++) {
        free(tableau[i]);
    }
    free(tableau);
    
    char success_msg[MAX_PATH + 100];
    snprintf(success_msg, sizeof(success_msg), "Le plan de classe a ete genere avec succes dans '%s' !", szOutputFileName);
    MessageBox(hwnd, success_msg, "Succes", MB_ICONINFORMATION);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
            // Fichier d'entrée
            CreateWindow("STATIC", "Fichier liste :", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
            hLabelFile = CreateWindow("STATIC", "Aucun fichier selectionne", WS_VISIBLE | WS_CHILD, 120, 20, 250, 20, hwnd, (HMENU)ID_LBL_FILE, NULL, NULL);
            CreateWindow("BUTTON", "...", WS_VISIBLE | WS_CHILD, 380, 15, 30, 30, hwnd, (HMENU)ID_BTN_FILE, NULL, NULL);

            // Dimensions de la salle
            CreateWindow("STATIC", "Rangees :", WS_VISIBLE | WS_CHILD, 20, 60, 80, 20, hwnd, NULL, NULL, NULL);
            hEditRows = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 100, 60, 50, 20, hwnd, (HMENU)ID_EDIT_ROWS, NULL, NULL);

            CreateWindow("STATIC", "Places/R :", WS_VISIBLE | WS_CHILD, 170, 60, 80, 20, hwnd, NULL, NULL, NULL);
            hEditSeats = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 250, 60, 50, 20, hwnd, (HMENU)ID_EDIT_SEATS, NULL, NULL);

            // Fichier de sortie
            CreateWindow("STATIC", "Fichier sortie :", WS_VISIBLE | WS_CHILD, 20, 100, 100, 20, hwnd, NULL, NULL, NULL);
            hLabelOutputFile = CreateWindow("STATIC", szOutputFileName, WS_VISIBLE | WS_CHILD, 120, 100, 250, 20, hwnd, (HMENU)ID_LBL_OUTPUT_FILE, NULL, NULL);
            CreateWindow("BUTTON", "...", WS_VISIBLE | WS_CHILD, 380, 95, 30, 30, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);

            // Bouton de génération
            CreateWindow("BUTTON", "Generer le plan d'examen", WS_VISIBLE | WS_CHILD, 20, 140, 390, 40, hwnd, (HMENU)ID_BTN_GEN, NULL, NULL);

            // Mention de copyright en bas a droite
            CreateWindow("STATIC", "\251EdgarPull\350s", WS_VISIBLE | WS_CHILD | SS_RIGHT, 310, 185, 100, 20, hwnd, NULL, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_FILE) {
                OPENFILENAME ofn;
                char szFile[260];
                
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = "Fichiers Texte (*.txt)\0*.txt\0Tous les fichiers (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn) == TRUE) {
                    strncpy(szInputFileName, ofn.lpstrFile, MAX_PATH - 1);
                    szInputFileName[MAX_PATH - 1] = '\0'; // Assurer la null-termination
                    SetWindowText(hLabelFile, szInputFileName);
                }
            }
            else if (LOWORD(wParam) == ID_BTN_SAVE) {
                OPENFILENAME ofn;
                char szFile[MAX_PATH];
                
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                strncpy(szFile, szOutputFileName, MAX_PATH - 1); // Pré-remplir avec le nom actuel
                szFile[MAX_PATH - 1] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = "Fichiers Texte (*.txt)\0*.txt\0Tous les fichiers (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                if (GetSaveFileName(&ofn) == TRUE) {
                    strncpy(szOutputFileName, ofn.lpstrFile, MAX_PATH - 1);
                    szOutputFileName[MAX_PATH - 1] = '\0';
                    SetWindowText(hLabelOutputFile, szOutputFileName);
                }
            }
            else if (LOWORD(wParam) == ID_BTN_GEN) {
                OnGenerate(hwnd);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    srand((unsigned int)time(NULL)); // Initialiser le générateur de nombres aléatoires une seule fois

    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = "InterfaceApp";
    wc.lpfnWndProc = WindowProcedure;
    if (!RegisterClass(&wc)) return -1;

    CreateWindow("InterfaceApp", "Generateur Plan de Classe", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 450, 240, NULL, NULL, hInst, NULL); // Ajuster la taille de la fenêtre

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
