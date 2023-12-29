//============================================================================================
// Insel.h : Die Insel mit den Raketen
//============================================================================================

class CInsel : public CStdRaum {
    // Construction
  public:
    CInsel(BOOL bHandy, ULONG PlayerNum);

    SBBMS VogelBms;
    SLONG VogelOffset;
    SLONG VogelSail;
    SLONG VogelY;
    SLONG LastVogelTime;

    SBBM ShipBm;
    SBBM ShipReflexBm;
    SBBMS ShipWaveBms;
    SLONG ShipOffset;
    BUFFER_V<UBYTE> ShipReflexionMask;

    SBBMS RocketPartBms;       // 10 Parts in 4 Farben
    SBBMS RocketPartReflexBms; // 10 Parts in 4 Farben

    SBBMS FernglasBms;
    SBBM IslandBm;
    SBBM ReflexionBm;      // Allgemeiner Reflex
    SBBM ReflexionInselBm; // Layer: nur Insel-Reflex, kein Wasser
    SBBM FrontBm;

    SBFX WellenFx;

    SLONG BlinkArrowsTimer;

    BUFFER_V<UBYTE> ReflexionMask;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CInsel)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CInsel();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CInsel)
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnPaint();
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    // DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
