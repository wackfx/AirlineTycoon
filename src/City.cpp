//============================================================================================
// City.Cpp - Routinen zur Verwaltung von Städten
//============================================================================================
#include <cmath>

#include "StdAfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SLONG ReadLine(BUFFER_V<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//--------------------------------------------------------------------------------------------
// Konstruktor:
//--------------------------------------------------------------------------------------------
CITIES::CITIES(const CString &TabFilename) : ALBUM_V<CITY>("Cities") { ReInit(TabFilename); }

//--------------------------------------------------------------------------------------------
// Initialisiert die Städte:
//--------------------------------------------------------------------------------------------
void CITIES::ReInit(const CString &TabFilename) {
    // CStdioFile    Tab;
    BUFFER_V<char> Line(300);
    SLONG Id = 0;
    SLONG Anz = 0;

    // Load Table header:
    auto FileData = LoadCompleteFile(FullFilename(TabFilename, ExcelPath));
    SLONG FileP = 0;

    // Die erste Zeile einlesen
    FileP = ReadLine(FileData, FileP, Line.getData(), 300);

    ReSize(MAX_CITIES);

    while (true) {
        if (FileP >= FileData.AnzEntries()) {
            break;
        }
        FileP = ReadLine(FileData, FileP, Line.getData(), 300);

        // if (!Tab.ReadString (Line.getData(), 300)) break;
        TeakStrRemoveEndingCodes(Line.getData(), "\xd\xa\x1a\r");

        // Tabellenzeile hinzufügen:
        Id = atol(strtok(Line.getData(), ";\x8\"")) + 0x1000000;

        // Hinzufügen (darf noch nicht existieren):
        if (IsInAlbum(Id) != 0) {
            TeakLibW_Exception(FNL, ExcNever);
        }

        CITY city;
        city.Name = strtok(nullptr, TabSeparator);
        city.Lage = strtok(nullptr, TabSeparator);
        city.Areacode = atoi(strtok(nullptr, TabSeparator));
        city.KuerzelGood = strtok(nullptr, TabSeparator);
        city.KuerzelReal = strtok(nullptr, TabSeparator);
        city.Wave = strtok(nullptr, TabSeparator);
        city.TextRes = atoi(strtok(nullptr, TabSeparator));
        city.AnzTexts = atoi(strtok(nullptr, TabSeparator));
        city.PhotoName = strtok(nullptr, TabSeparator);
        city.AnzPhotos = atoi(strtok(nullptr, TabSeparator));
        city.Einwohner = atoi(strtok(nullptr, TabSeparator));
        city.GlobusPosition.x = atoi(strtok(nullptr, TabSeparator));
        city.GlobusPosition.y = atoi(strtok(nullptr, TabSeparator));
        city.MapPosition.x = atoi(strtok(nullptr, TabSeparator));
        city.MapPosition.y = atoi(strtok(nullptr, TabSeparator));
        city.BuroRent = atoi(strtok(nullptr, TabSeparator));
        city.bNewInAddOn = atoi(strtok(nullptr, TabSeparator));

        city.Name = KorrigiereUmlaute(city.Name);

        push_front(Id, std::move(city));

#ifdef DEMO
        city.AnzPhotos = 0; // In der Demo keine Städtebilder
#endif

        Anz++;
    }

    ReSize(Anz);

    UseRealKuerzel(Sim.Options.OptionRealKuerzel);
}

