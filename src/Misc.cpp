//============================================================================================
// Misc.cpp : Diverse Sachen
//============================================================================================
#include "StdAfx.h"
#include <cmath>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <locale>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRect SprechblasenSizes[] = {CRect(15, 6, 266, 16), CRect(19, 8, 266, 31), CRect(32, 8, 308, 51),
                             CRect(30, 9, 498, 64), CRect(49, 8, 296, 31), CRect(62, 8, 338, 51)};

SLONG BeraterSlideY[] = {126, 142, -1, -1, -1, -1, -1, -1, 120, 120, -1, -1, 115, 115, 106 + 14, 106, 115 - 1, 115 + 7, 106 + 14, 100 + 11};

extern SB_CColorFX ColorFX;

TEAKRAND *pSurvisedRandom1 = nullptr;
TEAKRAND *pSurvisedRandom2 = nullptr;

const char TOKEN_STAT[] = "STAT";

//--------------------------------------------------------------------------------------------
// Liest eine Zeile aus einem Buffer aus:
//--------------------------------------------------------------------------------------------
SLONG ReadLine(BUFFER_V<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

SLONG ReadLine(BUFFER_V<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength) {
    SLONG c = 0;
    SLONG d = 0;

    for (c = BufferStart; c < Buffer.AnzEntries() && d < LineLength - 1; c++, d++) {
        Line[d] = Buffer[c];
        if (Line[d] == 13 || Line[d] == 10 || Line[d] == 26) {
            if (c + 1 >= Buffer.AnzEntries() || (Buffer[c + 1] != 13 && Buffer[c + 1] != 10 && Buffer[c + 1] != 26)) {
                Line[d + 1] = 0;
                return (c + 1);
            }
        }
    }

    return (c);
}

//--------------------------------------------------------------------------------------------
// Zählt alle Zeilen in einem Buffer
//--------------------------------------------------------------------------------------------
SLONG CountLines(BUFFER_V<UBYTE> &Buffer, SLONG BufferStart) {
    SLONG count = 1;

    for (SLONG c = BufferStart; c < Buffer.AnzEntries(); c++) {
        if (Buffer[c] == 13 || Buffer[c] == 10 || Buffer[c] == 26) {
            if (c + 1 >= Buffer.AnzEntries() || (Buffer[c + 1] != 13 && Buffer[c + 1] != 10 && Buffer[c + 1] != 26)) {
                count++;
            }
        }
    }

    return count;
}

//--------------------------------------------------------------------------------------------
// Gibt die Anzahl der gesetzten Bits zurück: (Bitte ein Bit.... Hahaha!)
//--------------------------------------------------------------------------------------------
SLONG GetAnzBits(ULONG Flags) {
    SLONG c = 0;
    SLONG rc = 0;

    for (c = rc = 0; c < 32; c++) {
        if ((Flags & (1 << c)) != 0U) {
            rc++;
        }
    }

    return (rc);
}

//--------------------------------------------------------------------------------------------
// Berechnet den Winkel eines Vektors; 0 liegt bei 12 Uhr
//--------------------------------------------------------------------------------------------
DOUBLE GetAlphaFromXY(XY Delta) {
    if (Delta.x == 0) {
        if (Delta.y > 0) {
            return (180);
        }
        return (0);
    }

    DOUBLE rc = NAN;

    if (Delta.x > 0) {
        rc = atan2(Delta.y, Delta.x) * 180 / 3.14159275 + 90;
    } else {
        rc = 180 - atan2(Delta.y, -Delta.x) * 180 / 3.14159275 + 90;
    }

    if (rc > 360) {
        rc -= 360;
    }
    return (rc);
}

//--------------------------------------------------------------------------------------------
// Löscht einen CString und legt ihn wieder an. Deckt so Fehler im Speicher auf.
//--------------------------------------------------------------------------------------------
void CheckCString(CString *String) {
    CString tmp;

    tmp = *String;
    *String = "";
    *String = tmp;
}

//--------------------------------------------------------------------------------------------
// Korrigiert die Umlaute wegen dem schönen Windows-Doppelsystem
//--------------------------------------------------------------------------------------------
CString KorrigiereUmlaute(CString &OriginalText) {
    CString rc;
    SLONG c = 0;

    // Keine Korrektur für Tschechien:
    if (gLanguage == LANGUAGE_1) {
        return (OriginalText);
    }

    for (c = 0; c < OriginalText.GetLength(); c++) {
        if (OriginalText[c] == '\x84') {
            rc += '\xE4'; // Nicht ändern - das macht so wie es ist schon Sinn!
        } else if (OriginalText[c] == '\x94') {
            rc += '\xF6';
        } else if (OriginalText[c] == '\x81') {
            rc += '\xFC';
        } else if (OriginalText[c] == '\x8E') {
            rc += '\xC4';
        } else if (OriginalText[c] == '\x99') {
            rc += '\xD6';
        } else if (OriginalText[c] == '\x9A') {
            rc += '\xDC';
        } else if (OriginalText[c] == '\xE1' && gLanguage == LANGUAGE_D) {
            rc += '\xDF';
        } else {
            rc += OriginalText[c];
        }
    }

    return (rc);
}

//--------------------------------------------------------------------------------------------
//Überprüft, ob der Cursor in einem Bereich ist und erledigt das Highlighting:
//--------------------------------------------------------------------------------------------
BOOL CheckCursorHighlight(const CRect &rect, UWORD FontColor, SLONG Look, SLONG TipId, SLONG ClickArea, SLONG ClickId, SLONG ClickPar1, SLONG ClickPar2) {
    return (CheckCursorHighlight(gMousePosition, rect, FontColor, Look, TipId, ClickArea, ClickId, ClickPar1, ClickPar2));
}

//--------------------------------------------------------------------------------------------
//Überprüft, ob der Cursor in einem Bereich ist und erledigt das Highlighting:
//--------------------------------------------------------------------------------------------
BOOL CheckCursorHighlight(const XY &CursorPos, const CRect &rect, UWORD FontColor, SLONG Look, SLONG TipId, SLONG ClickArea, SLONG ClickId, SLONG ClickPar1,
                          SLONG ClickPar2) {
    if (Sim.Players.Players[Sim.localPlayer].LocationWin == nullptr || ((Sim.Players.Players[Sim.localPlayer].LocationWin)->IsDialogOpen() == 0) ||
        ((Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuIsOpen() != 0)) {
        if (CursorPos.x >= rect.left && CursorPos.x <= rect.right && CursorPos.y >= rect.top && CursorPos.y <= rect.bottom) {
            SLONG dx = ReferenceCursorPos.x - CursorPos.x;
            SLONG dy = ReferenceCursorPos.y - CursorPos.y;

            SetMouseLook(Look, TipId, ClickArea, ClickId, ClickPar1, ClickPar2);

            gHighlightArea = CRect(rect.left + dx, rect.top + dy, rect.right + dx, rect.bottom + dy);
            gHighlightFontColor = FontColor;

            return TRUE;
        }
    }

    return FALSE;
}

//--------------------------------------------------------------------------------------------
// Zeichnet Werte in ein Perspektivisches Raster (Trapez) ein:
//--------------------------------------------------------------------------------------------
void DrawChart(SBBM &TipBm, ULONG Color, const BUFFER_V<SLONG> &Values, SLONG MinValue, SLONG MaxValue, const XY &TopLeft, const XY &TopRight,
               const XY &BottomLeft, const XY &BottomRight) {
    SLONG c = 0;
    SB_Hardwarecolor hwColor = TipBm.pBitmap->GetHardwarecolor(Color);

    if (MaxValue == -1) {
        for (c = 0; c < Values.AnzEntries(); c++) {
            if (Values[c] > MaxValue) {
                MaxValue = Values[c];
            }
        }
    }
    if (MinValue == -1) {
        MinValue = 999999;
        for (c = 0; c < Values.AnzEntries(); c++) {
            if (Values[c] < MinValue) {
                MinValue = Values[c];
            }
        }
    }

    if (MaxValue == 0) {
        MaxValue = 1;
    }

    if (MinValue == MaxValue) {
        MaxValue++;
    }

    for (c = 0; c < Values.AnzEntries() - 1; c++) {
        TipBm.Line(c * (TopRight.x - TopLeft.x + 1) / (Values.AnzEntries() - 1) + TopLeft.x,
                   (BottomRight.y * c + BottomLeft.y * (Values.AnzEntries() - 1 - c)) / (Values.AnzEntries() - 1) -
                       (Values[c] - MinValue) *
                           ((BottomRight.y * c + BottomLeft.y * (Values.AnzEntries() - 1 - c)) / (Values.AnzEntries() - 1) -
                            (TopRight.y * c + TopLeft.y * (Values.AnzEntries() - 1 - c)) / (Values.AnzEntries() - 1)) /
                           (MaxValue - MinValue),
                   (c + 1) * (TopRight.x - TopLeft.x + 1) / (Values.AnzEntries() - 1) + TopLeft.x,
                   (BottomRight.y * (c + 1) + BottomLeft.y * (Values.AnzEntries() - 1 - (c + 1))) / (Values.AnzEntries() - 1) -
                       (Values[c + 1] - MinValue) *
                           ((BottomRight.y * (c + 1) + BottomLeft.y * (Values.AnzEntries() - 1 - (c + 1))) / (Values.AnzEntries() - 1) -
                            (TopRight.y * (c + 1) + TopLeft.y * (Values.AnzEntries() - 1 - (c + 1))) / (Values.AnzEntries() - 1)) /
                           (MaxValue - MinValue),
                   hwColor);
    }
}

//--------------------------------------------------------------------------------------------
// Liest den aktuellen Taktzyklenzähler der CPU
//--------------------------------------------------------------------------------------------
#if 0
#pragma warning(disable : 4035)
SLONG ReadTimeStampCounter (void)
{
    __asm
    {
        _emit 0x0F
            _emit 0x31
    }
}
#pragma warning(default : 4035)
#endif

//--------------------------------------------------------------------------------------------
// Ja was wohl??? Konvertiert halt!
//--------------------------------------------------------------------------------------------
__int64 StringToInt64(const CString &String) {
    __int64 rc = 0;

    for (SLONG d = 0; d < String.GetLength(); d++) {
        rc += __int64(String[SLONG(d)]) << (8 * d);
    }

    return (rc);
}

//--------------------------------------------------------------------------------------------
// Ist in dem Raum schon jemand drin?
//--------------------------------------------------------------------------------------------
BOOL IsRoomBusy(UWORD RoomId, SLONG ExceptPlayer) {
    SLONG c = 0;
    SLONG d = 0;

    if ((RoomId == ROOM_AUFSICHT && Sim.Time == 9 * 60000) || RoomId == ROOM_AIRPORT) {
        return (FALSE);
    }

    for (c = 0; c < Sim.Players.AnzPlayers; c++) {
        if (Sim.Players.Players[c].IsOut == 0) {
            if (c != ExceptPlayer) {
                for (d = 0; d < 10; d++) {
                    if (Sim.Players.Players[c].Locations[d] == RoomId) {
                        return (TRUE);
                    }
                }
            }
        }
    }

    return (FALSE);
}

//--------------------------------------------------------------------------------------------
// Projeziert die Länge/Breite in Pixel; Gibt TRUE zurück wenn auf Erdvorderseite
//--------------------------------------------------------------------------------------------
BOOL EarthProjectize(const XY &NaturalPos, UWORD EarthAlpha, XY *PixelPos) {
    DOUBLE _a = 2.65;
    auto xx = UWORD(UWORD((NaturalPos.x + 180) * 65536 / 360) - EarthAlpha + 16384 - 1250);

    auto py = SLONG(sin((NaturalPos.y * _a) / 240.0 * (3.14159 / 2)) * 185 + 190);
    auto px = SLONG(sin((xx / 32768.0 - 1) * (3.14159)) * sqrt(184 * 184 - (py - 190) * (py - 190)) + 190);

    (*PixelPos) = XY(px, py) /*+XY(190,190)-XY(0, 40)*/;

    if (xx > 16386 && xx < 49152) {
        return (FALSE);
    }
    return (TRUE);
}

//--------------------------------------------------------------------------------------------
// Projeziert die Länge/Breite in Pixel; Gibt TRUE zurück wenn auf Erdvorderseite
//--------------------------------------------------------------------------------------------
BOOL EarthProjectize(const FXY &NaturalPos, UWORD EarthAlpha, XY *PixelPos) {
    DOUBLE _a = 2.65;
    auto xx = UWORD(UWORD((NaturalPos.x + 180) * 65536 / 360) - EarthAlpha + 16384 - 1250);

    auto py = SLONG(sin((NaturalPos.y * _a) / 240.0 * (3.14159 / 2)) * 185 + 190);
    auto px = SLONG(sin((xx / 32768.0 - 1) * (3.14159)) * sqrt(184 * 184 - (py - 190) * (py - 190)) + 190);

    (*PixelPos) = XY(px, py) /*+XY(190,190)-XY(0, 40)*/;

    if (xx > 16386 && xx < 49152) {
        return (FALSE);
    }
    return (TRUE);
}

//--------------------------------------------------------------------------------------------
// Erhöht die n-te Stelle von der Zahl "Value" um Add (wenn Add einstellig ist)
//--------------------------------------------------------------------------------------------
SLONG AddToNthDigit(SLONG Value, SLONG Digit, SLONG Add) {
    SLONG c = 0;
    SLONG betrag = 0;
    SLONG n = 0;

    if (Value == 0) {
        return (Add);
    }

    betrag = abs(Value);

    n = 1;

    for (c = 0; c < 20; c++, n = n * 10) {
        if (n <= betrag && betrag < n * 10) {
            while (SLONG(pow(10, Digit - 1)) > n && Digit > 0) {
                Digit--;
            }
            return (Value + Add * n / SLONG(pow(10, Digit - 1)));
        }
    }

    return (Value);
}

int CustomMessageBox(ULONG, LPCTSTR, char *, const SDL_MessageBoxButtonData *, int);

//--------------------------------------------------------------------------------------------
// Im Mess-Age (oder war's Message?) Zeitalter braucht man einfache Nachrichten (für Fehler)
//--------------------------------------------------------------------------------------------
void SimpleMessageBox(ULONG Type, LPCTSTR Title, LPCTSTR String, ...) {
    char Buffer[256];

    // Hilfskonstruktion für beliebige viele Argumente deklarieren:
    va_list Vars;

    // Tabelle initialisieren:
    va_start(Vars, String);

    // Die gesammten Parameter "reinvestieren":
    vsnprintf(Buffer, sizeof(Buffer), const_cast<char *>(String), Vars);

    // Daten bereinigen:
    va_end(Vars);

    const SDL_MessageBoxButtonData Buttons[] = {{/* .flags, .buttonid, .text */ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"}};
    CustomMessageBox(Type, Title, Buffer, Buttons, 1);
}

int AbortMessageBox(ULONG Type, LPCTSTR Title, LPCTSTR String, ...) {
    char Buffer[256];

    // Hilfskonstruktion für beliebige viele Argumente deklarieren:
    va_list Vars;

    // Tabelle initialisieren:
    va_start(Vars, String);

    // Die gesammten Parameter "reinvestieren":
    vsnprintf(Buffer, sizeof(Buffer), const_cast<char *>(String), Vars);

    // Daten bereinigen:
    va_end(Vars);

    const SDL_MessageBoxButtonData Buttons[] = {{/* .flags, .buttonid, .text */ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"},
                                                {/* .flags, .buttonid, .text */ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Abort"}};
    return CustomMessageBox(Type, Title, Buffer, Buttons, 2);
}

int CustomMessageBox(ULONG Type, LPCTSTR Title, char Buffer[256], const SDL_MessageBoxButtonData Buttons[], int buttonCount) {
    const SDL_MessageBoxColorScheme ColorScheme = {{/* .colors (.r, .g, .b) */
                                                    /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                                                    {255, 0, 0},
                                                    /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                                                    {0, 255, 0},
                                                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                                                    {255, 255, 0},
                                                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                                                    {0, 0, 255},
                                                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                                                    {255, 0, 255}}};
    const SDL_MessageBoxData Data = {Type, nullptr, Title, Buffer, buttonCount, Buttons, &ColorScheme};
    int buttonId = 0;
    SDL_ShowMessageBox(&Data, &buttonId);

    return buttonId;
}


//--------------------------------------------------------------------------------------------
// Berechnet das interne Datum aus "3-12-1994", "1/11/1992", "1.1.1900", ..
//--------------------------------------------------------------------------------------------
UWORD ConvertString2Date(char *String) {
    char *Back = nullptr;
    char *p = nullptr;
    unsigned short Date = 0;
    SLONG c = 0;
    SLONG Year = 0;

    Back = new char[strlen(String) + 1];
    strcpy(Back, String);

    p = strtok(Back, DateSeparator); // Woche
    Date = static_cast<UWORD>(atoi(p) - 1);

    p = strtok(nullptr, DateSeparator); // Monat
    Date += static_cast<UWORD>((atoi(p) - 1) * 30);

    p = strtok(nullptr, DateSeparator); // Jahr
    Year = static_cast<SLONG>(atoi(p) - 1950);

    // Umrechnen:
    Date = static_cast<UWORD>(Date * 13 * 30 / (12 * 30));

    for (c = 0; c < Year; c++) {
        Date += 365;
    }

    delete[] Back;

    return (Date);
}

//--------------------------------------------------------------------------------------------
// Bestimmt den Pfad der Applikation:
//--------------------------------------------------------------------------------------------
void DoAppPath() {
    // Vollen Programmnamen anfordern:
    char *buffer = SDL_GetBasePath();

    // eigentlichen Programmteil löschen:
    // while (strlen(buffer)>0 && buffer[(SLONG)(strlen(buffer)-1)]!='\\') buffer[(SLONG)(strlen(buffer)-1)]=0;

    // Verzeichnis-Namen der %§$@#"$! MS-Entwicklungsumgebung löschen:
    // if (strlen(buffer) > 6 && strnicmp(buffer + strlen(buffer) - 6, "debug\\", 6) == 0) buffer[(SLONG)(strlen(buffer) - 6)] = 0;
    // if (strlen(buffer) > 8 && strnicmp(buffer + strlen(buffer) - 8, "release\\", 8) == 0) buffer[(SLONG)(strlen(buffer) - 8)] = 0;

    AppPath = CString(buffer);
    SDL_free(buffer);
}

//--------------------------------------------------------------------------------------------
// Gibt einen vollständigen Filenamen mit dem vollen Pfad zurück:
//--------------------------------------------------------------------------------------------
CString FullFilename(const CString &Filename, const CString &PathString) {
    CString path;
    CString rc;

    path = PathString;
    if (std::filesystem::path::preferred_separator != '\\') {
        path.Replace('\\', std::filesystem::path::preferred_separator);
    }

    if (path[1] == ':') {
        rc.Format(path, (const char *)Filename);
    } else {
        rc.Format(AppPath + path, (const char *)Filename);
    }

    return (rc);
}

//--------------------------------------------------------------------------------------------
// Gibt einen vollständigen Filenamen mit dem vollen Pfad zurück wobei ein Parameter ok ist:
//--------------------------------------------------------------------------------------------
CString FullFilename(const CString &Filename, const CString &PathString, SLONG Num) {
    CString tmp;
    CString path;
    CString rc;

    tmp.Format((const char *)Filename, Num);

    path = PathString;
    if (std::filesystem::path::preferred_separator != '\\') {
        path.Replace('\\', std::filesystem::path::preferred_separator);
    }

    if (path[1] == ':') {
        rc.Format(path, tmp);
    } else {
        rc.Format(AppPath + path, tmp);
    }

    return (rc);
}

//--------------------------------------------------------------------------------------------
// Berechnet, wieviel ein Flug kostet:
//--------------------------------------------------------------------------------------------
SLONG CalculateFlightKerosin(SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit) {
    SLONG Kerosin = Cities.CalcDistance(VonCity, NachCity) / 1000 // weil Distanz in m übergeben wird
                    * Verbrauch / 160                             // Liter pro Barrel
                    / Geschwindigkeit;

    return (Kerosin);
}

//--------------------------------------------------------------------------------------------
// Berechnet, wieviel ein Flug kostet: (Kosten des Kerosins aus Tank wird einbezogen)
//--------------------------------------------------------------------------------------------
SLONG CalculateFlightCostRechnerisch(SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit, SLONG PlayerNum) {
    SLONG Kerosin = CalculateFlightKerosin(VonCity, NachCity, Verbrauch, Geschwindigkeit);
    SLONG Kosten = 0;

    // Kerosin aus dem Vorrat:
    if (PlayerNum != -1 && (Sim.Players.Players[PlayerNum].TankOpen != 0)) {
        SLONG tmp = std::min(Sim.Players.Players[PlayerNum].TankInhalt, Kerosin);
        Kosten += SLONG(Sim.Players.Players[PlayerNum].TankPreis * tmp);

        Kerosin -= tmp;
    }

    // Restliches Kerosin kaufen:
    Kosten += Kerosin * Sim.Kerosin;

    if (Kosten < 1000) {
        Kosten = 1000;
    }

    return (Kosten);
}

//--------------------------------------------------------------------------------------------
// Berechnet, wieviel ein Flug kostet (min. 1000)
//--------------------------------------------------------------------------------------------
SLONG CalculateFlightCost(SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit, SLONG PlayerNum) {
    SLONG Kerosin = CalculateFlightKerosin(VonCity, NachCity, Verbrauch, Geschwindigkeit);
    SLONG Kosten = 0;

    // Kerosin aus dem Vorrat:
    if (PlayerNum != -1 && (Sim.Players.Players[PlayerNum].TankOpen != 0)) {
        SLONG tmp = std::min(Sim.Players.Players[PlayerNum].TankInhalt, Kerosin);

        Kerosin -= tmp;
    }

    // Restliches Kerosin kaufen:
    Kosten += Kerosin * Sim.Kerosin;

    if (Kosten < 1000) {
        Kosten = 1000;
    }

    return (Kosten);
}

//--------------------------------------------------------------------------------------------
// Lädt die Einheiten-Tabelle:
//--------------------------------------------------------------------------------------------
void InitEinheiten(const CString &Filename) {
    SLONG c = 0;

    ETexte.Open(Filename, TEXTRES_CACHED);

    Einheiten.ReSize(14);

    for (c = 0; c < (bFirstClass != 0 ? 11 : 14); c++) {
        Einheiten[c].Name = ETexte.GetS(1000, 1000 + c);
        Einheiten[c].Faktor = atof(ETexte.GetS(1000, 2000 + c));
    }
}

//--------------------------------------------------------------------------------------------
// Rechnet um:
//--------------------------------------------------------------------------------------------
SLONG CEinheit::Umrechnung(SLONG Value) const { return (static_cast<SLONG>(Value * Faktor)); }

//--------------------------------------------------------------------------------------------
// Rechnet um:
//--------------------------------------------------------------------------------------------
__int64 CEinheit::Umrechnung64(__int64 Value) const { return (static_cast<__int64>(Value * Faktor)); }

//--------------------------------------------------------------------------------------------
// Rechnet in String um:
//--------------------------------------------------------------------------------------------
char *CEinheit::bString(SLONG Value) const { return (bprintf(Name, (LPCTSTR)Insert1000erDots(Umrechnung(Value)))); }

//--------------------------------------------------------------------------------------------
// Rechnet in String um:
//--------------------------------------------------------------------------------------------
char *CEinheit::bString64(__int64 Value) const {
    if (Value >= 1000000000 && Name == ETexte.GetS(1000, 1000 + EINH_DM)) {
        return (Einheiten[EINH_MIODM].bString64(Value));
    }

    return (bprintf(Name, (LPCTSTR)Insert1000erDots64(Umrechnung64(Value))));
}

//============================================================================================
// HEADLINES::
//============================================================================================
//--------------------------------------------------------------------------------------------
// Konstruktor:
//--------------------------------------------------------------------------------------------
HEADLINES::HEADLINES(const CString &TabFilename) { ReInit(TabFilename); }

//--------------------------------------------------------------------------------------------
// Sucht die Schlagzeile raus:
//--------------------------------------------------------------------------------------------
CHeadline HEADLINES::GetHeadline(SLONG Newspaper, SLONG Index) { return (Headline[Newspaper * 10 + Index]); }

//--------------------------------------------------------------------------------------------
// Schreibt die Headline auf den Schirm:
//--------------------------------------------------------------------------------------------
void HEADLINES::BlitHeadline(SLONG /*Newspaper*/, SBBM & /*Offscreen*/, const CPoint & /*p1*/, const CPoint & /*p2*/, BYTE /*Color*/) {}

//--------------------------------------------------------------------------------------------
// Initializes the data:
//--------------------------------------------------------------------------------------------
void HEADLINES::Init() {
    SLONG c = 0;

    for (c = 0; c < 3; c++) {
        CurrentChain[c] = 0;
        FlexiCity[c] = "";
        FlexiNumber[c] = 0;
    }

    for (c = 0; c < 30; c++) {
        Headline[c].Headline = "";
        Headline[c].PictureId = 0;
        Headline[c].PicturePriority = 0;
        Override[c].Headline = "";
        Override[c].PictureId = 0;
        Override[c].PicturePriority = 0;
    }
}

//--------------------------------------------------------------------------------------------
// Lädt die aktuellen Schlagzeilen:
//--------------------------------------------------------------------------------------------
void HEADLINES::ReloadHeadline() {
    SLONG c = 0;
    SLONG NewRand[3];
    BUFFER_V<char> Line(300);
    SLONG Zeitung = 0;
    SLONG Kette = 0;
    SLONG LastKette = 0;

    auto FileData = LoadCompleteFile(FullFilename(HeadlineFile, ExcelPath));
    SLONG FileP = 0;

    for (c = 0; c < 30; c++) {
        Headline[c].Headline.Empty();
        Headline[c].PictureId = 0;
    }

    IsInteresting = FALSE;

    TEAKRAND LocalRand(Sim.Date + Sim.StartTime);

    // Zufallsmeldungen raussuchen:
    for (c = 0; c < 3; c++) {
        NewRand[c] = LocalRand.Rand(NumRand[c]);
    }

    // Die erste Zeile einlesen
    FileP = ReadLine(FileData, FileP, Line.getData(), 300);

    while (true) {
        // Tab.ReadLine (Line.getData(), 300);
        if (FileP >= FileData.AnzEntries()) {
            break;
        }
        FileP = ReadLine(FileData, FileP, Line.getData(), 300);

        LastKette = Kette;
        Zeitung = atoi(strtok(Line.getData(), TabSeparator));
        Kette = atoi(strtok(nullptr, TabSeparator));

        // Default: Zufallsmeldung
        if ((Zeitung != 0) && (Kette == 0) && NewRand[Zeitung - 1] >= 0) {
            NewRand[Zeitung - 1]--;
            if (NewRand[Zeitung - 1] == -1) {
                Headline[(Zeitung - 1) * 10].Headline = strtok(nullptr, TabSeparator);
                Headline[(Zeitung - 1) * 10].PictureId = 0;
                Headline[(Zeitung - 1) * 10].PicturePriority = 0;
            }
        }

        // Oder besser: eine Kette neu anfangen
        if ((Zeitung != 0) && (Kette != 0) && (LastKette != 0) && CurrentChain[Zeitung - 1] == 0 && NewRand[Zeitung - 1] != -10 && Kette != LastKette + 1 &&
            LocalRand.Rand(20) == 0) {
            CurrentChain[Zeitung - 1] = Kette;
            NewRand[Zeitung - 1] = -10; // Token für: Kette angefangen/fortgesetzt
            Headline[(Zeitung - 1) * 10].Headline = strtok(nullptr, TabSeparator);
            Headline[(Zeitung - 1) * 10].PictureId = 0;
            Headline[(Zeitung - 1) * 10].PicturePriority = 0;
        }

        // Oder noch besser: eine Kette fortsetzen
        if ((Zeitung != 0) && CurrentChain[Zeitung - 1] + 1 == Kette && NewRand[Zeitung - 1] != -10) {
            CurrentChain[Zeitung - 1]++;
            NewRand[Zeitung - 1] = -10; // Token für: Kette angefangen/fortgesetzt
            Headline[(Zeitung - 1) * 10].Headline = strtok(nullptr, TabSeparator);
            Headline[(Zeitung - 1) * 10].PictureId = 0;
            Headline[(Zeitung - 1) * 10].PicturePriority = 0;
        }

        // Oder am besten: eine fremde Kette fortsetzen
        if ((Zeitung != 0) && CurrentChain[(Zeitung - 1 + 1) % 3] + 1 == Kette) {
            CurrentChain[Zeitung - 1]++;
            NewRand[Zeitung - 1] = -10; // Token für: Kette angefangen/fortgesetzt
            Headline[(Zeitung - 1) * 10].Headline = strtok(nullptr, TabSeparator);
            Headline[(Zeitung - 1) * 10].PictureId = 0;
            Headline[(Zeitung - 1) * 10].PicturePriority = 0;
        } else if ((Zeitung != 0) && CurrentChain[(Zeitung - 1 + 2) % 3] + 1 == Kette) {
            CurrentChain[Zeitung - 1]++;
            NewRand[Zeitung - 1] = -10; // Token für: Kette angefangen/fortgesetzt
            Headline[(Zeitung - 1) * 10].Headline = strtok(nullptr, TabSeparator);
            Headline[(Zeitung - 1) * 10].PictureId = 0;
            Headline[(Zeitung - 1) * 10].PicturePriority = 0;
        }
    }

    // War die Kette zu Ende, oder haben wir sie fortgesetzt?
    for (c = 0; c < 3; c++) {
        if (NewRand[c] == -1) {
            CurrentChain[c] = 0;
        }
    }
}

//--------------------------------------------------------------------------------------------
// Ersetzt bestimmte Tokens durch zufällige Inhalte:
//--------------------------------------------------------------------------------------------
void HEADLINES::InterpolateHeadline() {
    SLONG c = 0;
    SLONG d = 0;
    SLONG e = 0;
    SLONG count = 0;
    SLONG von = 0;
    SLONG bis = 0;
    CString Extrakt; // Der Teil in geschweiften Klammern

    TEAKRAND LocalRand(Sim.Date + Sim.StartTime);

    for (c = 0; c < 3; c++) {
        do {
            von = bis = -1;

            count = 0;

            // Extract info string, match "{" and "}":
            for (d = Headline[c * 10].Headline.GetLength() - 1; d >= 0; d--) {
                if (Headline[c * 10].Headline[static_cast<SLONG>(d)] == '}') {
                    if (count == 0) {
                        bis = d;
                    }
                    count++;
                }

                if (Headline[c * 10].Headline[static_cast<SLONG>(d)] == '{') {
                    count--;
                    if (count == 0) {
                        von = d;
                        break;
                    }
                }
            }

            if (von == -1 || bis == -1) {
                continue;
            }

            Extrakt = Headline[c * 10].Headline.Mid(von + 1, bis - von - 1);

            // City random:
            if (Extrakt.CompareNoCase("city") == 0) {
                SLONG CityId = 0;

                do {
                    CityId = LocalRand.Rand(Cities.AnzEntries());
                } while (Cities.IsInAlbum(CityId) == 0);

                FlexiCity[c] = Cities[CityId].Name;
                Headline[c * 10].Headline = Headline[c * 10].Headline.Mid(0, von) + FlexiCity[c] +
                                            Headline[c * 10].Headline.Mid(bis + 1, Headline[c * 10].Headline.GetLength() - bis - 1);
            } else if (Extrakt.CompareNoCase("{city}") == 0) {
                Headline[c * 10].Headline = Headline[c * 10].Headline.Mid(0, von) + FlexiCity[c] +
                                            Headline[c * 10].Headline.Mid(bis + 1, Headline[c * 10].Headline.GetLength() - bis - 1);
            }

            // Number random:
            else if (Extrakt.Mid(0, 5).CompareNoCase("rand(") == 0) {
                char Buffer[40];
                SLONG a = 0;
                SLONG b = 0;

                strcpy(Buffer, Extrakt);
                a = atoi(strtok(Buffer + 5, "),"));
                b = atoi(strtok(nullptr, "),"));

                FlexiNumber[c] = LocalRand.Rand((b - a + 1) + a);
                Headline[c * 10].Headline = Headline[c * 10].Headline.Mid(0, von) + bprintf("%li", FlexiNumber[c]) +
                                            Headline[c * 10].Headline.Mid(bis + 1, Headline[c * 10].Headline.GetLength() - bis - 1);
            } else if (Extrakt.CompareNoCase("{rand}") == 0) {
                Headline[c * 10].Headline = Headline[c * 10].Headline.Mid(0, von) + bprintf("%li", FlexiNumber[c]) +
                                            Headline[c * 10].Headline.Mid(bis + 1, Headline[c * 10].Headline.GetLength() - bis - 1);
            }

            // Number of the Airline
            else if (atoi(Extrakt) > 0) {
                Headline[c * 10].Headline = Headline[c * 10].Headline.Mid(0, von) +
                                            Sim.Players.Players[(atoi(Extrakt) - 1) % Sim.Players.AnzPlayers].Airline.c_str() +
                                            Headline[c * 10].Headline.Mid(bis + 1, Headline[c * 10].Headline.GetLength() - bis - 1);
            }
        } while (von != -1);

        for (d = 0; d < 10; d++) {
            if (Override[c * 10 + d].Headline.GetLength() > 0) {
                for (e = 0; e < 10; e++) {
                    if (Headline[c * 10 + e].Headline.IsEmpty()) {
                        IsInteresting = TRUE;
                        Headline[c * 10 + e] = Override[c * 10 + d];
                        Headline[c * 10 + e].Headline = KorrigiereUmlaute(Override[c * 10 + d].Headline);
                        Override[c * 10 + d].Headline.Empty();
                        break;
                    }
                }
            }
        }

        Headline[c * 10].Headline = KorrigiereUmlaute(Headline[c * 10].Headline);
    }
}

//--------------------------------------------------------------------------------------------
// Speichert eine neue Schlagzeile für morgen
//--------------------------------------------------------------------------------------------
void HEADLINES::AddOverride(SLONG Newspaper, const CString &HeadlineText, __int64 PictureId, SLONG PicturePriority) {
    SLONG c = 0;

    for (c = Newspaper * 10; c < Newspaper * 10 + 10; c++) {
        if (Override[c].Headline.IsEmpty()) {
            Override[c].Headline = HeadlineText;
            Override[c].PictureId = PictureId;
            Override[c].PicturePriority = PicturePriority;
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------
// Erzeugt ab und zu Vergleichsmeldungen:
//--------------------------------------------------------------------------------------------
void HEADLINES::ComparisonHeadlines() {
    if (Sim.Date > 8 && (Sim.Date & 3) == 2) {
        SLONG c = 0;
        SLONG d = 0;
        SLONG best = -1;
        SLONG best2 = -1;

        TEAKRAND LocalRand(Sim.Date + Sim.StartTime);

        switch (LocalRand.Rand(8)) {
        case 0: // Flugzeuge
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    if (best == -1 || Sim.Players.Players[c].Planes.GetNumUsed() > Sim.Players.Players[best].Planes.GetNumUsed()) {
                        best = c;
                    } else if (Sim.Players.Players[c].Planes.GetNumUsed() == Sim.Players.Players[best].Planes.GetNumUsed()) {
                        break;
                    }
                }
            }

            if (Sim.Players.Players[best].Planes.GetNumUsed() > 10) {
                AddOverride(1, bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2050)), (LPCTSTR)Sim.Players.Players[best].AirlineX),
                            GetIdFromString("PLANES") + best * 100, 10 + static_cast<SLONG>(best == Sim.localPlayer) * 10);
            }
            break;

        case 1: // Image
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    if (best == -1 || Sim.Players.Players[c].Image > Sim.Players.Players[best].Image) {
                        best = c;
                    } else if (Sim.Players.Players[c].Image == Sim.Players.Players[best].Image) {
                        break;
                    }
                }
            }

            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    if (c != best && (best2 == -1 || Sim.Players.Players[c].Image > Sim.Players.Players[best].Image)) {
                        best2 = c;
                    }
                }
            }

            if (best2 != -1 && Sim.Players.Players[best].Image > 100) {
                AddOverride(1,
                            bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2051)), (LPCTSTR)Sim.Players.Players[best].AirlineX,
                                    (LPCTSTR)Sim.Players.Players[best2].AirlineX),
                            GetIdFromString("LIEBSTE") + best * 100, 10 + static_cast<SLONG>(best == Sim.localPlayer) * 10);
            }
            break;

        case 2: // Routen
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    if (best == -1 || Sim.Players.Players[c].RentRouten.GetNumUsed() > Sim.Players.Players[best].RentRouten.GetNumUsed()) {
                        best = c;
                    } else if (Sim.Players.Players[c].RentRouten.GetNumUsed() == Sim.Players.Players[best].RentRouten.GetNumUsed()) {
                        break;
                    }
                }
            }

            if (Sim.Players.Players[best].RentRouten.GetNumUsed() > 30) {
                AddOverride(1,
                            bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2052)), (LPCTSTR)Sim.Players.Players[best].AirlineX,
                                    Sim.Players.Players[best].RentRouten.GetNumUsed()),
                            GetIdFromString("ROUTEN") + best * 100, 10 + static_cast<SLONG>(best == Sim.localPlayer) * 10);
            }
            break;

        case 3: // Geld
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    if (best == -1 || Sim.Players.Players[c].Money > Sim.Players.Players[best].Money) {
                        best = c;
                    } else if (Sim.Players.Players[c].Money == Sim.Players.Players[best].Money) {
                        break;
                    }
                }
            }

            if (Sim.Players.Players[best].Money > 25000000) {
                AddOverride(1, bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2053)), (LPCTSTR)Sim.Players.Players[best].AirlineX),
                            GetIdFromString("GELD") + best * 100, 10 + static_cast<SLONG>(best == Sim.localPlayer) * 10);
            }
            break;

        case 4: // Bester Pilot:
            for (c = Workers.Workers.AnzEntries() - 1; c >= 0; c--) {
                if (Workers.Workers[c].Employer == Sim.localPlayer && Workers.Workers[c].Typ == WORKER_PILOT) {
                    if (best == -1 || Workers.Workers[c].Talent > Workers.Workers[best].Talent) {
                        best = c;
                    }
                }
            }

            if (best != -1 && Workers.Workers[best].Talent > 90) {
                AddOverride(2,
                            bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2080)), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].AirlineX,
                                    (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX, (LPCTSTR)Workers.Workers[best].Name),
                            GetIdFromString("PILOT"), 20);
            } else {
                c = LocalRand.Rand(4);

                if ((Sim.Players.Players[c].IsOut == 0) && c != Sim.localPlayer) {
                    AddOverride(2,
                                bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2080)), (LPCTSTR)Sim.Players.Players[c].AirlineX,
                                        (LPCTSTR)Sim.Players.Players[c].NameX, (LPCTSTR)Workers.GetRandomName(TRUE)),
                                GetIdFromString("PILOT"), 20);
                }
            }
            break;

        case 5: // Beste Stewardess:
            for (c = Workers.Workers.AnzEntries() - 1; c >= 0; c--) {
                if (Workers.Workers[c].Employer == Sim.localPlayer && Workers.Workers[c].Typ == WORKER_STEWARDESS) {
                    if (best == -1 || Workers.Workers[c].Talent > Workers.Workers[best].Talent) {
                        best = c;
                    }
                }
            }

            if (best != -1 && Workers.Workers[best].Talent > 90) {
                AddOverride(2,
                            bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2081)), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].AirlineX,
                                    (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX, (LPCTSTR)Workers.Workers[best].Name),
                            GetIdFromString("STEWARD"), 20);
            } else {
                c = LocalRand.Rand(4);

                if ((Sim.Players.Players[c].IsOut == 0) && c != Sim.localPlayer) {
                    AddOverride(2,
                                bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2081)), (LPCTSTR)Sim.Players.Players[c].AirlineX,
                                        (LPCTSTR)Sim.Players.Players[c].NameX, (LPCTSTR)Workers.GetRandomName(FALSE)),
                                GetIdFromString("STEWARD"), 20);
                }
            }
            break;

        case 6: // Die meisten Passagiere:
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    if (best == -1 || Sim.Players.Players[c].NumPassengers > Sim.Players.Players[best].NumPassengers) {
                        best = c;
                    }
                }
            }

            AddOverride(2,
                        bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2082)), (LPCTSTR)Sim.Players.Players[best].AirlineX,
                                Sim.Players.Players[best].NumPassengers),
                        GetIdFromString("PASSAG"), 10);
            break;

        case 7: // Das schlechteste Flugzeug:
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if (Sim.Players.Players[c].IsOut == 0) {
                    for (d = Sim.Players.Players[c].Planes.AnzEntries() - 1; d >= 0; d--) {
                        if (Sim.Players.Players[c].Planes.IsInAlbum(d) != 0) {
                            if (best == -1 || Sim.Players.Players[c].Planes[d].Zustand < Sim.Players.Players[best].Planes[best2].Zustand) {
                                best = c;
                                best2 = d;
                            }
                        }
                    }
                }
            }

            if (best != -1 && Sim.Players.Players[best].Planes[best2].Zustand < 50) {
                AddOverride(2,
                            bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2083)), (LPCTSTR)Sim.Players.Players[best].AirlineX,
                                    (LPCTSTR)Sim.Players.Players[best].Planes[best2].Name),
                            GetIdFromString("TRASHP0") + (__int64(best) << (8 * 6)), 10);
            }
            break;
        default:
            hprintf("Misc.cpp: Default case should not be reached.");
            DebugBreak();
        }
    }

    // Sonderzeitungsmeldungen bei bes. Fortschritten im Raktenbau:
    if (Sim.Difficulty == DIFF_FINAL) {
        SLONG c = 0;

        if (Sim.Date == 2) {
            AddOverride(1, StandardTexte.GetS(TOKEN_MISC, 2070), 0, 5);
        }
        if (Sim.Date == 3) {
            AddOverride(0, StandardTexte.GetS(TOKEN_MISC, 2071), 0, 5);
        }
        if (Sim.Date == 5) {
            AddOverride(2, StandardTexte.GetS(TOKEN_MISC, 2072), 0, 5);
        }
        if (Sim.Date == 9) {
            AddOverride(1, StandardTexte.GetS(TOKEN_MISC, 2073), 0, 5);
        }

        for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
            if ((Sim.Players.Players[c].LastRocketFlags & ROCKET_PART_ONE) == ROCKET_PART_ONE) {
                break;
            }
        }

        if (c >= Sim.Players.Players.AnzEntries()) {
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if ((Sim.Players.Players[c].RocketFlags & ROCKET_PART_ONE) == ROCKET_PART_ONE) {
                    AddOverride(1, bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2060)), (LPCTSTR)Sim.Players.Players[c].AirlineX),
                                GetIdFromString("INTRVIEW"), 15 + static_cast<SLONG>(c == Sim.localPlayer) * 10);
                    break;
                }
            }
        }

        for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
            if ((Sim.Players.Players[c].LastRocketFlags & ROCKET_PART_TWO_A) == ROCKET_PART_TWO_A) {
                break;
            }
        }

        if (c >= Sim.Players.Players.AnzEntries()) {
            for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
                if ((Sim.Players.Players[c].RocketFlags & ROCKET_PART_TWO_A) == ROCKET_PART_TWO_A) {
                    AddOverride(1, bprintf((LPCTSTR)CString(StandardTexte.GetS(TOKEN_MISC, 2061)), (LPCTSTR)Sim.Players.Players[c].AirlineX), 0,
                                20 + static_cast<SLONG>(c == Sim.localPlayer) * 10);
                    break;
                }
            }
        }

        for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
            Sim.Players.Players[c].LastRocketFlags = Sim.Players.Players[c].RocketFlags;
        }
    }
}

