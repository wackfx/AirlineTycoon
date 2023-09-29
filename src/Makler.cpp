//============================================================================================
// Makler.cpp : Der Raum des Flugzeugmaklers
//============================================================================================
#include "StdAfx.h"
#include "Makler.h"
#include "glmakler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

extern SLONG timeMaklClose;

//////////////////////////////////////////////////////////////////////////////////////////////
// Makler Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// Konstruktor
//--------------------------------------------------------------------------------------------
CMakler::CMakler(BOOL bHandy, ULONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "makler.gli", GFX_MAKLER) {
    SetRoomVisited(PlayerNum, ROOM_MAKLER);

    Sim.FocusPerson = -1;

    AirportRoomPos = Airport.GetRandomTypedRune(RUNE_2SHOP, ROOM_MAKLER);
    AirportRoomPos.y = AirportRoomPos.y - 5000 + 93;

    KommVarLicht = 0;
    KommVarWasser = 0;

    LastWaterTime = AtGetTime();

    WaterFrame = 0;

    WaterBms.ReSize(pRoomLib, "SPRITZW0 SPRITZA0 SPRITZA1 SPRITZ00 SPRITZ01 SPRITZ02 SPRITZE0 SPRITZE1 SPRITZE2");

    DoorOpaqueBm.ReSize(pRoomLib, GFX_OPAQUE);
    DoorTransBms.ReSize(pRoomLib, "TRANS1", 4);

    SpringState = 0;

    KlappeFx.ReInit("klappe.raw");
    SpringFx.ReInit("spring.raw");

    SP_Makler.ReSize(9);
    SP_Makler.Clips[0].ReSize(0, "m_wait.smk", "", XY(470, 218), SPM_IDLE, CRepeat(1, 1), CPostWait(15, 15), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                              0, nullptr, // Warten
                              "A9A2A2E1", 0, 1, 2, 3);
    SP_Makler.Clips[1].ReSize(1, "m_dreh.smk", "m_dreh.raw", XY(470, 218), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &KommVarLicht,
                              SMACKER_CLIP_XOR | (SMACKER_CLIP_FRAME + 6 * SMACKER_CLIP_MULT), 1, nullptr, "A9E1", 0, 3);
    SP_Makler.Clips[2].ReSize(2, "m_druc.smk", "m_druc.raw", XY(470, 218), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, &KommVarWasser,
                              SMACKER_CLIP_XOR | (SMACKER_CLIP_FRAME + 4 * SMACKER_CLIP_MULT), 1, nullptr, "A9E1", 0, 3);

    // Zum Spieler drehen und sprechen:
    SP_Makler.Clips[3].ReSize(3, "m_turn.smk", "", XY(470, 218), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET,
                              0, nullptr, "A1E1", 4, 5);
    SP_Makler.Clips[4].ReSize(4, "m_rede.smk", "", XY(470, 218), SPM_TALKING, CRepeat(2, 2), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                              0, nullptr, "A9A5E1E1", 4, 7, 5, 6);
    SP_Makler.Clips[7].ReSize(7, "m_redeb.smk", "", XY(470, 218), SPM_TALKING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr,
                              SMACKER_CLIP_SET, 0, nullptr, "A9E1E1", 4, 5, 6);
    SP_Makler.Clips[5].ReSize(5, "m_list.smk", "", XY(470, 218), SPM_LISTENING, CRepeat(1, 1), CPostWait(10, 20), SMACKER_CLIP_CANCANCEL, nullptr,
                              SMACKER_CLIP_SET, 0, nullptr, "A9A9E1E1", 5, 8, 4, 6);
    SP_Makler.Clips[8].ReSize(8, "m_listk.smk", "", XY(470, 218), SPM_LISTENING, CRepeat(1, 1), CPostWait(1, 1), SMACKER_CLIP_DONTCANCEL, nullptr,
                              SMACKER_CLIP_SET, 0, nullptr, "A9", 5);
    SP_Makler.Clips[6].ReSize(6, "m_back.smk", "", XY(470, 218), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET,
                              0, nullptr, "A9", 0);

    // Raumanimationen
    {
        PLAYER &qPlayer = Sim.Players.Players[static_cast<SLONG>(PlayerNum)];

        NeonFx.ReInit("neon.raw");

        BubbleAnim.ReSize(pRoomLib, "BUL00", 7, nullptr, FALSE, ANIMATION_MODE_REPEAT, 0, 2);
        FishAnim.ReSize(pRoomLib, "FISH00", 35, nullptr, FALSE, ANIMATION_MODE_REPEAT, 400, 2, 300, 1);
        Lights1Anim.ReSize(pRoomLib, "LICHTL00", 5, &NeonFx, FALSE, ANIMATION_MODE_REPEAT, 0, 5);
        Lights2Anim.ReSize(pRoomLib, "LICHTR00", 5, nullptr, FALSE, ANIMATION_MODE_REPEAT, 0, 5);

        if (rand() % 2 == 0 || (CheatAnimNow != 0)) {
            if ((qPlayer.HasItem(ITEM_BH) == 0) && (qPlayer.HasItem(ITEM_HUFEISEN) == 0) && qPlayer.TrinkerTrust == FALSE && Sim.Difficulty != DIFF_TUTORIAL) {
                KlappenAnim.ReSize(pRoomLib, "KLAPPE00", 4, &KlappeFx, FALSE, ANIMATION_MODE_ONCE, 500, 5);
            } else {
                KlappenAnim.ReSize(pRoomLib, "KLAPOH00", 4, &KlappeFx, FALSE, ANIMATION_MODE_ONCE, 500, 5);
            }
        }
    }

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(40);
    }

    Talkers.Talkers[TALKER_MAKLER].IncreaseReference();
    DefaultDialogPartner = TALKER_MAKLER;

