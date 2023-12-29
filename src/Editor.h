//============================================================================================
// Editor.h : Der Flugzeugeditor
//============================================================================================

#define NUM_PLANE_BODY 5    // Anzahl der Rümpfe
#define NUM_PLANE_COCKPIT 5 // Anzahl der Cockpits
#define NUM_PLANE_HECK 7    // Anzahl der Hecks
#define NUM_PLANE_LWING 6   // Anzahl der hinteren Flügen
#define NUM_PLANE_MOT 8     // Anzahl der Motoren
#define NUM_PLANE_RWING 6   // Anzahl der vorderen Flügen

//--------------------------------------------------------------------------------------------
// Der Verbindungspunkt von 2 PlaneParts
//--------------------------------------------------------------------------------------------
class CPlanePartRelation {
  public:
    SLONG Id;             // [csv] Id der Relation
    SLONG FromBuildIndex; // [csv] An dieses Teil wird etwas geklebt
    SLONG ToBuildIndex;   // [csv] ..und zwar dieses Teil hier
    XY Offset2d;         // Offset für die 2d-Ansicht am Flughafen
    XY Offset3d;         // Offset für die 2d-Ansicht im Editor
    SLONG Note1;          // [CSV] Spezielle Anmerkung, z.B. zu auf/Aberwertung
    SLONG Note2;          // [CSV] dito
    SLONG Note3;          // [CSV] dito
    SLONG zAdd;
    SLONG Noise;                // [CSV] Zusätzlicher Lärm
    const char *Slot;          // Dieser Slot wird benötigt (BCHLMR)
    const char *RulesOutSlots; // Und diese Slots werden blockiert

  public:
    CPlanePartRelation(SLONG _Id, ULONG _FromBuildIndex, ULONG _ToBuildIndex, XY _Offset2d, XY _Offset3d, SLONG _Note1, SLONG _Note2, SLONG _Note3, SLONG _zAdd,
                       SLONG _Noise, const char *_Slot, const char *_RulesOutSlots) {
        Id = _Id;
        FromBuildIndex = _FromBuildIndex;
        ToBuildIndex = _ToBuildIndex;
        Offset2d = _Offset2d;
        Offset3d = _Offset3d;
        Note1 = _Note1;
        Note2 = _Note2;
        Note3 = _Note3;
        zAdd = _zAdd;
        Noise = _Noise, Slot = _Slot;
        RulesOutSlots = _RulesOutSlots;
    }

    void FromString(const CString &str);
    CString ToString(void) const;
};

// Das theoretische Teil aus dem Katalog:
class CPlaneBuild {
  public:
    SLONG Id{};               // [csv]
    const char *Shortname{}; // [csv] z.B. B1
    SLONG Cost{};             // [CSV] Soviel kostet das hier
    SLONG Weight{};           // [CSV] Soviel wiegt dieses Teil (Beispiel 149pass=62t 170pass=68t 272pass=148t 440pass=135t 550pass=160t)
    SLONG Power{};            // [CSV] Soviel Power hat es, falls es ein Triebwerk ist
    SLONG Noise{};            // [CSV] Soviel Krach verursacht es
    SLONG Wartung{};          // [CSV] So Wartungsintensiv ist dieses Teil
    SLONG Passagiere{};       // [CSV] Soviele Leute passen in diesen Part
    SLONG Verbrauch{};        // [CSV] Verbrauch in l/h
    SLONG BitmapIndex{};      // Index in das Array mit Bitmaps
    SLONG zPos{};

  public:
    CPlaneBuild() { Shortname = NULL; }

    CPlaneBuild(SLONG _Id, const char *_Shortname, SLONG _Cost, SLONG _Weight, SLONG _Power, SLONG _Noise, SLONG _Wartung, SLONG _Passagiere, SLONG _Verbrauch,
                SLONG _BitmapIndex, SLONG _zPos) {
        Id = _Id;
        Shortname = _Shortname;
        Cost = _Cost;
        Weight = _Weight;
        Power = _Power;
        Noise = _Noise;
        Wartung = _Wartung;
        Passagiere = _Passagiere;
        Verbrauch = _Verbrauch;
        BitmapIndex = _BitmapIndex;
        zPos = _zPos;
    }

    void FromString(const CString &str);
    CString ToString(void) const;
};

