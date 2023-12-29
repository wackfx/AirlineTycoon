//============================================================================================
// Buero.cpp : Das Büro des Spielers
//============================================================================================
#include "StdAfx.h"
#include "Buero.h"
#include "glBueroDrk.h"
#include "glbuero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

static XY PlOffset[4] = {XY(40, 150), XY(40, 150), XY(40, 150), XY(40, 150)};

static CRect MantelPos[4] = {CRect(396, 42, 533, 173), CRect(454, 102, 524, 266), CRect(454, 102, 524, 266), CRect(454, 102, 524, 266)};

class CWaitCursorNow {
  public:
    CWaitCursorNow() {
        MouseWait++;
        pCursor->SetImage(gCursorSandBm.pBitmap);
    };

    ~CWaitCursorNow() {
        MouseWait--;
        if (MouseWait == 0) {
            pCursor->SetImage(gCursorBm.pBitmap);
        }
    };
};

//--------------------------------------------------------------------------------------------
// Büro-Konstruktor:
//--------------------------------------------------------------------------------------------
CBuero::CBuero(BOOL handy, ULONG playerNum) : CStdRaum(handy, playerNum, "", 0) {
    CWaitCursorNow wc; // CD-Cursor anzeigen

    SetRoomVisited(PlayerNum, ROOM_BURO_A);

    OfficeState = Sim.Players.Players[static_cast<SLONG>(PlayerNum)].OfficeState;
    if (OfficeState == 3) {
        pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("buerodrk.gli", RoomPath)), &pGLibDark, L_LOCMEM);
        DarkBm.ReSize(pGLibDark, "BUERO");
    } else {
        pGLibDark = nullptr;
    }

    IsInBuro = TRUE;

    Sim.FocusPerson = -1;

    CString BuroName = bprintf("buero_%c.gli", 'a' + PlayerNum);

    /*GlowEffects.ReSize (2);
      GlowEffects[0] = XY (30,30);
      GlowEffects[1] = XY (40,30);    */

    KommVarLetters = 0;
    KommVarTelefon = 0;
    KommVarCloseLetter = 0;

    ReSize(BuroName, GFX_BUERO);

    AirportRoomPos = Airport.GetRandomTypedRune(RUNE_2SHOP, UBYTE(ROOM_BURO_A + PlayerNum * 10));
    AirportRoomPos.y = AirportRoomPos.y - 5000 + 93;

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(40);
    }

    LetterBm.ReSize(pRoomLib, GFX_LETTERS);
    DoorOpaqueBm.ReSize(pRoomLib, GFX_DOOR_O);
    DoorTransBm.ReSize(pRoomLib, GFX_DOOR_T);

    NoSaboBm.ReSize(pRoomLib, "NOSABO");

    // Raumanimationen
    if (PlayerNum == 1) {
        SP_Ticker.ReSize(1); // Uhrpendel:
        SP_Ticker.Clips[0].ReSize(0, "uhr.smk", "uhr.raw", XY(420, 109), SPM_IDLE, CRepeat(99, 99), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr,
                                  SMACKER_CLIP_SET, 0, nullptr, "A9", 0);
    }

    SP_Player.ReSize(11);
    SP_Player.Clips[0].ReSize(0, bprintf("pl%liwait.smk", PlayerNum + 1), "", PlOffset[PlayerNum], SPM_IDLE, CRepeat(1, 1), CPostWait(30, 30),
                              SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "A9A2A1A2E1E1", 0, 1, 2, 3, 4, 7);
    SP_Player.Clips[1].ReSize(1, bprintf("pl%liblat.smk", PlayerNum + 1), "plxblat.raw", PlOffset[PlayerNum], SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0),
                              SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "E1A1A1", 0, 0, 1);
    SP_Player.Clips[2].ReSize(2, CString(bprintf("pl%liscri1.smk", PlayerNum + 1)), CString(bprintf("pl%liscri1.raw", PlayerNum + 1)), PlOffset[PlayerNum],
                              SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "E1A1A1", 0, 0, 1);
    SP_Player.Clips[3].ReSize(3, CString(bprintf("pl%liscri2.smk", PlayerNum + 1)), CString(bprintf("pl%liscri2.raw", PlayerNum + 1)), PlOffset[PlayerNum],
                              SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "E1A1A1", 0, 0, 1);

    SP_Player.Clips[4].ReSize(4, bprintf("pl%librief.smk", PlayerNum + 1), "", PlOffset[PlayerNum], SPM_LETTER, CRepeat(1, 1), CPostWait(0, 0),
                              SMACKER_CLIP_DONTCANCEL, &KommVarLetters, SMACKER_CLIP_SET | SMACKER_CLIP_POST, 1, nullptr, "A1", 5);
    SP_Player.Clips[5].ReSize(5, bprintf("pl%librifw.smk", PlayerNum + 1), "", PlOffset[PlayerNum], SPM_LETTER, CRepeat(1, 1), CPostWait(30, 30),
                              SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "A1E1", 5, 6);
    SP_Player.Clips[6].ReSize(6, bprintf("pl%librifz.smk", PlayerNum + 1), "", PlOffset[PlayerNum], SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0),
                              SMACKER_CLIP_DONTCANCEL, &KommVarCloseLetter, SMACKER_CLIP_SET | SMACKER_CLIP_POST, 1, nullptr, "A1", 0);

    SP_Player.Clips[7].ReSize(7, bprintf("pl%litelef.smk", PlayerNum + 1), bprintf("pl%litelef.raw", PlayerNum + 1), PlOffset[PlayerNum], SPM_IDLE,
                              CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &KommVarTelefon, SMACKER_CLIP_XOR | SMACKER_CLIP_POST, 2, nullptr,
                              "A1A1A1", 8, 9, 10);
    SP_Player.Clips[8].ReSize(8, bprintf("pl%litelew.smk", PlayerNum + 1), "", PlOffset[PlayerNum], SPM_LISTENING, CRepeat(1, 1), CPostWait(30, 30),
                              SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "A1E1E1", 8, 9, 10);
    SP_Player.Clips[9].ReSize(9, bprintf("pl%liteler.smk", PlayerNum + 1), "", PlOffset[PlayerNum], SPM_TALKING, CRepeat(1, 1), CPostWait(0, 0),
                              SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "E1A1E1", 8, 9, 10);
    SP_Player.Clips[10].ReSize(10, bprintf("pl%litelez.smk", PlayerNum + 1), bprintf("pl%litelez.raw", PlayerNum + 1), PlOffset[PlayerNum], SPM_IDLE,
                               CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0, nullptr, "A1", 0);
}