//--------------------------------------------------------------------------------------------
// Bereitet die initialisirung vor:
//--------------------------------------------------------------------------------------------
void HEADLINES::ReInit(const CString &TabFilename) {
    // TEAKFILE      Tab;
    BUFFER_V<char> Line(300);
    SLONG c = 0;
    SLONG Zeitung = 0;
    SLONG Kette = 0;

    HeadlineFile = TabFilename;

    for (c = 0; c < 3; c++) {
        CurrentChain[c] = 0;
        NumRand[c] = 0;
    }

    // Load Table header:
    // Tab.Open (FullFilename (HeadlineFile, ExcelPath), TEAKFILE_READ);
    auto FileData = LoadCompleteFile(FullFilename(HeadlineFile, ExcelPath));
    SLONG FileP = 0;

    // Die erste Zeile einlesen
    // Tab.ReadLine (Line.getData(), 300);
    FileP = ReadLine(FileData, FileP, Line.getData(), 300);

    while (true) {
        if (FileP >= FileData.AnzEntries()) {
            break;
        }
        // Tab.ReadLine (Line.getData(), 300);
        FileP = ReadLine(FileData, FileP, Line.getData(), 300);

        Zeitung = atoi(strtok(Line.getData(), TabSeparator));
        Kette = atoi(strtok(nullptr, TabSeparator));

        if ((Zeitung != 0) && (Kette == 0)) {
            NumRand[Zeitung - 1]++;
        }
    }
}

