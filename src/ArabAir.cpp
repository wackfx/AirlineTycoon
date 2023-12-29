//============================================================================================
// ArabAir.cpp : Der Schalter und das Hinterzimmer von ArabAir!
//============================================================================================
#include "StdAfx.h"
#include "ArabAir.h"
#include "AtNet.h"
#include "glarab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DSBVOLUME_MIN (-10000)
#define DSBVOLUME_MAX 0

//--------------------------------------------------------------------------------------------
// Die Schalter wird eröffnet:
//--------------------------------------------------------------------------------------------
CArabAir::CArabAir(BOOL bHandy, ULONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "arab.gli", GFX_ARAB_AUS) {
    SetRoomVisited(PlayerNum, ROOM_ARAB_AIR);
    HandyOffset = 320;
    Sim.FocusPerson = -1;

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(60);
    }

    // if (gLanguage==LANGUAGE_D) FunkelAnim.ReSize (pRoomLib, "FUNK00", 7, NULL, FALSE, ANIMATION_MODE_REPEAT, 300, 2, 600, 1);

    Talkers.Talkers[TALKER_ARAB].IncreaseReference();
    DefaultDialogPartner = TALKER_ARAB;

    SP_Araber.ReSize(7);
    // SP_Araber.SetSpeakFx ("sayarab.raw");
    SP_Araber.Clips[0].ReSize(0, "arabw.smk", "", XY(425, 108), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                              nullptr, // Warten
                              "A9E1", 0, 1);
    SP_Araber.Clips[1].ReSize(1, "araba.smk", "", XY(425, 108), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                              nullptr, // Warten->Speak
                              "E1E1E1", 2, 4, 6);
    SP_Araber.Clips[2].ReSize(2, "arabl.smk", "", XY(425, 108), SPM_LISTENING, CRepeat(1, 1), CPostWait(50, 50), SMACKER_CLIP_CANCANCEL, nullptr,
                              SMACKER_CLIP_SET, 0, nullptr, // Listen
                              "A9E5E5", 2, 4, 6);
    SP_Araber.Clips[3].ReSize(3, "arabp.smk", "", XY(425, 108), SPM_TALKING, CRepeat(1, 1), CPostWait(20, 20), SMACKER_CLIP_CANCANCEL, nullptr,
                              SMACKER_CLIP_SET, 0, nullptr, // Psst!, danach Speaking
                              "A9", 4);
    SP_Araber.Clips[4].ReSize(4, "arabs.smk", "", XY(425, 108), SPM_TALKING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                              0, nullptr, // Speak
                              "A9E1E1", 4, 2, 6);
    SP_Araber.Clips[5].ReSize(5, "arabz.smk", "arabz.raw", XY(425, 108), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr,
                              SMACKER_CLIP_SET, 0, nullptr, // Speak->Wait
                              "A9", 0);
    SP_Araber.Clips[6].ReSize(0, "arabg.smk", "", XY(425, 108), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                              nullptr, // Umschauen
                              "A9", 5);

    GloveBm.ReSize(pRoomLib, "GLOVE");

    if (Sim.Options.OptionEffekte != 0) {
        RadioFX.ReInit("radio.raw");

        StartupFX.ReInit("arab.raw");
        StartupFX.Play(0, Sim.Options.OptionEffekte * 100 / 7);
    }

    if (Sim.Players.Players[static_cast<SLONG>(PlayerNum)].HasBerater(BERATERTYP_KEROSIN) != 0) {
        if (Sim.Kerosin < 400 &&
            Sim.Players.Players[static_cast<SLONG>(PlayerNum)].TankInhalt * 2 < Sim.Players.Players[static_cast<SLONG>(PlayerNum)].Tank * 4 &&
            Sim.Players.Players[static_cast<SLONG>(PlayerNum)].Money > 20000) {
            // Kerosin kaufen:
            Sim.Players.Players[static_cast<SLONG>(PlayerNum)].Messages.AddMessage(BERATERTYP_KEROSIN, StandardTexte.GetS(TOKEN_ADVICE, 3000));
        } else if (Sim.Kerosin > 600) {
            // Kerosin nicht kaufen:
            Sim.Players.Players[static_cast<SLONG>(PlayerNum)].Messages.AddMessage(BERATERTYP_KEROSIN, StandardTexte.GetS(TOKEN_ADVICE, 3001));
        }
    }
}