//--------------------------------------------------------------------------------------------
// Destruktor:
//--------------------------------------------------------------------------------------------
CBuero::~CBuero() {
    LetterBm.Destroy();
    DoorOpaqueBm.Destroy();
    DoorTransBm.Destroy();
    DarkBm.Destroy();
    NoSaboBm.Destroy();

    if ((pGLibDark != nullptr) && (pGfxMain != nullptr)) {
        pGfxMain->ReleaseLib(pGLibDark);
    }
    pGLibDark = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CBuero message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// void CBuero::OnPaint()
//--------------------------------------------------------------------------------------------
void CBuero::OnPaint() {
    SLONG d = 0;
    XY RoomPos;

    if (Sim.Date > 4) {
        Sim.GiveHint(HINT_RICK);
    }

    if (OfficeState != 3 && Sim.Players.Players[PlayerNum].OfficeState == 3) {
        OfficeState = 3;
        pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("buerodrk.gli", RoomPath)), &pGLibDark, L_LOCMEM);
        DarkBm.ReSize(pGLibDark, "BUERO");
    } else {
        pGLibDark = nullptr;
    }

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    SP_Ticker.Pump();
    SP_Ticker.BlitAtT(RoomBm);

    // Draw Persons:
    RoomBm.pBitmap->SetClipRect(CRect(539, 67, 639, 289));
    for (d = 0; d < Sim.Persons.AnzEntries(); d++) {
        // Entscheidung! Person malen:
        if ((Sim.Persons.IsInAlbum(d) != 0) && (Clans.IsInAlbum(Sim.Persons[d].ClanId) != 0)) {
            if (Clans[static_cast<SLONG>(Sim.Persons[d].ClanId)].Type >= CLAN_PLAYER1 &&
                Clans[static_cast<SLONG>(Sim.Persons[d].ClanId)].Type <= CLAN_PLAYER4) {
                PERSON &qPerson = Sim.Persons[d];
                CLAN &qClan = Clans[static_cast<SLONG>(qPerson.ClanId)];
                UBYTE Dir = qPerson.LookDir;
                UBYTE Phase = qPerson.Phase;

                if (Dir == 1 || Dir == 3) {
                    Dir = UBYTE(4 - Dir);
                }
                if (Dir == 8 && Phase < 4) {
                    Phase = UBYTE((Phase + 2) % 4);
                }
                if (Dir == 8 && Phase >= 4) {
                    Phase = UBYTE((Phase + 2) % 4 + 4);
                }

                if (abs(qPerson.ScreenPos.x - AirportRoomPos.x) < 70 && qPerson.ScreenPos.y >= AirportRoomPos.y + 5) {
                    qClan.BlitLargeAt(RoomBm, Dir, Phase, XY(595, 289) - (qPerson.ScreenPos - AirportRoomPos));
                }
            }
        } else {
            break;
        }
    }

    RoomBm.pBitmap->SetClipRect(CRect(0, 0, 640, 480));

    RoomBm.BlitFromT(DoorOpaqueBm, 521, 53);
    ColorFX.BlitTrans(DoorTransBm.pBitmap, RoomBm.pBitmap, XY(542, 67), nullptr, 2);

    if ((Sim.Players.Players[PlayerNum].SecurityFlags & 32) != 0U) {
        RoomBm.BlitFrom(NoSaboBm, 390, 309);
    }

    if (Sim.Players.Players[PlayerNum].Letters.AnzLetters() != 0) {
        RoomBm.BlitFromT(LetterBm, 351, 390);
    }

    SP_Player.Pump();
    SP_Player.BlitAtT(RoomBm);

    if (Sim.Players.Players[PlayerNum].OfficeState == 3) {
        RoomBm.BlitFromT(DarkBm, 0, 0);
    }

    if (KommVarLetters != 0) {
        MenuStart(MENU_LETTERS);
        MenuSetZoomStuff(XY(379, 382), 0.1, FALSE);
        KommVarLetters = 0;
    }

    if (KommVarCloseLetter != 0) {
        MenuStop();
        KommVarCloseLetter = 0;
    }

    if (KommVarTelefon == 3) {
        MenuStart(MENU_FILOFAX, 2);
        MenuSetZoomStuff(XY(363, 321), 0.1, FALSE);
        KommVarTelefon = 0;
    } else if (KommVarTelefon == 2) // Spieler wird angerufen
    {
        KommVarTelefon = 0;
    }

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::InitToolTips();

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        if (gMousePosition.IfIsWithin(535, 52, 639, 292)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_BURO_A, 999);
        } else if (gMousePosition.IfIsWithin(275, 162, 386, 236)) {
            SetMouseLook(CURSOR_HOT, 4400, ROOM_BURO_A, 10);
        } else if (gMousePosition.IfIsWithin(301, 66, 416, 160)) {
            SetMouseLook(CURSOR_HOT, 4401, ROOM_BURO_A, 20);
        } else if (gMousePosition.IfIsWithin(MantelPos[Sim.localPlayer].left, MantelPos[Sim.localPlayer].top, MantelPos[Sim.localPlayer].right,
                                             MantelPos[Sim.localPlayer].bottom)) {
            SetMouseLook(CURSOR_HOT, 4403, ROOM_BURO_A, 12);
        } else if ((Sim.Players.Players[PlayerNum].Letters.AnzLetters() != 0) && gMousePosition.IfIsWithin(318, 368, 450, 431)) {
            SetMouseLook(CURSOR_HOT, 4404, ROOM_BURO_A, 13);
        } else if (gMousePosition.IfIsWithin(322, 302, 432, 368)) {
            SetMouseLook(CURSOR_HOT, 4405, ROOM_BURO_A, 14);
        } else if (gMousePosition.IfIsWithin(396, 279, 490, 359)) {
            SetMouseLook(CURSOR_HOT, 4402, ROOM_BURO_A, 15);
        }
    }

    CStdRaum::PostPaint();
    CStdRaum::PumpToolTips();
}

