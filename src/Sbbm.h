//============================================================================================
// Klasse für meine Bitmaps:
//============================================================================================

#ifndef sbbm_h
#define sbbm_h

#include "HLine.h"
#include <utility>

//--------------------------------------------------------------------------------------------
// Liest den aktuellen Taktzyklenzähler der CPU
//--------------------------------------------------------------------------------------------
inline __int64 Read64TimeStampCounter(void) {
    /*__int64 rc;

      __asm
      {
      _emit 0x0F
      _emit 0x31
      mov DWORD PTR [rc], eax
      mov DWORD PTR [rc+4], edx
      }*/

    return 0;
}

class CBencher {
  private:
    BOOL Benching;
    __int64 Timer;

  public:
    CBencher() {
        Benching = FALSE;
        Timer = 0;
    }
    void Reset(void) {
        Benching = FALSE;
        Timer = 0;
    }
    void Start(void) {
        if (!Benching) {
            Benching = TRUE;
            Timer -= Read64TimeStampCounter();
        }
    }
    void Stop(void) {
        if (Benching) {
            Benching = FALSE;
            Timer += Read64TimeStampCounter();
        }
    }
    operator SLONG() {
        if (Benching)
            return (SLONG((Timer + Read64TimeStampCounter()) / 1000));
        else
            return (SLONG(Timer / 1000));
    }
};

class CBench {
  public:
    CBencher GameTime;  // Gesammtzeit
    CBencher BlitTime;  // Beim blitten
    CBencher FlipTime;  // Beim flippen
    CBencher FXTime;    // Bei Spezialeffekten
    CBencher InitTime;  // Initialisieren
    CBencher DiskTime;  // Disk-Zugriffe
    CBencher TextTime;  // Text-Routinen
    CBencher ClearTime; // Clear()'s
    CBencher SortTime;  // Sortieren
    CBencher WalkTime;  // Laufalgorithmen
    CBencher KITime;    // Gegnerintelligenz
    CBencher WaitTime;  // Warten
    CBencher AdminTime; // Allgemeine Verwaltung
    CBencher MiscTime1; // Sonstiges
    CBencher MiscTime2; // Sonstiges

  public:
    CBench() { GameTime.Start(); }
    SLONG GetMissing(void);
    void Reset(void) {
        GameTime.Reset();
        BlitTime.Reset();
        FXTime.Reset();
        InitTime.Reset();
        DiskTime.Reset();
        TextTime.Reset();
        ClearTime.Reset();
        SortTime.Reset();
        WalkTime.Reset();
        KITime.Reset();
        WaitTime.Reset();
        AdminTime.Reset();
        MiscTime1.Reset();
        MiscTime2.Reset();
        GameTime.Start();
        FlipTime.Reset();
    }
    void Report(void);
};

extern CBench Bench;
extern SDL_Renderer *lpDD;
extern SB_CBitmapMain *bitmapMain;

XYZ DetectCurrentDisplayResolution(void);

// Eine Teak Bitmap:
class SBBM {
  public:
    SB_CBitmapCore *pBitmap{nullptr};
    CHLObj *pHLObj{nullptr};

  public:
    XY Size; // Größe der Bitmap

  public: // Konstruktor (Bitmap1.Cpp):
    SBBM() = default;
    SBBM(SLONG xs, SLONG ys) {
        pHLObj = NULL;
        bitmapMain->CreateBitmap(&pBitmap, xs, ys, 0, CREATE_USECOLORKEY | CREATE_SYSMEM);
        SBBM::Size.x = pBitmap->GetXSize();
        SBBM::Size.y = pBitmap->GetYSize();
    }
    SBBM(XY Size) {
        pHLObj = NULL;
        bitmapMain->CreateBitmap(&pBitmap, Size.x, Size.y, 0, CREATE_USECOLORKEY | CREATE_SYSMEM);
        SBBM::Size.x = pBitmap->GetXSize();
        SBBM::Size.y = pBitmap->GetYSize();
    }
    SBBM(GfxLib *gfxLibrary, __int64 graphicID) {
        pHLObj = NULL;
        bitmapMain->CreateBitmap(&pBitmap, gfxLibrary, graphicID, CREATE_USECOLORKEY | CREATE_SYSMEM);
        SBBM::Size.x = pBitmap->GetXSize();
        SBBM::Size.y = pBitmap->GetYSize();
    }
    ~SBBM() { Destroy(); }
    void Destroy() {
        if (pBitmap && bitmapMain) {
            bitmapMain->ReleaseBitmap(pBitmap);
            pBitmap = nullptr;
        }
        pHLObj = NULL;
        pBitmap = 0;
        Size.x = Size.y = 0;
    }
    SBBM(const SBBM& o) {
        pHLObj = o.pHLObj;
        pBitmap = o.pBitmap;
        Size = o.Size;
        if (pBitmap) {
            pBitmap->IncRef();
        }
    }
    SBBM& operator=(const SBBM& o) {
        Destroy();
        pHLObj = o.pHLObj;
        pBitmap = o.pBitmap;
        Size = o.Size;
        if (pBitmap) {
            pBitmap->IncRef();
        }
        return *this;
    }
    SBBM(SBBM&& o) {
        pHLObj = std::exchange(o.pHLObj, nullptr);
        pBitmap = std::exchange(o.pBitmap, nullptr);
        Size = std::move(o.Size);
    }
    SBBM& operator=(SBBM&& o) {
        Destroy();
        std::swap(pHLObj, o.pHLObj);
        std::swap(pBitmap, o.pBitmap);
        std::swap(Size, o.Size);
        return *this;
    }