//--------------------------------------------------------------------------------------------
// Sortiert die aktuellen Schlagzeilen nach Priorität:
//--------------------------------------------------------------------------------------------
void HEADLINES::SortByPriority() {
    SLONG c = 0;

    for (c = 0; c < 9; c++) {
        if (Headline[c].PicturePriority < Headline[c + 1].PicturePriority && !Headline[c].Headline.IsEmpty() && !Headline[c + 1].Headline.IsEmpty()) {
            CHeadline tmp = Headline[c];
            Headline[c] = Headline[c + 1];
            Headline[c + 1] = tmp;

            c -= 2;
            if (c < -1) {
                c = -1;
            }
        }
    }

    for (c = 0; c < 9; c++) {
        if (Headline[10 + c].PicturePriority < Headline[10 + c + 1].PicturePriority && !Headline[10 + c].Headline.IsEmpty() &&
            !Headline[10 + c + 1].Headline.IsEmpty()) {
            CHeadline tmp = Headline[10 + c];
            Headline[10 + c] = Headline[10 + c + 1];
            Headline[10 + c + 1] = tmp;

            c -= 2;
            if (c < -1) {
                c = -1;
            }
        }
    }

    for (c = 0; c < 9; c++) {
        if (Headline[20 + c].PicturePriority < Headline[20 + c + 1].PicturePriority && !Headline[20 + c].Headline.IsEmpty() &&
            !Headline[20 + c + 1].Headline.IsEmpty()) {
            CHeadline tmp = Headline[20 + c];
            Headline[20 + c] = Headline[20 + c + 1];
            Headline[20 + c + 1] = tmp;

            c -= 2;
            if (c < -1) {
                c = -1;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Speichert ein Headlines-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator<<(TEAKFILE &File, const HEADLINES &Headlines) {
    SLONG c = 0;

    File << Headlines.HeadlineFile << Headlines.IsInteresting;

    for (c = 0; c < 30; c++) {
        File << Headlines.Headline[c].Headline << Headlines.Override[c].Headline;

        File.Write((const UBYTE *)&Headlines.Headline[c].PictureId, 8);
        File.Write((const UBYTE *)&Headlines.Override[c].PictureId, 8);
        File.Write((const UBYTE *)&Headlines.Override[c].PicturePriority, 4);
    }

    for (c = 0; c < 3; c++) {
        File << Headlines.CurrentChain[c];
        File << Headlines.NumRand[c];

        File << Headlines.FlexiCity[c];
        File << Headlines.FlexiNumber[c];
    }

    return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt ein Headlines-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator>>(TEAKFILE &File, HEADLINES &Headlines) {
    SLONG c = 0;

    File >> Headlines.HeadlineFile >> Headlines.IsInteresting;

    for (c = 0; c < 30; c++) {
        File >> Headlines.Headline[c].Headline >> Headlines.Override[c].Headline;

        File.Read(reinterpret_cast<UBYTE *>(&Headlines.Headline[c].PictureId), 8);
        File.Read(reinterpret_cast<UBYTE *>(&Headlines.Override[c].PictureId), 8);
        File.Read(reinterpret_cast<UBYTE *>(&Headlines.Override[c].PicturePriority), 4);
    }

    for (c = 0; c < 3; c++) {
        File >> Headlines.CurrentChain[c];
        File >> Headlines.NumRand[c];

        File >> Headlines.FlexiCity[c];
        File >> Headlines.FlexiNumber[c];
    }

    return (File);
}

//============================================================================================
// CMessages::
//============================================================================================

//--------------------------------------------------------------------------------------------
// Redet ein Berater?
//--------------------------------------------------------------------------------------------
BOOL CMessages::IsSilent() { return static_cast<BOOL>(Messages[0].BeraterTyp == -1 && Messages[1].BeraterTyp == -1); }

//--------------------------------------------------------------------------------------------
// Für einen neuen Tag initialisieren:
//--------------------------------------------------------------------------------------------
void CMessages::NewDay() {
    SLONG c = 0;

    for (c = 0; c < Messages.AnzEntries(); c++) {
        Messages[c].BeraterTyp = -1;
    }

    AktuellerBeraterTyp = -1;
    TalkCountdown = 0;
    LastMessage.BeraterTyp = -1;
}

//--------------------------------------------------------------------------------------------
// Berater entfernen:
//--------------------------------------------------------------------------------------------
void CMessages::NoComments() {
    if (Messages[0].Urgent == MESSAGE_COMMENT && TalkCountdown > 0) {

        TalkCountdown = 1;
    }
}

//--------------------------------------------------------------------------------------------
// Eine Nachricht in die Warteschlange hinzufügen:
//--------------------------------------------------------------------------------------------
void CMessages::AddMessage(SLONG BeraterTyp, const CString &Message, SLONG Urgent, SLONG Mood) {
    SLONG c = 0;

    if (bLeaveGameLoop != 0) {
        return;
    }

    if (BeraterTyp == BERATERTYP_GIRL && Sim.Options.OptionGirl == 0) {
        return;
    }
    if (BeraterTyp != BERATERTYP_GIRL && Sim.Options.OptionBerater == 0) {
        return;
    }

    if (Message.GetLength() > 0) {
        for (c = Messages.AnzEntries() - 1; c >= 0; c--) {
            // Keine Nachricht 2x in die Queue tun:
            if (Messages[c].Message == Message && Messages[c].BeraterTyp != -1) {
                return;
            }
        }
    }

    if (Sim.Time >= 9 * 60000 && Sim.Time <= 18 * 60000 && Sim.CallItADay == 0) {
        // Kommentare nicht schedulen, sondern ersetzen falls schon welche vorhanden:
        if (Urgent == MESSAGE_COMMENT) {
            for (c = Messages.AnzEntries() - 1; c >= 0; c--) {
                if (Messages[c].Urgent == MESSAGE_COMMENT && Messages[c].BeraterTyp == BeraterTyp) {
                    if (Message.GetLength() > 0) {
                        Messages[c].Message = Message;
                    }

                    Messages[c].Mood = Mood;
                    if (Message.GetLength() == 0) {
                        if (c == 0) {
                            if (BeraterWalkState != 1) {
                                TalkCountdown = 1;
                            }
                        } else {
                            for (; c < Messages.AnzEntries() - 2; c++) {
                                Messages[c] = Messages[c + 1];
                            }

                            Messages[Messages.AnzEntries() - 1].BeraterTyp = -1;
                        }
                    } else if (c == 0) {
                        if (BeraterWalkState == 1) {
                            BeraterWalkState = -1;
                        }
                        PaintMessage();
                    }

                    return;
                }
            }
        }

        if (Message.GetLength() > 0) {
            if (Urgent == MESSAGE_URGENT || Urgent == MESSAGE_COMMENT) {
                for (c = Messages.AnzEntries() - 1; c > 0; c--) {
                    Messages[c] = Messages[c - 1];
                    if (c == 1 && Messages[0].BeraterTyp != -1 && Messages[0].BeraterTyp != BeraterTyp) {
                        break;
                    }
                    if (c == 1 && (IsDialog != 0)) {
                        break;
                    }
                }

                Messages[c].BeraterTyp = BeraterTyp;
                Messages[c].Message = Message;
                Messages[c].Urgent = Urgent;
                Messages[c].Mood = Mood;

                if (c == 0 && (IsDialog == 0) && AktuellerBeraterTyp >= 100) {
                    AktuellerBeraterTyp = BeraterTyp;
                }

                // if (c==0 && AktuellerBeraterTyp==BeraterTyp)
                if (c == 0 && (AktuellerBeraterTyp == BeraterTyp || AktuellerBeraterTyp == -1)) {
                    PaintMessage();
                }
            } else if (Urgent == MESSAGE_NORMAL) {
                for (c = 0; c < Messages.AnzEntries(); c++) {
                    if (Messages[c].BeraterTyp == -1) {
                        break;
                    }
                }

                if (c < Messages.AnzEntries()) {
                    Messages[c].BeraterTyp = BeraterTyp;
                    Messages[c].Message = Message;
                    Messages[c].Urgent = Urgent;
                    Messages[c].Mood = Mood;

                    // if (c==0) PaintMessage ();
                    if (c == 0 && AktuellerBeraterTyp == Messages[c].BeraterTyp) {
                        PaintMessage();
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Läßt die aktuelle Nachricht verschwinden:
//--------------------------------------------------------------------------------------------
void CMessages::NextMessage() {
    if (TalkCountdown > 0) {
        TalkCountdown = 1;
    }
}

//--------------------------------------------------------------------------------------------
// Malt die Sprechblase neu und initialisiert den Sprecher:
//--------------------------------------------------------------------------------------------
void CMessages::PaintMessage() {
    SLONG c = 0;

    if (IsDialog == 0) {
        if (Messages[0].Message.GetLength() == 0) {
            for (SLONG c = 0; c < Messages.AnzEntries() - 1; c++) {
                Messages[c] = Messages[c + 1];
            }
        }

        AktuellerBeraterTyp = Messages[0].BeraterTyp;

        if (AktuellerBeraterTyp < 100) {
            AktuelleBeraterBitmap = 1;

            if (AktuellerBeraterTyp == BERATERTYP_GIRL) {
                AktuelleBeraterBitmap = 0;
            }
            if (AktuellerBeraterTyp == BERATERTYP_SICHERHEIT) {
                AktuelleBeraterBitmap = 8;

                for (c = 0; c < Workers.Workers.AnzEntries(); c++) {
                    if (Workers.Workers[c].Typ == BERATERTYP_SICHERHEIT && Workers.Workers[c].Employer == PlayerNum) {
                        AktuelleBeraterBitmap += Workers.Workers[c].Geschlecht;
                        break;
                    }
                }
            }
        }

        BlinkCountdown = 20;
        TalkPhase = 0;

        if (Messages[0].Message.GetLength() != 0) {
            for (c = 0; c < 4; c++) {
                if (SprechblaseBm.TryPrintAt(Messages[0].Message, FontSmallBlack, TEC_FONT_LEFT, SprechblasenSizes[c].left, SprechblasenSizes[c].top,
                                             SprechblasenSizes[c].right,
                                             SprechblasenSizes[c].bottom) < SprechblasenSizes[c].bottom - SprechblasenSizes[c].top) {
                    break;
                }
            }

            if (c < 4) {
                if (c < 3 && Messages[0].Mood != -1) {
                    c = std::max(SLONG(1), c);

                    SprechblaseBm.ReSize(::SprechblasenBms[c + 3].Size);
                    SprechblaseBm.BlitFrom(::SprechblasenBms[c + 3]);

                    SprechblaseBm.PrintAt(Messages[0].Message, FontSmallBlack, TEC_FONT_LEFT, SprechblasenSizes[c + 3].left, SprechblasenSizes[c + 3].top,
                                          SprechblasenSizes[c + 3].right, SprechblasenSizes[c + 3].bottom);
                } else {
                    SprechblaseBm.ReSize(::SprechblasenBms[c].Size);
                    SprechblaseBm.BlitFrom(::SprechblasenBms[c]);

                    SprechblaseBm.PrintAt(Messages[0].Message, FontSmallBlack, TEC_FONT_LEFT, SprechblasenSizes[c].left, SprechblasenSizes[c].top,
                                          SprechblasenSizes[c].right, SprechblasenSizes[c].bottom);
                }

                Messages[0].BubbleType = c;
            }
        }

        TalkCountdown = 10 + Messages[0].Message.GetLength() * 2;
    }
}

//--------------------------------------------------------------------------------------------
// Ticker-Routine:
//--------------------------------------------------------------------------------------------
void CMessages::Pump() {
    SLONG c = 0;

    SLONG HandyOffset = 0;

    if (Sim.Players.Players[PlayerNum].LocationWin != nullptr) {
        if (((Sim.Players.Players[PlayerNum].LocationWin)->DialogMedium != 0) && ((Sim.Players.Players[PlayerNum].LocationWin)->MenuIsOpen() == 0) &&
            ((Sim.Players.Players[PlayerNum].LocationWin)->IsDialogOpen() != 0)) {
            HandyOffset = 4;
        }
    }

    if (IsDialog != 0) {
        if (AktuellerBeraterTyp == -1) {
            AktuellerBeraterTyp = 100 + PlayerNum;
            AktuelleBeraterBitmap = 12 + PlayerNum;
        }

        if (AktuellerBeraterTyp < 100) {
            BeraterPosY += 9;
            BeraterWalkState = 1;
            if (BeraterPosY >= 440) {
                AktuellerBeraterTyp = 100 + PlayerNum;
                AktuelleBeraterBitmap = 12 + PlayerNum;
            }
        } else if (AktuellerBeraterTyp >= 100 && BeraterPosY > 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset]) {
            BeraterPosY -= 9;
            if (BeraterPosY <= 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset]) {
                BeraterPosY = 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset];
                BeraterWalkState = 0;
            }
        } else {
            BeraterWalkState = 0;

            if (BeraterPosY <= 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset]) {
                BeraterPosY = 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset];
            }

            if (IsDialogTalking != 0) {
                IsDialogTalking--;

                if (IsDialogTalking != 0) {
                    TalkPhase++;
                } else {
                    TalkPhase = 0;
                }
            }
        }
    } else {
        if (AktuellerBeraterTyp == -1 && Messages[0].BeraterTyp != -1) {
            PaintMessage();
            BeraterPosY = 440;
            BeraterWalkState = -1;
        } else if (AktuellerBeraterTyp != -1) {
            if (BeraterWalkState == -1) {
                BeraterPosY -= 9;
                if (BeraterPosY <= 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset]) {
                    BeraterPosY = 440 - BeraterSlideY[AktuelleBeraterBitmap + HandyOffset];
                    BeraterWalkState = 0;
                }
            } else if (BeraterWalkState == 1) {
                BeraterPosY += 9;
                if (BeraterPosY >= 440) {
                    BeraterPosY = 440;

                    AktuellerBeraterTyp = -1;
                }
            } else if (TalkCountdown >= 0) {
                TalkCountdown--;

                TalkPhase++;
                if (BlinkCountdown > 0) {
                    BlinkCountdown--;
                    if (BlinkCountdown == 0) {
                        BlinkCountdown = -30 - rand() % 150;
                    }
                } else if (BlinkCountdown < 0) {
                    BlinkCountdown++;
                    if (BlinkCountdown == 0) {
                        BlinkCountdown = 20;
                    }
                }

                if (TalkCountdown <= 0) {
                    LastMessage = Messages[0];

                    for (c = 0; c < Messages.AnzEntries() - 1; c++) {
                        Messages[c] = Messages[c + 1];
                    }

                    if (Messages[0].BeraterTyp != AktuellerBeraterTyp) {
                        if (IsMonolog == FALSE || Messages[0].BeraterTyp != -1) {
                            BeraterWalkState = 1;
                        }
                    } else {
                        PaintMessage();
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Läßt einen Berater sofort (ohne sanftes runtersliden) verschwinden:
//------------------------------------------------------------------------------
void CMessages::KillBerater() { BeraterPosY = 440; }

//------------------------------------------------------------------------------
// Der beginnt Spieler einen Dialog:
//------------------------------------------------------------------------------
void CMessages::StartDialog(SLONG PlayerNum) {
    CMessages::IsDialog = TRUE;
    CMessages::IsDialogTalking = FALSE;
    CMessages::PlayerNum = PlayerNum;

    TalkPhase = 0;

    if (AktuellerBeraterTyp != -1) {
        BeraterWalkState = 1;
    }
}

//------------------------------------------------------------------------------
// Beendet der Spieler einen Dialog
//------------------------------------------------------------------------------
void CMessages::StopDialog() { IsDialog = FALSE; }

//------------------------------------------------------------------------------
// Speichert ein Message-Objekt:
//------------------------------------------------------------------------------
TEAKFILE &operator<<(TEAKFILE &File, const CMessage &Message) {
    File << Message.BeraterTyp << Message.Message << Message.Urgent << Message.Mood;
    File << Message.BubbleType;

    return (File);
}

//------------------------------------------------------------------------------
// Lädt ein Message-Objekt:
//------------------------------------------------------------------------------
TEAKFILE &operator>>(TEAKFILE &File, CMessage &Message) {
    File >> Message.BeraterTyp >> Message.Message >> Message.Urgent >> Message.Mood;
    File >> Message.BubbleType;

    return (File);
}

//------------------------------------------------------------------------------
// Speichert ein Messages-Objekt:
//------------------------------------------------------------------------------
TEAKFILE &operator<<(TEAKFILE &File, const CMessages &Messages) {
    File << Messages.Messages << Messages.AktuellerBeraterTyp;
    File << Messages.TalkPhase << Messages.TalkCountdown;
    File << Messages.BlinkCountdown;
    File << Messages.BeraterPosY << Messages.BeraterWalkState;
    File << Messages.IsMonolog;
    File << Messages.IsDialog << Messages.IsDialogTalking;
    File << Messages.PlayerNum << Messages.AktuelleBeraterBitmap;
    File << Messages.LastMessage;

    return (File);
}

//------------------------------------------------------------------------------
// Lädt ein Messages-Objekt:
//------------------------------------------------------------------------------
TEAKFILE &operator>>(TEAKFILE &File, CMessages &Messages) {
    File >> Messages.Messages >> Messages.AktuellerBeraterTyp;
    File >> Messages.TalkPhase >> Messages.TalkCountdown;
    File >> Messages.BlinkCountdown;
    File >> Messages.BeraterPosY >> Messages.BeraterWalkState;
    File >> Messages.IsMonolog;
    File >> Messages.IsDialog >> Messages.IsDialogTalking;
    File >> Messages.PlayerNum >> Messages.AktuelleBeraterBitmap;
    File >> Messages.LastMessage;

    return (File);
}

//------------------------------------------------------------------------------
// Hierdurch wird aus 10000 das schönere 10.000
//------------------------------------------------------------------------------
CString Insert1000erDots(SLONG Value) {
    short c = 0;
    short d = 0; // Position in neuen bzw. alten String
    short l = 0; // Stringlänge
    short CharsUntilPoint = 0;
    char String[80];
    char Tmp[80];

    static char Dot = 0;

    if (Dot == 0) {
        Dot = std::use_facet<std::moneypunct<char, true>>(std::locale("")).thousands_sep();
        if (Dot != '.' && Dot != ',' && Dot != ':' && Dot != '/' && Dot != '\'' && Dot != '`') {
            Dot = '.';
        }
    }

    sprintf(Tmp, "%i", Value);

    l = short(strlen(Tmp));

    String[0] = Tmp[0];
    c = d = 1;

    CharsUntilPoint = short((l - c) % 3);

    if (Value < 0) {
        CharsUntilPoint = short((l - 1 - c) % 3 + 1);
    }

    for (; d < l; c++, d++) {
        if (CharsUntilPoint == 0 && d < l - 1) {
            String[c] = Dot;
            c++;
            CharsUntilPoint = 3;
        }

        String[c] = Tmp[d];
        CharsUntilPoint--;
    }

    String[c] = '\0';

    return (String);
}

//------------------------------------------------------------------------------
// Hierdurch wird aus 10000 das schönere 10.000
//------------------------------------------------------------------------------
CString Insert1000erDots64(__int64 Value) {
    short c = 0;
    short d = 0; // Position in neuen bzw. alten String
    short l = 0; // Stringlänge
    short CharsUntilPoint = 0;
    char String[40];
    char Tmp[40];

    static char Dot = 0;

    if (Dot == 0) {
        Dot = std::use_facet<std::moneypunct<char, true>>(std::locale("")).thousands_sep();
        if (Dot != '.' && Dot != ',' && Dot != ':' && Dot != '/' && Dot != '\'' && Dot != '`') {
            Dot = '.';
        }
    }

    sprintf(Tmp, "%lli", Value);

    l = short(strlen(Tmp));

    String[0] = Tmp[0];
    c = d = 1;

    CharsUntilPoint = short((l - c) % 3);

    if (Value < 0) {
        CharsUntilPoint = short((l - 1 - c) % 3 + 1);
    }

    for (; d < l; c++, d++) {
        if (CharsUntilPoint == 0 && d < l - 1) {
            String[c] = Dot;
            c++;
            CharsUntilPoint = 3;
        }

        String[c] = Tmp[d];
        CharsUntilPoint--;
    }

    String[c] = '\0';

    return (String);
}

//------------------------------------------------------------------------------
// Malt eine Sprechblase
//------------------------------------------------------------------------------
CRect PaintTextBubble(SBBM &OffscreenBm, const XY &p1, const XY &p2, const XY &Entry) {
    SLONG c = 0;
    SLONG l = 0;
    SLONG SizeY = 0;
    CRect rc(p1.x, p1.y, p2.x, p2.y);

    if (p2.y - p1.y < 68) {
        SizeY = XBubbleBms[8].Size.y;
        l = p2.y - p1.y - XBubbleBms[6].Size.y - SizeY;
    } else {
        SizeY = XBubbleBms[4].Size.y;
        l = p2.y - p1.y - XBubbleBms[0].Size.y - SizeY;
    }

    for (c = 0; c < l; c += XBubbleBms[2].Size.y) {
        OffscreenBm.BlitFrom(XBubbleBms[2], p1 + XY(0, SizeY + c));
        OffscreenBm.BlitFrom(XBubbleBms[3], XY(p2.x - XBubbleBms[3].Size.x, p1.y + SizeY + c));
    }

    if (p2.y - p1.y < 68) {
        OffscreenBm.BlitFrom(XBubbleBms[6], p1);
        OffscreenBm.BlitFrom(XBubbleBms[7], XY(p2.x - XBubbleBms[7].Size.x, p1.y));
        OffscreenBm.BlitFrom(XBubbleBms[8], XY(p1.x, p2.y - XBubbleBms[8].Size.y));
        OffscreenBm.BlitFrom(XBubbleBms[9], XY(p2.x - XBubbleBms[9].Size.x, p2.y - XBubbleBms[9].Size.y));
    } else {
        OffscreenBm.BlitFrom(XBubbleBms[0], p1);
        OffscreenBm.BlitFrom(XBubbleBms[1], XY(p2.x - XBubbleBms[1].Size.x, p1.y));
        OffscreenBm.BlitFrom(XBubbleBms[4], XY(p1.x, p2.y - XBubbleBms[4].Size.y));
        OffscreenBm.BlitFrom(XBubbleBms[5], XY(p2.x - XBubbleBms[5].Size.x, p2.y - XBubbleBms[5].Size.y));
    }

    if (Entry.x == p1.x) {
        OffscreenBm.BlitFromT(XBubbleBms[12], Entry - XY(XBubbleBms[12].Size.x - 2, XBubbleBms[12].Size.y / 2));
        rc.left -= XBubbleBms[12].Size.x;
    } else if (Entry.x == p2.x) {
        OffscreenBm.BlitFromT(XBubbleBms[13], Entry - XY(2, XBubbleBms[13].Size.y / 2));
        rc.right += XBubbleBms[13].Size.x;
    } else if (Entry.y == p1.y || Entry.y == p1.y - 1) {
        OffscreenBm.BlitFromT(XBubbleBms[11], Entry - XY(-XBubbleBms[11].Size.x / 2, XBubbleBms[11].Size.y - 3));
        rc.top -= XBubbleBms[11].Size.y;
    } else if (Entry.y == p2.y) {
        OffscreenBm.BlitFromT(XBubbleBms[10], Entry - XY(-XBubbleBms[10].Size.x / 2, 2));
        rc.bottom += XBubbleBms[10].Size.y;
    }

    return (rc);
}

SLONG iPastRandoms = 0;
SLONG PastRandoms[] = {0xdead, 0xdead, 0xdead};

void CheckEventSync(SLONG EventId);

void CheckEventSync(SLONG EventId) {
    hprintf("%li,", EventId);

    if (EventId != PastRandoms[iPastRandoms]) {
        DebugBreak();
    }
    iPastRandoms++;

    if (PastRandoms[iPastRandoms] == 0xdead && PastRandoms[iPastRandoms + 1] == 0xdead && PastRandoms[iPastRandoms + 2] == 0xdead) {
        DebugBreak(); /**/
    }
}

//--------------------------------------------------------------------------------------------
// Konstruktor mit Seed=0
//--------------------------------------------------------------------------------------------
TEAKRAND::TEAKRAND() { Seed = Value = 0; }

//--------------------------------------------------------------------------------------------
// Kondtruktor mit Seed Angabe
//--------------------------------------------------------------------------------------------
TEAKRAND::TEAKRAND(ULONG Seed) { this->Seed = Value = Seed; }

//--------------------------------------------------------------------------------------------
// nachträglicher Kondtruktor
//--------------------------------------------------------------------------------------------
void TEAKRAND::SRand(ULONG Seed) { this->Seed = Value = Seed; }

//--------------------------------------------------------------------------------------------
// nachträglicher Kondtruktor mit Seed aus Uhrzeit
//--------------------------------------------------------------------------------------------
void TEAKRAND::SRandTime() { Seed = Value = AtGetTime(); }

//--------------------------------------------------------------------------------------------
// Setzt den Zufallsgenerator auf dem exakten Zustand nach dem Konstruktor zurück:
//--------------------------------------------------------------------------------------------
void TEAKRAND::Reset() { Value = Seed; }

//--------------------------------------------------------------------------------------------
// eigentlicher Zufallszahlengenerator: (Xn+1 = Xn * 7381 + 269EC3h)
//--------------------------------------------------------------------------------------------
UWORD TEAKRAND::Rand() {
#ifdef ENABLE_ASM
    ULONG localValue = Value;

    _asm
        {
        pusha

             // Thanx to Machosoft!
            mov       eax, [localValue]
            mov       ecx, eax
            lea       eax, dword ptr [eax+eax*4]
            lea       eax, dword ptr [eax+eax*4]
            add       eax, ecx
            lea       eax, dword ptr [ecx+eax*8]
            shl       eax, 8
            sub       eax, ecx
            lea       eax, dword ptr [ecx+eax*4]
            add       eax, 00269ec3h
            mov       [localValue], eax

            popa
        }

    Value = localValue;

    return (UWORD(localValue >> 16));
#else
    Value = Value * 7381 + 0x269EC3;

    return (UWORD(Value >> 16));
#endif
}

//--------------------------------------------------------------------------------------------
// Zufallszahl aus dem Intervall [0..Max]
//--------------------------------------------------------------------------------------------
UWORD TEAKRAND::Rand(SLONG Max) {
    if (Max == 0) {
        AT_Log_I("RAND", "0 used as Rand(m) argument");
        return 0;
    }
        
    if (this == pSurvisedRandom1 || this == pSurvisedRandom2) {
        CheckEventSync(-Sim.TimeSlice);
        CheckEventSync(Max);
        CheckEventSync(SLONG(Value >> 16));
    }

    return (UWORD(Rand() % Max));
}

//--------------------------------------------------------------------------------------------
// Zufallszahl aus dem Intervall [Min..Max]
//--------------------------------------------------------------------------------------------
UWORD TEAKRAND::Rand(SLONG Min, SLONG Max) { return (UWORD(Rand() % (Max - Min + 1) + Min)); }

//--------------------------------------------------------------------------------------------
// Gibt den aktuellen "Seed" zus Generators zurück:
//--------------------------------------------------------------------------------------------
ULONG TEAKRAND::GetSeed() { return (Value); }

//--------------------------------------------------------------------------------------------
// Speichert den Zustand des Generators:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator<<(TEAKFILE &File, const TEAKRAND &r) {
    File << r.Seed << r.Value;

    return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt den Zustand des Generators:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator>>(TEAKFILE &File, TEAKRAND &r) {
    File >> r.Seed >> r.Value;

    return (File);
}

//--------------------------------------------------------------------------------------------
// Zählt mit Verzeichnisundwildcard String die Dateinamen:
//--------------------------------------------------------------------------------------------
SLONG CountMatchingFilelist(const CString &DirAndWildcards) {
    SLONG Pos = DirAndWildcards.Find('*');
    CString Dir = DirAndWildcards.Left(Pos);
    CString Ext = DirAndWildcards.Right(Pos);

    SLONG n = 0;

    // Liste holen:
    for (const auto &file : std::filesystem::directory_iterator(std::string(Dir))) {
        const std::filesystem::path &path = file.path();
        if (!file.is_directory() && path.extension() == std::string(Ext)) {
            n++;
        }
    }

    return (n);
}

//--------------------------------------------------------------------------------------------
// Erzeugt aus Verzeichnisundwildcard String ein Array mit Dateinamen:
//--------------------------------------------------------------------------------------------
void GetMatchingFilelist(const CString &DirAndWildcards, BUFFER_V<CString> &Array) {
    SLONG Pos = DirAndWildcards.Find("*");
    CString Dir = DirAndWildcards.Left(Pos);
    CString Ext = DirAndWildcards.Right(DirAndWildcards.GetLength() - Pos - 1);

    Array.ReSize(0);

    // Liste holen:
    for (const auto &file : std::filesystem::directory_iterator(std::string(Dir))) {
        const std::filesystem::path &path = file.path();
        if (!file.is_directory() && path.extension() == std::string(Ext)) {
            Array.ReSize(Array.AnzEntries() + 1);
            Array[Array.AnzEntries() - 1] = path.filename();
        }
    }

    // Liste sortieren:
    for (SLONG c = 0; c < Array.AnzEntries() - 1; c++) {
        if (Array[c] > Array[c + 1]) {
            CString tmp = Array[c];
            Array[c] = Array[c + 1];
            Array[c + 1] = tmp;
            c -= 2;
            if (c < -1) {
                c = -1;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Erzeugt aus Verzeichnisundwildcard String ein Array mit Dateinamen:
//--------------------------------------------------------------------------------------------
CString GetMatchingNext(const CString &DirAndWildcards, const CString &CurrentFilename, SLONG Add) {
    BUFFER_V<CString> Array;

    GetMatchingFilelist(DirAndWildcards, Array);

    if (Array.AnzEntries() == 0) {
        return ("");
    }

    // Nach aktuellem Eintrag suchen:
    for (SLONG c = 0; c < Array.AnzEntries(); c++) {
        if (Array[c] == CurrentFilename) {
            return (Array[(c + Add + Array.AnzEntries() * 100) % Array.AnzEntries()]);
        }
    }

    return Array[0];
}

//--------------------------------------------------------------------------------------------
// Gibt den nächstbesten, notfalls numerierten freien Dateinamen zurück (Input mit %s)
//--------------------------------------------------------------------------------------------
CString CreateNumeratedFreeFilename(const CString &DirAndFilename) {
    SLONG c = 0;

    for (c = 0; c < 100000; c++) {
        const char *fn = bprintf(DirAndFilename, c == 0 ? "" : bitoa(c));
        if (DoesFileExist(fn) == 0) {
            return (fn);
        }
    }

    return ("");
}

//--------------------------------------------------------------------------------------------
// Gibt nur den Dateinamen zurück:
//--------------------------------------------------------------------------------------------
CString GetFilenameFromFullFilename(CString FullFilename) {
    for (SLONG c = FullFilename.GetLength() - 1; c >= 0; c--) {
        if (FullFilename[c] == '/' || FullFilename[c] == '\\') {
            return (FullFilename.Mid(c + 1));
        }
    }

    return (FullFilename);
}

//--------------------------------------------------------------------------------------------
// Entfernt französische Akzente:
//--------------------------------------------------------------------------------------------
CString RemoveAccents(CString str) {
    for (SLONG c = str.GetLength() - 1; c >= 0; c--) {
        if (str[c] == '\xE1' || str[c] == '\xE0' || str[c] == '\xE2') {
            str.SetAt(c, 'a');
        }
        if (str[c] == '\xE9' || str[c] == '\xE8' || str[c] == '\xEA') {
            str.SetAt(c, 'e');
        }
        if (str[c] == '\xED' || str[c] == '\xEC' || str[c] == '\xEE') {
            str.SetAt(c, 'i');
        }
        if (str[c] == '\xF3' || str[c] == '\xF2' || str[c] == '\xF4') {
            str.SetAt(c, 'o');
        }
        if (str[c] == '\xFA' || str[c] == '\xF9' || str[c] == '\xFB') {
            str.SetAt(c, 'u');
        }

        if (str[c] == '\xC1' || str[c] == '\xC0' || str[c] == '\xC2') {
            str.SetAt(c, 'A');
        }
        if (str[c] == '\xC9' || str[c] == '\xC8' || str[c] == '\xCA') {
            str.SetAt(c, 'E');
        }
        if (str[c] == '\xCD' || str[c] == '\xCC' || str[c] == '\xCE') {
            str.SetAt(c, 'I');
        }
        if (str[c] == '\xD3' || str[c] == '\xD2' || str[c] == '\xD4') {
            str.SetAt(c, 'O');
        }
        if (str[c] == '\xDA' || str[c] == '\xD9' || str[c] == '\xDB') {
            str.SetAt(c, 'U');
        }
    }

    return (str);
}

#ifndef WIN32
//--------------------------------------------------------------------------------------------
// Win32 replacement:
//--------------------------------------------------------------------------------------------

BOOL OffsetRect(RECT *pRect, SLONG dx, SLONG dy) {
    pRect->left += dx;
    pRect->top += dy;
    pRect->right += dx;
    pRect->bottom += dy;
    return TRUE;
}

void DebugBreak() { assert(0); }
#endif

DWORD AtGetTime() {
    static std::chrono::nanoseconds now = std::chrono::steady_clock::now().time_since_epoch();
    static DWORD epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    return epoch + SDL_GetTicks64();
}

DWORD AtGetTickCount() { return SDL_GetTicks(); }

SLONG AtGetAsyncKeyState(SLONG vKey) { return (SDL_GetModState() & vKey) != 0 ? 0x8000 : 0; }

SLONG GetCurrentYear() {
    const time_t t = time(nullptr);
    const tm *currentTime = localtime(&t);
    return currentTime->tm_year + 1900;
}

CString getCurrentDayString() {
    CString output;
    if (AtGetTime() % 6000 > 3000 && (Sim.Difficulty == DIFF_ADDON03 || Sim.Difficulty == DIFF_ADDON04 || Sim.Difficulty == DIFF_ADDON06)) {
        SLONG n = 0;

        switch (Sim.Difficulty) {
        case DIFF_ADDON03:
            n = TARGET_DAYS - Sim.Date;
            break;
        case DIFF_ADDON04:
            n = TARGET_MILESDAYS - Sim.Date;
            break;
        case DIFF_ADDON06:
            n = TARGET_DAYS - Sim.Date;
            break;
        default:
            break;
        }

        output = bprintf(StandardTexte.GetS(TOKEN_STAT, 9010 + static_cast<SLONG>(n == 1)), n);
    } else {
        output = bitoa(Sim.Date + 1);
        output += ". ";
        output += StandardTexte.GetS(TOKEN_STAT, 9000);
    }
    return output;
}