#ifdef DEMO
    MenuStart(MENU_REQUEST, MENU_REQUEST_NO_MAKLER);
    MenuSetZoomStuff(XY(320, 220), 0.17, FALSE);
#endif
}

//--------------------------------------------------------------------------------------------
// Destruktor
//--------------------------------------------------------------------------------------------
CMakler::~CMakler() { Talkers.Talkers[TALKER_MAKLER].DecreaseReference(); }

//////////////////////////////////////////////////////////////////////////////////////////////
// Makler message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// void CMakler::OnPaint()
//--------------------------------------------------------------------------------------------
void CMakler::OnPaint() {
    SLONG NewTip = 0;
    PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

    if (Sim.Date > 5) {
        Sim.GiveHint(HINT_FLUGZEUGMAKLER);
    }

    if (SLONG(Sim.Time) >= timeMaklClose) {
        Sim.Players.Players[PlayerNum].LeaveRoom();
    }

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    // Draw Persons:
    RoomBm.pBitmap->SetClipRect(CRect(0, 149, 343, 149 + 86));
    for (SLONG d = 0; d < SLONG(Sim.Persons.AnzEntries()); d++) {
        // Entscheidung! Person malen:
        if ((Sim.Persons.IsInAlbum(d) != 0) && (Clans.IsInAlbum(Sim.Persons[d].ClanId) != 0) && Sim.Persons[d].State != Sim.localPlayer) {
            PERSON &qPerson = Sim.Persons[d];
            CLAN &qClan = Clans[static_cast<SLONG>(qPerson.ClanId)];
            UBYTE Dir = qPerson.LookDir;
            UBYTE Phase = qPerson.Phase;

            if (Dir == 1 || Dir == 3) {
                Dir = 4 - Dir;
            }
            if (Dir == 8 && Phase < 4) {
                Phase = UBYTE((Phase + 2) % 4);
            }
            if (Dir == 8 && Phase >= 4) {
                Phase = UBYTE((Phase + 2) % 4 + 4);
            }

            XY p = XY(126, 331) - (qPerson.ScreenPos - AirportRoomPos);

            if (p.x > -50 && p.y < 470 && abs(qPerson.ScreenPos.y - AirportRoomPos.y) < 40) {
                qClan.BlitLargeAt(RoomBm, Dir, Phase, p);
            }
        } else {
            break;
        }
    }
    RoomBm.pBitmap->SetClipRect(CRect(0, 0, 640, 440));

    RoomBm.BlitFromT(DoorOpaqueBm, 0, 149);
    ColorFX.BlitTrans(DoorTransBms[0].pBitmap, RoomBm.pBitmap, XY(325, 200), nullptr, 2);
    ColorFX.BlitTrans(DoorTransBms[1].pBitmap, RoomBm.pBitmap, XY(225, 184), nullptr, 2);
    ColorFX.BlitTrans(DoorTransBms[2].pBitmap, RoomBm.pBitmap, XY(102, 172), nullptr, 2);
    ColorFX.BlitTrans(DoorTransBms[3].pBitmap, RoomBm.pBitmap, XY(0, 157), nullptr, 2);
    // ColorFX.BlitOutline (DoorTransBms[3].pBitmap, RoomBm.pBitmap, XY(0,157), 0xffffff);

    // Die Raum-Animationen:
    BubbleAnim.BlitAt(RoomBm, 394, 400);
    FishAnim.BlitAt(RoomBm, 394, 400);

    if (KommVarLicht != 0) {
        Lights1Anim.BlitAt(RoomBm, 88, 400);
        Lights2Anim.BlitAt(RoomBm, 193, 375);
    } else {
        NeonFx.Stop();
    }

    KlappenAnim.BlitAt(RoomBm, 0, 0);

    SP_Makler.Pump();
    SP_Makler.BlitAtT(RoomBm);

    SLONG Frames = (AtGetTime() - LastWaterTime) / 100;
    if (Frames > 20) {
        Frames = 20;
    }
    if (Frames < 0) {
        Frames = 0;
    }

    if (SpringState == 0 && (KommVarWasser != 0)) {
        SpringFx.Play(DSBPLAY_NOSTOP | DSBPLAY_LOOPING, Sim.Options.OptionEffekte * 100 / 7);
    }

    if (SpringState == 1 && (KommVarWasser == 0)) {
        SpringFx.Stop();
    }

    SpringState = KommVarWasser;

    while (Frames > 0) {
        if (WaterFrame == 0 && (KommVarWasser != 0)) {
            WaterFrame++;
        } else if (KommVarWasser != 0) {
            WaterFrame++;
            if (WaterFrame >= 6) {
                WaterFrame = 3;
            }
        } else if (WaterFrame > 0) {
            WaterFrame++;
            if (WaterFrame >= 9) {
                WaterFrame = 0;
            }
        }

        LastWaterTime = AtGetTime();
        Frames--;
    }
    RoomBm.BlitFrom(WaterBms[WaterFrame], 226, 253);

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::InitToolTips();

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        if (gMousePosition.IfIsWithin(55, 106, 196, 405)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_MAKLER, 999);
        } else if (gMousePosition.IfIsWithin(335, 199, 640, 440)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_MAKLER, 10);
        } else if (gMousePosition.IfIsWithin(0, 15, 38, 79) && KlappenAnim.GetFrame() == 3 && (qPlayer.HasItem(ITEM_BH) == 0) &&
                   (qPlayer.HasItem(ITEM_HUFEISEN) == 0) && qPlayer.TrinkerTrust == FALSE && Sim.Difficulty != DIFF_TUTORIAL) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_MAKLER, 20);
        }
    }

    CStdRaum::PostPaint();

    if (CurrentMenu == MENU_BUYPLANE && ((gMousePosition - MenuPos).IfIsWithin(216, 6, 387, 212))) {
        NewTip = (gMousePosition.y - (MenuPos.y + 22)) / 13 + MenuPage;

        if (NewTip >= 0 && NewTip - MenuPage < 13 && NewTip < MenuDataTable.LineIndex.AnzEntries() &&
            (PlaneTypes.IsInAlbum(MenuDataTable.LineIndex[NewTip]) != 0)) {
            if (NewTip != CurrentTip) {
                MenuRepaint();
                DrawPlaneTipContents(OnscreenBitmap, &PlaneTypes[MenuDataTable.LineIndex[NewTip]], nullptr, XY(6, 6), XY(6, 28), &FontSmallBlack,
                                     &FontSmallBlack);
                CurrentTip = NewTip;
            }

            CheckCursorHighlight(
                ReferenceCursorPos,
                CRect(MenuPos.x + 216, MenuPos.y + (NewTip - MenuPage) * 13 + 25 - 3, MenuPos.x + 387, MenuPos.y + (NewTip - MenuPage) * 13 + 25 + 12),
                ColorOfFontBlack, CURSOR_HOT);
        } else {
            NewTip = -1;
        }
    } else {
        NewTip = -1;
    }

