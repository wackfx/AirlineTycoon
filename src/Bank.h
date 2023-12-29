//============================================================================================
// Bank.h : Der Bankraum (Header File)
//============================================================================================

class Bank : public CStdRaum {
    // Construction
  public:
    Bank(BOOL bHandy, ULONG PlayerNum);

    SBBM MoneyBm;

    CSmackerPerson SP_Modem;

    CAnimation MonitorAnim;
    CAnimation KameraAnim;
    SLONG CatchFly;
    SLONG FlyCaught;

    SB_CFont FontBankBlack;
    SB_CFont FontBankRed;

    CSmackerPerson SP_Mann;
    CSmackerPerson SP_Frau;
    CSmackerPerson SP_Pflanze;
    CSmackerPerson SP_Fliege;

    // Attributes
  public:
    SLONG CurrentText{};      // Nummer des aktuellen Dialogs
    SLONG CurrentValue{};     // Betrag über den gerade geredet wird..
    SBBM TipBm;               // Fenster für Flugzeug-Tips
    SLONG CurrentAktie;       //Über dieses Flugzeug wir geredet
    SLONG CurrentTip;         // Dieser Tip wird gerade angezeigt
    SLONG KontoType;          //-1=Kein Menü offen; 1=Kredit aufnehmen; 2=Tilgen
    SBBM ZettelBm;
    GfxLib *pMenuLib{};

    // Operations
  public:
    void ReloadBitmaps(void);
    void PaintKontoBm(SBBM &TipBm, SLONG KontoType);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(Bank)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~Bank();

    // Generated message map functions
  protected:
    //{{AFX_MSG(Bank)
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnPaint();
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    // DECLARE_MESSAGE_MAP()
};
