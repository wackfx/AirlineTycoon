//============================================================================================
// Museum.cpp : Das Museum mit den alten Flugzeugen
//============================================================================================
#include "StdAfx.h"
#include "Museum.h"
#include "glmuseum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static XY PlanePositions[3] = {XY(140, 281), XY(206, 293), XY(291, 276)};

//////////////////////////////////////////////////////////////////////////////////////////////
// Museum Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// Das Museum wird eröffnet:
//--------------------------------------------------------------------------------------------
CMuseum::CMuseum(BOOL bHandy, ULONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "museum.gli", GFX_MUSEUM) {
    SetRoomVisited(PlayerNum, ROOM_MUSEUM);

    Sim.FocusPerson = -1;

    pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("tippl.gli", GliPath)), &pMenuLib, L_LOCMEM);
    BlockBm.ReSize(pMenuLib, "BLOC1");

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(40);
    }

    MiniPlanes.ReSize(pRoomLib, "MINI0101 MINI0102 MINI0103 MINI0104 MINI0201 MINI0202 MINI0203 MINI0204 MINI0301 MINI0302 MINI0303 MINI0304");
    Sim.UpdateUsedPlanes();

    TriggerSkelett = -1;
    TriggerMann = -1;
    TriggerEmergency = -1;

    SP_Mann.ReSize(14);
    SP_Mann.Clips[0].ReSize(0, "mu_putza.smk", "mu_putza.raw", XY(420, 250), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr,
                            SMACKER_CLIP_SET, 0, nullptr, "A9A9E1", 0, 1, 2);
    SP_Mann.Clips[1].ReSize(1, "mu_putzb.smk", "mu_putzb.raw", XY(420, 250), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr,
                            SMACKER_CLIP_SET, 0, nullptr, "A9A9E1", 0, 1, 2);
    SP_Mann.Clips[2].ReSize(2, "mu_turn.smk", "", XY(420, 250), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerEmergency,
                            SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, nullptr, "A1E1E1E1", 5, 3, 5, 6);
    SP_Mann.Clips[3].ReSize(3, "mu_redea.smk", "", XY(420, 250), SPM_TALKING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                            0, nullptr, "A8A8E1E1", 3, 4, 5, 6);
    SP_Mann.Clips[4].ReSize(4, "mu_redeb.smk", "", XY(420, 250), SPM_TALKING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                            0, nullptr, "A8A8E1E1", 3, 4, 5, 6);
    SP_Mann.Clips[5].ReSize(5, "mu_redew.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(5, 5), SMACKER_CLIP_CANCANCEL, &TriggerMann,
                            SMACKER_CLIP_SET | SMACKER_CLIP_POST, -1, nullptr, "A9A1E1E1E1", 5, 9, 3, 5, 6);
    SP_Mann.Clips[6].ReSize(6, "mu_turnz.smk", "", XY(420, 250), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerEmergency,
                            SMACKER_CLIP_SET | SMACKER_CLIP_PRE, 0, nullptr, "A9", 0);

    SP_Mann.Clips[9].ReSize(9, "mu_redew.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(370, 370), SMACKER_CLIP_CANCANCEL, &TriggerSkelett,
                            SMACKER_CLIP_SET | SMACKER_CLIP_PRE, 1, &TriggerMann, "A1E1E1E1", 5, 3, 5, 6);

    SP_Mann.Clips[7].ReSize(7, "mu_guck.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerMann,
                            SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, nullptr, "A1", 8);
    SP_Mann.Clips[8].ReSize(8, "mu_guckz.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerSkelett,
                            SMACKER_CLIP_SET | SMACKER_CLIP_POST, 3, nullptr, "A1", 5);
    SP_Mann.Clips[10].ReSize(10, "", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, &TriggerMann,
                             SMACKER_CLIP_SET | SMACKER_CLIP_POST, -1, nullptr, "A1", 11);

    // Er erwischt ihn:
    SP_Mann.Clips[11].ReSize(11, "mu_guck.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(10, 10), SMACKER_CLIP_DONTCANCEL, &TriggerMann,
                             SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, nullptr, "A1", 12);
    SP_Mann.Clips[12].ReSize(12, "mu_guckz.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr,
                             SMACKER_CLIP_SET, 0, nullptr, "A1", 13);
    SP_Mann.Clips[13].ReSize(11, "mu_hit.smk", "", XY(420, 250), SPM_LISTENING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, &TriggerSkelett,
                             SMACKER_CLIP_SET | (SMACKER_CLIP_FRAME + 3 * SMACKER_CLIP_MULT), 4, &TriggerMann, "A1", 5);

    SP_Skelett.ReSize(8);
    SP_Skelett.Clips[0].ReSize(0, "flyingmw.smk", "", XY(420, 180), SPM_IDLE, CRepeat(1, 1), CPostWait(60, 60), SMACKER_CLIP_CANCANCEL, nullptr,
                               SMACKER_CLIP_SET, 0, &TriggerSkelett, "A9", 0);
    SP_Skelett.Clips[1].ReSize(1, "", "", XY(420, 180), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerSkelett,
                               SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, nullptr, "A9A9", 2, 6);

    // Er wird nicht erwischt:
    SP_Skelett.Clips[2].ReSize(2, "flyingm.smk", "flyingm.raw", XY(420, 180), SPM_IDLE, CRepeat(1, 3), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerMann,
                               SMACKER_CLIP_SET | SMACKER_CLIP_POST, 7, &TriggerEmergency, "A9", 0);

    // Er wird erwischt:
    SP_Skelett.Clips[6].ReSize(6, "flyingm.smk", "flyingm.raw", XY(420, 180), SPM_IDLE, CRepeat(1, 2), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerMann,
                               SMACKER_CLIP_SET | SMACKER_CLIP_POST, 10, &TriggerEmergency, "A9", 7);
    SP_Skelett.Clips[7].ReSize(7, "flyingm.smk", "flyingm.raw", XY(420, 180), SPM_IDLE, CRepeat(2, 2), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr,
                               SMACKER_CLIP_SET, 0, &TriggerEmergency, "A9", 0);

    SP_Skelett.Clips[3].ReSize(3, "flygrinz.smk", "flygrinz.raw", XY(420, 180), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL,
                               &TriggerSkelett, SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, &TriggerEmergency, "A9", 0);

    SP_Skelett.Clips[4].ReSize(4, "", "", XY(420, 180), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerSkelett,
                               SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, nullptr, "A9", 5);
    SP_Skelett.Clips[5].ReSize(5, "fly_auah.smk", "", XY(420, 180), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &TriggerSkelett,
                               SMACKER_CLIP_SET | SMACKER_CLIP_PRE, -1, &TriggerEmergency, "A9", 0);

    // Raumanimationen
    RotorAnim.ReSize(pRoomLib, "ROTOR01", 2, nullptr, FALSE, ANIMATION_MODE_REPEAT, 0, 1);

    // Hintergrundsounds:
    if (Sim.Options.OptionEffekte != 0) {
        TriebwerkFx.ReInit("museum.raw");
        TriebwerkFx.Play(DSBPLAY_NOSTOP | DSBPLAY_LOOPING, Sim.Options.OptionEffekte * 100 / 7);
    }

    Talkers.Talkers[TALKER_MUSEUM].IncreaseReference();
    DefaultDialogPartner = TALKER_MUSEUM;

    ReloadBitmaps();
}

