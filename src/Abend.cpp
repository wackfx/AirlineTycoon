//============================================================================================
// Abend.cpp : Die Feierabendsequenz
//============================================================================================
// Link: "Abend.h"
//============================================================================================
#include "StdAfx.h"
#include "Abend.h"
#include "glabend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DSBVOLUME_MIN (-10000)
#define DSBVOLUME_MAX 0

extern SB_CColorFX ColorFX;

SLONG bCAbendOpen = 0;

//--------------------------------------------------------------------------------------------
// Der Feierabend beginnt
//--------------------------------------------------------------------------------------------
CAbend::CAbend(BOOL bHandy, ULONG PlayerNum) : CStdRaum(bHandy, PlayerNum, CString(bprintf("abend%li.gli", Sim.GetSeason())), 0) {
    LastTime = 0xffffffff;

    Room = 1; // Uhr-Bild
    Ticker = 0;
    Sim.b18Uhr = FALSE;
    Sim.CallItADayAt = 0;

    bCAbendOpen++;

    Sim.FocusPerson = -1;
    LastHour = Sim.GetHour();

    Sim.Players.UpdateStatistics();

    Sim.Players.Players[static_cast<SLONG>(PlayerNum)].WalkStop();

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(0);
    }

    if ((Sim.Options.OptionEffekte != 0) && Sim.Time <= 17 * 60000) {
        if (rand() % 3 == 0) {
            StartupFX.ReInit("birds.raw");
        }
        StartupFX.Play(0, Sim.Options.OptionEffekte * 100 / 7);
    } else if ((Sim.Options.OptionEffekte != 0) && rand() % 3 == 0) {
        StartupFX.Stop();
        StartupFX.ReInit("hupe.raw");
        StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
    }

    AirportBm.ReSize(pRoomLib, GFX_AIRPORT);
    NightBm.ReSize(pRoomLib, GFX_NACHT);

    // if (Sim.Players.Players[(SLONG)PlayerNum].CallItADay)
    {
        Sim.Players.Players[static_cast<SLONG>(PlayerNum)].Messages.NewDay();
        Room = 2;
    }
    // else Sim.bNoTime = TRUE;

    for (SLONG c = 0; c < 4; c++) {
        Sim.Players.Players[c].bReadyForBriefing = 0;
        Sim.Players.Players[c].IsStuck = 0;
        if (CheatTestGame == 0) {
            Sim.Players.Players[c].GameSpeed = 0;
        }

        if (Sim.Players.GetAnzRobotPlayers() == 0) {
            Sim.Players.Players[c].bReadyForMorning = 0; // Keine Synchronisierung durch Robots
        } else {
            Sim.Players.Players[c].bReadyForMorning = 1; // Synchronisierung durch Robots; wird um 18 Uhr auf false gesetzt
        }

        if (Sim.Players.Players[c].WaitWorkTill == 0) {
            Sim.Players.Players[c].WaitWorkTill = -1;
        }
    }

    StartSeason = Sim.GetSeason();
}

//--------------------------------------------------------------------------------------------
// Destruktor
//--------------------------------------------------------------------------------------------
CAbend::~CAbend() {
    StartupFX.SetVolume(DSBVOLUME_MIN);
    StartupFX.Stop();
    AirportBm.Destroy();
    NightBm.Destroy();
    // Sim.bNoTime  = FALSE;
    Sim.DayState = 4; // Tag wurde beendet

    bCAbendOpen--;
}

//--------------------------------------------------------------------------------------------
// Setzt die Animation fort (20x pro Sekunde):
//--------------------------------------------------------------------------------------------
void CAbend::DoOneStep() {
    // Uhr
    /*if (Room==1)
      {
      if (++Ticker>10)
      {
      Room   = 2;
      Ticker = 0;

    //CStdRaum::ReSize("airport.lbm");
    }
    }
    else //Flughafen
    {
    if (++Ticker>10 && Sim.Time>=18*60000)
    {
    Sim.bNoTime = FALSE;
    Sim.DayState=4;  //Sequenz beenden
    }
    }*/
}