//--------------------------------------------------------------------------------------------
// Schaltet um zwischen reellen und praktischen Kuerzeln:
//--------------------------------------------------------------------------------------------
void CITIES::UseRealKuerzel(BOOL Real) {
    SLONG c = 0;

    for (c = AnzEntries() - 1; c >= 0; c--) {
        if (IsInAlbum(c) != 0) {
            if (Real != 0) {
                (*this)[c].Kuerzel = (*this)[c].KuerzelReal;
            } else {
                (*this)[c].Kuerzel = (*this)[c].KuerzelGood;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Berechnet die Entfernung zweier beliebiger Städte: (in Meter)
//--------------------------------------------------------------------------------------------
SLONG CITIES::CalcDistance(SLONG CityId1, SLONG CityId2) {
    if (HashTable.AnzEntries() == 0) {
        HashTable.ReSize(AnzEntries() * AnzEntries());
        HashTable.FillWith(0);
    }

    if (CityId1 >= 0x1000000) {
        CityId1 = (*this)(CityId1);
    }
    if (CityId2 >= 0x1000000) {
        CityId2 = (*this)(CityId2);
    }

    if (HashTable[SLONG(CityId1 + CityId2 * AnzEntries())] != 0) {
        SLONG rc = HashTable[SLONG(CityId1 + CityId2 * AnzEntries())];

        return (min(rc, 16440000));
    }

    FXYZ Vector1;      // Vektor vom Erdmittelpunkt zu City1
    FXYZ Vector2;      // Vektor vom Erdmittelpunkt zu City2
    FLOAT Alpha = NAN; // Winkel in Grad zwischen den Vektoren (für Kreissegment)

    // Berechnung des ersten Vektors:
    Vector1.x = static_cast<FLOAT>(cos((*this)[CityId1].GlobusPosition.x * 3.14159 / 180.0));
    Vector1.z = static_cast<FLOAT>(sin((*this)[CityId1].GlobusPosition.x * 3.14159 / 180.0));

    Vector1.y = static_cast<FLOAT>(sin((*this)[CityId1].GlobusPosition.y * 3.14159 / 180.0));

    Vector1.x *= sqrt(1 - Vector1.y * Vector1.y);
    Vector1.z *= sqrt(1 - Vector1.y * Vector1.y);

    // Berechnung des zweiten Vektors:
    Vector2.x = static_cast<FLOAT>(cos((*this)[CityId2].GlobusPosition.x * 3.14159 / 180.0));
    Vector2.z = static_cast<FLOAT>(sin((*this)[CityId2].GlobusPosition.x * 3.14159 / 180.0));

    Vector2.y = static_cast<FLOAT>(sin((*this)[CityId2].GlobusPosition.y * 3.14159 / 180.0));

    Vector2.x *= sqrt(1 - Vector2.y * Vector2.y);
    Vector2.z *= sqrt(1 - Vector2.y * Vector2.y);

    // Berechnung des Winkels zwischen den Vektoren:
    Alpha = static_cast<FLOAT>(acos((Vector1.x * Vector2.x + Vector1.y * Vector2.y + Vector1.z * Vector2.z) / Vector1.abs() / Vector2.abs()) * 180.0 / 3.14159);

    // Berechnung der Länge des Kreissegments: (40040174 = Äquatorumfang)
    HashTable[SLONG(CityId1 + CityId2 * AnzEntries())] = SLONG(fabs(Alpha) * 40040174.0 / 360.0);

    SLONG rc = HashTable[SLONG(CityId1 + CityId2 * AnzEntries())];

    return (min(rc, 16440000));
}

//--------------------------------------------------------------------------------------------
// Berechnet die Dauer eines Fluges:
//--------------------------------------------------------------------------------------------
SLONG CITIES::CalcFlugdauer(SLONG CityId1, SLONG CityId2, SLONG Speed) {
    SLONG d = (CalcDistance(CityId1, CityId2) / Speed + 999) / 1000 + 1 + 2 - 2;

    if (d < 2) {
        d = 2;
    }

    return (d);
}

//--------------------------------------------------------------------------------------------
// Gibt eine zufällige Stadt zurück:
//--------------------------------------------------------------------------------------------
SLONG CITIES::GetRandomUsedIndex(TEAKRAND *pRand) {
    SLONG c = 0;
    SLONG n = 0;

    for (c = AnzEntries() - 1; c >= 0; c--) {
        if (IsInAlbum(c) != 0) {
            n++;
        }
    }

    if (pRand != nullptr) {
        n = pRand->Rand(n) + 1;
    } else {
        n = (rand() % n) + 1;
    }

    for (c = AnzEntries() - 1; c >= 0; c--) {
        if (IsInAlbum(c) != 0) {
            n--;
            if (n == 0) {
                return (c);
            }
        }
    }

    DebugBreak();
    return (0);
}

//--------------------------------------------------------------------------------------------
// Gibt eine zufällige Stadt von diesem AreaCode zurück:
//--------------------------------------------------------------------------------------------
SLONG CITIES::GetRandomUsedIndex(SLONG AreaCode, TEAKRAND *pRand) {
    SLONG c = 0;
    SLONG n = 0;

    for (c = AnzEntries() - 1; c >= 0; c--) {
        if ((IsInAlbum(c) != 0) && (*this)[c].Areacode == AreaCode) {
            n++;
        }
    }

    if (pRand != nullptr) {
        n = pRand->Rand(n) + 1;
    } else {
        n = (rand() % n) + 1;
    }

    for (c = AnzEntries() - 1; c >= 0; c--) {
        if ((IsInAlbum(c) != 0) && (*this)[c].Areacode == AreaCode) {
            n--;
            if (n == 0) {
                return (c);
            }
        }
    }

    DebugBreak();
    return (0);
}

//--------------------------------------------------------------------------------------------
// Gibt die Nummer der Stadt mit dem angegebnen Namen zurück:
//--------------------------------------------------------------------------------------------
ULONG CITIES::GetIdFromName(const char *Name) {
    SLONG c = 0;

    for (c = 0; c < AnzEntries(); c++) {
        if ((IsInAlbum(c) != 0) && stricmp(Name, (LPCTSTR)at(c).Name) == 0) {
            return (GetIdFromIndex(c));
        }
    }

    TeakLibW_Exception(FNL, ExcNever);
    return (0);
}

//--------------------------------------------------------------------------------------------
// Gibt die Nummer der Stadt mit dem angegebnen Namen zurück:
//--------------------------------------------------------------------------------------------
ULONG CITIES::GetIdFromNames(const char *Name, ...) {
    SLONG c = 0;

    va_list va_marker;
    LPCTSTR tmp = Name;
    for (va_start(va_marker, Name); tmp != (nullptr); tmp = va_arg(va_marker, LPCTSTR)) {
        for (c = 0; c < AnzEntries(); c++) {
            if ((IsInAlbum(c) != 0) && stricmp(tmp, (LPCTSTR)at(c).Name) == 0) {
                return (GetIdFromIndex(c));
            }
        }
    }

    TeakLibW_Exception(FNL, ExcNever);
    return (0);
}

//--------------------------------------------------------------------------------------------
// CRentCity::
//--------------------------------------------------------------------------------------------
// Gibt zurück, wieviele Routen der Spieler besitzt:
//--------------------------------------------------------------------------------------------
SLONG CRentCities::GetNumUsed() {
    SLONG c = 0;
    SLONG Anz = 0;

    for (c = 0; c < Cities.AnzEntries(); c++) {
        if ((Cities.IsInAlbum(c) != 0) && RentCities[c].Rang != 0) {
            Anz++;
        }
    }

    return (Anz);
}

//--------------------------------------------------------------------------------------------
// Speichert ein RentCity-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator<<(TEAKFILE &File, const CRentCity &r) {
    File << r.Rang << r.Image << r.Miete;

    return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt ein RentCity-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator>>(TEAKFILE &File, CRentCity &r) {
    File >> r.Rang >> r.Image >> r.Miete;

    return (File);
}