//--------------------------------------------------------------------------------------------
// Reload misc. Bitmaps:
//--------------------------------------------------------------------------------------------
void CMuseum::ReloadBitmaps() { Talkers.Talkers[TALKER_MUSEUM].DecreaseReference(); }

//--------------------------------------------------------------------------------------------
// Kauft dem Museum das gebrauchte Flugzeug ab:
//--------------------------------------------------------------------------------------------
void CMuseum::BuyUsedPlane() {
    /*if (Sim.Players.Players[PlayerNum].Planes.GetNumFree()==0) Sim.Players.Players[PlayerNum].Planes.Planes.ReSize
    (Sim.Players.Players[PlayerNum].Planes.AnzEntries()+10); Sim.Players.Players[PlayerNum].Planes += Sim.UsedPlanes[0x1000000+CurrentDetail];
      Sim.Players.Players[PlayerNum].ChangeMoney (
      -Sim.UsedPlanes[0x1000000+CurrentDetail].CalculatePrice(),
      2010,                //Kauf des Flugzeuges
      Sim.UsedPlanes[0x1000000+CurrentDetail].Name);

    //Sim.Players.Players[PlayerNum].Money  -= Sim.UsedPlanes[0x1000000+CurrentDetail].CalculatePrice();

    Sim.CreateRandomUsedPlane (CurrentDetail);*/
}

//--------------------------------------------------------------------------------------------
// Verkauft dem Museum das gebrauchte Flugzeug:
//--------------------------------------------------------------------------------------------
void CMuseum::SellUsedPlane() {
    // Sim.Players.Players[PlayerNum].Money  += Sim.Players.Players[PlayerNum].Planes[CurrentDetail].CalculatePrice();
    /*Sim.Players.Players[PlayerNum].ChangeMoney (
      Sim.Players.Players[PlayerNum].Planes[CurrentDetail].CalculatePrice(),
      2011,                //Verkauf des Flugzeuges
      Sim.Players.Players[PlayerNum].Planes[CurrentDetail].Name);

      Sim.Players.Players[PlayerNum].Planes -= CurrentDetail;*/
}