//--------------------------------------------------------------------------------------------
// Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CArabAir::~CArabAir() {
    StartupFX.SetVolume(DSBVOLUME_MIN);
    StartupFX.Stop();
    Talkers.Talkers[TALKER_ARAB].DecreaseReference();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CArabAir message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// void CArabAir::OnPaint()
//--------------------------------------------------------------------------------------------
void CArabAir::OnPaint() {
    if (bHandy == 0) {
        SetMouseLook(CURSOR_NORMAL, 0, ROOM_ARAB_AIR, 0);
    }

    if (Sim.Date > 5) {
        Sim.GiveHint(HINT_ARABAIR);
    }

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    DrawChart(RoomBm, 0xff2020, Sim.KerosinPast, -1, -1, XY(313, 136), XY(371, 146), XY(313, 166), XY(371, 176));

    SP_Araber.Pump();
    SP_Araber.BlitAtT(RoomBm);

    // FunkelAnim.BlitAt (RoomBm, 232, 202);

    if ((Sim.Players.Players[PlayerNum].HasItem(ITEM_GLOVE) == 0) && (Sim.ItemGlove != 0)) {
        RoomBm.BlitFromT(GloveBm, 314, 280);
    }

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::InitToolTips();

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        if (gMousePosition.IfIsWithin(0, 232, 144, 398) || gMousePosition.IfIsWithin(0, 288, 314, 380) || gMousePosition.IfIsWithin(0, 351, 639, 439)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_ARAB_AIR, 999);
        } else if (gMousePosition.IfIsWithin(314, 280, 314 + 72, 280 + 62) && (Sim.ItemGlove != 0) &&
                   (Sim.Players.Players[PlayerNum].HasItem(ITEM_GLOVE) == 0)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_ARAB_AIR, 12);
        } else if (gMousePosition.IfIsWithin(437, 89, 600, 323)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_ARAB_AIR, 10);
        } else if (gMousePosition.IfIsWithin(313, 136, 371, 176)) {
            SetMouseLook(CURSOR_HOT, 5000, bprintf(LPCTSTR(CString(StandardTexte.GetS(TOKEN_TOOLTIP, 4500))), Sim.Kerosin), ROOM_ARAB_AIR, 11);

            if (ToolTipState == FALSE) {
                ToolTipTimer = AtGetTime() - 601;
            }
        }
    }

    CStdRaum::PostPaint();
    CStdRaum::PumpToolTips();
}

//--------------------------------------------------------------------------------------------
// void CArabAir::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CArabAir::OnLButtonDown(UINT nFlags, CPoint point) {
    XY RoomPos;

    DefaultOnLButtonDown();

    if (ConvertMousePosition(point, &RoomPos) == 0) {
        CStdRaum::OnLButtonDown(nFlags, point);
        return;
    }

    if (PreLButtonDown(point) == 0) {
        if (gMousePosition.IfIsWithin(179, 215, 242, 261)) {
            RadioFX.Play(0, Sim.Options.OptionEffekte * 100 / 7);
        }

        if (MouseClickArea == ROOM_ARAB_AIR && MouseClickId == 999) {
            Sim.Players.Players[PlayerNum].LeaveRoom();
        } else if (MouseClickArea == ROOM_ARAB_AIR && MouseClickId == 10) {
            StartDialog(TALKER_ARAB, MEDIUM_AIR, 0);
        } else if (MouseClickArea == ROOM_ARAB_AIR && MouseClickId == 12) {
            Sim.Players.Players[PlayerNum].BuyItem(ITEM_GLOVE);

            if (Sim.Players.Players[PlayerNum].HasItem(ITEM_GLOVE) != 0) {
                Sim.ItemGlove = 0;
                SIM::SendSimpleMessage(ATNET_TAKETHING, 0, ITEM_GLOVE);
            }
        } else if (MouseClickArea == ROOM_ARAB_AIR && MouseClickId == 11) {
            StartDialog(TALKER_ARAB, MEDIUM_AIR, 100);
        } else {
            CStdRaum::OnLButtonDown(nFlags, point);
        }
    }
}

//--------------------------------------------------------------------------------------------
// void CArabAir::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CArabAir::OnRButtonDown(UINT nFlags, CPoint point) {
    DefaultOnRButtonDown();

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
