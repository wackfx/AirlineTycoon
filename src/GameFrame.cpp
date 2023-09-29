//============================================================================================
// GameFrame.cpp : implementation file
//============================================================================================
// Link: "Gameframe.h"
//============================================================================================
#include "StdAfx.h"
#include "AtNet.h"
#include "CVideo.h"
#include "Intro.h"
#include "NewGamePopup.h" //Fenster zum Wahl der Gegner und der Spielstärke
#include "Outro.h"
#include "Synthese.h"
#include "glpause.h"

#include "SbLib.h"
#include "network.h"
extern SBNetwork gNetwork;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char FpsMsg[50];

SB_CColorFX ColorFX;
SLONG OptionsShortcut = -1;

/*SB_CBitmapCore *pSpeedBm=NULL;*/

XY MouseCursorOffset = XY(0, 0);

SLONG CBench::GetMissing() {
    return (SLONG(GameTime) - SLONG(BlitTime) - SLONG(FXTime) - SLONG(InitTime) - SLONG(DiskTime) - SLONG(TextTime) - SLONG(ClearTime) - SLONG(SortTime) -
            SLONG(WalkTime) - SLONG(AdminTime) - SLONG(WaitTime) - SLONG(FlipTime) - SLONG(KITime) - SLONG(MiscTime1) - SLONG(MiscTime2));
}

