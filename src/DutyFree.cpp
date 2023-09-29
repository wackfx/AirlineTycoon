//============================================================================================
// DutyFree.cpp : Der DutyFree Laden. Hier gibt's Anzüge (unter anderem)
//============================================================================================
// Link: "DutyFree.h"
//============================================================================================
#include "StdAfx.h"
#include "AtNet.h"
#include "DutyFree.h"
#include "glduty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

//////////////////////////////////////////////////////////////////////////////////////////////
// DutyFree Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// Konstruktor:
//--------------------------------------------------------------------------------------------
CDutyFree::CDutyFree(BOOL bHandy, ULONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "dutyfree.gli", GFX_DUTYFREE) {
    SetRoomVisited(PlayerNum, ROOM_SHOP1);

    Sim.FocusPerson = -1;

    if (bHandy == 0) {
        AmbientManager.SetGlobalVolume(40);
    }
    DefaultDialogPartner = TALKER_DUTYFREE;

    AirportRoomPos = Airport.GetRandomTypedRune(RUNE_2SHOP, ROOM_SHOP1) + XY(88, 0);

    pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("tip.gli", GliPath)), &pMenuLib, L_LOCMEM);
    ZettelBm.ReSize(pMenuLib, "BLOC1");

    OpaqueBm.ReSize(pRoomLib, GFX_OPAQUE);
    TransBm.ReSize(pRoomLib, GFX_TRANS);
    HufeisenBm.ReSize(pRoomLib, "OHNEHUF");
    GeigenkastenBm.ReSize(pRoomLib, "KONTRA");

    SchilderBms.ReSize(pRoomLib, "SCHILDD SCHILDE SCHILDF SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE "
                                 "SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE "
                                 "SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE SCHILDE");

    SP_Frau.ReSize(9);
    SP_Frau.Clips[0].ReSize(0, "dukau.smk", "dukau.raw", XY(310, 47), SPM_IDLE, CRepeat(2, 4), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr,
                            SMACKER_CLIP_SET, 0, nullptr, "A6A4A3E1E1", 0, 1, 2, 6, 7);
    SP_Frau.Clips[1].ReSize(1, "dublase.smk", "dublase.raw", XY(310, 47), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr,
                            SMACKER_CLIP_SET, 0, nullptr, "A1", 0);
    SP_Frau.Clips[2].ReSize(2, "dunagelh.smk", "", XY(310, 47), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                            nullptr, "A1", 3);
    SP_Frau.Clips[3].ReSize(3, "dunagelf.smk", "dunagelf.raw", XY(310, 47), SPM_IDLE, CRepeat(5, 5), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr,
                            SMACKER_CLIP_SET, 0, nullptr, "A1", 4);
    SP_Frau.Clips[4].ReSize(4, "dunagelg.smk", "", XY(310, 47), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                            nullptr, "A6A1E1", 3, 5, 5);
    SP_Frau.Clips[5].ReSize(5, "dunagelz.smk", "", XY(310, 47), SPM_IDLE, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_DONTCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                            nullptr, "A1E1E1", 0, 6, 7);
    SP_Frau.Clips[6].ReSize(6, "durede.smk", "", XY(310, 47), SPM_TALKING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET, 0,
                            nullptr, "A1E1E1", 6, 7, 0);
    SP_Frau.Clips[7].ReSize(7, "duredew.smk", "", XY(310, 47), SPM_LISTENING, CRepeat(1, 1), CPostWait(0, 0), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                            0, nullptr, "A9A1E1E1", 7, 8, 6, 0);
    SP_Frau.Clips[8].ReSize(8, "duredek.smk", "", XY(310, 47), SPM_LISTENING, CRepeat(1, 1), CPostWait(1, 1), SMACKER_CLIP_CANCANCEL, nullptr, SMACKER_CLIP_SET,
                            0, nullptr, "A1E1E1", 7, 6, 0);

    PayFX.ReInit("pay.raw");

    if (Sim.Options.OptionEffekte != 0) {
        StartupFX.ReInit("windchim.raw");
        StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
    }
}

