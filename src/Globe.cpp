//============================================================================================
// Globe.cpp : Der Nachfolger des Schedulers
//============================================================================================
// Link: "Globe.h"
//============================================================================================
#include "StdAfx.h"
#include "glglobe.h"
#include "glglobea.h"

#define GFX_GLOBE (0x00000045424f4c47)
#define GFX_GLASS (0x0000005353414c47)

#define RDTSC __asm _emit 0x0F __asm _emit 0x31

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// Ikonen
// static SLONG IconsPos[] = {  84,340 , 111,340 , 204,340 , 440,340 , 521,340 , 562,340 ,
SLONG IconsPos[] = {
    25,  277, 186, 345, 186, 395, 400, 340, 428, 390, 566, 380, 29, 391, 75, 421, 565, 421, 607, 391, 353, 397, 247, 396,  // 4 Figuren und 2 Pfeile
    545, 353, 63,  121, 84,  49,  64,  275, 90,  347, 55,  197, 29, 391, 75, 421, 565, 421, 607, 391, 136, 225, 555, 225}; // 4 Figuren und 2 Pfeile

static XY GlobeOffset[] = {XY(141, 16), XY(177, 29)};

extern DOUBLE a;

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
// Konstruktor
//--------------------------------------------------------------------------------------------
CGlobe::CGlobe(BOOL bHandy, SLONG PlayerNum) : CPlaner(bHandy, PlayerNum, Sim.Players.Players[PlayerNum].EarthAlpha, FALSE) {
    SLONG c = 0;
    PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

    CWaitCursorNow wc; // CD-Cursor anzeigen

    CurrentBlock = -1;
    CWait MouseWait;

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(40);
    }

    OfficeState = qPlayer.OfficeState;
    if (OfficeState == 3) {
        pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("buerodrk.gli", RoomPath)), &pGLibDark, L_LOCMEM);
        DarkBm.ReSize(pGLibDark, "BUERO");
    } else {
        pGLibDark = nullptr;
    }

    FensterVisible = (Sim.IsTutorial) != 0 ? 1 : qPlayer.GlobeFileOpen;

    KonstruktorFinished = 0;

    CPlaner::IsLaptop = FALSE;
    DragFlightMode = 0;

    CurrentDragId = -1;
    pBlock = nullptr;
    // CurrentIcon       = -1;
    EarthTargetAlpha = EarthAlpha;
    IsInClientArea = FALSE;
    IsInClientAreaB = FALSE;

    CurrentPostItType = 0;
    CurrentPostItId = -1;

    Copyprotection = static_cast<BOOL>(Sim.bExeChanged == 1);

#ifdef WIN32
    EarthBm.ReSize(const_cast<char *>((LPCTSTR)FullFilename("earthglo.lbm", GliPath)), SYSRAMBM);
#else
    EarthBm.ReSize(const_cast<char *>((LPCTSTR)FullFilename("earthglo.tga", GliPath)), SYSRAMBM);
