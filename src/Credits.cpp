// CCredits.cpp : implementation file
//
#include "StdAfx.h"
#include "Credits.h"
#include "glcredit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Anzahl aus dem Ressource-Dateien:
SLONG MaxCredits;

//////////////////////////////////////////////////////////////////////////////////////////////
// Credits
//////////////////////////////////////////////////////////////////////////////////////////////
CCredits::CCredits(BOOL bHandy, SLONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "", 0) {
    SLONG c = 0;
    CRect rect(0, 0, 640, 480);

    gMouseStartup = TRUE;
    LastTime = AtGetTime();

    pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("credits.gli", RoomPath)), &pGLibCredits, L_LOCMEM);
    Background.ReSize(pGLibCredits, GFX_BACK);
    Left.ReSize(pGLibCredits, GFX_LEFT);
    Right.ReSize(pGLibCredits, GFX_RIGHT);

    for (c = 0; c < 25; c++) {
        TextLines[c].ReSize(640 - 80, 40);
        TextLines[c].Clear(0);
    }

    /*if (!Create(NULL, "CCredits", WS_VISIBLE|WS_CHILD, rect, theApp.m_pMainWnd, 42))
      {
      ::MessageBox (NULL, "Create failed", "ERROR", MB_OK );
      return;
      }
      if (bFullscreen) SetWindowPos (&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);*/

    ScrollPos = -2;

    for (MaxCredits = 0;; MaxCredits++) {
        if (strcmp(StandardTexte.GetS(TOKEN_CREDITS, 1000 + MaxCredits), "EOF") == 0) {
            MaxCredits--;
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------
// CCredits::~CCredits()
//--------------------------------------------------------------------------------------------
CCredits::~CCredits() {
    gMouseStartup = FALSE;
    Background.Destroy();
    Left.Destroy();
    Right.Destroy();
    if ((pGfxMain != nullptr) && (pGLibCredits != nullptr)) {
        pGfxMain->ReleaseLib(pGLibCredits);
    }
    if (pCursor != nullptr) {
        pCursor->SetImage(gCursorBm.pBitmap);
    }
}

//--------------------------------------------------------------------------------------------
// Lädt die Bitmap neu:
//--------------------------------------------------------------------------------------------
void CCredits::ReloadBitmap() {}

/////////////////////////////////////////////////////////////////////////////
// CCredits message handlers

//--------------------------------------------------------------------------------------------
// CCredits::OnPaint
//--------------------------------------------------------------------------------------------
void CCredits::OnPaint() {
    SLONG c = 0;
    SLONG Time = AtGetTime();

    while (Time - LastTime > 0) {
        if ((AtGetAsyncKeyState(ATKEY_CONTROL) / 256) != 0) {
            LastTime += 10;
        } else {
            LastTime += 60;
        }
        OnTimer(0);
    }

    if (bActive != 0) {
        PrimaryBm.BlitFrom(Background, 320 - Background.Size.x / 2, 0);

        for (c = 0; c < 25; c++) {
            SLONG y = 480 - ((ScrollPos - c * 40 + 25 * 40 * 100) % (25 * 40));

            PrimaryBm.BlitFromT(TextLines[c], 40, y);
        }
    }

    PrimaryBm.BlitFromT(Left, 0, 0);
    PrimaryBm.BlitFromT(Right, 640 - Right.Size.x, 0);
}

//--------------------------------------------------------------------------------------------
// CCredits::OnLButtonDown
//--------------------------------------------------------------------------------------------
void CCredits::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) {
    //"Abbrechen"
    Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
// CCredits::OnRButtonDown
//--------------------------------------------------------------------------------------------
void CCredits::OnRButtonDown(UINT /*nFlags*/, CPoint /*point*/) {
    DefaultOnRButtonDown();

    //"Abbrechen"
    Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
// CCredits::OnTimer
//--------------------------------------------------------------------------------------------
void CCredits::OnTimer(UINT /*nIDEvent*/) {
    ScrollPos++;

    if ((ScrollPos % 40) == 0) {
        TextLines[(ScrollPos / 40) % 25].Clear(0);
        if ((ScrollPos / 40) <= MaxCredits && strlen(StandardTexte.GetS(TOKEN_CREDITS, 1000 + ScrollPos / 40)) > 1) {
            TextLines[(ScrollPos / 40) % 25].PrintAt(StandardTexte.GetS(TOKEN_CREDITS, 1000 + ScrollPos / 40), FontBigGrey, TEC_FONT_CENTERED, XY(00, 0),
                                                     XY(640 - 80, 40));
        }
    }
}

//--------------------------------------------------------------------------------------------
// CCredits::OnChar
//--------------------------------------------------------------------------------------------
void CCredits::OnChar(UINT key, UINT /*nRepCnt*/, UINT /*nFlags*/) {
    //"Abbrechen"
    if (key != ATKEY_CONTROL) {
        Sim.Gamestate = GAMESTATE_BOOT;
    }
}

//--------------------------------------------------------------------------------------------
// CCredits::OnKeyDown
//--------------------------------------------------------------------------------------------
void CCredits::OnKeyDown(UINT key, UINT /*nRepCnt*/, UINT /*nFlags*/) {
    //"Abbrechen"
    if (key != ATKEY_CONTROL) {
        Sim.Gamestate = GAMESTATE_BOOT;
    }
}

//--------------------------------------------------------------------------------------------
// BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL CCredits::OnSetCursor(void *pWnd, UINT nHitTest, UINT message) { return (FrameWnd->OnSetCursor(pWnd, nHitTest, message)); }

//--------------------------------------------------------------------------------------------
// void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void CCredits::OnMouseMove(UINT nFlags, CPoint point) { FrameWnd->OnMouseMove(nFlags, point); }