    void ReSize(SLONG xs, SLONG ys) {
        ReSize(XY(xs, ys));
        if (pBitmap) {
            SBBM::Size.x = pBitmap->GetXSize();
            SBBM::Size.y = pBitmap->GetYSize();
        } else {
            SBBM::Size.x = 0;
            SBBM::Size.y = 0;
        }
    }
    SLONG ReSize(XY Size, SLONG flags = CREATE_SYSMEM) {
        if (Size != SBBM::Size) {
            Destroy();
            SLONG rc = bitmapMain->CreateBitmap(&pBitmap, Size.x, Size.y, 0, CREATE_USECOLORKEY | flags);
            SBBM::Size.x = pBitmap->GetXSize();
            SBBM::Size.y = pBitmap->GetYSize();
            return (rc);
        }
        return (0);
    }
    void ReSize(CHLPool *pHLPool, const CString &graphicID);
    void ReSize(CHLPool *pHLPool, __int64 graphicID);
    void ReSize(GfxLib *gfxLibrary, CString graphicStr);
    void ReSize(GfxLib *gfxLibrary, __int64 graphicID, SLONG flags = CREATE_SYSMEM) {
        Destroy();
        bitmapMain->CreateBitmap(&pBitmap, gfxLibrary, graphicID, CREATE_USECOLORKEY | flags);
        SBBM::Size.x = pBitmap->GetXSize();
        SBBM::Size.y = pBitmap->GetYSize();
    }

    void Line(XY p1, XY p2, BOOL Fat, SB_Hardwarecolor *pColor, SLONG NumColors);

    // Services:
  public:
    // Base Services:
    BOOL SetPixel(XY Pos, dword Color) {
        pBitmap->SetPixel(Pos.x, Pos.y, Color);
        return (TRUE);
    }
    BOOL SetPixel(SLONG x, SLONG y, dword Color) {
        return (SetPixel(XY(x, y), Color));
        return (TRUE);
    }
    dword GetPixel(SLONG x, SLONG y) { return (pBitmap->GetPixel(x, y)); }
    dword Clear(dword color = 0) {
        Bench.ClearTime.Start();
        pBitmap->Clear((SB_Hardwarecolor)color);
        Bench.ClearTime.Stop();
        return (TRUE);
    }
    BOOL FillWith(dword Color) {
        Bench.ClearTime.Start();
        pBitmap->Clear((SB_Hardwarecolor)Color);
        Bench.ClearTime.Stop();
        return (TRUE);
    }
    BOOL Line(const XY &From, const XY &To, dword Color) {
        pBitmap->Line(From.x, From.y, To.x, To.y, Color);
        return (TRUE);
    }
    BOOL Line(SLONG x1, SLONG y1, SLONG x2, SLONG y2, dword Color) {
        return (Line(XY(x1, y1), XY(x2, y2), Color));
        return (TRUE);
    }
    BOOL Line(const XY &From, const XY &To, SB_Hardwarecolor Color) {
        pBitmap->Line(From.x, From.y, To.x, To.y, Color);
        return (TRUE);
    }
    BOOL Line(SLONG x1, SLONG y1, SLONG x2, SLONG y2, SB_Hardwarecolor Color) {
        return (Line(XY(x1, y1), XY(x2, y2), Color));
        return (TRUE);
    }
    BOOL FatLine(SLONG x1, SLONG y1, SLONG x2, SLONG y2, SB_Hardwarecolor Color) {
        Line(XY(x1 + 1, y1), XY(x2 + 1, y2), Color);
        Line(XY(x1, y1 + 1), XY(x2, y2 + 1), Color);
        Line(XY(x1 + 1, y1 + 1), XY(x2 + 1, y2 + 1), Color);
        return (Line(XY(x1, y1), XY(x2, y2), Color));
    }