#endif

    pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("globe.gli", RoomPath)), &pGLibGlobe, L_LOCMEM);
    pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("globe_a.gli", RoomPath)), &pGLibStd, L_LOCMEM);

    ReSize(bprintf("globe_a%c.gli", 'a' + PlayerNum), GFX_GLOBE);
    GlasLeer.ReSize(pRoomLib, GFX_GLASS);

    MapPlaneBms[0].ReSize(pGLibStd, "PL_B00", 1 + 8);
    MapPlaneBms[1].ReSize(pGLibStd, "PL_V00", 1 + 8);
    MapPlaneBms[2].ReSize(pGLibStd, "PL_R00", 1 + 8);
    MapPlaneBms[3].ReSize(pGLibStd, "PL_J00", 1 + 8);
    LockBm.ReSize(pGLibStd, "LOCK");

    Back.ReSize(pGLibStd, GFX_BACK);
    Next.ReSize(pGLibStd, GFX_NEXT);
    Index1.ReSize(pGLibStd, "INDEX0", 4);
    IndexA.ReSize(pGLibStd, GFX_INDEXR_1, GFX_INDEXR_2, GFX_INDEXR_3, NULL);
    Inhalt.ReSize(pGLibStd, GFX_INHALT);
    Filofax.ReSize(pGLibStd, GFX_FILOFAX);
    FiloTops.ReSize(pGLibStd, "FILOTOP1", 2);
    Karte.ReSize(pGLibStd, GFX_KARTE);
    TurnLeftBm.ReSize(::pGLibStd, GFX_ID_01);
    TurnRightBm.ReSize(::pGLibStd, GFX_IE_01);
    TimeTables.ReSize(pGLibStd, "TIMETAB1", 2);

    if (Copyprotection != 0) {
        FiloEdge.ReSize(pGLibStd, GFX_FILOECK2);
    } else {
        FiloEdge.ReSize(pGLibStd, GFX_FILOECKE);
    }

    QuietschFX.ReInit("quietsch.raw");
    Quietsching = FALSE;

    for (c = 0; c < 6; c++) {
        pGLibIcons[c] = nullptr;
    }

    KonstruktorFinished = 1;

    FrameWnd->Invalidate();
    MessagePump();
    FrameWnd->Invalidate();
    MessagePump();

    MessagePump();

    qPlayer.Blocks.RepaintAll = FALSE;

    // Globus-Block nötigenfalls erzeugen:
    if (qPlayer.Blocks.AnzEntries() == 0) {
        qPlayer.Blocks.ReSize(1);
    }
    if (qPlayer.Blocks.IsInAlbum(ULONG(0)) == 0) {

        ULONG Id = (qPlayer.Blocks *= BLOCK());
        BLOCK &qBlock = qPlayer.Blocks[Id];

        qBlock.PlayerNum = PlayerNum;
        qBlock.ScreenPos = XY(80, 100);
        qBlock.IsTopWindow = TRUE;

        qBlock.BlockType = 0;
        qBlock.SelectedId = 0;
        qBlock.Index = TRUE;
        qBlock.Page = 0;
        qBlock.DoubleBlock = TRUE;
        qBlock.Tip = TIP_NONE;
        qBlock.TipInUse = TIP_NONE;

        qBlock.SelectedIdB = 0;
        qBlock.IndexB = TRUE;
        qBlock.PageB = 0;
        qBlock.TipB = TIP_NONE;
        qBlock.TipInUseB = TIP_NONE;

        qBlock.Base = this;

        for (c = 0; c < 6; c++) {
            qBlock.Indexes[c] = 1;
            qBlock.SelectedIds[c] = 0;
            qBlock.Pages[c] = 0;
        }

        if (Sim.IsTutorial != 0) {
            qBlock.BlockTypeB = 4;
        } else {
            qBlock.BlockTypeB = 3;
            qBlock.BlockType = 2;
            qBlock.Index = TRUE;
        }

        qBlock.RefreshData(PlayerNum);
        qBlock.Refresh(PlayerNum, FALSE);

        Limit(SLONG(-qPlayer.Blocks[Id].Bitmap.Size.x / 2), qPlayer.Blocks[Id].ScreenPos.x, static_cast<SLONG>(640 - qPlayer.Blocks[Id].Bitmap.Size.x / 2));
        Limit(static_cast<SLONG>(-20), qPlayer.Blocks[Id].ScreenPos.y, static_cast<SLONG>(400));
    }

    // Base-Pointer der Blöcke initialisieren:
    for (c = qPlayer.Blocks.AnzEntries() - 1; c >= 1; c--) {
        if (qPlayer.Blocks.IsInAlbum(ULONG(c)) != 0) {
            qPlayer.Blocks[c].Base = nullptr;
        }
    }

    if (qPlayer.Blocks.IsInAlbum(ULONG(0)) != 0) {
        qPlayer.Blocks[ULONG(0)].Base = this;
    }

    // Refreshen:
    if (qPlayer.Blocks.IsInAlbum(ULONG(0)) != 0) {
        BLOCK &qBlock = qPlayer.Blocks[ULONG(0)];

        qBlock.RefreshData(PlayerNum);
        qBlock.Page = min(qBlock.Page / 6, qBlock.AnzPages - 1) * 6;
        qBlock.PageB = min(qBlock.PageB / 6, qBlock.AnzPagesB - 1) * 6;
        qBlock.Refresh(PlayerNum, FALSE);

        Limit(SLONG(-qBlock.Bitmap.Size.x / 2), qBlock.ScreenPos.x, static_cast<SLONG>(640 - qBlock.Bitmap.Size.x / 2));
        Limit(static_cast<SLONG>(-20), qBlock.ScreenPos.y, static_cast<SLONG>(400));
    }

    Sim.Players.Players[Sim.localPlayer].UpdateAuftragsUsage();
    Sim.Players.Players[Sim.localPlayer].UpdateFrachtauftragsUsage();
    Sim.Players.Players[Sim.localPlayer].Planes.UpdateGlobePos(EarthAlpha);

    KonstruktorFinished = 2;

    PaintGlobe();
    PaintGlobeRoutes();
}