//--------------------------------------------------------------------------------------------
// void CBuero::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CBuero::OnLButtonDown(UINT nFlags, CPoint point) {
    XY RoomPos;

    DefaultOnLButtonDown();

    if (ConvertMousePosition(point, &RoomPos) == 0) {
        CStdRaum::OnLButtonDown(nFlags, point);
        return;
    }

    // Nichts machen, wenn gerade der Brief geschlossen wird:
    if (SP_Player.GetClip() == 6) {
        return;
    }

    if (PreLButtonDown(point) == 0) {
        // Klickt ist zu früh? Dann abbrechen!
        if (MouseClickArea == ROOM_BURO_A && MouseClickId == 10 && Sim.Tutorial < 1401 && (Sim.IsTutorial != 0)) {
            return;
        }

        if (MouseClickId != 10 && MouseClickId != 0 && Sim.Tutorial <= 1401 && (Sim.IsTutorial != 0)) {
            if (MouseClickArea == ROOM_BURO_A && MouseClickId == 999) {
                Sim.Players.Players[Sim.localPlayer].Messages.AddMessage(BERATERTYP_GIRL, StandardTexte.GetS(TOKEN_TUTORIUM, 1520));
            } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 13) {
                Sim.Players.Players[Sim.localPlayer].Messages.AddMessage(BERATERTYP_GIRL, StandardTexte.GetS(TOKEN_TUTORIUM, 1521));
            } else if (MouseClickArea == ROOM_BURO_A) {
                Sim.Players.Players[Sim.localPlayer].Messages.AddMessage(BERATERTYP_GIRL, StandardTexte.GetS(TOKEN_TUTORIUM, 1522));
            }
            return;
        }

        if (MouseClickArea == ROOM_BURO_A && MouseClickId == 999) {
            Sim.Players.Players[PlayerNum].LeaveRoom();
        } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 10) {
            Sim.Players.Players[PlayerNum].EnterRoom(ROOM_GLOBE);
        } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 12) {
            if (Sim.Date > 1) {
                Sim.GiveHint(HINT_FEIERABEND);
            }
            MenuStart(MENU_REQUEST, MENU_REQUEST_CALLITADAY, 0);
            MenuSetZoomStuff(XY(480, 170), 0.1, FALSE);
        } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 20) {
            MenuStart(MENU_KEROSIN);
            MenuSetZoomStuff(XY(357, 107), 0.1, FALSE);
        } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 13) {
            SP_Player.SetDesiredMood(SPM_LETTER);
        } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 14) {
            KommVarTelefon = 1;
            SP_Player.SetDesiredMood(SPM_LISTENING);
            // MenuStart (MENU_FILOFAX, 2);
            // MenuSetZoomStuff (XY(363, 321), 0.1, FALSE);
        } else if (MouseClickArea == ROOM_BURO_A && MouseClickId == 15) {
            if (Sim.Players.Players[PlayerNum].Planes.GetNumUsed() > 0) {
                Sim.Players.Players[PlayerNum].EnterRoom(ROOM_PLANEPROPS);
            }
        } else {
            CStdRaum::OnLButtonDown(nFlags, point);
        }
    }
}