//--------------------------------------------------------------------------------------------
// Destruktor:
//--------------------------------------------------------------------------------------------
CDutyFree::~CDutyFree() {
    ZettelBm.Destroy();
    HufeisenBm.Destroy();
    SchilderBms.Destroy();
    GeigenkastenBm.Destroy();
    if ((pMenuLib != nullptr) && (pGfxMain != nullptr)) {
        pGfxMain->ReleaseLib(pMenuLib);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CDutyFree message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// void CDutyFree::OnPaint():
//--------------------------------------------------------------------------------------------
void CDutyFree::OnPaint() {
    SLONG NewTip = 0;
    PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
    SLONG lasty = 600;

    if (Sim.Date > 4) {
        Sim.GiveHint(HINT_DUTYFREE);
    }

    if (bHandy == 0) {
        SetMouseLook(CURSOR_NORMAL, 0, ROOM_SHOP1, 0);
    }

    // Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    if (Sim.Date > 0 && Sim.Players.Players[PlayerNum].ArabTrust == 0) {
        RoomBm.BlitFrom(GeigenkastenBm, 275, 197);
    }

    if (!((qPlayer.HasItem(ITEM_HUFEISEN) == 0) && qPlayer.TrinkerTrust == FALSE)) {
        RoomBm.BlitFrom(HufeisenBm, 14, 36);
    }

    // Draw Persons:
    RoomBm.pBitmap->SetClipRect(CRect(432, 70, 559, 246));
    for (auto d = SLONG(Sim.Persons.AnzEntries() - 1); d >= 0; d--) {
        // Entscheidung! Person malen:
        if ((Sim.Persons.IsInAlbum(d) != 0) && (Clans.IsInAlbum(Sim.Persons[d].ClanId) != 0)) {
            PERSON &qPerson = Sim.Persons[d];
            CLAN &qClan = Clans[static_cast<SLONG>(qPerson.ClanId)];
            UBYTE Dir = qPerson.LookDir;

            if (Dir < 4) {
                Dir = UBYTE((Dir + 2) & 3);
            }

            XY p = qPerson.ScreenPos - AirportRoomPos;
            XY pp;

            if (abs(p.x) < 150) {
                pp.y = 280;
                pp.x = 511 - p.x * 2 * 400 / (pp.y + 100) + (pp.y - 300);

                if (p.y < 300 && lasty >= 300) {
                    ColorFX.BlitTrans(TransBm.pBitmap, RoomBm.pBitmap, XY(432, 70), nullptr, 4);
                }

                lasty = p.y;

                if (pp.x > 390 && pp.x < 700) {
                    qClan.BlitLargeAt(RoomBm, Dir, qPerson.Phase, pp);
                }
            }
        }
    }

    RoomBm.pBitmap->SetClipRect(CRect(0, 0, 640, 440));

    // DrawTransparency:
    if (lasty >= 300) {
        ColorFX.BlitTrans(TransBm.pBitmap, RoomBm.pBitmap, XY(432, 70), nullptr, 4);
    }
    ColorFX.BlitTrans(TransBm.pBitmap, RoomBm.pBitmap, XY(432, 70), nullptr, 4);
    RoomBm.BlitFromT(OpaqueBm, 432, 70);

    SP_Frau.Pump();
    SP_Frau.BlitAtT(RoomBm);

    // Ggf. Onscreen-Texte einbauen:
    CStdRaum::InitToolTips();

    // Ggf. Tips einblenden:
    NewTip = -1; // Default: Keiner

    if (Sim.Date <= DAYS_WITHOUT_LAPTOP || Sim.LaptopSoldTo != -1) {
        RoomBm.BlitFromT(SchilderBms[gLanguage], 434, 387);
    }

    if ((IsDialogOpen() == 0) && (MenuIsOpen() == 0)) {
        if (gMousePosition.IfIsWithin(493, 52, 570, 224)) {
            SetMouseLook(CURSOR_EXIT, 0, ROOM_SHOP1, 999);
        } else if ((qPlayer.HasItem(ITEM_HUFEISEN) == 0) && qPlayer.TrinkerTrust == FALSE && gMousePosition.IfIsWithin(11, 33, 42, 68)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, 800);
        } else if (gMousePosition.IfIsWithin(448, 327, 542, 439) || gMousePosition.IfIsWithin(396, 395, 542, 439)) {
            if (qPlayer.LaptopQuality == 4 && (qPlayer.HasItem(ITEM_LAPTOP) == 0)) {
                qPlayer.LaptopQuality = 3;
            }

            if (qPlayer.LaptopQuality < 4) {
                NewTip = 0 + qPlayer.LaptopQuality; // Laptop
                SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, 10);
            }
        } else if (gMousePosition.IfIsWithin(399, 55, 492, 301)) {
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, 111);
        } else if (gMousePosition.IfIsWithin(280, 196, 350, 287) && (qPlayer.HasItem(ITEM_MG) == 0) && Sim.Date > 0 &&
                   Sim.Players.Players[PlayerNum].ArabTrust == 0) {
            NewTip = 400;
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, NewTip);
        } else if (gMousePosition.IfIsWithin(329, 247, 395, 309) && (qPlayer.HasItem(ITEM_PRALINEN) == 0) && (qPlayer.HasItem(ITEM_PRALINEN_A) == 0)) {
            NewTip = 801;
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, NewTip);
        } else if (gMousePosition.IfIsWithin(346, 306, 445, 357) && (qPlayer.HasItem(ITEM_FILOFAX) == 0)) {
            NewTip = 500;
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, NewTip);
        } else if (gMousePosition.IfIsWithin(545, 334, 582, 439) && (qPlayer.HasItem(ITEM_HANDY) == 0)) {
            NewTip = 600;
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, NewTip);
        } else if (gMousePosition.IfIsWithin(258, 289, 361, 344) && (qPlayer.HasItem(ITEM_BIER) == 0)) {
            NewTip = 700;
            SetMouseLook(CURSOR_HOT, 0, ROOM_SHOP1, NewTip);
        }

        if (NewTip != -1 && (NewTip != CurrTip)) {
            CurrTip = NewTip;
            TipBm.ReSize(ZettelBm.Size);
            TipBm.BlitFrom(ZettelBm);
            DrawItemTipContents(TipBm, NewTip, &FontSmallBlack, &FontSmallBlack);
        }

        switch (NewTip) {
        // Laptop:
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            RoomBm.BlitFromT(TipBm, 48, 94);
            break;

            // MG:
        case 400:
        case 500: // Terminkalender
        case 600: // Handy
        case 700:
            RoomBm.BlitFromT(TipBm, 48, 94);
            break;

        case 801:
            RoomBm.BlitFromT(TipBm, 68, 94);
            break;
        case -1:
            // no tip
            break;
        default:
            hprintf("DutyFree.cpp: Default case should not be reached.");
            DebugBreak();
        }
    }
    CStdRaum::PostPaint();
    CStdRaum::PumpToolTips();
}

