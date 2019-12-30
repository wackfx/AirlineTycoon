//============================================================================================
// Werbung.h : Die Werbeagentur!
//============================================================================================

class CWerbung : public CStdRaum
{
// Construction
public:
	CWerbung(BOOL qHandy, ULONG PlayerNum);

// Operations
public:
   SBBM        TipBm;               //Fenster f�r Flugzeug-Details
   SBBM        DiskBm;
   SBBM        NoSaboBm;
   SLONG       CurrentTip;          //Dieser Tip wird gerade angezeigt

   SBFX        CoffeeFx;
   SBFX        SpratzelFx;

   SBBMS       JahreszeitBms;

   CAnimation  BoyAnim;
   CAnimation  ScreenAnim;
   CAnimation  KaffeeAnim;
   CAnimation  SpratzelAnim;

   CSmackerPerson SP_Mann;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWerbung)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWerbung();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWerbung)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