class CPlaneBuilds : public ALBUM_V<CPlaneBuild> {
  public:
    CPlaneBuilds() : ALBUM_V<CPlaneBuild>("PlaneBuilds") {}

    ULONG IdFrom(CString ShortName);
};

// Die Notes definieren besondere Flugzeugeigenschaften:
#define NOTE_STD 100
#define NOTE_KAPUTT 101      // Geht oft kaputt
#define NOTE_KAPUTTXL 102    // Geht sehr oft kaputt
#define NOTE_VERBRAUCH 101   // Hoher Verbrauch
#define NOTE_VERBRAUCHXL 102 // Sehr hoher Verbrauch
#define NOTE_SPEED300 103    // Maximale Geschwindigkeit
#define NOTE_SPEED400 104    // Maximale Geschwindigkeit
#define NOTE_SPEED500 105    // Maximale Geschwindigkeit
#define NOTE_SPEED600 106    // Maximale Geschwindigkeit
#define NOTE_SPEED700 107    // Maximale Geschwindigkeit
#define NOTE_SPEED800 108    // Maximale Geschwindigkeit
#define NOTE_PILOT1 109      // Zusätzliche Piloten
#define NOTE_PILOT2 110      // Zusätzliche Piloten
#define NOTE_PILOT3 111      // Zusätzliche Piloten
#define NOTE_PILOT4 112      // Zusätzliche Piloten
#define NOTE_BEGLEITER4 113  // Zusätzliche Begleiter
#define NOTE_BEGLEITER6 114  // Zusätzliche Begleiter
#define NOTE_BEGLEITER8 115  // Zusätzliche Begleiter
#define NOTE_BEGLEITER10 116 // Zusätzliche Begleiter

class CEditor : public CStdRaum {
    // Construction
  public:
    CEditor(BOOL bHandy, ULONG PlayerNum);

    // Data
  public:
    SBFX BackFx;
    SBBMS PartBms;    // Die Parts wie sie verwendet werden
    SBBMS SelPartBms; // Die Parts in der Auswahl unten

    CXPlane Plane;
    CString PlaneFilename;

    XY GripAtPos;
    XY GripAtPosB;
    XY GripAtPos2d;
    XY GripAtPosB2d;
    SLONG GripRelation;
    SLONG GripRelationB{};
    SLONG GripRelationPart{};

    SB_CFont FontBankBlack;
    SB_CFont FontBankRed;
    SB_CFont FontYellow;

    SBBMS ButtonPartLRBms;
    SBBMS ButtonPlaneLRBms;
    SBBMS OtherButtonBms;
    SBBMS MaskenBms;

    SLONG DragDropMode;
    CString PartUnderCursor;      // Das Part was dranklebt oder Leerstring
    CString PartUnderCursorB;     // Der andere Flügel, der ggf. mit dranklebt
    SLONG RelationIdUnderCursor{}; // Für das Snap-In die passende Relation

    bool bBodyOutlined{};    // Ist Body markiert?
    bool bCockpitOutlined{}; // Ist Cockpit markiert?
    bool bHeckOutlined{};    // Ist Heck markiert?
    bool bWingOutlined{};    // Sind Flügel markiert?
    bool bMotorOutlined{};   // Ist Motor markiert?

    bool bAllowB;
    bool bAllowC;
    bool bAllowH;
    bool bAllowW;
    bool bAllowM;

    SLONG sel_b; // Index von 0.. für die aktuelle Wahl des Bodies
    SLONG sel_c; // Index von 0.. für die aktuelle Wahl des Cockpits
    SLONG sel_h; // Index von 0.. für die aktuelle Wahl des Hecks
    SLONG sel_w; // Index von 0.. für die aktuelle Wahl des Flügels
    SLONG sel_m; // Index von 0.. für die aktuelle Wahl des Motors

    SB_CFont FontNormalRed;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CEditor)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void CheckUnusablePart(SLONG iDirection);
    void DeleteCurrent(void);
    void UpdateButtonState(void);
    void DoLButtonWork(UINT nFlags, const CPoint &point);
    virtual ~CEditor();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CEditor)
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void OnPaint();
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    // DECLARE_MESSAGE_MAP()
};

CPlaneBuild &GetPlaneBuild(const CString &Shortname);
SLONG GetPlaneBuildIndex(const CString &Shortname);

/////////////////////////////////////////////////////////////////////////////