//--------------------------------------------------------------------------------------------
// Destruktor
//--------------------------------------------------------------------------------------------
CMuseum::~CMuseum() {
    if ((Sim.DialogOvertureFlags & DIALOG_MUSEUM) != 0U) {
        Sim.DialogOvertureFlags |= DIALOG_MUSEUM2;
    }

    BlockBm.Destroy();
    MiniPlanes.Destroy();
    if ((pMenuLib != nullptr) && (pGfxMain != nullptr)) {
        pGfxMain->ReleaseLib(pMenuLib);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Museum message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// void CMuseum::OnPaint()
//--------------------------------------------------------------------------------------------
void CMuseum::OnPaint() {
    SLONG c = 0;
    SLONG NewTip = 0;

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    // Die Raum-Animationen:
    RotorAnim.BlitAt(RoomBm, 82, 304);
    // SkelettAnim.BlitAt (RoomBm, 410, 169);

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::InitToolTips();

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        if (gMousePosition.IfIsWithin(0, 0, 233, 227) || gMousePosition.IfIsWithin(533, 312, 605, 352)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_MUSEUM, 999);
        } else if (gMousePosition.IfIsWithin(125, 226, 375 - 1, 330)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_MUSEUM, 10);
        } else if (gMousePosition.IfIsWithin(408, 193, 590, 430)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_MUSEUM, 11);
        }
    }

    SP_Skelett.Pump();
    SP_Skelett.BlitAtT(RoomBm);
    SP_Mann.Pump();
    SP_Mann.BlitAtT(RoomBm);
    // SP_SwingPlane.Pump ();
    // SP_SwingPlane.BlitAtT (RoomBm);

    for (c = 0; c < 3; c++) {
        if (Sim.UsedPlanes[0x1000000 + c].Name.GetLength() > 0) {
            SLONG Size = (Sim.UsedPlanes[0x1000000 + c].ptPassagiere - 100) * 4 / 400;
            // SLONG Size = (PlaneTypes[Sim.UsedPlanes[0x1000000+c].TypeId].Passagiere-100)*4/400;
            if (Size < 0) {
                Size = 0;
            }
            if (Size > 3) {
                Size = 3;
            }

            RoomBm.BlitFromT(MiniPlanes[c * 4 + Size], PlanePositions[c]);
        }
    }

    CStdRaum::PostPaint();

    // Ggf. Tips einblenden
    if (gMousePosition.IfIsWithin(125, 226, 365 - 1, 330) && (IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        NewTip = (gMousePosition.x - 125) * 3 / (365 - 125);

        if (NewTip != CurrentTip) {
            if (Sim.UsedPlanes[0x1000000 + NewTip].Name.GetLength() > 0) {
                if (Sim.Players.Players[PlayerNum].HasBerater(BERATERTYP_FLUGZEUG) != 0) {
                    if (Sim.UsedPlanes[0x1000000 + NewTip].Baujahr < 1960) {
                        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, StandardTexte.GetS(TOKEN_ADVICE, 4001), MESSAGE_COMMENT);
                    } else if (Sim.UsedPlanes[0x1000000 + NewTip].Zustand < 20) {
                        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, StandardTexte.GetS(TOKEN_ADVICE, 4000), MESSAGE_COMMENT);
                    } else if (Sim.UsedPlanes[0x1000000 + NewTip].Zustand < 40) {
                        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, StandardTexte.GetS(TOKEN_ADVICE, 4002), MESSAGE_COMMENT);
                    } else if (Sim.UsedPlanes[0x1000000 + NewTip].Zustand < 60) {
                        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, StandardTexte.GetS(TOKEN_ADVICE, 4003), MESSAGE_COMMENT);
                    } else if (Sim.UsedPlanes[0x1000000 + NewTip].Zustand < 80) {
                        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, StandardTexte.GetS(TOKEN_ADVICE, 4004), MESSAGE_COMMENT);
                    } else {
                        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, StandardTexte.GetS(TOKEN_ADVICE, 4005), MESSAGE_COMMENT);
                    }
                }

                DrawPlaneTip(TipBm, &BlockBm, nullptr, &Sim.UsedPlanes[0x1000000 + NewTip]);
                // DrawPlaneTip (TipBm, &BlockBm, &PlaneTypes[Sim.UsedPlanes[0x1000000+NewTip].TypeId], &Sim.UsedPlanes[0x1000000+NewTip]);
            } else {
                NewTip = -1;
                SetMouseLook(CURSOR_NORMAL, 0, ROOM_MUSEUM, 0);
            }
        }

        if (NewTip != -1) {
            PrimaryBm.BlitFromT(TipBm, 245 - TipBm.Size.x / 2 + (NewTip - 1) * 100, WinP1.y + 50);
        }

        CurrentTip = NewTip;
    } else {
        Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, "", MESSAGE_COMMENT);
        if (CurrentMenu == MENU_SELLPLANE && (gMousePosition - MenuPos).IfIsWithin(216, 6, 387, 212)) {
            NewTip = (gMousePosition.y - (MenuPos.y + 25)) / 13 + MenuPage;

            if (NewTip >= 0 && NewTip - MenuPage < 13 && NewTip < MenuDataTable.LineIndex.AnzEntries() &&
                (Sim.Players.Players[PlayerNum].Planes.IsInAlbum(MenuDataTable.LineIndex[NewTip]) != 0)) {
                if (NewTip != CurrentTip) {
                    MenuRepaint();
                    // DrawPlaneTipContents (OnscreenBitmap, &PlaneTypes[Sim.Players.Players[(SLONG)PlayerNum].Planes[MenuDataTable.LineIndex[NewTip]].TypeId],
                    // &Sim.Players.Players[(SLONG)PlayerNum].Planes[MenuDataTable.LineIndex[NewTip]],
                    DrawPlaneTipContents(OnscreenBitmap, nullptr, &Sim.Players.Players[PlayerNum].Planes[MenuDataTable.LineIndex[NewTip]], XY(6, 6), XY(6, 28),
                                         &FontSmallBlack, &FontSmallBlack);
                }

                if (MenuDataTable.ValueFlags[0 + NewTip * MenuDataTable.AnzColums] != 0U) {
                    CheckCursorHighlight(
                        ReferenceCursorPos,
                        CRect(MenuPos.x + 216, MenuPos.y + (NewTip - MenuPage) * 13 + 25 - 2, MenuPos.x + 387, MenuPos.y + (NewTip - MenuPage) * 13 + 25 + 12),
                        ColorOfFontRed, CURSOR_HOT);
                } else {
                    CheckCursorHighlight(
                        ReferenceCursorPos,
                        CRect(MenuPos.x + 216, MenuPos.y + (NewTip - MenuPage) * 13 + 25 - 2, MenuPos.x + 387, MenuPos.y + (NewTip - MenuPage) * 13 + 25 + 12),
                        ColorOfFontBlack, CURSOR_HOT);
                }

                CurrentTip = NewTip;
            } else {
                NewTip = -1;
            }
        } else {
            NewTip = -1;
        }

        if (NewTip != CurrentTip) {
            MenuRepaint();
            CurrentTip = NewTip;
        }
    }

    CStdRaum::PumpToolTips();
}