//--------------------------------------------------------------------------------------------
// Destruktor
//--------------------------------------------------------------------------------------------
CGlobe::~CGlobe() {
    Sim.Players.Players[PlayerNum].GlobeFileOpen = FensterVisible;

    QuietschFX.Stop();

    PicBitmap.Destroy();

    Back.Destroy();
    Next.Destroy();
    Index1.Destroy();
    IndexA.Destroy();
    Inhalt.Destroy();
    Filofax.Destroy();
    FiloEdge.Destroy();
    FiloTops.Destroy();
    Karte.Destroy();
    TurnLeftBm.Destroy();
    TurnRightBm.Destroy();
    TimeTables.Destroy();
    LockBm.Destroy();

    MapPlaneBms[0].Destroy();
    MapPlaneBms[1].Destroy();
    MapPlaneBms[2].Destroy();
    MapPlaneBms[3].Destroy();

    if ((pGLibGlobe != nullptr) && (pGfxMain != nullptr)) {
        pGfxMain->ReleaseLib(pGLibGlobe);
    }
    if ((pGLibStd != nullptr) && (pGfxMain != nullptr)) {
        pGfxMain->ReleaseLib(pGLibStd);
    }
    pGLibGlobe = nullptr;

    DarkBm.Destroy();

    if ((pGLibDark != nullptr) && (pGfxMain != nullptr)) {
        pGfxMain->ReleaseLib(pGLibDark);
    }
    pGLibDark = nullptr;

    for (SLONG c = 0; c < 6; c++) {
        if ((pGLibIcons[c] != nullptr) && (pGfxMain != nullptr)) {
            IconBms[c].Destroy();
            pGfxMain->ReleaseLib(pGLibIcons[c]);
            pGLibIcons[c] = nullptr;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Globe message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// void CGlobe::OnPaint()
//--------------------------------------------------------------------------------------------
void CGlobe::OnPaint() {
    PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
    XY RoomPos = qPlayer.CursorPos;

    static SLONG LastHour;
    static ULONG RefreshTicker = 0;

    if (OfficeState != 3 && Sim.Players.Players[PlayerNum].OfficeState == 3) {
        OfficeState = 3;
        pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("buerodrk.gli", RoomPath)), &pGLibDark, L_LOCMEM);
        DarkBm.ReSize(pGLibDark, "BUERO");
    } else {
        pGLibDark = nullptr;
    }

    if (KonstruktorFinished != 2) {
        return;
    }

    if (Copyprotection != 0) {
        FensterVisible = FALSE;
    }

    // Blöcke: Tips löschen:
    if (qPlayer.Blocks.IsInAlbum(ULONG(0)) != 0) {
        qPlayer.Blocks[ULONG(0)].SetTip(TIP_NONE, TIP_NONE, 0);
    }

    RefreshTicker++;
    if ((RefreshTicker & 1) == 0) {
        if (qPlayer.Blocks.IsInAlbum((RefreshTicker >> 1) % qPlayer.Blocks.AnzEntries()) != 0) {
            qPlayer.Blocks[(RefreshTicker >> 1) % qPlayer.Blocks.AnzEntries()].Refresh(Sim.localPlayer, FALSE);
        }
    }

    if (Sim.GetHour() != LastHour) {
        Sim.Players.Players[Sim.localPlayer].UpdateAuftragsUsage();
        Sim.Players.Players[Sim.localPlayer].UpdateFrachtauftragsUsage();
        LastHour = Sim.GetHour();
    }

    if (UsedToRotate == 2) {
        UsedToRotate = 1;
    }

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();
    CStdRaum::InitToolTips();

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0) && pBlock == nullptr) {
        if (gMousePosition.IfIsWithin(495, 241, 640, 440)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_LAST_MINUTE, 999);
        }
        if (gMousePosition.IfIsWithin(540, 0, 640, 320)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_LAST_MINUTE, 999);
        }
    }

    // CPlaner::DoPollingStuff ();

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        // Alle 10 Minuten ein Repaint erzwingen:
        if (LastPaintedMinute != Sim.GetMinute() && Sim.GetMinute() % 10 == 0) {
            if (qPlayer.Blocks.IsInAlbum(ULONG(0)) != 0) {
                qPlayer.Blocks[ULONG(0)].Refresh(PlayerNum, FALSE);
            }
        }
        LastPaintedMinute = Sim.GetMinute();

        ///////////////////////////////////////////////////////////////////////////////////////////
        //// Mouse evaluation                                                                  ////
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        // Is Cursor over Block?
        CurrentBlock = -1;
        IsInClientArea = FALSE;
        IsInClientAreaB = FALSE;
        pBlock = nullptr;
        if ((FensterVisible != 0) && (qPlayer.Blocks.IsInAlbum(ULONG(0)) != 0)) {
            if (gMousePosition.IfIsWithin(qPlayer.Blocks[ULONG(0)].ScreenPos,
                                          qPlayer.Blocks[ULONG(0)].ScreenPos + qPlayer.Blocks[ULONG(0)].Bitmap.Size - XY(1, 1))) {
                if (qPlayer.Blocks[ULONG(0)].Bitmap.GetPixel((gMousePosition - qPlayer.Blocks[ULONG(0)].ScreenPos).x,
                                                             (gMousePosition - qPlayer.Blocks[ULONG(0)].ScreenPos).y) != 255) {
                    CurrentBlock = 0;
                    pBlock = &qPlayer.Blocks[ULONG(0)];

                    CurrentBlockPos = gMousePosition - pBlock->ScreenPos;

                    if (CurrentBlockPos.IfIsWithin(48, 72, 48 + 172, 72 + 170)) {
                        IsInClientArea = TRUE;
                        ClientPos = CurrentBlockPos - XY(48, 72);
                        ReferenceCursorPos = gMousePosition;
                    } else {
                        IsInClientArea = FALSE;
                    }

                    if (CurrentBlockPos.IfIsWithin(48 + 232, 72, 48 + 172 + 232, 72 + 170)) {
                        IsInClientAreaB = TRUE;
                        ClientPosB = CurrentBlockPos - XY(48 + 232, 72);
                        ReferenceCursorPos = gMousePosition;
                    } else {
                        IsInClientAreaB = FALSE;
                    }
                }
            }
        }

        CPlaner::DoPollingStuff();

        if (pBlock == nullptr) {
            if (gMousePosition.IfIsWithin(353, 397, 393, 417)) {
                SetMouseLook(CURSOR_HOT, 0, ROOM_GLOBE, 100, 10);
                RoomBm.BlitFromT(TurnLeftBm, 353, 397);
            }
            if (gMousePosition.IfIsWithin(247, 396, 287, 416)) {
                SetMouseLook(CURSOR_HOT, 0, ROOM_GLOBE, 100, 11);
                RoomBm.BlitFromT(TurnRightBm, 247, 396);
            }
        } else {
            if (CurrentBlockPos.IfIsWithin(28, 260, 108, 290)) {
                SetMouseLook(CURSOR_HOT, 3002, ROOM_GLOBE, 100, 22); // Flugzeuge
            }
            if (CurrentBlockPos.IfIsWithin(108, 260, 188, 290)) {
                SetMouseLook(CURSOR_HOT, 3001, ROOM_GLOBE, 100, 21); // Städte
            }
            if (CurrentBlockPos.IfIsWithin(169, 260, 268, 290)) {
                SetMouseLook(CURSOR_HOT, 3005, ROOM_GLOBE, 100, 25); // Infos
            }

            if (pBlock->Page / pBlock->PageSize < pBlock->AnzPages - 1 && CurrentBlockPos.IfIsWithin(208, 233, 231, 255)) {
                SetMouseLook(CURSOR_RIGHT, 0, ROOM_GLOBE, 100, 30); // Next
                if ((gMouseLButton != 0) && AtGetTime() - gMouseLButtonDownTimer > 1000) {
                    OnLButtonDown(0, CPoint(0, 0));
                    gMouseLButtonDownTimer = AtGetTime() - 870;
                }
            } else if (pBlock->Page > 0 && CurrentBlockPos.IfIsWithin(40, 237, 60, 256)) {
                SetMouseLook(CURSOR_LEFT, 0, ROOM_GLOBE, 100, 31); // Prev
                if ((gMouseLButton != 0) && AtGetTime() - gMouseLButtonDownTimer > 1000) {
                    OnLButtonDown(0, CPoint(0, 0));
                    gMouseLButtonDownTimer = AtGetTime() - 870;
                }
            } else if (pBlock->BlockType > 0 && pBlock->Index != 1 && CurrentBlockPos.IfIsWithin(0, 178 - 100, 36, 281 - 100)) { // Klick auf "Index"...
                SetMouseLook(CURSOR_HOT, 0, ROOM_GLOBE, 100, 35);
            }

            if (pBlock->DoubleBlock != 0) {
                if (pBlock->PageB / pBlock->PageSizeB < pBlock->AnzPagesB - 1 && CurrentBlockPos.IfIsWithin(208 + 232, 233, 231 + 232, 255)) //"Next" B...
                {
                    SetMouseLook(CURSOR_RIGHT, 0, ROOM_GLOBE, 100, 40);
                    if ((gMouseLButton != 0) && AtGetTime() - gMouseLButtonDownTimer > 1000) {
                        OnLButtonDown(0, CPoint(0, 0));
                        gMouseLButtonDownTimer = AtGetTime() - 870;
                    }
                } else if (pBlock->PageB > 0 && CurrentBlockPos.IfIsWithin(40 + 232, 237, 60 + 232, 256)) //"Prev" B...
                {
                    SetMouseLook(CURSOR_LEFT, 0, ROOM_GLOBE, 100, 41);
                    if ((gMouseLButton != 0) && AtGetTime() - gMouseLButtonDownTimer > 1000) {
                        OnLButtonDown(0, CPoint(0, 0));
                        gMouseLButtonDownTimer = AtGetTime() - 870;
                    }
                } else if (pBlock->IndexB != 1 && CurrentBlockPos.IfIsWithin(0 + 232, 178 - 100, 36 + 232, 281 - 100)) { // Klick auf "Index" B...
                    SetMouseLook(CURSOR_HOT, 0, ROOM_GLOBE, 100, 45);
                }
            }

            if ((pBlock != nullptr) && pBlock->BlockType == 2) {
                if (CurrentBlockPos.IfIsWithin(272, 258, 343, 289)) {
                    SetMouseLook(CURSOR_HOT, 3003, ROOM_GLOBE, 100, 23); // Aufträge
                }
                if (CurrentBlockPos.IfIsWithin(343, 258, 414, 289)) {
                    SetMouseLook(CURSOR_HOT, 3008, ROOM_GLOBE, 100, 26); // Fracht
                }
                if (CurrentBlockPos.IfIsWithin(414, 258, 486, 289)) {
                    SetMouseLook(CURSOR_HOT, 3004, ROOM_GLOBE, 100, 24); // Routen
                }
            }
        }

        // Block erscheinen oder verschwinden lassen
        if (pBlock == nullptr && FensterVisible == FALSE) {
            if (gMousePosition.x < FiloEdge.Size.x && gMousePosition.y > 440 - FiloEdge.Size.y && gMousePosition.y < 440) {
                SetMouseLook(CURSOR_HOT, 0, 0, 0, 0);
            }
        }

        // Window moving:
        if (CurrentDragId != -1 && ((qPlayer.Buttons & 1) != 0)) {
            qPlayer.Blocks[CurrentDragId].ScreenPos = RoomPos - DragOffset;
            Limit(SLONG(-qPlayer.Blocks[CurrentDragId].Bitmap.Size.x / 2), qPlayer.Blocks[CurrentDragId].ScreenPos.x,
                  static_cast<SLONG>(640 - qPlayer.Blocks[CurrentDragId].Bitmap.Size.x / 2));
            Limit(static_cast<SLONG>(-20), qPlayer.Blocks[CurrentDragId].ScreenPos.y, static_cast<SLONG>(400));
        }

        // Ggf. Erde rotieren lassen:
        if (CurrentBlock == -1 && CurrentDragId == -1 && ((qPlayer.Buttons & 1) != 0)) {
            if (MouseClickArea == ROOM_GLOBE && MouseClickId == 100 && MouseClickPar1 == 11) {
                CPlaner::TurnGlobe(1000); // Turn Left
                if ((Quietsching != 0) && (qPlayer.GlobeOiled == 0)) {
                    QuietschFX.Play(DSBPLAY_NOSTOP | DSBPLAY_LOOPING, Sim.Options.OptionEffekte * 100 / 7);
                }
                Quietsching = TRUE;
            } else if (MouseClickArea == ROOM_GLOBE && MouseClickId == 100 && MouseClickPar1 == 10) {
                CPlaner::TurnGlobe(-1000); // Turn Right
                if ((Quietsching != 0) && (qPlayer.GlobeOiled == 0)) {
                    QuietschFX.Play(DSBPLAY_NOSTOP | DSBPLAY_LOOPING, Sim.Options.OptionEffekte * 100 / 7);
                }
                Quietsching = TRUE;
            } else {
                if (Quietsching != 0) {
                    QuietschFX.Stop();
                }
                Quietsching = FALSE;
            }
        } else {
            if (Quietsching != 0) {
                QuietschFX.Stop();
            }
            Quietsching = FALSE;
        }
    } else {
        CPlaner::DoPollingStuff();

        if (Quietsching != 0) {
            QuietschFX.Stop();
        }
        Quietsching = FALSE;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// Painting                                                                          ////
    ///////////////////////////////////////////////////////////////////////////////////////////

    if (bActive != 0) {
        PaintGlobeInScreen(GlobeOffset[0]);

        if (Sim.Players.Players[PlayerNum].OfficeState == 3) {
            RoomBm.BlitFromT(DarkBm, 0, 0);
            RoomBm.BlitFromT(DarkBm, 500, 0);
        }

        // Blöcke zeichnen
        if (FensterVisible != 0) {
            qPlayer.Blocks[ULONG(0)].IsTopWindow = TRUE;
            qPlayer.Blocks[ULONG(0)].UpdateTip(PlayerNum, FALSE);
            if (qPlayer.Blocks.IsInAlbum(ULONG(0)) != 0) {
                qPlayer.Blocks[ULONG(0)].BlitAt(RoomBm);
            }

            if (qPlayer.Blocks[ULONG(0)].IsTopWindow != 0) {
                if (MouseClickId == 150) {
                    RoomBm.BlitFromT(FlugplanBms[55], qPlayer.Blocks[ULONG(0)].ScreenPos + XY(70, 117) + XY(0, 105));
                } else if (MouseClickId == 151) {
                    RoomBm.BlitFromT(FlugplanBms[56], qPlayer.Blocks[ULONG(0)].ScreenPos + XY(59, 117) + XY(120, 105));
                }
            }

            CPlaner::DoPostPaintPollingStuff(XY(48, 73) + XY(21, -2) + XY(2, 18));
        } else if (Sim.Players.Players[PlayerNum].OfficeState != 3) {
            RoomBm.BlitFromT(FiloEdge, 0, 440 - FiloEdge.Size.y);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// Cursor & Onscreen Stuff                                                           ////
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (CurrentPostItType != 0) {
        XY PostItPos = gMousePosition - PostItBm.Size / SLONG(2);

        if (ClientPos.IfIsWithin(24, 17, 167, 149)) {
            // An diese Stelle (links vom Cursor) kommt der Flug hin:
            SLONG Date = Sim.Date + ((ClientPos - PostItBm.Size / SLONG(2) + XY(3, 9)).y - 17) / 19;
            SLONG Time = ((ClientPos - PostItBm.Size / SLONG(2) + XY(3, 9)).x - 24) / 6;

            PostItPos.x = Time * 6 + 24;
            PostItPos.y = (Date - Sim.Date) * 19 + 17;

            PostItPos += (gMousePosition - ClientPos);
        }

        if (RoomBm.BlitFromT(PostItBm, PostItPos) == 0) {
            PaintPostIt();
            RoomBm.BlitFromT(PostItBm, PostItPos);
        }
    }

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::PostPaint();
    CStdRaum::PumpToolTips();
}

//--------------------------------------------------------------------------------------------
// void CGlobe::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CGlobe::OnLButtonDown(UINT nFlags, CPoint point) {
    XY Dummy;

    if (gMousePosition.y >= 440) {
        PreLButtonDown(gMousePosition);
        DefaultOnLButtonDown();
        CStdRaum::OnLButtonDown(nFlags, point);
        return;
    }

    if ((DragFlightMode != 0) || MouseClickId == 105) {
        DefaultOnLButtonDown();
        return;
    }

    if (KonstruktorFinished == 0) {
        return;
    }

    if (ConvertMousePosition(point, &Dummy) == 0) {
        // CWnd::OnLButtonDown(nFlags, point);
        return;
    }

    if (PreLButtonDown(gMousePosition) != 0) {
        return;
    }

    HandleLButtonDown();
    if (MouseClickArea == ROOM_LAST_MINUTE && MouseClickId == 999) {
        if ((Sim.IsTutorial != 0) && Sim.Tutorial <= 1506) {
            return;
        }
        Sim.Players.Players[PlayerNum].LeaveRoom();
    }

    // Block erscheinen oder verschwinden lassen
    if (pBlock == nullptr) {
        if (gMousePosition.x < FiloEdge.Size.x && gMousePosition.y > 440 - FiloEdge.Size.y && gMousePosition.y < 440) {
            if (FensterVisible == FALSE) {
                FensterVisible = TRUE;
                gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                PaintGlobe();
                PaintGlobeRoutes();
            }
        } else {
            if (FensterVisible == TRUE && gMousePosition.y < 440) {
                if (!(MouseClickArea == ROOM_LAST_MINUTE && MouseClickId == 999)) {
                    // gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
                    PaintGlobe();
                    PaintGlobeRoutes();
                }
            }
        }
    }

    // Klick auf ein FLugzeug auf der Weltkarte:
    if (CurrentBlock == -1 && MouseClickArea == ROOM_GLOBE && MouseClickId == 101) {
        BLOCK &qBlock = Sim.Players.Players[PlayerNum].Blocks[ULONG(0)];

        if (FensterVisible == 0) {
            FensterVisible = TRUE;
            gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
        }

        qBlock.BlockType = 2;
        qBlock.SelectedId = MouseClickPar1;
        qBlock.Index = FALSE;
        qBlock.Page = 0;

        qBlock.RefreshData(PlayerNum);
        qBlock.Refresh(PlayerNum, FALSE);

        GlobeBm.Clear(0);
        PaintGlobe();
        PaintGlobeRoutes();
    } else {
        // Test, ob Klick auf Block
        if (CurrentBlock != -1) {
            BLOCK &qBlock = *pBlock;

            // Art des Blocks wechseln:
            if (MouseClickArea == ROOM_GLOBE && MouseClickId == 100) {
                if (MouseClickPar1 == 21 || MouseClickPar1 == 22 || MouseClickPar1 == 25) {
                    if (qBlock.BlockType != MouseClickPar1 - 20) {
                        // Tutorial und zu früh? Dann abbruch!
                        if ((Sim.IsTutorial != 0) && Sim.Tutorial != 1502) {
                            return;
                        }

                        // Alte Parameter sichern:
                        if (qBlock.BlockType != 0) {
                            qBlock.Indexes[qBlock.BlockType - 1] = qBlock.Index;
                            qBlock.SelectedIds[qBlock.BlockType - 1] = qBlock.SelectedId;
                            qBlock.Pages[qBlock.BlockType - 1] = qBlock.Page;
                        }

                        // Neue Parameter laden:
                        qBlock.BlockType = MouseClickPar1 - 20;
                        qBlock.Index = qBlock.Indexes[MouseClickPar1 - 21];
                        qBlock.SelectedId = qBlock.SelectedIds[MouseClickPar1 - 21];
                        qBlock.Page = qBlock.Pages[MouseClickPar1 - 21];
                        qBlock.RefreshData(PlayerNum);

                        // Tutorium: Spieler öffnet Flugzeugliste:
                        if (qBlock.BlockType == 2 && (Sim.IsTutorial != 0) && Sim.Tutorial == 1502) {
                            Sim.Tutorial = 1503;
                            Sim.Players.Players[Sim.localPlayer].Messages.NextMessage();
                            Sim.Players.Players[Sim.localPlayer].Messages.AddMessage(
                                BERATERTYP_GIRL, bprintf(StandardTexte.GetS(TOKEN_TUTORIUM, 1503), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].AirlineX));

                            GlowEffects.ReSize(1);
                            GlowBitmapIndices.ReSize(1);
                            GlowEffects[0] = XY(26 + 10, 52 + 5);
                            GlowBitmapIndices[0] = 3;
                        }

                        if (qBlock.Index == 0 && pBlock->BlockType == 1) {
                            pBlock->LoadCityPhotoLib(Cities[pBlock->SelectedId]);
                        }
                        qBlock.Refresh(PlayerNum, IsLaptop);
                    }
                }

                if (MouseClickPar1 == 23 || MouseClickPar1 == 24 || MouseClickPar1 == 26) {
                    if (qBlock.BlockTypeB != MouseClickPar1 - 20) {
                        if ((Sim.IsTutorial == 0) || MouseClickPar1 == 23) {
                            // Tutorium: Spieler kann Routen nicht öffnen:
                            if (MouseClickPar1 - 20 != 3 && (Sim.IsTutorial != 0)) {
                                return;
                            }

                            // Tutorium: Spieler kann Aufträge nicht zu früh nicht öffnen:
                            if (MouseClickPar1 - 20 == 3 && (Sim.IsTutorial != 0) && Sim.Tutorial < 1504) {
                                return;
                            }

                            // Alter Parameter sichern:
                            if (qBlock.BlockType != 0) {
                                qBlock.Indexes[qBlock.BlockType - 1] = qBlock.IndexB;
                                qBlock.SelectedIds[qBlock.BlockType - 1] = qBlock.SelectedIdB;
                                qBlock.Pages[qBlock.BlockType - 1] = qBlock.PageB;
                            }

                            // Neue Parameter laden:
                            qBlock.BlockTypeB = MouseClickPar1 - 20;
                            qBlock.IndexB = qBlock.Indexes[MouseClickPar1 - 21];
                            qBlock.SelectedIdB = qBlock.SelectedIds[MouseClickPar1 - 21];
                            qBlock.PageB = qBlock.Pages[MouseClickPar1 - 21];
                            qBlock.RefreshData(PlayerNum);
                            qBlock.AnzPagesB = max(0, (qBlock.TableB.AnzRows - 1) / 6) + 1;

                            // Tutorium: Spieler öffnet Auftragsliste:
                            if (qBlock.BlockTypeB == 3 && (Sim.IsTutorial != 0) && Sim.Tutorial == 1504) {
                                Sim.Tutorial = 1505;

                                GlowEffects.ReSize(1);
                                GlowBitmapIndices.ReSize(1);
                                GlowEffects[0] = XY(26 + 10 + 232, 52 + 5);
                                GlowBitmapIndices[0] = 3;

                                Sim.Players.Players[Sim.localPlayer].Messages.NextMessage();
                                Sim.Players.Players[Sim.localPlayer].Messages.AddMessage(BERATERTYP_GIRL, StandardTexte.GetS(TOKEN_TUTORIUM, 1505));
                            }

                            if (qBlock.IndexB == 1) {
                                qBlock.AnzPagesB = max(0, (qBlock.TableB.AnzRows - 1) / 6) + 1;
                            }
                            if (qBlock.IndexB == 2) {
                                qBlock.AnzPagesB = 7;
                            }
                            if (qBlock.IndexB == 0) {
                                qBlock.AnzPagesB = 1;
                            }

                            qBlock.Refresh(PlayerNum, IsLaptop);
                        }
                    }
                }

                if (MouseClickPar1 == 30) {
                    ButtonNext(); // Klick auf "Next"...
                } else if (MouseClickPar1 == 31) {
                    ButtonPrev(); // Klick auf "Prev"...
                } else if (MouseClickPar1 == 35) {
                    ButtonIndex(); // Klick auf "Index"...
                } else if (MouseClickPar1 == 40) {
                    ButtonNextB(); // Klick auf "NextB"...
                } else if (MouseClickPar1 == 41) {
                    ButtonPrevB(); // Klick auf "PrevB"...
                } else if (MouseClickPar1 == 45) {
                    ButtonIndexB(); // Klick auf "IndexB"...
                }
            }

            // Ggf. Window moving...
            else if (CurrentBlockPos.IfIsWithin(20, 0, 481, 49)) {
                if (CurrentDragId == -1) {
                    DragStart = gMousePosition;
                }
                CurrentDragId = CurrentBlock;
                DragOffset = gMousePosition - Sim.Players.Players[PlayerNum].Blocks[CurrentBlock].ScreenPos;
            }
        } // Ende: Test, ob Klick auf Block
    }

    CStdRaum::OnLButtonDown(nFlags, point);
}

