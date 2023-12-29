
//============================================================================================
// Statistk.h : Der Statistik-Screen
//============================================================================================

class CStatButton {
  public:
    SLONG Id{};                   // Frei wählbar, aber bitte immer eindeutig
    SLONG HelpId{};               // ToolTipID
    CRect HotArea{};              // MouseArea, wo der Button gehighlighted wird
    XY BitmapOffset{};            // Hierhin wird die Button-Bitmap geblittet
    XY *BitmapOffset2{};          // Referenzmöglichkeit für bewegte Buttons
    SBBM *BitmapNormal{nullptr};  // Normale Bitmap oder NULL
    SBBM *BitmapHi{nullptr};      // Bitmap fürs Highlight oder NULL
    SBBM *BitmapClicked{nullptr}; // Bitmap fürs Geclickte oder NULL
    SBBM *BitmapSuperHi{nullptr}; // Nur für Toggle-Buttons: Highlight, wenn geklickt
    BOOL IsToggle{};              // Ist es ein Toggle-Button?
    DWORD LastClicked{};          // Wann wurde er zum letzten Mal geklickt (intern)

  public:
    void ReSize(SLONG Id, SLONG HelpId, CRect HotArea, XY BitmapOffset, XY *BitmapOffset2, SBBM *BitmapNormal, SBBM *BitmapHi, SBBM *BitmapClicked,
                SBBM *BitmapSuperHi, BOOL IsToggle);
};

#define LINE_DISTANCE 10 // Abstand der Zeilen

#define TYP_LINEFEED 0   // Leerzeile
#define TYP_GROUP 1      // Gruppe
#define TYP_SUM_CURR 2   // Summe einer Gruppe
#define TYP_SUM_CURR_N 3 // Summe einer Gruppe (-)
#define TYP_SUM_DIFF 4   // Differenz der Summen
#define TYP_SHAREVALUE 5 // Wert aller Aktien zusammen

#define TYP_VALUE 20          // Normale Zahl
#define TYP_CURRENCY 21       // Währung
#define TYP_PERCENT 22        // Prozent
#define TYP_SINGLE_PERCENT 23 // Prozent, aber nicht in Abhängigkeit von der Vorvariable
#define TYP_UNAVAILABLE 24    // ???, weil der Berater fehlt

typedef struct tagItem {
    bool visible{};
    SWORD textId{};
    SWORD define{};
    SWORD typOfItem{};
    SWORD beraterSkill{};
    SWORD beraterSkillInfo{};
} ITEM;

class CStatistik : public CStdRaum {
    // Construction
  public:
    CStatistik(BOOL bHandy, ULONG PlayerNum);

    BUFFER_V<CStatButton> StatButtons; // Die Statistik Buttons

    SBBM HighlightBar; // Der helle Balken für die optische Auswahl
    SBBM TopSaver;     // Verhindert, daß der obere Rand von den Zoomicons überschrien wird

    SBBM DropDownBackgroundBm; // Die Hintergrundbitmap
    SBBM DropDownBm;           // Die Hintergrundbitmap mit Linien drauf
    SBBMS UpDownArrows;        // Die Buttons zum runter/raufscrollen

    SBBM TextTableBm; // Die Bitmap mit den Texten (kann von der Drop-Down Liste überdeckt werden)

    SBBMS LupeLogoBms;    // Die Lupe mit dem Firmenlogo
    SBBMS LupeZoomBms;    // Die Lupe mit +/s
    SBBMS ButtonGeldBms;  // Die Buttons mit dem Geld
    SBBMS ButtonRouteBms; // Die Buttons mit der Route

    SBBMS ExitBms; // Das Exit-Schild
    SBBMS LogoBms; // Die Vier großen Spielerlogos in 3 varianten
    SBBMS PobelBms;

    XY DropDownPos;
    SLONG DropDownSpeed;

    SBBM Haeckchen;

    SB_CFont StatFonts[5]; // 0=Schwarz, 1-4=Farben der Spieler

    // Diese Werte am besten in der Registry
    // speichern.
    bool _fGraphVisible;                  // true -> Der Graph ist sichtbar, ansonsten die schnöden Zahlen
    std::array<bool, 4> _playerMask{};    // Diese Spieler wurden zur Ansicht ausgewählt
    BYTE _group;                          // Die angewählte Gruppe (*0=Finanzen, 1=?, 2=?)
    SLONG _days;                          // Anzahl der darzustellenden Tage
    SLONG _newDays;                       // Für eine Animation
    std::array<std::array<ITEM, STAT_MAX_ITEMS>, STAT_MAX_GROUPS> _iArray{};

    // Temporäre Werte
    short _selectedItem;
    short _oldSelectedItem;
    DOUBLE _yAxis{};
    DOUBLE _xGraph{};
    DOUBLE _yGraph{};

    bool _fRepaint;

    // Attributes
  public:
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CStatistik)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CStatistik();

    // Generated message map functions
  protected:
    void RepaintGraphWindow(void);
    void CalcGraph(void);

    void RepaintTextWindow(void);

    //{{AFX_MSG(CStatistik)
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnPaint();
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    // DECLARE_MESSAGE_MAP()
};
