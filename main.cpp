#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
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

double skaiciuotiEilutesSumaPriesNuolaida(double kaina, int kiekis)
{
    return kaina * kiekis;
}

double skaiciuotiEilutesNuolaida(double kaina, int kiekis)
{
    double sumaPries = skaiciuotiEilutesSumaPriesNuolaida(kaina, kiekis);
    if (kiekis >= 10)
    {
        return sumaPries * 0.10;
    }
    return 0.0;
}

double skaiciuotiEilutesMoketinaSuma(double kaina, int kiekis)
{
    return skaiciuotiEilutesSumaPriesNuolaida(kaina, kiekis) - skaiciuotiEilutesNuolaida(kaina, kiekis);
}

void skaiciuotiVisoUzsakymoSumas(
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius,
    double &visoPriesNuolaidas,
    double &visoNuolaidos,
    double &visoMoketina
)
{
    visoPriesNuolaidas = 0.0;
    visoNuolaidos = 0.0;
    visoMoketina = 0.0;

    for (int i = 0; i < irasuSkaicius; i++)
    {
        double eilPries = skaiciuotiEilutesSumaPriesNuolaida(kainos[i], kiekiai[i]);
        double eilNuol = skaiciuotiEilutesNuolaida(kainos[i], kiekiai[i]);
        double eilMok = eilPries - eilNuol;

        visoPriesNuolaidas += eilPries;
        visoNuolaidos += eilNuol;
        visoMoketina += eilMok;
    }
}

double skaiciuotiKategorijosMoketinaSuma(
    const string kategorijos[],
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius,
    const string &kategorija
)
{
    double suma = 0.0;

    for (int i = 0; i < irasuSkaicius; i++)
    {
        if (kategorijos[i] == kategorija)
        {
            suma += skaiciuotiEilutesMoketinaSuma(kainos[i], kiekiai[i]);
        }
    }

    return suma;
}

void isvestiPrekiuLentele(
    ostream &out,
    const string kategorijos[],
    const string pavadinimai[],
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius
)
{
    out << fixed << setprecision(2);
    out << "Nr. | Kategorija | Pavadinimas | Kaina (Eur) | Kiekis | Suma pries nuol. (Eur) | Nuolaida (Eur) | Moketina (Eur)" << endl;
    out << "----------------------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < irasuSkaicius; i++)
    {
        double sumaPries = skaiciuotiEilutesSumaPriesNuolaida(kainos[i], kiekiai[i]);
        double nuolaida = skaiciuotiEilutesNuolaida(kainos[i], kiekiai[i]);
        double moketina = sumaPries - nuolaida;

        out << (i + 1) << ". | "
            << kategorijos[i] << " | "
            << pavadinimai[i] << " | "
            << kainos[i] << " | "
            << kiekiai[i] << " | "
            << sumaPries << " | "
            << nuolaida << " | "
            << moketina << endl;
    }
}

void perziuretiKategorija(
    const string kategorijos[],
    const string pavadinimai[],
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius,
    const string &kategorija
)
{
    cout << fixed << setprecision(2);
    cout << endl << "Kategorija: " << kategorija << endl;

    int rasta = 0;
    for (int i = 0; i < irasuSkaicius; i++)
    {
        if (kategorijos[i] == kategorija)
        {
            double sumaPries = skaiciuotiEilutesSumaPriesNuolaida(kainos[i], kiekiai[i]);
            double nuolaida = skaiciuotiEilutesNuolaida(kainos[i], kiekiai[i]);
            double moketina = sumaPries - nuolaida;

            rasta++;
            cout << rasta << ". " << pavadinimai[i]
                 << " | Kaina: " << kainos[i] << " Eur"
                 << " | Kiekis: " << kiekiai[i]
                 << " | Suma pries nuolaida: " << sumaPries << " Eur"
                 << " | Nuolaida: " << nuolaida << " Eur"
                 << " | Moketina: " << moketina << " Eur" << endl;
        }
    }

    if (rasta == 0)
    {
        cout << "Pasirinktoje kategorijoje prekiu nera." << endl;
    }

    double katSuma = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, kategorija
    );
    cout << "Kategorijos '" << kategorija << "' moketina suma po nuolaidu: " << katSuma << " Eur" << endl;
}

void perziuretiVisoUzsakymoSumas(
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius
)
{
    double visoPries = 0.0;
    double visoNuol = 0.0;
    double visoMok = 0.0;

    skaiciuotiVisoUzsakymoSumas(kainos, kiekiai, irasuSkaicius, visoPries, visoNuol, visoMok);

    cout << fixed << setprecision(2);
    cout << endl << "--- Viso uzsakymo sumos ---" << endl;
    cout << "Suma pries nuolaidas:  " << visoPries << " Eur" << endl;
    cout << "Bendra nuolaidu suma:  " << visoNuol << " Eur" << endl;
    cout << "Galutine moketina suma: " << visoMok << " Eur" << endl;
}

void perziuretiSumasPagalKategorijas(
    const string kategorijos[],
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius
)
{
    double sumaDviraciai = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, "Dviraciai"
    );
    double sumaDalys = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, "Dalys"
    );
    double sumaAksesuarai = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, "Aksesuarai"
    );

    cout << fixed << setprecision(2);
    cout << endl << "--- Moketinos sumos pagal kategorijas (po nuolaidu) ---" << endl;
    cout << "Dviraciai:  " << sumaDviraciai << " Eur" << endl;
    cout << "Dalys:      " << sumaDalys << " Eur" << endl;
    cout << "Aksesuarai: " << sumaAksesuarai << " Eur" << endl;
}