//--------------------------------------------------------------------------------------------
// void CGlobe::OnLButtonUp(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CGlobe::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) {
    DefaultOnLButtonUp();

    if (KonstruktorFinished == 0) {
        return;
    }

    HandleLButtonUp();
}

//--------------------------------------------------------------------------------------------
// void CGlobe::OnLButtonDblClk(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CGlobe::OnLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/) {
    if (KonstruktorFinished == 0) {
        return;
    }

    CPlaner::HandleLButtonDouble();
}

//--------------------------------------------------------------------------------------------
// void CGlobe::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CGlobe::OnRButtonDown(UINT /*nFlags*/, CPoint /*point*/) { CPlaner::HandleRButtonDown(); }

//--------------------------------------------------------------------------------------------
// void CGlobe::OnRButtonUp(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CGlobe::OnRButtonUp(UINT /*nFlags*/, CPoint /*point*/) { CPlaner::HandleRButtonUp(); }

//--------------------------------------------------------------------------------------------
// Nachrichten weiterreichen:
//--------------------------------------------------------------------------------------------
void CGlobe::OnChar(UINT nChar, UINT a, UINT b) { CStdRaum::OnChar(nChar, a, b); }
void CGlobe::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    switch (nChar) {
    case ATKEY_UP:
    case ATKEY_LEFT:
        CPlaner::ButtonPrev();
        return;
    case ATKEY_DOWN:
    case ATKEY_RIGHT:
        CPlaner::ButtonNext();
        return;
    case ATKEY_BACK:
        if (!MenuIsOpen()) {
            CPlaner::ButtonIndex();
        } else {
            CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);
        }
        return;
    default:
        CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}