#ifdef DEMO
    if (!IsDialogOpen() && !MenuIsOpen())
        SetMouseLook(CURSOR_EXIT, 0, ROOM_MAKLER, 999);
#endif

    CStdRaum::PumpToolTips();

    if ((MenuIsOpen() != 0) && NewTip != CurrentTip) {
        MenuRepaint();
        CurrentTip = NewTip;
    }
}

//--------------------------------------------------------------------------------------------
// void CMakler::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CMakler::OnLButtonDown(UINT nFlags, CPoint point) {
    XY RoomPos;
    PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

    DefaultOnLButtonDown();

    if (ConvertMousePosition(point, &RoomPos) == 0) {
        CStdRaum::OnLButtonDown(nFlags, point);
        return;
    }

    if (PreLButtonDown(point) == 0) {
        // Raum verlassen:
        if (MouseClickArea == ROOM_MAKLER && MouseClickId == 999) {
            Sim.Players.Players[PlayerNum].LeaveRoom();
        } else if (MouseClickArea == ROOM_MAKLER && MouseClickId == 10) {
            StartDialog(TALKER_MAKLER, MEDIUM_AIR);
        } else if (MouseClickArea == ROOM_MAKLER && MouseClickId == 20) {
            if ((qPlayer.HasSpaceForItem() != 0) && (Sim.Players.Players[PlayerNum].HasItem(ITEM_BH) == 0)) {
                Sim.Players.Players[PlayerNum].BuyItem(ITEM_BH);

                SLONG cs = KlappenAnim.CounterStart;
                KlappenAnim.ReSize(pRoomLib, "KLAPOH00", 4, &KlappeFx, FALSE, ANIMATION_MODE_ONCE, 500, 5);
                KlappenAnim.CounterStart = cs;
            }
        } else {
            CStdRaum::OnLButtonDown(nFlags, point);
        }
    }
}

//--------------------------------------------------------------------------------------------
// void CMakler::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CMakler::OnRButtonDown(UINT nFlags, CPoint point) {
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