//--------------------------------------------------------------------------------------------
// void CBuero::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CBuero::OnRButtonDown(UINT nFlags, CPoint point) {
    DefaultOnRButtonDown();

    if (Sim.Tutorial == 1401 && (Sim.IsTutorial != 0)) {
        return;
    }

    // Nichts machen, wenn gerade der Brief geschlossen wird:
    if (SP_Player.GetClip() == 6) {
        return;
    }

    // Außerhalb geklickt? Dann Default-Handler!
    if (point.x < WinP1.x || point.y < WinP1.y || point.x > WinP2.x || point.y > WinP2.y) {
        return;
    }

    if (MenuIsOpen() != 0) {
        MenuRightClick(point);
    } else {
        if ((IsDialogOpen() == 0) && point.y < 440) {
            Sim.Players.Players[PlayerNum].LeaveRoom();
        }

        CStdRaum::OnRButtonDown(nFlags, point);
    }
}

//============================================================================================
// CLetters::
//============================================================================================
// Löscht alle Briefe:
//============================================================================================
void CLetters::Clear() {
    SLONG c = 0;

    for (c = 0; c < Letters.AnzEntries(); c++) {
        Letters[c].Date = -1;
    }
}

//--------------------------------------------------------------------------------------------
// Fügt dem Spieler einen weiteren Brief hinzu:
//--------------------------------------------------------------------------------------------
void CLetters::SortLetters() {
    SLONG c = 0;

    for (c = 0; c < Letters.AnzEntries() - 1; c++) {
        if ((Letters[c].Date == -1 && Letters[c + 1].Date != -1) ||
            (Letters[c].Date != -1 && Letters[c + 1].Date != -1 && Letters[c].Date < Letters[c + 1].Date)) {
            CLetter tmp;

            tmp = Letters[c + 1];
            Letters[c + 1] = Letters[c];
            Letters[c] = tmp;

            c -= 2;
            if (c < -1) {
                c = -1;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Fügt dem Spieler einen weiteren Brief hinzu:
//--------------------------------------------------------------------------------------------
void CLetters::AddLetter(BOOL IsLetter, const CString &Subject, const CString &Letter, const CString &Absender, SLONG Picture) {
    SortLetters();

    SLONG c = 0;

    for (c = 0; c < Letters.AnzEntries(); c++) {
        if (Letters[c].Date != -1 && Letters[c].Subject == Subject && Letters[c].Letter == Letter && Letters[c].Absender == Absender) {
            return;
        }
    }

    Letters[Letters.AnzEntries() - 1].IsLetter = IsLetter;
    Letters[Letters.AnzEntries() - 1].Date = Sim.Date;
    Letters[Letters.AnzEntries() - 1].Subject = Subject;
    Letters[Letters.AnzEntries() - 1].Letter = Letter;
    Letters[Letters.AnzEntries() - 1].Absender = Absender;
    Letters[Letters.AnzEntries() - 1].PictureId = Picture;

    SortLetters();
}

//--------------------------------------------------------------------------------------------
// Wieviele Briefe hat der Spieler?:
//--------------------------------------------------------------------------------------------
SLONG CLetters::AnzLetters() {
    SLONG c = 0;
    SLONG Anz = 0;

    for (Anz = c = 0; c < Letters.AnzEntries(); c++) {
        if (Letters[c].Date != -1) {
            Anz++;
        }
    }

    return (Anz);
}

//--------------------------------------------------------------------------------------------
// Speichert ein Letter-Objekt;
//--------------------------------------------------------------------------------------------
TEAKFILE &operator<<(TEAKFILE &File, const CLetter &l) {
    File << l.IsLetter << l.Date << l.Subject << l.Letter << l.Absender << l.PictureId;

    return (File);
}

//--------------------------------------------------------------------------------------------
// Lädt ein Letter-Objekt;
//--------------------------------------------------------------------------------------------
TEAKFILE &operator>>(TEAKFILE &File, CLetter &l) {
    File >> l.IsLetter >> l.Date >> l.Subject >> l.Letter >> l.Absender >> l.PictureId;

    return (File);
}