//--------------------------------------------------------------------------------------------
// void CDutyFree::OnLButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void CDutyFree::OnLButtonDown(UINT nFlags, CPoint point) {
    XY RoomPos;
    PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

    DefaultOnLButtonDown();

    // Außerhalb geklickt? Dann Default-Handler!
    if (ConvertMousePosition(point, &RoomPos) == 0) {
        CStdRaum::OnLButtonDown(nFlags, point);
        return;
    }

    if (PreLButtonDown(RoomPos) == 0) {
        if (MouseClickArea == ROOM_SHOP1 && MouseClickId == 999) {
            qPlayer.LeaveRoom();
        } else if (MouseClickArea == ROOM_SHOP1 && MouseClickId == 800) {
            StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 800);
        } else if (MouseClickArea == ROOM_SHOP1 && MouseClickId == 801) {
            StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 3000);
        } else if (MouseClickArea == ROOM_SHOP1 && MouseClickId == 111) {
            if (qPlayer.LaptopVirus == 1) {
                StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2000);
            } else if (qPlayer.LaptopVirus == 2) {
                StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2010);
            } else if (qPlayer.LaptopVirus == 3) {
                StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2020);
            } else {
                StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2);
            }
        } else if (MouseClickArea == ROOM_SHOP1) {
            if (MouseClickId == 10) {
                if (qPlayer.LaptopVirus == 1) {
                    StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2000);
                } else if (qPlayer.LaptopVirus == 2) {
                    StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2010);
                } else if (qPlayer.LaptopVirus == 3) {
                    StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 2020);
                } else if (Sim.Date <= DAYS_WITHOUT_LAPTOP) {
                    // Laptop ist in der ersten 7 Tagen gesperrt:
                    StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 3);
                } else if (Sim.LaptopSoldTo == -1) {
                    if (qPlayer.HasItem(ITEM_LAPTOP) != 0) {
                        qPlayer.DropItem(ITEM_LAPTOP);
                    }
                    if (qPlayer.HasSpaceForItem() != 0) {
                        if (qPlayer.HasItem(ITEM_LAPTOP) == 0) {
                            qPlayer.BuyItem(ITEM_LAPTOP);
                        }

                        SIM::SendSimpleMessage(ATNET_TAKETHING, 0, ITEM_LAPTOP, PlayerNum);

                        SLONG delta = -atoi(StandardTexte.GetS(TOKEN_ITEM, 2000 + qPlayer.LaptopQuality));
                        qPlayer.ChangeMoney(delta, 9999, StandardTexte.GetS(TOKEN_ITEM, 1000 + qPlayer.LaptopQuality));
                        SIM::SendSimpleMessage(ATNET_CHANGEMONEY, 0, Sim.localPlayer, delta, STAT_A_SONSTIGES);

                        qPlayer.DoBodyguardRabatt(atoi(StandardTexte.GetS(TOKEN_ITEM, 2000 + qPlayer.LaptopQuality)));

                        qPlayer.LaptopQuality++;
                        if (Sim.Options.OptionEffekte != 0) {
                            PayFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                        }

                        switch (qPlayer.LaptopQuality) {
                        case 1:
                            qPlayer.LaptopBattery = 40;
                            break;
                        case 2:
                            qPlayer.LaptopBattery = 80;
                            break;
                        case 3:
                            qPlayer.LaptopBattery = 200;
                            break;
                        case 4:
                            qPlayer.LaptopBattery = 1440;
                            break;
                        default:
                            hprintf("DutyFree.cpp: Default case should not be reached.");
                            DebugBreak();
                        }

                        if (qPlayer.LaptopQuality < 4) {
                            StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 4);
                        }

                        Sim.LaptopSoldTo = PlayerNum;
                    }
                } else {
                    StartDialog(TALKER_DUTYFREE, MEDIUM_AIR, 1, Sim.LaptopSoldTo);
                }
            } else if (MouseClickId == 400 || MouseClickId == 500 || MouseClickId == 600 || MouseClickId == 700) {
                if (qPlayer.HasSpaceForItem() != 0) {
                    if (MouseClickId == 400) {
                        qPlayer.BuyItem(ITEM_MG);
                    } else if (MouseClickId == 500) {
                        qPlayer.BuyItem(ITEM_FILOFAX);
                    } else if (MouseClickId == 600) {
                        qPlayer.BuyItem(ITEM_HANDY);
                    } else if (MouseClickId == 700) {
                        qPlayer.BuyItem(ITEM_BIER);
                    }

                    if (Sim.Options.OptionEffekte != 0) {
                        PayFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
                    }

                    SLONG delta = -atoi(StandardTexte.GetS(TOKEN_ITEM, 2000 + MouseClickId));
                    qPlayer.ChangeMoney(delta, 9999, StandardTexte.GetS(TOKEN_ITEM, 1000 + MouseClickId));
                    SIM::SendSimpleMessage(ATNET_CHANGEMONEY, 0, Sim.localPlayer, delta, STAT_A_SONSTIGES);

                    qPlayer.DoBodyguardRabatt(atoi(StandardTexte.GetS(TOKEN_ITEM, 2000 + MouseClickId)));
                }
            }
        } else {
            CStdRaum::OnLButtonDown(nFlags, point);
        }
    }
}

//--------------------------------------------------------------------------------------------
// void CDutyFree::OnRButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void CDutyFree::OnRButtonDown(UINT nFlags, CPoint point) {
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