//--------------------------------------------------------------------------------------------
// void CMuseum::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CMuseum::OnLButtonDown(UINT nFlags, CPoint point) {
    XY RoomPos;

    DefaultOnLButtonDown();

    if (ConvertMousePosition(point, &RoomPos) == 0) {
        CStdRaum::OnLButtonDown(nFlags, point);
        return;
    }

    if (PreLButtonDown(point) == 0) {
        if (MouseClickArea == ROOM_MUSEUM && MouseClickId == 999) {
            Sim.Players.Players[PlayerNum].LeaveRoom();
        } else if (MouseClickArea == ROOM_MUSEUM && MouseClickId == 10 && point.x >= 125 && point.x <= 365 - 1) {
            SLONG NewTip = (point.x - 125) * 3 / (365 - 125);
            if (Sim.UsedPlanes[0x1000000 + NewTip].Name.GetLength() > 0) {
                Sim.Players.Players[PlayerNum].Messages.AddMessage(BERATERTYP_FLUGZEUG, "", MESSAGE_COMMENT);
                StartDialog(TALKER_MUSEUM, MEDIUM_AIR, NewTip);
            }
        } else if (MouseClickArea == ROOM_MUSEUM && MouseClickId == 11) {
            StartDialog(TALKER_MUSEUM, MEDIUM_AIR);
        } else {
            CStdRaum::OnLButtonDown(nFlags, point);
        }
    }
}

//--------------------------------------------------------------------------------------------
// void CMuseum::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CMuseum::OnRButtonDown(UINT nFlags, CPoint point) {
    DefaultOnRButtonDown();

    // Außerhalb geklickt? Dann Default-Handler!
    if (point.x < WinP1.x || point.y < WinP1.y || point.x > WinP2.x || point.y > WinP2.y - StatusLineSizeY) {
        return;
    }

    // Click während ein On-Screen Fenster offen ist?
    if (MenuIsOpen() != 0) {
        MenuRightClick(point);
    } else if (point.y < 440) {
        if ((IsDialogOpen() == 0) && point.y < 440) {
            Sim.Players.Players[PlayerNum].LeaveRoom();
        }

        CStdRaum::OnRButtonDown(nFlags, point);
    }
}
