//============================================================================================
// Globe.h : header file
//============================================================================================
class CGlobe : public CPlaner {
    // Construction:
  public:
    CGlobe(BOOL bHandy, SLONG PlayerNum);

    // Attributes:
  public:
    SBFX QuietschFX;
    SLONG Quietsching;

    // Die Ikonen & Bitmaps:
    GfxLib *pGLibIcons[6]{};
    SBBMS IconBms[6];

    // Sonstiges:
    GfxLib *pGLibGlobe{}; // Library für den Raum: Animationen der Objekte; wird immer geladen
    GfxLib *pGLibStd{};   // Library für den Globe: Filofax, Register, ...

    GfxLib *pGLibDark;
    SBBM DarkBm;
    SBBM LockBm;
    SLONG OfficeState;

    // Bitmaps:
    SBBM GlasLeer;
    SBBM Back, Next;              // Eselsohren für vor und zurück
    SBBMS Index1, IndexA;         // Das linke und das rechte Indexsystem
    SBBM Inhalt;                  // Die Fahne für den Inhalt
    SBBM Filofax;                 // Das Filofax selbst
    SBBM FiloEdge;                // Das Filofax in der Ecke
    SBBMS FiloTops;               // Das oben umgeblätterte Filofax
    SBBM Karte;                   // Die Weltkarte
    SBBM TurnLeftBm, TurnRightBm; // Dreht die Weltkugel
    SBBMS TimeTables;             // Die Hintergrundbilder für den Flugplan

    BOOL Copyprotection;

    // Overrides
  public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGlobe)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CGlobe();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CGlobe)
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void OnPaint();
    virtual void OnRButtonUp(UINT nFlags, CPoint point);
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual void OnChar(UINT nChar, UINT a, UINT b);
    virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
    //}}AFX_MSG
    // DECLARE_MESSAGE_MAP()
};