void CBench::Report() {
    GameTime.Stop();
    hprintf("CBench - Report");
    hprintf("---------------------------------------");
    hprintf(" GameTime  : %15s.000 cycles", Insert1000erDots(SLONG(GameTime)).c_str());
    hprintf(" BlitTime  : %15s.000 cycles = %.2f %% ", Insert1000erDots(SLONG(BlitTime)).c_str(),
            SLONG(BlitTime) * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    hprintf(" FlipTime  : %15s.000 cycles = %.2f %% ", Insert1000erDots(SLONG(FlipTime)).c_str(),
            SLONG(FlipTime) * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    hprintf(" FXTime    : %15s.000 cycles", Insert1000erDots(SLONG(FXTime)).c_str());
    hprintf(" InitTime  : %15s.000 cycles", Insert1000erDots(SLONG(InitTime)).c_str());
    hprintf(" DiskTime  : %15s.000 cycles", Insert1000erDots(SLONG(DiskTime)).c_str());
    hprintf(" TextTime  : %15s.000 cycles", Insert1000erDots(SLONG(TextTime)).c_str());
    hprintf(" ClearTime : %15s.000 cycles", Insert1000erDots(SLONG(ClearTime)).c_str());
    hprintf(" SortTime  : %15s.000 cycles = %.2f %% ", Insert1000erDots(SLONG(SortTime)).c_str(),
            SLONG(SortTime) * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    hprintf(" WalkTime  : %15s.000 cycles = %.2f %% ", Insert1000erDots(SLONG(WalkTime)).c_str(),
            SLONG(WalkTime) * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    hprintf(" KITime    : %15s.000 cycles = %.2f %% ", Insert1000erDots(SLONG(KITime)).c_str(), SLONG(KITime) * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    hprintf(" WaitTime  : %15s.000 cycles", Insert1000erDots(SLONG(WaitTime)).c_str());
    hprintf(" AdminTime : %15s.000 cycles = %.2f %% ", Insert1000erDots(SLONG(AdminTime)).c_str(),
            SLONG(AdminTime) * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    hprintf(" MiscTime1 : %15s.000 cycles", Insert1000erDots(SLONG(MiscTime1)).c_str());
    hprintf(" MiscTime2 : %15s.000 cycles", Insert1000erDots(SLONG(MiscTime2)).c_str());
    hprintf("---------------------------------------");
    hprintf(" Missing   : %15s.000 cycles = %.2f %% ", Insert1000erDots(GetMissing()).c_str(), GetMissing() * 100.0 / (SLONG(GameTime) - SLONG(InitTime)));
    GameTime.Start();
}

extern CTakeOffApp *pTakeOffApp;

/////////////////////////////////////////////////////////////////////////////////////////////
// GameFrame
/////////////////////////////////////////////////////////////////////////////////////////////

void CheatSound();

void CheatSound() {
    /*SBFX *fx=new SBFX;

      gUniversalFx.ReInit ("cheat.raw");

      SBFX *Elements[100];
      for (SLONG c=0; c<2; c++)
      Elements[c]=&gUniversalFx;

      fx->Fusion ((const SBFX**)Elements, 2);
      fx->Play (0, Sim.Options.OptionEffekte*100/7); */

    /*BUFFER_V<SBFX> Fxs;

      gUniversalFx.ReInit ("test.raw");
      gUniversalFx.Tokenize (Fxs);*/

    /*Fxs[0].Play (0, Sim.Options.OptionEffekte*100/7);
      Sleep (2000);
      Fxs[1].Play (0, Sim.Options.OptionEffekte*100/7);
      Sleep (2000); */

    /*SBFX *fx=new SBFX;

      SBFX *Elements[100];
      Elements[0]=&Fxs[0];
      Elements[1]=&Fxs[1];

      fx->Fusion ((const SBFX**)Elements, 2);
      fx->Play (0, Sim.Options.OptionEffekte*100/7);*/

    gUniversalFx.ReInit("cheat.raw");
    gUniversalFx.Play(0, Sim.Options.OptionEffekte * 100 / 7);
}

void MessagePump() {

    SDL_StartTextInput();
    while (SDL_PollEvent(&FrameWnd->Mess) != 0) {
        FrameWnd->ProcessEvent(FrameWnd->Mess);
    }
}

// LPDIRECTDRAWSURFACE FrontSurf=NULL;

/*void Copier (void *)
  {
  SLONG c=0;
  SLONG t, t2;

  t=AtGetTime();

  while (1)
  {
  if (pSpeedBm && FrontSurf)
  {
  RECT srcRect = { 0, 0, 640, 480 };
//VGACriticalSection.Lock();
FrontSurf->Blt(&srcRect, pSpeedBm->GetSurface(), &srcRect, DDBLT_WAIT|DDBLT_ASYNC, NULL);
//FrontSurf->Blt(gMousePosition.x, gMousePosition.y, gCursorBm.pBitmap->GetSurface(), &CRect(0,0,32,32), DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY|DDBLTFAST_ASYNC);
//FrontSurf->Blt(&CRect(50+c%100, 100, 50+32+c%100, 132), gCursorBm.pBitmap->GetSurface(), &CRect(0,0,32,32), DDBLT_WAIT|DDBLT_KEYSRC|DDBLT_ASYNC, NULL);
//VGACriticalSection.Unlock();
c++;

if ((c%100)==0)
{
t2=AtGetTime();
hprintf ("%f Frames pro Sekunde", 100000.0/(t2-t));
t=t2;
}

Sleep(20);
}
else Sleep(1000);
}
}*/

void GameFrame::UpdateWindow() const {
    // windowed size:
    SLONG width = 640;
    SLONG height = 480;

    SDL_DisplayMode DM;
    SDL_GetDesktopDisplayMode(0, &DM);
    SLONG screenWidth = DM.w;
    SLONG screenHeight = DM.h;

    switch (Sim.Options.OptionFullscreen) {
    case (0): // Fullscreen
        SDL_SetWindowSize(m_hWnd, screenWidth, screenHeight);
        SDL_SetWindowFullscreen(m_hWnd, SDL_TRUE);
        break;
    case (1): // Windowed
        SDL_SetWindowFullscreen(m_hWnd, 0);
        SDL_SetWindowResizable(m_hWnd, SDL_TRUE);
        SDL_SetWindowBordered(m_hWnd, SDL_TRUE);
        SDL_SetWindowSize(m_hWnd, width, height);
        SDL_SetWindowPosition(m_hWnd, screenWidth / 2 - width / 2, screenHeight / 2 - height / 2);
        break;
    case (2): // Borderless Fullscreen
        SDL_SetWindowFullscreen(m_hWnd, SDL_FALSE);
        SDL_SetWindowResizable(m_hWnd, SDL_FALSE);
        SDL_SetWindowBordered(m_hWnd, SDL_FALSE);
        SDL_SetWindowPosition(m_hWnd, 0, 0);
        SDL_SetWindowSize(m_hWnd, screenWidth, screenHeight);
        break;
    default:
        hprintf("GameFrame.cpp: Default case should not be reached.");
        DebugBreak();
    }

    UpdateFrameSize();
}

constexpr SLONG getAspectWidth(SLONG height) { return static_cast<SLONG>(static_cast<float>(height) * (640.0f / 480.0f)); }

void GameFrame::UpdateFrameSize() const {
    SLONG screenW = 0, screenH = 0;
    SDL_GetWindowSize(m_hWnd, &screenW, &screenH);
    SDL_RenderSetLogicalSize(lpDD, screenW, screenH);

    if (Sim.Options.OptionKeepAspectRatio == 0) {
        PrimaryBm.PrimaryBm.SetTarget(XY{0, 0}, XY{screenW, screenH});
    } else {
        const SLONG aspectWidth = getAspectWidth(screenH);

        const SLONG x = (screenW - aspectWidth) / 2;
        PrimaryBm.PrimaryBm.SetTarget(XY{x, 0}, XY{aspectWidth, screenH});
    }
}

void GameFrame::TranslatePointToGameSpace(CPoint *p) const {
    SLONG screenW = 0;
    SLONG screenH = 0;
    SDL_GetRendererOutputSize(SDL_GetRenderer(m_hWnd), &screenW, &screenH);

    FLOAT x = static_cast<FLOAT>(p->x);
    FLOAT y = static_cast<FLOAT>(p->y);

    if (Sim.Options.OptionKeepAspectRatio == 1) {
        const SLONG aspectWidth = getAspectWidth(screenH);
        x -= static_cast<FLOAT>(screenW - aspectWidth) / 2.0f;

        screenW = aspectWidth;
    }

    x /= static_cast<FLOAT>(screenW);
    x *= 640;
    y /= static_cast<FLOAT>(screenH);
    y *= 480;

    p->x = static_cast<SLONG>(x);
    p->y = static_cast<SLONG>(y);
}

void GameFrame::TranslatePointToScreenSpace(SLONG &x, SLONG &y) const {
    SLONG screenW = 0;
    SLONG screenH = 0;
    SDL_GetRendererOutputSize(SDL_GetRenderer(m_hWnd), &screenW, &screenH);

    const SLONG aspectWidth = getAspectWidth(screenH);
    const SLONG origScreenW = screenW;
    if (Sim.Options.OptionKeepAspectRatio == 1) {
        screenW = aspectWidth;
    }

    FLOAT _x = static_cast<FLOAT>(x);
    FLOAT _y = static_cast<FLOAT>(y);
    _x /= 640;
    _x *= static_cast<FLOAT>(screenW);
    _y /= 480;
    _y *= static_cast<FLOAT>(screenH);

    
    if (Sim.Options.OptionKeepAspectRatio == 1) {
        _x += static_cast<FLOAT>(origScreenW - aspectWidth) / 2.0f;
    }

    x = static_cast<SLONG>(_x);
    y = static_cast<SLONG>(_y);
}

//--------------------------------------------------------------------------------------------
// Konstruktor:
//--------------------------------------------------------------------------------------------
GameFrame::GameFrame() {
    pGLibPause = nullptr;
    PauseFade = 0;

    if (lpDD == nullptr) {
        MB();
        SDL_Delay(100);
        MB();
        SDL_Delay(100);
        MB();
    }

    const XYZ resolution = DetectCurrentDisplayResolution();
    if (resolution.x <= 640 || resolution.y <= 480) {
        bFullscreen = TRUE;
    }

    // Base backup screen size - only used in windowed mode
    SLONG width = 640;
    SLONG height = 480;

    if (!static_cast<bool>(bFullscreen) || Sim.Options.OptionFullscreen == 0 || Sim.Options.OptionFullscreen == 2) {
        SDL_DisplayMode DM;
        SDL_GetDesktopDisplayMode(0, &DM);
        width = DM.w;
        height = DM.h;
    }
    CRect rect(0, 0, width, height);

    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    SDL_Window *h = nullptr;

    switch (Sim.Options.OptionFullscreen) {
    case (0): // Fullscreen
        h = SDL_CreateWindow("Airline Tycoon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, rect.Width(), rect.Height(), SDL_WINDOW_FULLSCREEN);
        break;
    case (1): // Windowed
        h = SDL_CreateWindow("Airline Tycoon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, rect.Width(), rect.Height(), SDL_WINDOW_RESIZABLE);
        break;
    case (2): // Borderless Fullscreen
        h = SDL_CreateWindow("Airline Tycoon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, rect.Width(), rect.Height(), SDL_WINDOW_BORDERLESS);
        break;
    default:
        hprintf("GameFrame.cpp: Default case should not be reached.");
        DebugBreak();
    }

    if (h == nullptr) {
        SimpleMessageBox(MESSAGEBOX_ERROR, "ERROR", "CreateWindow failed");
        return;
    }

    m_hWnd = h;

    pGfxMain = new GfxMain(lpDD);

    PrimaryBm.ReSize(h, bFullscreen, XY(640, 480));
    PrimaryBm.ReSizePartB(h, bFullscreen, XY(640, 480));
    pCursor = new SB_CCursor(&PrimaryBm.PrimaryBm);
    PrimaryBm.PrimaryBm.AssignCursor(pCursor);

    bitmapMain = new SB_CBitmapMain(lpDD);

    pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("glbasis.gli", GliPath)), &pGLibBasis, L_LOCMEM);
    gCursorBm.ReSize(pGLibBasis, GFX_CURSOR, CREATE_VIDMEM);
    gCursorLBm.ReSize(pGLibBasis, GFX_CURSORL, CREATE_VIDMEM);
    gCursorRBm.ReSize(pGLibBasis, GFX_CURSORR, CREATE_VIDMEM);
    gCursorHotBm.ReSize(pGLibBasis, GFX_CURSORH, CREATE_VIDMEM);
    gCursorExitBms.ReSize(pGLibBasis, "EXIT01", 12, CREATE_VIDMEM);
    gCursorMoveHBm.ReSize(pGLibBasis, GFX_CURSORV, CREATE_VIDMEM);
    gCursorMoveVBm.ReSize(pGLibBasis, GFX_CURSORW, CREATE_VIDMEM);
    gCursorSandBm.ReSize(pGLibBasis, GFX_CURSORS, CREATE_VIDMEM);
    // MP: wizzardmaker:
    // gCursorNoBm.ReSize (10,10);
    gCursorNoBm.ReSize(XY(10, 10), CREATE_VIDMEM);
    gCursorNoBm.FillWith(0);

    CRect cliprect(2, 2, 638, 478);
    // if (bFullscreen) ClipCursor (&cliprect);

    if (gUseWindowsMouse == 0) {
        pCursor->SetImage(gCursorBm.pBitmap);
    }

    ColorFX.ReInit(SB_COLORFX_FADE, 8, gCursorSandBm.pBitmap);

    UpdateWindow();

    /*_beginthread (Copier, 0, NULL);

      if (!gUseWindowsMouse)
      {
      SetCursor(NULL);
      if (!bCursorCaptured) SetCapture();
      bCursorCaptured = TRUE;
      } */

    MouseLook = CURSOR_NORMAL;
}

//--------------------------------------------------------------------------------------------
// Destruktor:
//--------------------------------------------------------------------------------------------
GameFrame::~GameFrame() {
    SSE::SetMusicCallback(nullptr);
    // ClipCursor (NULL);

    if (pCursor != nullptr) {
        delete pCursor;
        pCursor = nullptr;
    }

    TextBricks.ReSize(0);
    Sim.Players.Players.ReSize(0);
    PlaneTypes.ReSize(0);
    Clans.ReSize(0);
    Bricks.ReSize(0);

    // SBBM:
    gNotepadButtonL.Destroy();
    gNotepadButtonM.Destroy();
    gNotepadButtonR.Destroy();
    gBlendBm.Destroy();
    gToolTipBm.Destroy();
    gCursorBm.Destroy();
    gCursorLBm.Destroy();
    gCursorRBm.Destroy();
    gCursorHotBm.Destroy();
    gCursorExitBms.Destroy();
    gCursorMoveHBm.Destroy();
    gCursorMoveVBm.Destroy();
    gCursorSandBm.Destroy();
    gCursorFeetBms[0].Destroy();
    gCursorFeetBms[1].Destroy();
    gDialogBarBm.Destroy();
    gPostItBms.Destroy();
    gCrossBm.Destroy();
    gAusrufBm.Destroy();
    gLockBm.Destroy();
    gCityMarkerBm.Destroy();
    gCursorNoBm.Destroy();

    // SBBMS:
    gTutoriumBms.Destroy();
    gRepeatMessageBms.Destroy();
    PauseBm.Destroy();
    gItemBms.Destroy();
    gToolTipBms.Destroy();
    gUniversalPlaneBms.Destroy();
    RuneBms.Destroy();
    StatusLineBms.Destroy();
    gClockBms.Destroy();
    SprechblasenBms.Destroy();
    XBubbleBms.Destroy();
    gZettelBms.Destroy();
    LogoBms.Destroy();
    SmallLogoBms.Destroy();
    TinyLogoBms.Destroy();
    MoodBms.Destroy();
    SmileyBms.Destroy();
    FlugplanBms.Destroy();
    gSmokeBms.Destroy();
    gStenchBms.Destroy();
    gInfoBms.Destroy();

    // BUFFER_V<...>
    BeraterBms.ReSize(0);
    FlugplanIconBms.ReSize(0);

    if (bitmapMain != nullptr) {
        delete bitmapMain;
        bitmapMain = nullptr;
    }

    if (pGfxMain != nullptr) {
        delete pGfxMain;
        pGfxMain = nullptr;
    }

    if (m_hWnd != nullptr) {
        SDL_DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }

    bLeaveGameLoop = TRUE;
    Hdu.HercPrintf(0, "logging ends..");
}

void GameFrame::ProcessEvent(const SDL_Event &event) const {
    switch (event.type) {
    case SDL_WINDOWEVENT: {
        if (event.window.windowID == SDL_GetWindowID(FrameWnd->m_hWnd)) {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                SDL_Quit();
                Sim.Gamestate = GAMESTATE_QUIT;
                bLeaveGameLoop = TRUE;
            } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                UpdateFrameSize();
            } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                FrameWnd->OnActivateApp(TRUE, 0);
                FrameWnd->OnSetCursor(nullptr, HTCLIENT, 0);
            } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                // FrameWnd->OnActivateApp(FALSE, 0);
                // FrameWnd->OnSetCursor(NULL, HTNOWHERE, 0);
            } else if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                FrameWnd->OnSetCursor(nullptr, HTCLIENT, 0);
            } else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
                FrameWnd->OnSetCursor(nullptr, HTNOWHERE, 0);
            }
        }
    } break;
    case SDL_MOUSEMOTION: {
        CPoint pos = CPoint(event.motion.x, event.motion.y);
        TranslatePointToGameSpace(&pos);
        FrameWnd->OnMouseMove(0, pos);
    } break;
    case SDL_TEXTINPUT:
        FrameWnd->OnChar(event.text.text[0], 0, 0);

        FrameWnd->OnKeyDown(event.text.text[0], 0, InputFlags::FromTextInput);
        break;
    case SDL_KEYDOWN:
    {
		//UINT nFlags = event.key.keysym.scancode | ((SDL_GetModState() & KMOD_LALT) << 5);
		FrameWnd->OnKeyDown(KeycodeToUpper(event.key.keysym.sym), event.key.repeat, InputFlags::None);
    } break;
    case SDL_MOUSEBUTTONDOWN:
    {
		CPoint pos = CPoint(event.button.x, event.button.y);
		TranslatePointToGameSpace(&pos);
		if (event.button.button == SDL_BUTTON_LEFT) {
			if (event.button.clicks == 2){
				FrameWnd->OnLButtonDblClk(WM_LBUTTONDBLCLK, pos);
            } else {
				FrameWnd->OnLButtonDown(WM_LBUTTONDOWN, pos);
            }
		} else if (event.button.button == SDL_BUTTON_RIGHT){
			FrameWnd->OnRButtonDown(WM_RBUTTONDOWN, pos);
        }
    } break;
    case SDL_KEYUP:
    {
		FrameWnd->OnKeyUp(event.key.keysym.sym, event.key.repeat, 0);
    } break;
    case SDL_MOUSEBUTTONUP:
    {
		CPoint pos = CPoint(event.button.x, event.button.y);
		TranslatePointToGameSpace(&pos);
		if (event.button.button == SDL_BUTTON_LEFT) {
			FrameWnd->OnLButtonUp(WM_LBUTTONUP, pos);
		} else if (event.button.button == SDL_BUTTON_RIGHT) {
			FrameWnd->OnRButtonUp(WM_RBUTTONUP, pos);
		}
    } break;
    default:
        break;
    }
}

void GameFrame::Invalidate() {
    CStdRaum *w = nullptr;
    SLONG c = 0;

    if (TopWin != nullptr) {
        TopWin->OnPaint();
    } else {
        for (c = 0; c < Sim.Players.AnzPlayers; c++) {
            w = Sim.Players.Players[c].LocationWin;

            if (w != nullptr) {
                // if (XY(point).IfIsWithin (w->WinP1.x, w->WinP1.y, w->WinP2.x, w->WinP2.y))
                w->OnPaint();
            }
        }
    }

    OnPaint();
}

//--------------------------------------------------------------------------------------------
// Die Nachricht an alle Sub-Fenster weiterleiten::
//--------------------------------------------------------------------------------------------
void GameFrame::RePostMessage(const CPoint &Pos) const {
    CStdRaum *w = nullptr;
    SLONG c = 0;

    // An alle direkten Sub-Windows schicken:
    if (TopWin != nullptr) {
        TopWin->ProcessEvent(Mess, Pos);
    } else {
        for (c = 0; c < Sim.Players.AnzPlayers; c++) {
            w = Sim.Players.Players[c].LocationWin;

            if (w != nullptr) {
                // if (XY(point).IfIsWithin (w->WinP1.x, w->WinP1.y, w->WinP2.x, w->WinP2.y))
                w->ProcessEvent(Mess, Pos);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GameFrame message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void GameFrame::OnSysKeyDown(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/) {}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void GameFrame::OnSysKeyUp(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/) {}

//--------------------------------------------------------------------------------------------
// Prepares the fade-Bitmap
//--------------------------------------------------------------------------------------------
void GameFrame::PrepareFade() {
    gBlendBm.ReSize(PrimaryBm.Size);

    // Erklärung, bei der Kopie dieses Code-Fragments...
    if (bLeaveGameLoop == 0) {
        SB_CBitmapKey SrcKey(PrimaryBm.PrimaryBm);
        SB_CBitmapKey TgtKey(*gBlendBm.pBitmap);

        if (SrcKey.Bitmap != nullptr) {
            for (SLONG y = 0; y < 480; y++) {
                memcpy(static_cast<char *>(TgtKey.Bitmap) + y * TgtKey.lPitch, static_cast<char *>(SrcKey.Bitmap) + y * SrcKey.lPitch, 640 * 2);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// GameFrame::OnPaint()
//--------------------------------------------------------------------------------------------
void GameFrame::OnPaint() {
    static DWORD LastTime = 0xffffffff;
    static SLONG LastMouseLook = -1;
    static SLONG LastFeet = -1;
    DWORD Time = AtGetTime();
    SLONG c = 0;

    // Ggf. die Cursor bewegen:
    if (Time - LastTime > 30) {
        for (c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
            if (Sim.Players.Players[c].Owner == 0) {
                if (Sim.Players.Players[c].CursorPos == CPoint(-1, -1)) {
                    Sim.Players.Players[c].CursorPos = CPoint((Sim.Players.Players[c].WinP1 + Sim.Players.Players[c].WinP2) / SLONG(2));
                }
                if (Sim.Players.Players[c].CursorPos.x < Sim.Players.Players[c].WinP1.x) {
                    Sim.Players.Players[c].CursorPos.x = Sim.Players.Players[c].WinP1.x;
                }
                if (Sim.Players.Players[c].CursorPos.y < Sim.Players.Players[c].WinP1.y) {
                    Sim.Players.Players[c].CursorPos.y = Sim.Players.Players[c].WinP1.y;
                }
                if (Sim.Players.Players[c].CursorPos.x > Sim.Players.Players[c].WinP2.x) {
                    Sim.Players.Players[c].CursorPos.x = Sim.Players.Players[c].WinP2.x;
                }
                if (Sim.Players.Players[c].CursorPos.y > Sim.Players.Players[c].WinP2.y) {
                    Sim.Players.Players[c].CursorPos.y = Sim.Players.Players[c].WinP2.y;
                }
            }
        }

        LastTime = Time;
    }

    if ((bActive != 0) && ((Sim.bPause == 0) || PauseFade == 0)) {
        TXY<SLONG> rcWindow;

        if (bCursorCaptured != 0) {
            // Administrate ToolTip
            if (::ToolTipId != ToolTipNewId) {
                ::ToolTipId = ToolTipNewId;
                ::ToolTipPos = ToolTipNewPos;
                ::ToolTipPos.y += 32;
                if (::ToolTipPos.y > 460) {
                    ::ToolTipPos.y -= 64;
                }
                ToolTipTimer = AtGetTime();
                ToolTipState = FALSE;
            } else {
                if (AtGetTime() - ToolTipTimer > 5000) {
                    ToolTipState = FALSE;
                } else if (ToolTipState == FALSE && AtGetTime() - ToolTipTimer > 600) {
                    gToolTipBm.ReSize(500, 25);

                    CString str;

                    if (ToolTipId > 0) {
                        str = StandardTexte.GetS(TOKEN_TOOLTIP, ToolTipId);
                        if (str == "Triebwerke auswählen") {
                            str = "Scheinwerfer auswählen";
                        }
                    } else if (ToolTipId < 0) {
                        str = ToolTipString;
                    }

                    SLONG SizeX = 640;

                    while (SizeX > 0) {
                        SLONG Size = gToolTipBm.TryPrintAt(str, FontBigGrey, TEC_FONT_LEFT, 0, 0, SizeX, 25);
                        if (Size >= 0 && Size < 25) {
                            SizeX -= 64;
                        } else {
                            break;
                        }
                    }

                    while (SizeX < 640) {
                        SLONG Size = gToolTipBm.TryPrintAt(str, FontBigGrey, TEC_FONT_LEFT, 0, 0, SizeX, 25);
                        if (Size < 0 || Size >= 25) {
                            SizeX += 8;
                        } else {
                            break;
                        }
                    }

                    SizeX = (SizeX + 26 + 4) / 28 * 28;

                    if ((SizeX & 1) != 0) {
                        SizeX++;
                    }
                    gToolTipBm.ReSize(SizeX + 2, 28);
                    gToolTipBm.FillWith(0);

                    for (c = 0; c < SizeX; c += 28) {
                        if (c == 0) {
                            gToolTipBm.BlitFrom(gToolTipBms[0], c, 0);
                        } else {
                            gToolTipBm.BlitFrom(gToolTipBms[1], c, 0);
                        }
                    }
                    gToolTipBm.BlitFrom(gToolTipBms[2], SizeX - 28, 0);

                    gToolTipBm.PrintAt(str, FontBigGrey, TEC_FONT_CENTERED, 0, 2, SizeX, 28);
                    Limit(SLONG(0), ToolTipPos.x, 639 - SizeX);

                    ToolTipState = TRUE;
                }
            }

            if ((ToolTipState != 0) && (ToolTipId != 0)) {
                SLONG px = gMousePosition.x + 16 - gToolTipBm.Size.x / 2;
                SLONG py = 0;

                UpdateStatusBar();

                if (px < 2) {
                    px = 2;
                }
                if (px > 639 - gToolTipBm.Size.x) {
                    px = 639 - gToolTipBm.Size.x;
                }

                if (gMousePosition.y < 439) {
                    py = gMousePosition.y + 32;
                } else {
                    py = gMousePosition.y;
                    if (gMousePosition.x + 32 + gToolTipBm.Size.x < 630) {
                        px = gMousePosition.x + 32;
                    } else {
                        px = gMousePosition.x - 5 - gToolTipBm.Size.x;
                    }

                    if (py > 480 - 28) {
                        py = 480 - 28;
                    }
                }

                ColorFX.BlitWhiteTrans(FALSE, gToolTipBm.pBitmap, &PrimaryBm.PrimaryBm, XY(px, py));
            }

            if (gUseWindowsMouse == 0) {
                if (((MouseWait != 0) || (gMouseStartup != 0)) && (pCursor != nullptr) && nOptionsOpen == 0) {
                    if (gMouseStartup != 0) {
                        pCursor->SetImage(gCursorNoBm.pBitmap);
                    } else if (LastMouseLook != 99) {
                        MouseCursorOffset = XY(16, 16);
                        pCursor->SetImage(gCursorSandBm.pBitmap);
                        LastMouseLook = 99;
                    }
                } else {
                    if ((MouseLook != LastMouseLook || LastFeet != gShowCursorFeet || MouseLook == CURSOR_EXIT || gShowCursorFeet != -1) &&
                        (pCursor != nullptr)) {
                        LastMouseLook = MouseLook;
                        LastFeet = gShowCursorFeet;

                        MouseCursorOffset = XY(0, 0);

                        if (MouseLook == CURSOR_NORMAL || (Sim.bPause != 0)) {
                            if (gShowCursorFeet == 1) {
                                pCursor->SetImage(gCursorFeetBms[0][static_cast<SLONG>(Sim.TickerTime) % gCursorFeetBms[0].AnzEntries()].pBitmap);
                                MouseCursorOffset = XY(24, 13);
                            } else if (gShowCursorFeet == 0) {
                                pCursor->SetImage(gCursorFeetBms[1][static_cast<SLONG>(Sim.TickerTime) % gCursorFeetBms[1].AnzEntries()].pBitmap);
                                MouseCursorOffset = XY(24, 13);
                            } else {
                                pCursor->SetImage(gCursorBm.pBitmap);
                            }

                            if (gShowCursorFeet != -1) {
                                if (gMousePosition.y > 380 && (Sim.Players.Players[Sim.localPlayer].LocationWin != nullptr)) {
                                    (Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount = 3;
                                }
                            }
                        } else if (MouseLook == CURSOR_HOT) {
                            pCursor->SetImage(gCursorHotBm.pBitmap);
                        } else if (MouseLook == CURSOR_EXIT) {
                            pCursor->SetImage(gCursorExitBms[SLONG((Sim.TickerTime / 2) % 12)].pBitmap);
                            MouseCursorOffset = XY(15, 12);
                        } else if (MouseLook == CURSOR_LEFT) {
                            pCursor->SetImage(gCursorLBm.pBitmap);
                            MouseCursorOffset = XY(2, 16);
                        } else if (MouseLook == CURSOR_RIGHT) {
                            pCursor->SetImage(gCursorRBm.pBitmap);
                            MouseCursorOffset = XY(29, 16);
                        } else if (MouseLook == CURSOR_MOVE_H) {
                            pCursor->SetImage(gCursorMoveHBm.pBitmap);
                            MouseCursorOffset = XY(16, 0);
                        } else if (MouseLook == CURSOR_MOVE_V) {
                            pCursor->SetImage(gCursorMoveVBm.pBitmap);
                            MouseCursorOffset = XY(0, 16);
                        }
                        SLONG _x = gMousePosition.x;
                        SLONG _y = gMousePosition.y;
                        TranslatePointToScreenSpace(_x, _y);
                        pCursor->MoveImage(_x - MouseCursorOffset.x, _y - MouseCursorOffset.y);
                    }
                }
            }
        }

        SDL_GetWindowPosition(m_hWnd, &rcWindow.x, &rcWindow.y);
        PrimaryBm.Flip(rcWindow.x, rcWindow.y, TRUE);
    }
    if ((bActive != 0) && (Sim.bPause != 0)) {
        TXY<SLONG> rcWindow;

        if (pGLibPause == nullptr) {
            pGfxMain->LoadLib(const_cast<char *>((LPCTSTR)FullFilename("pause.gli", RoomPath)), &pGLibPause, L_LOCMEM);
            PauseBm.ReSize(pGLibPause, GFX_PAUSE);
        }

        if (PauseFade == 0) {
            if (Sim.localPlayer != -1 && (Sim.Players.Players[Sim.localPlayer].LocationWin != nullptr)) {
                (Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount = 32;
            }
            gBlendBm.ReSize(PrimaryBm.Size);

            // Definitiv extrem krank: Wenn man per FastBlt Daten aus der Grafikkarte
            // ins System-RAM kopiert wird das ganze Game 50% langsamer. Vermutlich
            // hat DirectX Probleme aus irgend einem Grund und verschiebt einige Bitmaps
            // aus dem Video ins System RAM. Und das bremst AT aus. Also verschieben wir
            // per memcpy, dann geht alles wunderbar. Der Bug trat übrigens nur im Fenster-
            // Modus auf. Vielleicht, weil da weniger RAM frei ist.
            {
                SB_CBitmapKey SrcKey(PrimaryBm.PrimaryBm);
                SB_CBitmapKey TgtKey(*gBlendBm.pBitmap);

                for (SLONG y = 0; y < 480; y++) {
                    memcpy(static_cast<char *>(TgtKey.Bitmap) + y * TgtKey.lPitch, static_cast<char *>(SrcKey.Bitmap) + y * SrcKey.lPitch, 640 * 2);
                }
            }

            PauseFade++;
        } else if (PauseFade < 8) {
            UpdateStatusBar();
            if (Sim.Options.OptionBlenden != 0) {
                ColorFX.ApplyOn2(8 - PauseFade, gBlendBm.pBitmap, PauseFade, PauseBm.pBitmap, &PrimaryBm.PrimaryBm);
            } else {
                PrimaryBm.BlitFrom(PauseBm);
            }

            PauseFade++;
        } else {
            PrimaryBm.BlitFrom(PauseBm);
        }

        SDL_GetWindowPosition(m_hWnd, &rcWindow.x, &rcWindow.y);
        PrimaryBm.Flip(rcWindow.x, rcWindow.y, TRUE);
    }
}

//--------------------------------------------------------------------------------------------
// GameFrame::OnEraseBkgnd(CDC* pDC):
//--------------------------------------------------------------------------------------------
BOOL GameFrame::OnEraseBkgnd(void * /*pDC*/) {
    // ReferTo (pDC);
    return FALSE;
}

//--------------------------------------------------------------------------------------------
// Wird beim Taskwechsel aufgerufen:
//--------------------------------------------------------------------------------------------
void GameFrame::OnActivateApp(BOOL bActive, DWORD /*hTask*/) {
    if (::bActive != bActive) {
        ::bActive = bActive;

        if (bActive != 0) {

            CRect rect(2, 2, 638, 478);

            if (Sim.bNetwork != 0) {
                SIM::SendSimpleMessage(ATNET_ACTIVATEAPP, 0, -1, Sim.localPlayer);
            }

            // if (bFullscreen) ClipCursor (&rect);

            // Re-Aktiviere TakeOff (ignoriert die Message beim normalen StartUp):
            if (gItemBms.AnzEntries() > 0) {
                // Großalarm, wegen Fullscreen: Zahlreiche Bitmaps müssen restauriert werden:
                // PrimaryBm.PrimaryBm.GetPrimarySurface()->Restore();

                // if (PrimaryBm.PrimaryBm.GetLastPage())
                //   PrimaryBm.PrimaryBm.GetLastPage()->Restore();

                // Und die Bricks:
                if (Sim.Jahreszeit != -1) {
                    Bricks.RestoreBricks();
                    AIRPORT::UpdateStaticDoorImage();
                }
            }

            if (Sim.Players.Players[Sim.localPlayer].LocationWin != nullptr) {
                (Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount = 3;
            }

            Pause(false); // AG:
            SDL_RaiseWindow(m_hWnd);

            Invalidate();
        } else {
            FlushTalkers();

            if (Sim.bNetwork != 0) {
                SIM::SendSimpleMessage(ATNET_ACTIVATEAPP, 0, 1, Sim.localPlayer);
            }

            // if (bFullscreen) ClipCursor (NULL);
            Pause(true); // AG:
        }
    }
}

//--------------------------------------------------------------------------------------------
// BOOL GameFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) :
//--------------------------------------------------------------------------------------------
BOOL GameFrame::OnSetCursor(void * /*pWnd*/, UINT nHitTest, UINT /*message*/) {
    if (gUseWindowsMouse == 0) {
        // Keinen Cursor anzeigen:
        if (nHitTest == HTCLIENT && (bActive != 0)) {
            // if (!bCursorCaptured) SetCapture();
            bCursorCaptured = TRUE;
            if (pCursor != nullptr) {
                pCursor->Show(true);
            }
        } else {
            // if (bCursorCaptured) ReleaseCapture();
            bCursorCaptured = FALSE;

            if (pCursor != nullptr) {
                pCursor->Show(false);
            }
        }
    }

    // ReferTo (message, pWnd);
    return TRUE;
}

//--------------------------------------------------------------------------------------------
// void GameFrame::OnMouseMove(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void GameFrame::OnMouseMove(UINT /*nFlags*/, CPoint point) {
    gMousePosition = point;

    for (SLONG c = 0; c < Sim.Players.AnzPlayers; c++) {
        if (Sim.Players.Players[c].Owner == 0) {
            Sim.Players.Players[c].CursorPos = point;
        }
    }

    gMousePosition.x += 2;
    gMousePosition.y += 2;

    gKlackerPlanes.TimeSinceStart = 0;

    if (gUseWindowsMouse == 0) {
        if ((bActive != 0) && (pCursor != nullptr) && bNoQuickMouse == FALSE) {
            SLONG _x = gMousePosition.x;
            SLONG _y = gMousePosition.y;
            FrameWnd->TranslatePointToScreenSpace(_x, _y);
            pCursor->MoveImage(_x - MouseCursorOffset.x, _y - MouseCursorOffset.y);
        }
    }
}

void GameFrame::OnCaptureChanged(void * /*unused*/) {}

//--------------------------------------------------------------------------------------------
// user pressed F1
//--------------------------------------------------------------------------------------------
BOOL GameFrame::OnHelpInfo(void * /*unused*/) {
    ToolTipState = FALSE;

    ToolTipTimer = AtGetTime() - 601;

    return (TRUE);
}

//--------------------------------------------------------------------------------------------
// void GameFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags):
//--------------------------------------------------------------------------------------------
void GameFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    static char TypeBuffer[30]; // Für Cheats
    SLONG nTargetRoom = 0;

    if (nChar == ATKEY_RETURN) {
        if (Sim.localPlayer != -1 && Sim.Players.Players.AnzEntries() == 4 && (Sim.Players.Players[Sim.localPlayer].LocationWin != nullptr)) {
            SLONG CurrentMenu = (Sim.Players.Players[Sim.localPlayer].LocationWin)->CurrentMenu;

            if (CurrentMenu == MENU_REQUEST || CurrentMenu == MENU_BROADCAST) {
                if (CurrentMenu == MENU_BROADCAST && strlen((Sim.Players.Players[Sim.localPlayer].LocationWin)->Optionen[0]) == 0) {
                    (Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuStop();
                } else {
                    ::MouseClickPar1 = 1;
                    ::MouseClickArea = -101;
                    ::MouseClickId = MENU_REQUEST;
                    (Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuLeftClick(XY(0, 0));
                }
            } else if (CurrentMenu == MENU_NONE && Sim.Players.GetAnzHumanPlayers() > 1) {
                if (Sim.Time > 9 * 60000 && (Sim.Players.Players[Sim.localPlayer].LocationWin)->IsDialogOpen() == 0) {
                    (Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuStart(MENU_BROADCAST);
                }
            }
        }
    }

    if (gLanguage == LANGUAGE_D || gLanguage == LANGUAGE_N) {
        // Deutsch, Niederländisch
        switch (nChar) {
        case 'J':
            nTargetRoom = 'J';
            break;
        case 'T':
            nTargetRoom = ROOM_REISEBUERO;
            break;
        case 'B':
            nTargetRoom = ROOM_BURO_A;
            break;
        case 'C':
            nTargetRoom = ROOM_RICKS;
            break;
        case 'D':
            nTargetRoom = ROOM_SHOP1;
            break;
        case 'E':
            nTargetRoom = ROOM_WERBUNG;
            break;
        case 'G':
            nTargetRoom = ROOM_GLOBE;
            break;
        case 'U':
            nTargetRoom = ROOM_AUFSICHT;
            break;
        case 'S':
            nTargetRoom = ROOM_RUSHMORE;
            break;
        case 'I':
            nTargetRoom = ROOM_KIOSK;
            break;
        case 'L':
            nTargetRoom = ROOM_LAST_MINUTE;
            break;
        case 'M':
            nTargetRoom = ROOM_MUSEUM;
            break;
        case 'N':
            nTargetRoom = ROOM_NASA;
            break;
        case 'P':
            nTargetRoom = ROOM_PERSONAL_A;
            break;
        case 'R':
            nTargetRoom = ROOM_ROUTEBOX;
            break;
        case 'F':
            nTargetRoom = ROOM_MAKLER;
            break;
        case 'K':
            nTargetRoom = ROOM_BANK;
            break;
        case 'W':
            nTargetRoom = ROOM_WERKSTATT;
            break;
        case 'A':
            nTargetRoom = ROOM_ARAB_AIR;
            break;
        case 'H':
            nTargetRoom = ROOM_FRACHT;
            break;
        case 'X':
            nTargetRoom = ROOM_DESIGNER;
            break;
        case 'Y':
            nTargetRoom = ROOM_SECURITY;
            break;
        default:
            break;
        }
    } else if (gLanguage == LANGUAGE_O) {
        // Portugisisch
        switch (nChar) {
        case 'J':
            nTargetRoom = 'J';
            break;
        case 'T':
            nTargetRoom = ROOM_REISEBUERO;
            break;
        case 'O':
            nTargetRoom = ROOM_BURO_A;
            break;
        case 'C':
            nTargetRoom = ROOM_RICKS;
            break;
        case 'D':
            nTargetRoom = ROOM_SHOP1;
            break;
        case 'I':
            nTargetRoom = ROOM_WERBUNG;
            break;
        case 'G':
            nTargetRoom = ROOM_GLOBE;
            break;
        case 'U':
            nTargetRoom = ROOM_AUFSICHT;
            break;
        case 'S':
            nTargetRoom = ROOM_RUSHMORE;
            break;
        case 'K':
            nTargetRoom = ROOM_KIOSK;
            break;
        case 'L':
            nTargetRoom = ROOM_LAST_MINUTE;
            break;
        case 'M':
            nTargetRoom = ROOM_MUSEUM;
            break;
        case 'N':
            nTargetRoom = ROOM_NASA;
            break;
        case 'P':
            nTargetRoom = ROOM_PERSONAL_A;
            break;
        case 'R':
            nTargetRoom = ROOM_ROUTEBOX;
            break;
        case 'E':
            nTargetRoom = ROOM_MAKLER;
            break;
        case 'B':
            nTargetRoom = ROOM_BANK;
            break;
        case 'W':
            nTargetRoom = ROOM_WERKSTATT;
            break;
        case 'A':
            nTargetRoom = ROOM_ARAB_AIR;
            break;
        case 'H':
            nTargetRoom = ROOM_FRACHT;
            break;
        default:
            break;
        }
    } else {
        // Englisch, Sonstige:
        switch (nChar) {
        case 'J':
            nTargetRoom = 'J';
            break;
        case 'T':
            nTargetRoom = ROOM_REISEBUERO;
            break;
        case 'B':
            nTargetRoom = ROOM_BANK;
            break;
        case 'C':
            nTargetRoom = ROOM_RICKS;
            break;
        case 'D':
            nTargetRoom = ROOM_SHOP1;
            break;
        case 'E':
            nTargetRoom = ROOM_MAKLER;
            break;
        case 'G':
            nTargetRoom = ROOM_GLOBE;
            break;
        case 'U':
            nTargetRoom = ROOM_AUFSICHT;
            break;
        case 'S':
            nTargetRoom = ROOM_RUSHMORE;
            break;
        case 'O':
            nTargetRoom = ROOM_BURO_A;
            break;
        case 'L':
            nTargetRoom = ROOM_LAST_MINUTE;
            break;
        case 'M':
            nTargetRoom = ROOM_MUSEUM;
            break;
        case 'N':
            nTargetRoom = ROOM_NASA;
            break;
        case 'P':
            nTargetRoom = ROOM_PERSONAL_A;
            break;
        case 'R':
            nTargetRoom = ROOM_ROUTEBOX;
            break;
        case 'I':
            nTargetRoom = ROOM_WERBUNG;
            break;
        case 'K':
            nTargetRoom = ROOM_KIOSK;
            break;
        case 'W':
            nTargetRoom = ROOM_WERKSTATT;
            break;
        case 'A':
            nTargetRoom = ROOM_ARAB_AIR;
            break;
        case 'H':
            nTargetRoom = ROOM_FRACHT;
            break;
        case 'X':
            nTargetRoom = ROOM_DESIGNER;
            break;
        case 'Y':
            nTargetRoom = ROOM_SECURITY;
            break;
        default:
            break;
        }
    }

    gKlackerPlanes.TimeSinceStart = 0;

    if ((nOptionsOpen != 0) && (Sim.bNetwork != 0) && Sim.localPlayer < Sim.Players.Players.AnzEntries() &&
        Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom() != 0) {
        return;
    }

    PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

    static SLONG LastKeyTime;

    char CheatRunningman1[] = "SRUNNING";
    char CheatRunningman2[] = "SMAN";
    char strCheatMentat[] = "RMENTATMONEY";

    char CheatPanic1[] = "SPAN";
    char CheatPanic2[] = "SIC";

    // if (nChar==ATKEY_SPACE) hprintf ("---------------------<SPACE PRESSED>---------------------");
    /*if (nChar==ATKEY_SPACE)
      {
      for (SLONG c=0; c<4; c++)
      if (c!=1)
      {
      Sim.Players.Players[1].OwnsAktien[0] += Sim.Players.Players[c].OwnsAktien[0];
      Sim.Players.Players[c].OwnsAktien[0]=0;
      }
      }*/

    if (Sim.localPlayer != -1) {
        if (Sim.Players.Players.AnzEntries() > Sim.localPlayer) {
            if (qPlayer.GetRoom() != ROOM_OPTIONS) {
                if (qPlayer.LocationWin == nullptr ||
                    ((qPlayer.LocationWin)->CurrentMenu != MENU_RENAMEPLANE && (qPlayer.LocationWin)->CurrentMenu != MENU_BROADCAST)) {
                    switch (nTargetRoom) {
                    case 'J':
                        if (Sim.localPlayer != -1 && Sim.Players.Players.AnzEntries() == 4 && (qPlayer.LocationWin != nullptr)) {
                            if ((qPlayer.LocationWin)->CurrentMenu == MENU_REQUEST) {
                                ::MouseClickPar1 = 1;
                                ::MouseClickArea = -101;
                                ::MouseClickId = MENU_REQUEST;
                                (qPlayer.LocationWin)->MenuLeftClick(XY(0, 0));
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    if (Sim.Time > 9 * 60000 && (Editor == 0)) {
        if (Sim.Gamestate == (GAMESTATE_PLAYING | GAMESTATE_WORKING)) {
            if (Sim.GetHour() >= 8 && Sim.GetHour() < 18) {
                if ((Sim.CallItADay == 0) && (Sim.IsTutorial == 0)) {
                    if ((qPlayer.LocationWin != nullptr) && ((qPlayer.LocationWin)->IsDialogOpen() == 0) && ((qPlayer.LocationWin)->MenuIsOpen() == 0)) {
                        if (qPlayer.GetRoom() != ROOM_OPTIONS) {
                            BOOL doRun = FALSE;

                            if (TypeBuffer[29] == static_cast<char>(nChar) && AtGetTime() - LastKeyTime < 300) {
                                doRun = TRUE;
                            }

                            LastKeyTime = AtGetTime();

                            SLONG StatePar = Sim.Persons[Sim.Persons.GetPlayerIndex(Sim.localPlayer)].StatePar;

                            // Sind wir im Pseudo-Raum "Treppe"? Oder ist schon Feierabend?
                            if ((StatePar < ROOM_STAIRSVON || StatePar > ROOM_STAIRSBIS) && qPlayer.GetRoom() != ROOM_ABEND) {
                                if (Sim.Time > 9 * 60000) {
                                    if (qPlayer.IsWalking2Player == -1) {
                                        switch (nTargetRoom) {
                                        case ROOM_REISEBUERO: // AirTravel
                                            if (GlobalUse(USE_TRAVELHOLDING)) {
                                                Sim.InvalidateHint(HINT_AIRTRAVEL);
                                                if (qPlayer.IsLocationInQueue(ROOM_REISEBUERO) == 0) {
                                                    qPlayer.WalkToRoom(ROOM_REISEBUERO);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_BURO_A:
                                            Sim.InvalidateHint(HINT_BUERO);
                                            if (qPlayer.IsLocationInQueue(UWORD(ROOM_BURO_A + Sim.localPlayer * 10)) == 0) {
                                                qPlayer.WalkToGlobe = 0;
                                                qPlayer.WalkToRoom(UBYTE(ROOM_BURO_A + Sim.localPlayer * 10));
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_RICKS:
                                            Sim.InvalidateHint(HINT_RICKS);
                                            if (qPlayer.IsLocationInQueue(ROOM_RICKS) == 0) {
                                                qPlayer.WalkToRoom(ROOM_RICKS);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_SHOP1:
                                            Sim.InvalidateHint(HINT_DUTYFREE);
                                            if (qPlayer.IsLocationInQueue(ROOM_SHOP1) == 0) {
                                                qPlayer.WalkToRoom(ROOM_SHOP1);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_WERBUNG:
                                            Sim.InvalidateHint(HINT_WERBUNG);
                                            if ((qPlayer.IsLocationInQueue(ROOM_WERBUNG) == 0) && AtGetAsyncKeyState(ATKEY_SHIFT) / 256 == 0) {
                                                qPlayer.WalkToRoom(ROOM_WERBUNG);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_GLOBE:
                                            Sim.InvalidateHint(HINT_GLOBE);
                                            if (qPlayer.IsLocationInQueue(UWORD(ROOM_GLOBE)) == 0) {
                                                if (qPlayer.GetRoom() != (UBYTE(ROOM_BURO_A + Sim.localPlayer * 10))) {
                                                    qPlayer.WalkToGlobe = 1;
                                                    qPlayer.WalkToRoom(UBYTE(ROOM_BURO_A + Sim.localPlayer * 10));
                                                    qPlayer.LeaveAllRooms();
                                                } else {
                                                    qPlayer.EnterRoom(ROOM_GLOBE);
                                                }
                                            }
                                            break;

                                        case ROOM_AUFSICHT:
                                            Sim.InvalidateHint(HINT_AUFSICHT);
                                            if (qPlayer.IsLocationInQueue(ROOM_AUFSICHT) == 0) {
                                                qPlayer.WalkToRoom(ROOM_AUFSICHT);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_RUSHMORE:
                                            Sim.InvalidateHint(HINT_FERNGLASS);
                                            if ((qPlayer.IsLocationInQueue(ROOM_RUSHMORE) == 0) && (qPlayer.IsLocationInQueue(ROOM_INSEL) == 0) &&
                                                (qPlayer.IsLocationInQueue(ROOM_WELTALL) == 0)) {
                                                if (Sim.Difficulty == DIFF_FINAL) {
                                                    qPlayer.WalkToRoom(ROOM_INSEL);
                                                } else if (Sim.Difficulty == DIFF_ADDON10) {
                                                    qPlayer.WalkToRoom(ROOM_WELTALL);
                                                } else {
                                                    qPlayer.WalkToRoom(ROOM_RUSHMORE);
                                                }

                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_KIOSK:
                                            Sim.InvalidateHint(HINT_KIOSK);
                                            if (qPlayer.IsLocationInQueue(ROOM_KIOSK) == 0) {
                                                qPlayer.WalkToRoom(ROOM_KIOSK);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_LAST_MINUTE: // LastMinute
                                            if (GlobalUse(USE_TRAVELHOLDING)) {
                                                Sim.InvalidateHint(HINT_LASTMINUTE);
                                                if (qPlayer.IsLocationInQueue(ROOM_LAST_MINUTE) == 0) {
                                                    qPlayer.WalkToRoom(ROOM_LAST_MINUTE);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_MUSEUM: // Museum
                                            Sim.InvalidateHint(HINT_MUSEUM);
                                            if (qPlayer.IsLocationInQueue(ROOM_MUSEUM) == 0) {
                                                qPlayer.WalkToRoom(ROOM_MUSEUM);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_DESIGNER: // Aircraft-Designer
                                            if (qPlayer.IsLocationInQueue(ROOM_DESIGNER) == 0) {
                                                if (XY(-9999, -9999) != Airport.GetRandomTypedRune(RUNE_2SHOP, ROOM_DESIGNER, true)) {
                                                    qPlayer.WalkToRoom(ROOM_DESIGNER);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_SECURITY: // Security
                                            if (qPlayer.IsLocationInQueue(ROOM_SECURITY) == 0) {
                                                if (XY(-9999, -9999) != Airport.GetRandomTypedRune(RUNE_2SHOP, ROOM_SECURITY, true)) {
                                                    qPlayer.WalkToRoom(ROOM_SECURITY);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_NASA:
                                            if (Sim.Difficulty == DIFF_FINAL || Sim.Difficulty == DIFF_ADDON10) {
                                                Sim.InvalidateHint(HINT_NASA);
                                                if (qPlayer.IsLocationInQueue(ROOM_NASA) == 0) {
                                                    qPlayer.WalkToRoom(ROOM_NASA);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_PERSONAL_A:
                                            Sim.InvalidateHint(HINT_PERSONALBUERO);
                                            if (qPlayer.IsLocationInQueue(UWORD(ROOM_PERSONAL_A + Sim.localPlayer * 10)) == 0) {
                                                qPlayer.WalkToRoom(UBYTE(ROOM_PERSONAL_A + Sim.localPlayer * 10));
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_ROUTEBOX:
                                            if (Sim.Difficulty > DIFF_TUTORIAL || Sim.Difficulty == DIFF_FREEGAME) {
                                                Sim.InvalidateHint(HINT_ROUTEBOX);
                                                if (qPlayer.IsLocationInQueue(ROOM_ROUTEBOX) == 0) {
                                                    qPlayer.WalkToRoom(ROOM_ROUTEBOX);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_MAKLER:
                                            Sim.InvalidateHint(HINT_FLUGZEUGMAKLER);
                                            if (qPlayer.IsLocationInQueue(ROOM_MAKLER) == 0) {
                                                qPlayer.WalkToRoom(ROOM_MAKLER);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_BANK:
                                            Sim.InvalidateHint(HINT_BANK);
                                            if (qPlayer.IsLocationInQueue(ROOM_BANK) == 0) {
                                                qPlayer.WalkToRoom(ROOM_BANK);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_WERKSTATT:
                                            Sim.InvalidateHint(HINT_WERKSTATT);
                                            if (qPlayer.IsLocationInQueue(ROOM_WERKSTATT) == 0) {
                                                qPlayer.WalkToRoom(ROOM_WERKSTATT);
                                                qPlayer.LeaveAllRooms();
                                            }
                                            break;

                                        case ROOM_ARAB_AIR:
                                            if (Sim.Difficulty > DIFF_TUTORIAL || Sim.Difficulty == DIFF_FREEGAME) {
                                                Sim.InvalidateHint(HINT_ARABAIR);
                                                if (qPlayer.IsLocationInQueue(ROOM_ARAB_AIR) == 0) {
                                                    qPlayer.WalkToRoom(ROOM_ARAB_AIR);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        case ROOM_FRACHT:
                                            if (Sim.Difficulty >= DIFF_ADDON02 || Sim.Difficulty == DIFF_FREEGAME) {
                                                Sim.InvalidateHint(HINT_FRACHT);
                                                if (qPlayer.IsLocationInQueue(ROOM_FRACHT) == 0) {
                                                    qPlayer.WalkToRoom(ROOM_FRACHT);
                                                    qPlayer.LeaveAllRooms();
                                                }
                                            }
                                            break;

                                        default:
                                            doRun = FALSE;
                                            break;
                                        }

                                        if (nFlags != InputFlags::FromTextInput) {
                                    		switch (nChar) {
	                                        case ATKEY_TAB:
	                                            (qPlayer.LocationWin)->MenuStart(MENU_REQUEST, MENU_REQUEST_CALLITADAY, 0);
	                                            (qPlayer.LocationWin)->MenuSetZoomStuff(XY(320, 220), 0.17, FALSE);
	                                            nChar = 0;
	                                            break;

	                                        case ATKEY_ESCAPE:
	                                        case ATKEY_F2:
	                                            if (qPlayer.IsLocationInQueue(ROOM_OPTIONS) == 0) {
	                                                qPlayer.EnterRoom(ROOM_OPTIONS);
	                                            } else {
	                                                qPlayer.LeaveRoom();
	                                            }
	                                            break;

	                                        case ATKEY_F3:
	                                            if (qPlayer.IsLocationInQueue(ROOM_OPTIONS) == 0) {
	                                                OptionsShortcut = 5;
	                                                qPlayer.EnterRoom(ROOM_OPTIONS);
	                                            }
	                                            break;

	                                        case ATKEY_F4:
	                                            if (qPlayer.IsLocationInQueue(ROOM_OPTIONS) == 0) {
	                                                OptionsShortcut = 6;
	                                                qPlayer.EnterRoom(ROOM_OPTIONS);
	                                            }
	                                            break;
	                                        default:
	                                            break;
	                                        }
                                        }
                                    }
                                }
                            }

                            if ((doRun != 0) && Sim.Players.Players[Sim.localPlayer].IsStuck == 0) {
                                Sim.Persons[Sim.Persons.GetPlayerIndex(Sim.localPlayer)].Running = TRUE;
                                qPlayer.BroadcastPosition();
                            }
                        }
                    }
                }
            }
        }
    }

    if (nChar == ATKEY_ESCAPE || nChar == 'N') {
        if (Sim.localPlayer != -1 && Sim.Players.Players.AnzEntries() == 4 && (qPlayer.LocationWin != nullptr)) {
            if ((qPlayer.LocationWin)->CurrentMenu == MENU_REQUEST) {
                ::MouseClickPar1 = 2;
                ::MouseClickArea = -101;
                ::MouseClickId = MENU_REQUEST;
                (qPlayer.LocationWin)->MenuLeftClick(XY(0, 0));
            }
        }
    }

    if (nChar == ATKEY_ESCAPE && (TopWin != nullptr)) {
        if (Sim.Gamestate == (GAMESTATE_INTRO | GAMESTATE_WORKING)) {
            (dynamic_cast<CIntro *>(TopWin))->OnKeyDown(nChar, nRepCnt, nFlags);
        }
        if (Sim.Gamestate == (GAMESTATE_OUTRO | GAMESTATE_WORKING)) {
            (dynamic_cast<COutro *>(TopWin))->OnKeyDown(nChar, nRepCnt, nFlags);
        }
    }

    // Cheat: Mission gewinnen:
    if (nChar >= '1' && nChar <= '4' && (CheatMissions != 0)) {
        switch (Sim.Difficulty) {
        case DIFF_TUTORIAL:
            Sim.Players.Players[SLONG(nChar - '1')].NumAuftraege = 5;
            break;

        case DIFF_FIRST:
            Sim.Players.Players[SLONG(nChar - '1')].NumPassengers = TARGET_PASSENGERS;
            break;

        case DIFF_EASY:
            Sim.Players.Players[SLONG(nChar - '1')].Gewinn = TARGET_GEWINN;
            break;

        case DIFF_HARD:
            Sim.Players.Players[SLONG(nChar - '1')].Image = 700;
            break;

        case DIFF_FINAL:
            Sim.Players.Players[SLONG(nChar - '1')].RocketFlags = ROCKET_ALL;
            break;

        case DIFF_ADDON01:
            Sim.Players.Players[SLONG(nChar - '1')].Credit = 0;
            break;

        case DIFF_ADDON02:
            Sim.Players.Players[SLONG(nChar - '1')].NumFracht = 1000;
            break;

        case DIFF_ADDON03:
            Sim.Players.Players[SLONG(nChar - '1')].NumFrachtFree = 100000;
            break;

        case DIFF_ADDON04:
            Sim.Players.Players[SLONG(nChar - '1')].NumMiles = 800000;
            break;

        case DIFF_ADDON08:
            Sim.Players.Players[SLONG(nChar - '1')].Kurse[0] = 300;
            break;

        case DIFF_ADDON09:
            Sim.Players.Players[SLONG(nChar - '1')].NumOrderFlights = TARGET_NUM_UHRIG;
            break;

        case DIFF_ADDON10:
            Sim.Players.Players[SLONG(nChar - '1')].RocketFlags = ROCKET_ALL;
            break;
        default:
            break;
        }
    }

    if (nChar == ATKEY_SPACE) {
        if (qPlayer.LocationWin != nullptr) {
            if ((qPlayer.LocationWin)->CalculatorIsOpen != 0) {
                SBFX *pTargetFx = new SBFX;

                SynthesizeNumber(pTargetFx, bprintf("p%li\\", Sim.localPlayer + 1), (qPlayer.LocationWin)->CalculatorValue, rand() % 2);

                pTargetFx->Play(0, Sim.Options.OptionEffekte * 100 / 7);
            }
        }

        if (Sim.IsTutorial != 0) {
            qPlayer.Messages.IsMonolog = FALSE;
            qPlayer.Messages.NextMessage();
            qPlayer.Messages.Pump();
            qPlayer.Messages.NextMessage();
            qPlayer.Messages.Pump();
            qPlayer.Messages.NextMessage();
            Sim.IsTutorial = FALSE;

            if (qPlayer.LocationWin != nullptr) {
                (qPlayer.LocationWin)->GlowEffects.ReSize(0);
            }

            if (Sim.GetHour() == 9 && Sim.GetMinute() == 0 && (Sim.Tutorial == 1300 || Sim.Tutorial == 1200 + 40)) {
                Sim.bNoTime = FALSE;
                Sim.DayState = 2;
                if (qPlayer.GetRoom() != ROOM_AIRPORT) {
                    qPlayer.LeaveRoom();
                }
            }

            UpdateStatusBar();
        } else if (TopWin == nullptr) {
            if (Sim.localPlayer >= 0 && Sim.localPlayer <= 3) {
                SLONG Index = Sim.Persons.GetPlayerIndex(Sim.localPlayer);
                if (Sim.Persons.IsInAlbum(Index) != 0) {
                    PERSON &qPerson = Sim.Persons[Index];

                    if (qPlayer.GetRoom() == ROOM_AIRPORT) {
                        if ((qPlayer.LocationWin != nullptr) && ((qPlayer.LocationWin)->IsDialogOpen() == 0) && ((qPlayer.LocationWin)->MenuIsOpen() == 0)) {
                            if (qPerson.Dir != 8 && (Sim.bNetwork == 0)) {
                                bgWarp = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }

    // Cheats:
    if (nChar >= 'A' && nChar < 'Z') {
        memmove(TypeBuffer, TypeBuffer + 1, 29);
        TypeBuffer[29] = static_cast<char>(nChar);

        // Halbherzig versteckte Cheats:
        if ((memcmp(TypeBuffer + 30 - strlen(CheatRunningman2) + 1, CheatRunningman2 + 1, strlen(CheatRunningman2 + 1)) == 0) &&
            (memcmp(TypeBuffer + 30 - strlen(CheatRunningman2) - strlen(CheatRunningman1) + 2, CheatRunningman1 + 1, strlen(CheatRunningman1 + 1)) == 0)) {
            CheatRunningman ^= 1;
            CheatSound();
        }

        /*if (!memcmp (TypeBuffer+30-strlen(strCheatMoreNuns)+1, strCheatMoreNuns+1, strlen(strCheatMoreNuns+1)))
          {
          if (CheatMoreNuns==1) CheatMoreNuns=0;
          else CheatMoreNuns=1;
          CheatSound ();
          }*/

        if (memcmp(TypeBuffer + 30 - 6, strCheatMentat + 1, 6) == 0) {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                CheatBerater += 100;
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7010), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
            }
        }

        if ((memcmp(TypeBuffer + 30 - strlen(CheatPanic2) + 1, CheatPanic2 + 1, strlen(CheatPanic2 + 1)) == 0) &&
            (memcmp(TypeBuffer + 30 - strlen(CheatPanic2) - strlen(CheatPanic1) + 2, CheatPanic1 + 1, strlen(CheatPanic1 + 1)) == 0)) {
            for (SLONG c = 0; c < Sim.Persons.AnzEntries(); c++) {
                if ((Sim.Persons.IsInAlbum(c) != 0) &&
                    (Clans[static_cast<SLONG>(Sim.Persons[c].ClanId)].Type < 20 || Clans[static_cast<SLONG>(Sim.Persons[c].ClanId)].Type > 29)) {
                    Sim.Persons[c].State = PERSON_2EXIT;
                    Sim.Persons[c].Target = Airport.GetRandomExit();
                    CheatSound();
                }
            }
        }

        // Wirklich versteckte Cheats:
        // SHOWIT
        if (TypeBuffer[24] == 'S' && TypeBuffer[25] == 'H' && TypeBuffer[26] == 'O' && TypeBuffer[27] == 'W' && TypeBuffer[28] == 'I' &&
            TypeBuffer[29] == 'T') {
            CheatAnimNow ^= 1;
            CheatSound();
        }

        // ATTESTIT
        if (TypeBuffer[22] == 'A' && TypeBuffer[23] == 'T' && TypeBuffer[24] == 'T' && TypeBuffer[25] == 'E' && TypeBuffer[26] == 'S' &&
            TypeBuffer[27] == 'T' && TypeBuffer[28] == 'I' && TypeBuffer[29] == 'T') {
            CheatTestGame ^= 1;
            srand(AtGetTime());
            CheatSound();
        }
        // ATTESTXX
        if (TypeBuffer[22] == 'A' && TypeBuffer[23] == 'T' && TypeBuffer[24] == 'T' && TypeBuffer[25] == 'E' && TypeBuffer[26] == 'S' &&
            TypeBuffer[27] == 'T' && TypeBuffer[28] == 'X' && TypeBuffer[29] == 'X') {
            CheatTestGame ^= 2;
            srand(AtGetTime());
            CheatSound();
        }

        // DONALDTRUMP:
        if (TypeBuffer[19] == 'D' && TypeBuffer[20] == 'O' && TypeBuffer[21] == 'N' && TypeBuffer[22] == 'A' && TypeBuffer[23] == 'L' &&
            TypeBuffer[24] == 'D' && TypeBuffer[25] == 'T' && TypeBuffer[26] == 'R' && TypeBuffer[27] == 'U' && TypeBuffer[28] == 'M' &&
            TypeBuffer[29] == 'P') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;
                qPlayer.History.HistoricMoney += 10000000;
                qPlayer.Money += 10000000;
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7011), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 0);
            }
        }

        // MEGARICHMAN
        if (TypeBuffer[19] == 'M' && TypeBuffer[20] == 'E' && TypeBuffer[21] == 'G' && TypeBuffer[22] == 'A' && TypeBuffer[23] == 'R' &&
            TypeBuffer[24] == 'I' && TypeBuffer[25] == 'C' && TypeBuffer[26] == 'H' && TypeBuffer[27] == 'M' && TypeBuffer[28] == 'A' &&
            TypeBuffer[29] == 'N') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;
                qPlayer.History.HistoricMoney += 1000000000;
                qPlayer.Money += 1000000000;
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7011), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 0);
            }
        }

        // RICHENEMY
        if (TypeBuffer[21] == 'R' && TypeBuffer[22] == 'I' && TypeBuffer[23] == 'C' && TypeBuffer[24] == 'H' && TypeBuffer[25] == 'E' &&
            TypeBuffer[26] == 'N' && TypeBuffer[27] == 'E' && TypeBuffer[28] == 'M' && TypeBuffer[29] == 'Y') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;

                for (SLONG c = 0; c < 4; c++) {
                    if (Sim.localPlayer != c) {
                        Sim.Players.Players[c].History.HistoricMoney += 10000000;
                        Sim.Players.Players[c].Money += 10000000;
                    }
                }
                CheatSound();
            }
        }

        // NOTFAIR
        if (TypeBuffer[19] == 'N' && TypeBuffer[20] == 'O' && TypeBuffer[21] == 'T' && TypeBuffer[22] == 'F' && TypeBuffer[23] == 'A' &&
            TypeBuffer[24] == 'I' && TypeBuffer[25] == 'R') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;

                for (SLONG c = 0; c < 4; c++) {
                    if (Sim.localPlayer != c) {
                        Sim.Players.Players[c].History.HistoricMoney += 1000000000;
                        Sim.Players.Players[c].Money += 1000000000;
                    }
                }
                CheatSound();
            }
        }

        // MOREGLUE
        if (TypeBuffer[22] == 'M' && TypeBuffer[23] == 'O' && TypeBuffer[24] == 'R' && TypeBuffer[25] == 'E' && TypeBuffer[26] == 'G' &&
            TypeBuffer[27] == 'L' && TypeBuffer[28] == 'U' && TypeBuffer[29] == 'E') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                for (SLONG c = 0; c < 6; c++) {
                    if (qPlayer.Items[c] == ITEM_NONE) {
                        qPlayer.Items[c] = ITEM_GLUE;
                    }
                }

                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7015), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 0);
            }
        }

        // MORESTENCH
        if (TypeBuffer[20] == 'M' && TypeBuffer[21] == 'O' && TypeBuffer[22] == 'R' && TypeBuffer[23] == 'E' && TypeBuffer[24] == 'S' &&
            TypeBuffer[25] == 'T' && TypeBuffer[26] == 'E' && TypeBuffer[27] == 'N' && TypeBuffer[28] == 'C' && TypeBuffer[29] == 'H') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                for (SLONG c = 0; c < 6; c++) {
                    if (qPlayer.Items[c] == ITEM_NONE) {
                        qPlayer.Items[c] = ITEM_STINKBOMBE;
                    }
                }

                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7016), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 0);
            }
        }

        // THINKPAD
        if (TypeBuffer[22] == 'T' && TypeBuffer[23] == 'H' && TypeBuffer[24] == 'I' && TypeBuffer[25] == 'N' && TypeBuffer[26] == 'K' &&
            TypeBuffer[27] == 'P' && TypeBuffer[28] == 'A' && TypeBuffer[29] == 'D') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;
                qPlayer.Items[5] = ITEM_LAPTOP;
                qPlayer.LaptopBattery = 60 * 24;
                qPlayer.LaptopQuality = 4;
                qPlayer.ReformIcons();
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7012), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
            }
        }

        // NODEBTS
        if (TypeBuffer[23] == 'N' && TypeBuffer[24] == 'O' && TypeBuffer[25] == 'D' && TypeBuffer[26] == 'E' && TypeBuffer[27] == 'B' &&
            TypeBuffer[28] == 'T' && TypeBuffer[29] == 'S') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;
                qPlayer.Credit = 0;
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7013), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 1);
            }
        }

        // FAMOUS
        if (TypeBuffer[24] == 'F' && TypeBuffer[25] == 'A' && TypeBuffer[26] == 'M' && TypeBuffer[27] == 'O' && TypeBuffer[28] == 'U' &&
            TypeBuffer[29] == 'S') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;
                qPlayer.Image = 1000;
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7014), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 2);
            }
        }

        // ODDONEOUT
        if (TypeBuffer[21] == 'O' && TypeBuffer[22] == 'D' && TypeBuffer[23] == 'D' && TypeBuffer[24] == 'O' && TypeBuffer[25] == 'N' &&
            TypeBuffer[26] == 'E' && TypeBuffer[27] == 'O' && TypeBuffer[28] == 'U' && TypeBuffer[29] == 'T') {
            if ((Sim.bAllowCheating != 0) || (Sim.bNetwork == 0)) {
                Sim.bCheatedSession = 1;
                for (SLONG c = 0; c < 4; c++) {
                    if (Sim.localPlayer != c) {
                        Sim.Players.Players[c].Image = 1000;
                    }
                }
                CheatSound();

                SIM::SendChatBroadcast(bprintf(StandardTexte.GetS(TOKEN_MISC, 7014), (LPCTSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                SIM::SendSimpleMessage(ATNET_CHEAT, 0, Sim.localPlayer, 2);
            }
        }

        // BUBBLEGUM
        if (TypeBuffer[21] == 'B' && TypeBuffer[22] == 'U' && TypeBuffer[23] == 'B' && TypeBuffer[24] == 'B' && TypeBuffer[25] == 'L' &&
            TypeBuffer[26] == 'E' && TypeBuffer[27] == 'G' && TypeBuffer[28] == 'U' && TypeBuffer[29] == 'M') {
            CheatBubbles ^= 1;
            CheatSound();
        }

        // SHOWALL
        if (TypeBuffer[23] == 'S' && TypeBuffer[24] == 'H' && TypeBuffer[25] == 'O' && TypeBuffer[26] == 'W' && TypeBuffer[27] == 'A' &&
            TypeBuffer[28] == 'L' && TypeBuffer[29] == 'L') {
            CheatShowAll ^= 1;
            CheatSound();
        }

        // WINNING
        if (TypeBuffer[23] == 'W' && TypeBuffer[24] == 'I' && TypeBuffer[25] == 'N' && TypeBuffer[26] == 'N' && TypeBuffer[27] == 'I' &&
            TypeBuffer[28] == 'N' && TypeBuffer[29] == 'G') {
            switch (Sim.Difficulty) {
            case DIFF_TUTORIAL:
                qPlayer.NumAuftraege = 5;
                break;

            case DIFF_FIRST:
                qPlayer.NumPassengers = TARGET_PASSENGERS;
                break;

            case DIFF_EASY:
                qPlayer.Gewinn = TARGET_GEWINN;
                break;

            case DIFF_HARD:
                qPlayer.Image = 700;
                break;

            case DIFF_FINAL:
                qPlayer.RocketFlags = ROCKET_ALL;
                break;

            case DIFF_ADDON01:
                qPlayer.Credit = 0;
                break;

            case DIFF_ADDON02:
                qPlayer.NumFracht = 1000;
                break;

            case DIFF_ADDON03:
                qPlayer.NumFrachtFree = 100000;
                break;

            case DIFF_ADDON04:
                qPlayer.NumMiles = 800000;
                break;

            case DIFF_ADDON08:
                qPlayer.Kurse[0] = 300;
                break;

            case DIFF_ADDON09:
                qPlayer.NumOrderFlights = TARGET_NUM_UHRIG;
                break;

            case DIFF_ADDON10:
                qPlayer.RocketFlags = ROCKET_ALL;
                break;

            case DIFF_ATFS01:
                qPlayer.NumPassengers = BTARGET_PASSENGERS;
                qPlayer.Money = BTARGET_KONTO;
                break;

            case DIFF_ATFS02:
                break;

            case DIFF_ATFS03:
                break;

            case DIFF_ATFS04:
                break;

            case DIFF_ATFS05:
                break;

            case DIFF_ATFS06:
                break;

            case DIFF_ATFS07:
                break;

            case DIFF_ATFS08:
                break;

            case DIFF_ATFS09:
                break;

            case DIFF_ATFS10:
                break;
            default:
                break;
            }
            CheatSound();
        }

        // ATWINNING:
        if (TypeBuffer[21] == 'A' && TypeBuffer[22] == 'T' && TypeBuffer[23] == 'W' && TypeBuffer[24] == 'I' && TypeBuffer[25] == 'N' &&
            TypeBuffer[26] == 'N' && TypeBuffer[27] == 'I' && TypeBuffer[28] == 'N' && TypeBuffer[29] == 'G') {
            CheatMissions ^= 1;
            CheatSound();
        }

        // COFFEECUP
        if (TypeBuffer[21] == 'C' && TypeBuffer[22] == 'O' && TypeBuffer[23] == 'F' && TypeBuffer[24] == 'F' && TypeBuffer[25] == 'E' &&
            TypeBuffer[26] == 'E' && TypeBuffer[27] == 'C' && TypeBuffer[28] == 'U' && TypeBuffer[29] == 'P') {
            Sim.Players.Players[0].SecurityFlags = (1 << 7);
            Sim.Players.Players[Sim.localPlayer].ArabTrust = 6; // Für Spieler 2

            /*Sim.Players.Players[Sim.localPlayer].ArabMode2  = 1; //Bakterien im Kaffee
              Sim.Players.Players[Sim.localPlayer].ArabOpfer2 = 2; //Für Spieler 2
              Sim.Players.Players[Sim.localPlayer].ArabTrust  = 6; //Für Spieler 2

              Sim.Players.Players[Sim.localPlayer].NetSynchronizeSabotage (); */
            CheatSound();
        }

        if (nChar == ATKEY_F5) {
            SLONG x = 0;
            x++;

            // SecurityFlags
        }

        // ATMISSALL
        if (TypeBuffer[21] == 'A' && TypeBuffer[22] == 'T' && TypeBuffer[23] == 'M' && TypeBuffer[24] == 'I' && TypeBuffer[25] == 'S' &&
            TypeBuffer[26] == 'S' && TypeBuffer[27] == 'A' && TypeBuffer[28] == 'L' && TypeBuffer[29] == 'L') {
            Sim.MaxDifficulty = DIFF_FINAL;
            Sim.MaxDifficulty2 = DIFF_ADDON10;
            if (Sim.Gamestate == (GAMESTATE_INIT | GAMESTATE_WORKING) && (TopWin != nullptr)) {
                (dynamic_cast<NewGamePopup *>(TopWin))->RefreshKlackerField();
            }
            CheatSound();
        }

        // EVOLUTIONCOMPLETE
        if (TypeBuffer[13] == 'E' && TypeBuffer[14] == 'V' && TypeBuffer[15] == 'O' && TypeBuffer[16] == 'L' && TypeBuffer[17] == 'U' &&
            TypeBuffer[18] == 'T' && TypeBuffer[19] == 'I' && TypeBuffer[20] == 'O' && TypeBuffer[21] == 'N' && TypeBuffer[22] == 'C' &&
            TypeBuffer[23] == 'O' && TypeBuffer[24] == 'M' && TypeBuffer[25] == 'P' && TypeBuffer[26] == 'L' && TypeBuffer[27] == 'E' &&
            TypeBuffer[28] == 'T' && TypeBuffer[29] == 'E') {
            Sim.MaxDifficulty3 = DIFF_ATFS10;
            if (Sim.Gamestate == (GAMESTATE_INIT | GAMESTATE_WORKING) && (TopWin != nullptr)) {
                (dynamic_cast<NewGamePopup *>(TopWin))->RefreshKlackerField();
            }
            CheatSound();
        }

        // EXPANDER
        if (TypeBuffer[22] == 'E' && TypeBuffer[23] == 'X' && TypeBuffer[24] == 'P' && TypeBuffer[25] == 'A' && TypeBuffer[26] == 'N' &&
            TypeBuffer[27] == 'D' && TypeBuffer[28] == 'E' && TypeBuffer[29] == 'R' && Sim.CheckIn < 5 && (Sim.bNetwork == 0)) {
            Sim.ExpandAirport = TRUE;
            CheatSound();
        }

        // ATMISSNONE
        if (TypeBuffer[20] == 'A' && TypeBuffer[21] == 'T' && TypeBuffer[22] == 'M' && TypeBuffer[23] == 'I' && TypeBuffer[24] == 'S' &&
            TypeBuffer[25] == 'S' && TypeBuffer[26] == 'N' && TypeBuffer[27] == 'O' && TypeBuffer[28] == 'N' && TypeBuffer[29] == 'E') {
            Sim.MaxDifficulty = DIFF_TUTORIAL;
            Sim.MaxDifficulty2 = DIFF_ADDON01;
            Sim.MaxDifficulty3 = DIFF_ATFS01;
            if (Sim.Gamestate == (GAMESTATE_INIT | GAMESTATE_WORKING) && (TopWin != nullptr)) {
                (dynamic_cast<NewGamePopup *>(TopWin))->RefreshKlackerField();
            }
            CheatSound();
        }

        // CROWD
        if (TypeBuffer[25] == 'C' && TypeBuffer[26] == 'R' && TypeBuffer[27] == 'O' && TypeBuffer[28] == 'W' && TypeBuffer[29] == 'D' && (Sim.bNetwork == 0)) {
            if ((qPlayer.LocationWin != nullptr) && qPlayer.GetRoom() == ROOM_AIRPORT) {
                (dynamic_cast<AirportView *>(qPlayer.LocationWin))->PersonsToAdd += 200;
            }
            CheatSound();
        }

        if (TypeBuffer[24] == 'A' && TypeBuffer[25] == 'T' && TypeBuffer[26] == 'P' && TypeBuffer[27] == 'R' && TypeBuffer[28] == 'O' &&
            TypeBuffer[29] == 'T') {
            if (Sim.bExeChanged == FALSE) {
                Sim.TicksSinceLoad = 20 * 60 * 10 - 10;
                Sim.TicksPlayed = 20 * 60 * 60 * 2 + 1;
                Sim.NumRestored = 1;
                CheatSound();
            } else {
                Sim.bExeChanged = 30;
            }
        }
    }

    if (Editor == EDITOR_NONE) {
        if (nChar == ATKEY_F6) {
            // Sim.Players.Players[(SLONG)0].ArabMode2   = 3;
            // Sim.Players.Players[(SLONG)0].ArabOpfer2  = 1; //Sim.localPlayer;
            // Sim.Players.Players[(SLONG)0].NetSynchronizeSabotage ();
            // Sim.Players.Players[(SLONG)0].ArabActive  = FALSE;
            // Sim.Players.Players[(SLONG)0].ArabPlane   = qPlayer.Planes.GetRandomUsedIndex();
        }

        /*if (nChar==ATKEY_F6)
          {
          qPlayer.Money+=30000000;
          }*/

        /*if (nChar==ATKEY_F6)
          {
          static n=0;  n++;

          for (SLONG c=0; c<Sim.KerosinPast.AnzEntries()-1; c++)
          Sim.KerosinPast[c]=Sim.KerosinPast[c+1];

          Sim.Kerosin += (rand()%21)-10 + (rand()%20<2)*((rand()%41)-20) + SLONG(sin ((Sim.Date+n+rand()%6)/3.0)*20) + SLONG(sin ((Sim.Date+n)*1.7)*20);
          Limit (300l, Sim.Kerosin, 700l);
          Sim.KerosinPast[9]=Sim.Kerosin;
          } */

        // if (nChar==ATKEY_F12) DebugBreak();

        /*if (nChar==ATKEY_TAB)
          {
          Sim.localPlayer=(Sim.localPlayer+1)&3;
          hprintf ("Sim.localPlayer now is Player %li", Sim.localPlayer+1);
          } */

        if (nChar == ATKEY_F5) {
            for (SLONG d = 0; d < 4; d++) {
                if (Sim.Players.Players[d].Owner == 1) {
                    for (SLONG c = Sim.Players.Players[d].Planes.AnzEntries() - 1; c >= 0; c--) {
                        if (Sim.Players.Players[d].Planes.IsInAlbum(c) != 0) {
                            hprintf("Dumping Player %li, Plane %li", d, c);
                            Sim.Players.Players[d].Planes[c].Flugplan.Dump(true);
                        }
                    }
                }
            }
        }

        if (nChar == ATKEY_F6) {
            /*if (Sim.Players.Players[0].Sympathie[1]<200)
              Sim.Players.Players[0].Sympathie[1]=250;
              else
              Sim.Players.Players[0].Sympathie[1]=-250;

            Sim.Players.Players[(SLONG)2].DoRoutes   = 1;
            Sim.Players.Players[(SLONG)3].DoRoutes   = 1;
            Sim.Players.Players[(SLONG)2].Image      = 150;
            Sim.Players.Players[(SLONG)3].Image      = 150;

            qPlayer.ArabTrust = 5;

            Sim.Players.Players[(SLONG)2].ArabMode2  = 2;
            Sim.Players.Players[(SLONG)2].ArabOpfer2 = 0;

            Sim.Players.Players[(SLONG)2].ArabHints = 110;
            Sim.Players.Players[(SLONG)2].ArabMode  = 3;
            Sim.Players.Players[(SLONG)2].ArabOpfer = 3;
            Sim.Players.Players[(SLONG)2].ArabActive = FALSE;
            Sim.Players.Players[(SLONG)2].ArabPlane  = qPlayer.Planes.GetRandomUsedIndex();

            qPlayer.OwnsAktien[3] = 8000;
            Sim.Players.Players[3].OwnsAktien[3] = 0; */

            /*hprintf ("Statusbericht der Spieler:");
              for (c=0; c<Sim.Players.AnzPlayers; c++)
              if (!Sim.Players.Players[c].IsOut)
              {
              PERSON &qPerson = Sim.Persons[Sim.Persons.GetPlayerIndex (c)];

              hprintf ("%s: %s (%li) - Sympathien: %li, %li, %li, %li", (LPCTSTR)Sim.Players.Players[c].NameX, (LPCTSTR)Sim.Players.Players[c].AirlineX,
              (LPCTSTR)Sim.Players.Players[c].Owner, (LPCTSTR)Sim.Players.Players[c].Sympathie[0], (LPCTSTR)Sim.Players.Players[c].Sympathie[1],
              (LPCTSTR)Sim.Players.Players[c].Sympathie[2], (LPCTSTR)Sim.Players.Players[c].Sympathie[3]); hprintf ("- Owner %li - TopLocation: %li",
              (LPCTSTR)Sim.Players.Players[c].Owner, (LPCTSTR)Sim.Players.Players[c].GetRoom()); hprintf ("- (%li,%li)->(%li,%li)", qPerson.Position.x,
              qPerson.Position.y, qPerson.Target.x, qPerson.Target.y);
              }

              if (qPlayer.Items[0]==0xff)
              qPlayer.Items[0]=0;
              else
              qPlayer.Items[0]+=6;

              if (qPlayer.Items[0]>20)
              qPlayer.Items[0]=0;

              for (c=1; c<6; c++)
              {
              qPlayer.Items[c]=UBYTE(qPlayer.Items[0]+c);
              if (qPlayer.Items[c]>20)
              qPlayer.Items[c]=0xff;
              }

              qPlayer.LaptopBattery = 60*24;
              qPlayer.LaptopQuality = 4;

              qPlayer.ReformIcons ();
              qPlayer.Money+=10000000;

              qPlayer.WasInRoom.FillWith (TRUE);*/
        }
    }

    RePostMessage(CPoint(0, 0));

    if (nChar == ATKEY_PAUSE && (gDisablePauseKey == FALSE || (Sim.bPause != 0)) && (Sim.Gamestate & 15) == GAMESTATE_PLAYING) {
        if (Sim.bNetwork != 0) {
            SIM::SendSimpleMessage(ATNET_PAUSE, 0);
        }

        Pause(Sim.bPause == 0);
    }
}

//--------------------------------------------------------------------------------------------
// void GameFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags):
//--------------------------------------------------------------------------------------------
void GameFrame::OnKeyUp(UINT /*unused*/, UINT /*unused*/, UINT /*unused*/) { RePostMessage(CPoint(0, 0)); }

//--------------------------------------------------------------------------------------------
// void GameFrame::OnLButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void GameFrame::OnLButtonDown(UINT /*nFlags*/, CPoint point) {
    // Alles blockieren, wenn im Optionsmenü:
    if ((nOptionsOpen != 0) && (Sim.bNetwork != 0) && Sim.localPlayer < Sim.Players.Players.AnzEntries() &&
        Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom() != 0) {
        return;
    }

    gMouseClickPosition = gMousePosition;

    RePostMessage(point);
    // ReferTo (nFlags, point);
}

//--------------------------------------------------------------------------------------------
// Reposting some stuff to player windows
//--------------------------------------------------------------------------------------------
void GameFrame::OnLButtonDblClk(UINT /*nFlags*/, CPoint point) {
    // Alles blockieren, wenn im Optionsmenü:
    if ((nOptionsOpen != 0) && (Sim.bNetwork != 0) && Sim.localPlayer < Sim.Players.Players.AnzEntries() &&
        Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom() != 0) {
        return;
    }

    RePostMessage(point);
    // ReferTo (nFlags, point);
}
void GameFrame::OnLButtonUp(UINT /*nFlags*/, CPoint point) {
    // Alles blockieren, wenn im Optionsmenü:
    if ((nOptionsOpen != 0) && (Sim.bNetwork != 0) && Sim.localPlayer < Sim.Players.Players.AnzEntries() &&
        Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom() != 0) {
        return;
    }

    RePostMessage(point);
    // ReferTo (nFlags, point);
}
void GameFrame::OnRButtonDown(UINT /*nFlags*/, CPoint point) {
    // Alles blockieren, wenn im Optionsmenü:
    if ((nOptionsOpen != 0) && (Sim.bNetwork != 0) && Sim.localPlayer < Sim.Players.Players.AnzEntries() &&
        Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom() != 0) {
        return;
    }

    RePostMessage(point);
    // ReferTo (nFlags, point);
}
void GameFrame::OnRButtonUp(UINT /*nFlags*/, CPoint point) {
    // Alles blockieren, wenn im Optionsmenü:
    if ((nOptionsOpen != 0) && (Sim.bNetwork != 0) && Sim.localPlayer < Sim.Players.Players.AnzEntries() &&
        Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom() != 0) {
        return;
    }

    RePostMessage(point);
    // ReferTo (nFlags, point);
}

//--------------------------------------------------------------------------------------------
// void GameFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags):
//--------------------------------------------------------------------------------------------
void GameFrame::OnChar(UINT nChar, UINT /*unused*/, UINT /*unused*/) {
    gKlackerPlanes.TimeSinceStart = 0;

    if (Sim.localPlayer != -1) {
        if (Sim.Players.Players.AnzEntries() > Sim.localPlayer) {
            if (Sim.Players.Players[Sim.localPlayer].GetRoom() != ROOM_OPTIONS) {
                if (Sim.Players.Players[Sim.localPlayer].LocationWin == nullptr ||
                    ((Sim.Players.Players[Sim.localPlayer].LocationWin)->CurrentMenu != MENU_RENAMEPLANE)) {
                    switch (nChar) {
                    case '+':
                        Sim.InvalidateHint(HINT_GAMESPEED);
                        Sim.Players.Players[Sim.localPlayer].GameSpeed = (Sim.Players.Players[Sim.localPlayer].GameSpeed + 1) % 4;

                        SIM::SendSimpleMessage(ATNET_SETSPEED, 0, Sim.localPlayer, Sim.Players.Players[Sim.localPlayer].GameSpeed);

                        if (Sim.Players.Players[Sim.localPlayer].LocationWin != nullptr) {
                            (Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount = 3;
                        }
                        break;

                    case '-':
                        Sim.InvalidateHint(HINT_GAMESPEED);
                        Sim.Players.Players[Sim.localPlayer].GameSpeed = (Sim.Players.Players[Sim.localPlayer].GameSpeed + 3) % 4;

                        SIM::SendSimpleMessage(ATNET_SETSPEED, 0, Sim.localPlayer, Sim.Players.Players[Sim.localPlayer].GameSpeed);

                        if (Sim.Players.Players[Sim.localPlayer].LocationWin != nullptr) {
                            (Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount = 3;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Aktiviert einen Tool-Tip
//--------------------------------------------------------------------------------------------
void SetToolTip(SLONG TipId, XY Pos) {
    ::ToolTipNewId = TipId;
    ::ToolTipNewPos = Pos;
}

//--------------------------------------------------------------------------------------------
// Aktiviert einen Tool-Tip
//--------------------------------------------------------------------------------------------
void SetMouseLook(SLONG Look, SLONG TipId, SLONG ClickArea, SLONG ClickId, SLONG ClickPar1, SLONG ClickPar2) {
    if (PleaseCancelTextBubble == 0) {
        SetToolTip(TipId, gMousePosition);

        ::MouseLook = Look;
        ::MouseClickArea = ClickArea;
        ::MouseClickId = ClickId;
        ::MouseClickPar1 = ClickPar1;
        ::MouseClickPar2 = ClickPar2;
    }
}

//--------------------------------------------------------------------------------------------
// Aktiviert einen Tool-Tip wobei eine TextString übergeben wird.
//--------------------------------------------------------------------------------------------
void SetMouseLook(SLONG Look, SLONG TipId, const CString &Description, SLONG ClickArea, SLONG ClickId, SLONG ClickPar1, SLONG ClickPar2) {
    if (PleaseCancelTextBubble == 0) {
        SetToolTip(-TipId, gMousePosition);

        ::ToolTipString = Description;
        ::MouseLook = Look;
        ::MouseClickArea = ClickArea;
        ::MouseClickId = ClickId;
        ::MouseClickPar1 = ClickPar1;
        ::MouseClickPar2 = ClickPar2;
    }
}

//--------------------------------------------------------------------------------------------
// Schaltet Pausemodus ein oder aus AG:
//--------------------------------------------------------------------------------------------
bool GameFrame::Pause(bool fPause) {
    if (fPause) // Pause einschalten ?
    {
        if (Sim.bPause == 0) {
            Sim.bPause = 1;

            if (bMidiAvailable != 0) {
                PauseMidi();
            }

            return (true);
        }
    } else // Pause ausschalten ?
    {
        if (Sim.bPause > 0) {
            Sim.bPause = 0;

            if (bMidiAvailable != 0) {
                ResumeMidi();
                SetMidiVolume(Sim.Options.OptionMusik);
            }

            return (true);
        }
    }

    return (false);
}

//--------------------------------------------------------------------------------------------
// void GameFrame::OnLButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void DefaultOnLButtonDown() {
    if (Sim.bPause != 0) {
        FrameWnd->Pause(Sim.bPause == 0);
    }

    ToolTipTimer = AtGetTime() - 8000;

    gMouseLButtonDownTimer = AtGetTime();

    for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
        if (Sim.Players.Players[c].Owner == 0 && gMousePosition.IfIsWithin(Sim.Players.Players[c].WinP1, Sim.Players.Players[c].WinP2)) {
            Sim.Players.Players[c].Buttons |= 1;
        }
    }

    UpdateStatusBar();

    gMouseLButton = TRUE;
    PlayerDidntMove = 0;

    if (Sim.Options.OptionEffekte != 0) {
        if ((gpClickFx != nullptr) && (Sim.Options.OptionDigiSound != 0)) {
            gpClickFx->Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);
        }
    }
}

void DefaultOnLButtonUp() {
    gMouseLButton = FALSE;

    for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
        if (Sim.Players.Players[c].Owner == 0) {
            Sim.Players.Players[c].Buttons &= ~1;
        }
    }
}
void DefaultOnRButtonDown() {
    gMouseRButton = TRUE;
    PlayerDidntMove = 0;

    for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
        if (Sim.Players.Players[c].Owner == 0 && gMousePosition.IfIsWithin(Sim.Players.Players[c].WinP1, Sim.Players.Players[c].WinP2)) {
            Sim.Players.Players[c].Buttons |= 2;
        }
    }
}
void DefaultOnRButtonUp() {
    gMouseRButton = FALSE;

    for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++) {
        if (Sim.Players.Players[c].Owner == 0) {
            Sim.Players.Players[c].Buttons &= ~2;
        }
    }
}
