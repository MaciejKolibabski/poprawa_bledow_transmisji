#include <iostream>
#include <cstring>
#include <algorithm>
#include <string>

using namespace std;

int  macierz_kontrolna[8][16] = {   // dla 2 bledow
        //Macierz kontrolna pochodzi z generetora (link poniżej):
        //http://www.ece.unb.ca/cgi-bin/tervo/polygen2.pl
        {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1}
};

int rozmiar =8;

using namespace std;

void zamiana_na_ASCII(FILE *plik, int tablica[]) {
    int a = 128;
    char kod = 0;
    for (int i = 0; i < rozmiar; i++) {
        kod += a * tablica[i]; //zamiana na ascii
        a /= 2;
    }
    fputc(kod, plik); //zapis do pliku w postaci ascii wiadomosci
}

void zamiana_na_binarne(int tab[], int a) {

    for (int i = rozmiar - 1; i > -1; i--) {
        tab[i] = a % 2;  //zamiana na binarne znakow tabvlicy tab_wiadomosc
        a /= 2;
    }
}

void kodowanie(int macierz_kontrolna [8][16], FILE *wiadomosc_wejciowa, FILE *zakodowane2) {
    int znak;
    int tab_wiadomosc[rozmiar]; //znaki/litery
    int tab[rozmiar]; //bity parzystosci

    while ((znak = fgetc(wiadomosc_wejciowa)) != EOF) {
        for (int i = 0; i < rozmiar; i++) {
            tab[i] = 0;             //wyzerowanie tablicy bitów parzystoœci
        }

        zamiana_na_binarne(tab_wiadomosc, znak);

        for (int i = 0; i < rozmiar; i++) {
            for (int j = 0; j < rozmiar; j++) {
                tab[i] += tab_wiadomosc[j] * macierz_kontrolna[i][j]; //zliczanie bitow parzystoœci poprzez mnozenie przez macierz kontrolna
            }
            tab[i] %= 2; //zapis w postaci zer i jedynek poprzez 2
        }

        zamiana_na_ASCII( zakodowane2, tab_wiadomosc); //tab_wiadomosc to nasze znaki
        zamiana_na_ASCII( zakodowane2, tab); //tab to tablica bitow parzystosci

    }
    cout << endl;
    cout << "Zakodowanie powiodlo sie";
}

void odkoduj(int macierz_kontrolna[8][16],FILE *zakodowana2, FILE *odkodowana) {

    int tab_zakodowana[(rozmiar * 2)]; // W TABLICY SA WIERSZE Z PLIKU CODED
    int tab_bledow[rozmiar]; // TABLICA ZAWIERA BLEDY Z DANEGO WIERSZA
    int znak;
    int licz_bledow = 0;
    int tab_wiadomosc[rozmiar];

    while ( true ) {

        znak=fgetc(zakodowana2);
        if (znak==EOF) {
            break;
        }

        zamiana_na_binarne(tab_wiadomosc, znak); //odczyt znaku
        for(int i=0;i<rozmiar;i++){
            tab_zakodowana[i]=tab_wiadomosc[i];
        }

        znak=fgetc(zakodowana2);
        zamiana_na_binarne(tab_wiadomosc, znak);  //odczyt bitów parzystości

        for(int i=0;i<rozmiar;i++){
            tab_zakodowana[i+8]=tab_wiadomosc[i];
        }

        int blad1;
        int blad2;

        for (int i = 0; i < rozmiar; i++) {
            tab_bledow[i] = 0;
            for (int j = 0; j < (rozmiar * 2); j++) {
                tab_bledow[i] += tab_zakodowana[j] * macierz_kontrolna[i][j];
            }
            tab_bledow[i] %= 2; // jesli koncowka 0 to ok, jesli 1 to jest blad

            if (tab_bledow[i] == 1) {
                licz_bledow = 1;
            }
        }

        if (licz_bledow != 0) {
            int ilosc_bledow = 0;
            for (int i = 0; i < 15; i++) {
                for (int j = i + 1; j < (rozmiar * 2); j++) {
                    ilosc_bledow = 1;
                    for (int k = 0; k < rozmiar; k++) {
                        if ((tab_bledow[k] != macierz_kontrolna[k][i]) ^ macierz_kontrolna[k][j]) {
                            ilosc_bledow = 0;
                            break;
                        }
                    }
                    if (ilosc_bledow == 1) { //2bledy
                        blad1 = i;
                        blad2 = j;
                        tab_zakodowana[blad1] = !tab_zakodowana[blad1]; //zamiana błędnych bitów na poprawne
                        tab_zakodowana[blad2] = !tab_zakodowana[blad2]; //zamiana błędnych bitów na poprawne
                        i = (rozmiar * 2);
                        break;
                    }
                }
            }
            if (ilosc_bledow == 0) {     // 1 blad
                for (int i = 0; i < (rozmiar * 2); i++) {
                    for (int j = 0; j < rozmiar; j++) {
                        if (macierz_kontrolna[j][i] != tab_bledow[j]) { //szukanie identycznej kolumny naszej tablicy z macierz¹ kontroln¹
                            break;
                        }
                        if(j == 7) { //czemu do 7
                            tab_zakodowana[i] = !tab_zakodowana[i]; //jeœli zosta³a znaleziona, to zamiana na 0/1 lub 1/0
                            i = (rozmiar * 2); // Jesli blad zosta³ znaleziony to konczymy petle
                        }
                    }
                }
            }
        }
        licz_bledow = 0;
        zamiana_na_ASCII(odkodowana, tab_zakodowana);
    }
}

int main() {
    cout<<"Autorzy: "<<endl;
    cout<<"Maciej Kolibabski nr.indeksu: 229916"<<endl;
    cout<<"Kacper Swiader nr.indeksu: 230023"<<endl;
    int wybor;
    string nazwa_pliku;
    cout <<"1. Zakoduj wiadomosc"<<endl;
    cout<< "2. Odkoduj wiadomosc "<<endl;
    cout<< "Wpisz opcje: ";
    cin>>wybor;

    switch (wybor) {
        case 1: {
            nazwa_pliku = "wiadomosc.txt";
            char plik[15];
            strcpy(plik, nazwa_pliku.c_str());
            FILE *zakodowanyplik;
            zakodowanyplik = fopen(plik, "rb");

            if (zakodowanyplik == nullptr) {
                cout << "Wystapil blad z plikiem";
                return EXIT_FAILURE;
            }
            FILE *kod2 = fopen("zakodowane.txt", "wb");
            kodowanie(macierz_kontrolna , zakodowanyplik, kod2);
            fclose(zakodowanyplik);
            break;
        }
        case 2: {
            FILE *zakodowana2 = fopen("zakodowane.txt", "rwb");
            FILE *odkodowana = fopen("odkodowane.txt", "wb");
            odkoduj(macierz_kontrolna, zakodowana2, odkodowana);
            cout << "\n Plik odkodowany poprawnie \n";
            fclose(odkodowana);

            break;
        }
        default:
            cout<<"Nie ma takiej opcji ";
    }
    return 0;
}