//============================================================================================
// Kiosk.h : Das Zeitungskiosk
//============================================================================================

/////////////////////////////////////////////////////////////////////////////
// Kiosk window

class CKiosk : public CStdRaum
{
// Construction
public:
	CKiosk(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   TEXT_STYLE  StyleHeadline;
   SLONG       CurrentTip;
   SBBM        Newspapers[3];          //Die drei Zeitungen
   SBBMS       NewspaperTemplates[3];  //Die drei Zeitungen (Vorlagen)
   SBBMS       NewspaperDefs[3];       //Die Default-Bilder und Artikel
   float       NewspaperZoom[3];       //Die aktuellen Zoom-Zust�nde
   SBBM        SleeperBm;
   SBBM        StinkBm;

   SBFX        SchnarchFx;

   CSmackerPerson SP_Mann;

   void RepaintTip (void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Kiosk)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKiosk();

	// Generated message map functions
protected:
	//{{AFX_MSG(CKiosk)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