void isvestiVisaAtaskaita(
    ostream &out,
    const string kategorijos[],
    const string pavadinimai[],
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius
)
{
    double visoPries = 0.0;
    double visoNuol = 0.0;
    double visoMok = 0.0;

    skaiciuotiVisoUzsakymoSumas(kainos, kiekiai, irasuSkaicius, visoPries, visoNuol, visoMok);

    double sumaDviraciai = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, "Dviraciai"
    );
    double sumaDalys = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, "Dalys"
    );
    double sumaAksesuarai = skaiciuotiKategorijosMoketinaSuma(
        kategorijos, kainos, kiekiai, irasuSkaicius, "Aksesuarai"
    );

    out << fixed << setprecision(2);
    out << "================================================================================================================" << endl;
    out << "                          DVIRACIU PARDUOTUVES UZSAKYMO ATASKAITA" << endl;
    out << "================================================================================================================" << endl;
    isvestiPrekiuLentele(out, kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius);
    out << "----------------------------------------------------------------------------------------------------------------" << endl;
    out << "Viso uzsakymo suma pries nuolaidas: " << visoPries << " Eur" << endl;
    out << "Bendra nuolaidu suma:               " << visoNuol << " Eur" << endl;
    out << "Galutine moketina suma:            " << visoMok << " Eur" << endl;
    out << "----------------------------------------------------------------------------------------------------------------" << endl;
    out << "Kategoriju suvestine (moketina suma po nuolaidu):" << endl;
    out << "  Dviraciai:  " << sumaDviraciai << " Eur" << endl;
    out << "  Dalys:      " << sumaDalys << " Eur" << endl;
    out << "  Aksesuarai: " << sumaAksesuarai << " Eur" << endl;
    out << "================================================================================================================" << endl;
}

bool issaugotiAtaskaitaIFaila(
    const string &failoVardas,
    const string kategorijos[],
    const string pavadinimai[],
    const double kainos[],
    const int kiekiai[],
    int irasuSkaicius
)
{
    ofstream fout;
    fout.open(failoVardas.c_str(), ios::out | ios::trunc);

    if (fout.fail())
    {
        cout << "Klaida: nepavyko atverti failo '" << failoVardas << "' irasymui." << endl;
        return false;
    }

    isvestiVisaAtaskaita(fout, kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius);

    if (fout.fail())
    {
        cout << "Klaida: irasant ataskaita i faila '" << failoVardas << "'." << endl;
        fout.close();
        return false;
    }

    fout.close();
    if (fout.fail())
    {
        cout << "Klaida: uzdarant faila '" << failoVardas << "'." << endl;
        return false;
    }

    return true;
}

string pasirinktiKategorijaMeniu()
{
    int pasirinkimas = 0;
    while (true)
    {
        cout << endl << "Pasirinkite kategorija:" << endl;
        cout << "1. Dviraciai" << endl;
        cout << "2. Dalys" << endl;
        cout << "3. Aksesuarai" << endl;
        cout << "Jusu pasirinkimas: ";

        if (!(cin >> pasirinkimas))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Netinkama ivestis. Iveskite skaiciu nuo 1 iki 3." << endl;
            continue;
        }

        switch (pasirinkimas)
        {
            case 1:
                return "Dviraciai";
            case 2:
                return "Dalys";
            case 3:
                return "Aksesuarai";
            default:
                cout << "Tokios kategorijos nera. Bandykite dar karta." << endl;
                break;
        }
    }
}

void isvestiMeniu()
{
    cout << endl << "=== DVIRACIU PARDUOTUVES UZSAKYMO SKAICIUOKLE ===" << endl;
    cout << "1. Perziureti visas uzsakymo prekes." << endl;
    cout << "2. Perziureti pasirinktos kategorijos prekes." << endl;
    cout << "3. Perziureti viso uzsakymo sumas." << endl;
    cout << "4. Perziureti sumas pagal kategorijas." << endl;
    cout << "5. Parodyti visa uzsakymo ataskaita." << endl;
    cout << "6. Issaugoti visa ataskaita i faila." << endl;
    cout << "7. Iseiti." << endl;
    cout << "Pasirinkite punkta (1-7): ";
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

    int pasirinkimas = 0;
    do
    {
        isvestiMeniu();

        if (!(cin >> pasirinkimas))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Netinkamas pasirinkimas. Iveskite skaiciu nuo 1 iki 7." << endl;
            pasirinkimas = -1;
            continue;
        }

        switch (pasirinkimas)
        {
            case 1:
                cout << endl;
                isvestiPrekiuLentele(cout, kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius);
                break;

            case 2:
            {
                string kat = pasirinktiKategorijaMeniu();
                perziuretiKategorija(kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius, kat);
                break;
            }

            case 3:
                perziuretiVisoUzsakymoSumas(kainos, kiekiai, irasuSkaicius);
                break;

            case 4:
                perziuretiSumasPagalKategorijas(kategorijos, kainos, kiekiai, irasuSkaicius);
                break;

            case 5:
                cout << endl;
                isvestiVisaAtaskaita(cout, kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius);
                break;

            case 6:
                if (issaugotiAtaskaitaIFaila("ataskaita.txt", kategorijos, pavadinimai, kainos, kiekiai, irasuSkaicius))
                {
                    cout << "Ataskaita sekmingai issaugota faile 'ataskaita.txt'." << endl;
                }
                break;

            case 7:
                cout << "Darbas baigtas." << endl;
                break;

            default:
                cout << "Tokio pasirinkimo nera. Iveskite skaiciu nuo 1 iki 7." << endl;
                break;
        }
    } while (pasirinkimas != 7);

    return 0;
}