//--------------------------------------------------------------------------------------------
// void CAbend::OnPaint()
//--------------------------------------------------------------------------------------------
void CAbend::OnPaint() {
    if (Sim.Time >= 18 * 60000) {
        Sim.bNoTime = FALSE;
        Sim.DayState = 4;
    }

    // Jahrezeit wechseln?
    if (StartSeason != Sim.GetSeason()) {
        // Ja!
        SBBM TempBm(NightBm.Size);

        // Trick: Die Night-Bm als nicht-dateigebunden erzeugen ==> Beim laden der neuen GLib bleibt sie erhalten
        TempBm.BlitFrom(NightBm);
        NightBm.Destroy();
        NightBm.ReSize(TempBm.Size);
        NightBm.BlitFrom(TempBm);

        AirportBm.Destroy();

        pGfxMain->ReleaseLib(pRoomLib);

        pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename(CString(bprintf("abend%li.gli", Sim.GetSeason())), RoomPath)), &pRoomLib, L_LOCMEM);
        AirportBm.ReSize(pRoomLib, GFX_AIRPORT);
    }

    if (LastHour != Sim.GetHour()) {
        LastHour = Sim.GetHour();

        if ((Sim.Options.OptionEffekte != 0) && rand() % 5 == 0) {
            dword status = 0;
            if (StartupFX.pFX != nullptr) {
                StartupFX.pFX->GetStatus(&status);
            }
            if ((status & DSBSTATUS_PLAYING) == 0) {
                if (rand() % 3 != 0) {
                    switch (rand() % 6) {
                    case 0:
                        StartupFX.ReInit("prop.raw");
                        break;
                    case 1:
                        StartupFX.ReInit("flyby.raw");
                        break;
                    case 2:
                        StartupFX.ReInit("flyby2.raw");
                        break;
                    case 3:
                        StartupFX.ReInit("flyby3.raw");
                        break;
                    case 4:
                        StartupFX.ReInit("flyby4.raw");
                        break;
                    case 5:
                        StartupFX.ReInit("flyby5.raw");
                        break;
                    default:
                        hprintf("Abend.cpp: Default case should not be reached.");
                        DebugBreak();
                    }
                    StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                } else {
                    if (LastHour >= 10 && LastHour < 22) {
                        switch (rand() % 3) {
                        case 0:
                            StartupFX.ReInit("chilp.raw");
                            break;
                        case 1:
                            StartupFX.ReInit("chilp2.raw");
                            break;
                        case 2:
                            StartupFX.ReInit("bark.raw");
                            break;
                        default:
                            hprintf("Abend.cpp: Default case should not be reached.");
                            DebugBreak();
                        }
                        StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                    } else if (LastHour >= 22 || LastHour < 3) {
                        switch (rand() % 3) {
                        case 0:
                            StartupFX.ReInit("eule.raw");
                            break;
                        case 1:
                            StartupFX.ReInit("loon.raw");
                            break;
                        case 2:
                            StartupFX.ReInit("zirpen.raw");
                            break;
                        default:
                            hprintf("Abend.cpp: Default case should not be reached.");
                            DebugBreak();
                        }
                        StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                    } else if (LastHour == 6) {
                        StartupFX.ReInit("hahn.raw");
                        StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                    }
                }
            }
        } else if ((Sim.Options.OptionEffekte != 0) && rand() % 3 == 0 && LastHour == 6) {
            dword status = 0;
            if (StartupFX.pFX != nullptr) {
                StartupFX.pFX->GetStatus(&status);
            }
            if ((status & DSBSTATUS_PLAYING) == 0) {
                StartupFX.ReInit("hahn.raw");
                StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
            }
        }
    }

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    PrimaryBm.PrimaryBm.SetClipRect(CRect(0, 0, 640, 440));
    if (Sim.GetHour() > 7 && Sim.GetHour() < 20) {
        PrimaryBm.BlitFrom(AirportBm, 0, 0);
    } else if (Sim.GetHour() < 5 || Sim.GetHour() > 22) {
        PrimaryBm.BlitFrom(NightBm, 0, 0);
    } else if (Sim.GetHour() >= 5 && Sim.GetHour() <= 7) {
        SLONG Level = ((Sim.GetHour() - 5) * 60 + Sim.GetMinute()) * 8 / 180;
        ColorFX.ApplyOn2(Level, AirportBm.pBitmap, 8 - Level, NightBm.pBitmap, &PrimaryBm.PrimaryBm);
    } else if (Sim.GetHour() >= 20 && Sim.GetHour() <= 22) {
        SLONG Level = ((Sim.GetHour() - 20) * 60 + Sim.GetMinute()) * 8 / 180;
        ColorFX.ApplyOn2(8 - Level, AirportBm.pBitmap, Level, NightBm.pBitmap, &PrimaryBm.PrimaryBm);
    }
    PrimaryBm.PrimaryBm.SetClipRect(CRect(0, 0, 640, 480));

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::InitToolTips();
    CStdRaum::PostPaint();
    CStdRaum::PumpToolTips();
}

//--------------------------------------------------------------------------------------------
// void CAbend::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CAbend::OnRButtonDown(UINT nFlags, CPoint point) {
    DefaultOnRButtonDown();

    // Außerhalb geklickt? Dann Default-Handler!
    if (point.x < WinP1.x || point.y < WinP1.y || point.x > WinP2.x || point.y > WinP2.y) {
        return;
    }

    CStdRaum::OnRButtonDown(nFlags, point);

    Sim.bNoTime = FALSE;
    Sim.DayState = 4; // Sequenz beenden
}

//--------------------------------------------------------------------------------------------
// void CAbend::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CAbend::OnLButtonDown(UINT /*nFlags*/, CPoint point) {
    DefaultOnLButtonDown();

    // Außerhalb geklickt? Dann Default-Handler!
    if (point.x < WinP1.x || point.y < WinP1.y || point.x > WinP2.x || point.y > WinP2.y) {
        return;
    }

    if (Room == 1) {
        Ticker = 59;
    }
}

//--------------------------------------------------------------------------------------------
// void CAbend::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//--------------------------------------------------------------------------------------------
void CAbend::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    if (nChar == ATKEY_ESCAPE) {
        Sim.bNoTime = FALSE;
        Sim.DayState = 4; // Sequenz beenden
    }

    if (nChar == ATKEY_SPACE && Room == 1) {
        Ticker = 59;
    }

    CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);
}