    BOOL ShiftUp(SLONG y);

    // Blit Services:
    BOOL BlitFrom(SBBM &TecBitmap) { return (BlitFrom(TecBitmap, XY(0l, 0l))); }
    BOOL BlitFrom(SBBM &TecBitmap, XY Target) const;
    BOOL BlitFrom(SBBM &TecBitmap, SLONG tx, SLONG ty) { return (BlitFrom(TecBitmap, XY(tx, ty))); }
    BOOL BlitFromT(SBBM &TecBitmap) { return (BlitFromT(TecBitmap, XY(0l, 0l))); }
    BOOL BlitFromT(SBBM &TecBitmap, XY Target) const;
    BOOL BlitFromT(SBBM &TecBitmap, SLONG tx, SLONG ty) { return (BlitFromT(TecBitmap, XY(tx, ty))); }
    static BOOL BlitFrom(SBBM &TecBitmap, XY p1, XY p2);
    static BOOL BlitFrom(SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2);
    static BOOL BlitFromT(SBBM &TecBitmap, XY p1, XY p2);
    static BOOL BlitFromT(SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2);
    BOOL BlitPartFrom(SBBM &TecBitmap, XY Target, const XY &p1, const XY &p2) const;
    BOOL BlitPartFrom(SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG x1, SLONG y1, SLONG x2, SLONG y2) {
        return (BlitPartFrom(TecBitmap, XY(tx, ty), XY(x1, y1), XY(x2, y2)));
    }
    BOOL BlitPartFromT(SBBM &TecBitmap, XY Target, const XY &p1, const XY &p2) const;
    BOOL BlitPartFromT(SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG x1, SLONG y1, SLONG x2, SLONG y2) {
        return (BlitPartFromT(TecBitmap, XY(tx, ty), XY(x1, y1), XY(x2, y2)));
    }

    BOOL BlitFrom(SBBM &TecBitmap, const CRect &r1, const XY &dest) const;
    static BOOL TextOut(SLONG x, SLONG y, COLORREF Back, COLORREF Front, const CString &String);
    SLONG PrintAt(const char *Str, SB_CFont &Font, SLONG Flags) { return (PrintAt(Str, Font, Flags, XY(0, 0), Size - XY(1, 1))); }
    SLONG PrintAt(const char *Str, SB_CFont &Font, SLONG Flags, const XY &p1, const XY &p2) const;
    SLONG PrintAt(const char *Str, SB_CFont &Font, SLONG Flags, SLONG x1, SLONG y1, SLONG x2, SLONG y2) {
        return (PrintAt(Str, Font, Flags, XY(x1, y1), XY(x2, y2)));
    }
    SLONG TryPrintAt(const char *Str, SB_CFont &Font, SLONG Flags) { return (TryPrintAt(Str, Font, Flags, XY(0, 0), Size - XY(1, 1))); }
    SLONG TryPrintAt(const char *Str, SB_CFont &Font, SLONG Flags, const XY &p1, const XY &p2) const;
    SLONG TryPrintAt(const char *Str, SB_CFont &Font, SLONG Flags, SLONG x1, SLONG y1, SLONG x2, SLONG y2) {
        return (TryPrintAt(Str, Font, Flags, XY(x1, y1), XY(x2, y2)));
    }
    /*ONG PrintAt (const char *Str, const TEXT_STYLE &StdStyle, UBYTE Flags, SLONG Zeilenabstand=0) { return(PrintAt (Str, StdStyle, Flags, XY(0,0),
      Size-XY(1,1), Zeilenabstand)); } SLONG PrintAt (const char *Str, const TEXT_STYLE &StdStyle, UBYTE Flags, const XY &p1, const XY &p2, SLONG
      Zeilenabstand=0); SLONG PrintAt (const char *Str, const TEXT_STYLE &StdStyle, UBYTE Flags, SLONG x1, SLONG y1, SLONG x2, SLONG y2, SLONG Zeilenabstand=0)
      { return (PrintAt (Str, StdStyle, Flags, XY(x1,y1), XY(x2,y2), Zeilenabstand)); } SLONG PrintAt (const char *Str, TEXT_STYLE *StyleStr, UBYTE Flags, SLONG
      Zeilenabstand=0) { return (PrintAt (Str, StyleStr, Flags, XY(0,0), Size-XY(1,1), Zeilenabstand)); } SLONG PrintAt (const char *Str, TEXT_STYLE *StyleStr,
      UBYTE Flags, XY p1, XY p2, SLONG Zeilenabstand=0);
      SLONG PrintAt (const char *Str, TEXT_STYLE *StyleStr, UBYTE Flags, SLONG x1, SLONG y1, SLONG x2, SLONG y2, SLONG Zeilenabstand=0) { return (PrintAt (Str,
      StyleStr, Flags, XY(x1,y1), XY(x2,y2), Zeilenabstand)); }*/

