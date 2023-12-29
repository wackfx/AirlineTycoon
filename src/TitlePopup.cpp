//============================================================================================
// TitlePopup.Cpp : Titelbild mit primitiven Auswahlmöglichkeiten
//============================================================================================
#include "StdAfx.h"
#include "TitlePopup.h"
#include "gltitel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TitlePopup

TitlePopup::TitlePopup(BOOL bHandy, SLONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "", 0) {
    Sim.Gamestate = UBYTE((Sim.Gamestate & (~GAMESTATE_WORKING)) | GAMESTATE_DONE);
}

TitlePopup::~TitlePopup() = default;

/////////////////////////////////////////////////////////////////////////////
// TitlePopup message handlers

void TitlePopup::OnPaint() {}

void TitlePopup::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) {}
