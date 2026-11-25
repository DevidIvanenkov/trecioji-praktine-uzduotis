#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

const int MAKS_IRASU = 100;

bool arTinkamaKategorija(const string &kategorija)
{
    if (kategorija == "Dviraciai" || kategorija == "Dalys" || kategorija == "Aksesuarai")
    {
        return true;
    }
    return false;
}

bool iskaidytiEilute(
    const string &eilute,
    string &kategorija,
    string &pavadinimas,
    double &kaina,
    int &kiekis
)
{
    size_t k1 = eilute.find(';');
    if (k1 == string::npos)
    {
        return false;
    }

    size_t k2 = eilute.find(';', k1 + 1);
    if (k2 == string::npos)
    {
        return false;
    }

    size_t k3 = eilute.find(';', k2 + 1);
    if (k3 == string::npos)
    {
        return false;
    }

    kategorija = eilute.substr(0, k1);
    pavadinimas = eilute.substr(k1 + 1, k2 - (k1 + 1));
    string kainaTekstu = eilute.substr(k2 + 1, k3 - (k2 + 1));
    string kiekisTekstu = eilute.substr(k3 + 1);

    if (!kiekisTekstu.empty() && kiekisTekstu[kiekisTekstu.size() - 1] == '\r')
    {
        kiekisTekstu.erase(kiekisTekstu.size() - 1);
    }

    if (kategorija.empty() || pavadinimas.empty() || kainaTekstu.empty() || kiekisTekstu.empty())
    {
        return false;
    }

    for (size_t i = 0; i < kiekisTekstu.size(); i++)
    {
        if (kiekisTekstu[i] < '0' || kiekisTekstu[i] > '9')
        {
            return false;
        }
    }

    char *pabaigaKaina = nullptr;
    kaina = strtod(kainaTekstu.c_str(), &pabaigaKaina);
    if (pabaigaKaina == kainaTekstu.c_str() || *pabaigaKaina != '\0')
    {
        return false;
    }

    char *pabaigaKiekis = nullptr;
    long kiekisIlgas = strtol(kiekisTekstu.c_str(), &pabaigaKiekis, 10);
    if (pabaigaKiekis == kiekisTekstu.c_str() || *pabaigaKiekis != '\0')
    {
        return false;
    }

    if (kaina <= 0.0 || kiekisIlgas <= 0 || kiekisIlgas > 1000000000L)
    {
        return false;
    }

    kiekis = static_cast<int>(kiekisIlgas);

    if (!arTinkamaKategorija(kategorija))
    {
        return false;
    }

    return true;
}

bool nuskaitytiUzsakyma(
    const string &failoVardas,
    string kategorijos[],
    string pavadinimai[],
    double kainos[],
    int kiekiai[],
    int &irasuSkaicius
)
{
    ifstream fin;
    fin.open(failoVardas.c_str());

    if (fin.fail())
    {
        cout << "Klaida: nepavyko atverti failo '" << failoVardas << "'." << endl;
        return false;
    }

    int n = 0;
    if (!(fin >> n))
    {
        cout << "Klaida: nepavyko nuskaityti prekiu irasu skaiciaus." << endl;
        fin.close();
        return false;
    }

    if (n < 1 || n > MAKS_IRASU)
    {
        cout << "Klaida: prekiu skaicius (" << n << ") turi buti nuo 1 iki 100." << endl;
        fin.close();
        return false;
    }

    string eilute;
    getline(fin, eilute);

    int nuskaityta = 0;
    while (nuskaityta < n && getline(fin, eilute))
    {
        if (eilute.empty() || (eilute.size() == 1 && eilute[0] == '\r'))
        {
            continue;
        }

        string kat, pav;
        double kaina = 0.0;
        int kiek = 0;

        if (!iskaidytiEilute(eilute, kat, pav, kaina, kiek))
        {
            cout << "Klaida: netinkami duomenys " << (nuskaityta + 1) << "-oje prekes eiluteje." << endl;
            fin.close();
            return false;
        }

        kategorijos[nuskaityta] = kat;
        pavadinimai[nuskaityta] = pav;
        kainos[nuskaityta] = kaina;
        kiekiai[nuskaityta] = kiek;
        nuskaityta++;
    }

    fin.close();

    if (nuskaityta != n)
    {
        cout << "Klaida: nuskaityta tik " << nuskaityta << " irasu is " << n << "." << endl;
        return false;
    }

    irasuSkaicius = nuskaityta;
    return true;
}

int main()
{
    string kategorijos[MAKS_IRASU];
    string pavadinimai[MAKS_IRASU];
    double kainos[MAKS_IRASU];
    int kiekiai[MAKS_IRASU];
    int irasuSkaicius = 0;

    if (!nuskaitytiUzsakyma("uzsakymas.txt", kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius))
    {
        cout << "Programa baigia darba, nes nepavyko nuskaityti duomenu." << endl;
        return 1;
    }

    cout << "Sekmingai nuskaityta prekiu irasu: " << irasuSkaicius << endl;
    return 0;
}