    friend class SBPRIMARYBM;
};

// Diverse Bitmaps, einfach gesammelt
class SBBMS : public BUFFER_V<SBBM> {
  public:
    SBBMS() = default;
    void Destroy() { BUFFER_V<SBBM>::ReSize(0); }
    void ReSize(SLONG anz) { BUFFER_V<SBBM>::ReSize(anz); }
    void ReSize(GfxLib *gfxLibrary, __int64 graphicID, ...);
    void ReSize(CHLPool *pPool, const BUFFER_V<__int64> &graphicIds);
    void ReSize(GfxLib *gfxLibrary, const BUFFER_V<__int64> &graphicIds, SLONG flags = CREATE_SYSMEM);
    void ReSize(GfxLib *gfxLibrary, const CString &graphicstr);
    void ReSize(GfxLib *gfxLibrary, const CString &graphicstr, SLONG Anzahl, SLONG flags = CREATE_SYSMEM);
};

// Definition einer sichtbaren (Primary) Bitmap:
class SBPRIMARYBM {
  public:
    SBBM Offscreen;
    SB_CPrimaryBitmap PrimaryBm;
    BOOL Fullscreen{};

  public:
    XY Size; // Größe der Bitmap

  public:
    SBPRIMARYBM() = default;
    void ReSize(SDL_Window *Wnd, BOOL Fullscreen, const XY &Resolution);

    void ReSizePartB(SDL_Window * /*Wnd*/, BOOL /*Fullscreen*/, const XY &/*Resolution*/) {
        // if (!Fullscreen) Offscreen.ReSize (Resolution);
    }
    void Flip(SLONG WindowX, SLONG WindowsY, BOOL ShowFPS = FALSE) { Flip(XY(WindowX, WindowsY), ShowFPS); }
    void Flip(XY WindowPos, BOOL ShowFPS = FALSE);
    BOOL IsFullscreen(void) { return (Fullscreen); }
    void SetVSync(BOOL toggle) { PrimaryBm.SetVSync(toggle); }
    dword Clear(dword color = 0);

    // Blit Services:
    BOOL BlitFrom(SBBM &TecBitmap) { return (BlitFrom(TecBitmap, XY(0l, 0l))); }
    BOOL BlitFrom(SBBM &TecBitmap, const CRect &r1, const XY &dest);
    BOOL BlitFrom(SBBM &TecBitmap, XY Target);
    BOOL BlitFrom(SBBM &TecBitmap, SLONG tx, SLONG ty) { return (BlitFrom(TecBitmap, XY(tx, ty))); }
    BOOL BlitFromT(SBBM &TecBitmap) { return (BlitFromT(TecBitmap, XY(0l, 0l))); }
    BOOL FlipBlitFromT(SBBM &TecBitmap, XY Target);
    BOOL BlitFromT(SBBM &TecBitmap, XY Target);
    BOOL BlitFromT(SBBM &TecBitmap, const CRect &r1, const XY &dest);
    BOOL BlitFromT(SBBM &TecBitmap, SLONG tx, SLONG ty) { return (BlitFromT(TecBitmap, XY(tx, ty))); }
    static BOOL BlitFrom(SBBM &TecBitmap, XY p1, XY p2);
    static BOOL BlitFrom(SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2);
    static BOOL BlitFromT(SBBM &TecBitmap, XY p1, XY p2);
    static BOOL BlitFromT(SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2);
    BOOL TextOut(SLONG x, SLONG y, COLORREF Back, COLORREF Front, const CString &String);
};

#endif
