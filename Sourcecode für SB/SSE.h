
/******************************************************************************\
*
* Projekt:	SSE (Spellbound SoundEngine)
* Author:	Armin Gessert
* Begin:		16.12.1996
* Purpose:  Include-Datei
*
\******************************************************************************/

#ifndef _SSE_
#define _SSE_

#include "dsound.h"

#define	MAX_FX_BUFFER			(4)
#define	EVENTS					(2)

#define	IDSB	IDirectSoundBuffer

#define	SSE_OK					NOERROR
#define	SSE_DSOUND_NOINIT		MAKE_DSHRESULT(1000)
#define	SSE_NOTCREATED			MAKE_DSHRESULT(1010)
#define	SSE_ALREADYCREATED	MAKE_DSHRESULT(1020)
#define	SSE_NOFILENAME			MAKE_DSHRESULT(1030)
#define	SSE_CANNOTLOAD			MAKE_DSHRESULT(1040)
#define	SSE_CANNOTREAD			MAKE_DSHRESULT(1050)
#define	SSE_NOTSUPPORTED		MAKE_DSHRESULT(1060)
#define	SSE_SOUNDDISABLED		MAKE_DSHRESULT(1070)
#define	SSE_MUSICDISABLED		MAKE_DSHRESULT(1080)
#define	SSE_MAXFXREACHED		MAKE_DSHRESULT(1090)
#define	SSE_CANNOTPLAY			MAKE_DSHRESULT(1100)
#define	SSE_NOMUSICLOADED		MAKE_DSHRESULT(1110)
#define	SSE_NOSOUNDLOADED		MAKE_DSHRESULT(1120)
#define SSE_INVALIDPARAM		MAKE_DSHRESULT(1130)
#define SSE_CANNOTTEST			MAKE_DSHRESULT(1140)
#define SSE_NORMALMODE			MAKE_DSHRESULT(1150)
#define SSE_COMPATIBLEMODE		MAKE_DSHRESULT(1160)

#define	SSE_MCIERROR			MAKE_DSHRESULT(3000)
#define	SSE_MIDIERR_STOP		MAKE_DSHRESULT(3010)

// Interne Flags
#define	STATE_PAUSED			(0x0001)
#define	STATE_PLAYING			(0x0002)

// Zus�tzliche Flags f�r ::Play()
#define	DSBPLAY_RESUME			(0x10000000)
#define	DSBPLAY_SETPAN			(0x20000000)
#define	DSBPLAY_FIRE			(0x40000000)
#define	DSBPLAY_PRIORITY		(0x01000000)
#define	DSBPLAY_HIGHPRIORITY	(0x02000000)
#define	DSBPLAY_NOSTOP			(0x04000000)
//#define	DSBPLAY_LOOPING		(0x08000000)

class FX;
class MIDI;

typedef struct _mixerBounds
{
	long	lMinimum;
	long	lMaximum;
	DWORD	dwSteps;

} MIXERBOUNDS;

/******************************************************************************\
*
* Notwendige Forward-Deklarationen
*
\******************************************************************************/

class SSE;

/******************************************************************************\
*
* Typen f�r die Objektdaten
*
\******************************************************************************/

typedef struct _DigitalData
{
	SB_CString	file;
	SSE*		   pSSE;
	word		   state;
	bool		   fNoStop;
	dword		   time;
} DigitalData;	

typedef struct _FXData
{
	IDSB*		pBuffer[MAX_FX_BUFFER];	// Secondary buffer(s)
	word		ref;
	dword		bufferSize;					// Die aktuelle Gr��e des DSBuffers

	dword		samplesPerSec;	// Primary buffer frequency
	word		channels;			// Kan�le
	word		bitsPerSample;	// Bits per Sample of mono data

	long		pan;
	long		volume;
} FXData;
	
typedef struct _MusicData
{
	SB_CString	file;
	SSE*			pSSE;
	word			state;
	bool			fNoStop;
	dword			time;
	char			alias[9];
} MusicData;
	
typedef struct _DigiMusicData
{
	IDirectSoundBuffer *		pBuffer;			// Der Sound-Puffer
	IDirectSoundNotify *		pNotify;			// Das Notify-Objekt
	dword						bufferSize;			// Die Gr��e des Buffers
	dword						bufferSecs;			// Wieviele Sekunden werden im Puffer gehalten
	dword						triggerSecs;		// Nach wieviel gespielten Sekunden wird neu geladen

	HANDLE						hFile;				// Das Handle der zu spielenden Datei
	dword						fileSizeRemaining;	// Die noch zu lesende Dateigr��e
	dword						initialFileSize;	// Die Gesamtgr��e der Datei
	dword						initialFilePos;		// (Nach Pause) Startposition im File

	dword						cNotifyStructs;		// Anzahl der zu setzenden Notifies
	DSBPOSITIONNOTIFY	*		pNotifyStructs;		// Die Notify-Strukturen
	HANDLE						hEvent[EVENTS];		// Die Events, die bei den Notifies generiert werden

	unsigned long				hThread;			// Das Handle des Hilfsthreads
	unsigned int				threadId;			// Die ID des Threads
	bool						threadStop;			// Wird der Thread aufgefordet zu stoppen
	bool						threadWaiting;		// Der Thread wartet

	bool						compatibleMode;		// Soll der kompatible (Terratec-freundliche)
													// Modus verwendet werden?

	dword						samplesPerSec;		// Die Samplefrequenz
	word						channels;			// Kan�le
	word						bitsPerSample;		// Bittiefe

	dword						state;
	bool						looped;				// Soll der Sound geloopt werden?
	bool						critical;			// Ist der Thread gerade im kritischen Teil

	long						pan;				// Pan-Wert
	long						volume;				// gemerkte Lautst�rke

} DigiMusicData;

/******************************************************************************\
*
* Class:		SSE (Spellbound SoundEngine)
*
\******************************************************************************/
class SSE
{
	friend class DIGITAL;
	friend class FX;

	friend class MUSIC;
	friend class MIDI;

	friend class DIGIMUSIC;

	public:
		DllExport SSE(void *hWnd, dword samplesPerSec = 22050, word channels = 2, word bitsPerSample = 16, word maxFX = 8);
		DllExport ~SSE();

		DllExport HRESULT	EnableDS ();
		DllExport HRESULT	DisableDS ();
		DllExport HRESULT	CreateFX (FX** ppFX, char* file = 0, dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);
		DllExport HRESULT	CreateMidi (MIDI** ppMidi, char* file = 0);
		DllExport HRESULT	CreateDigimusic (DIGIMUSIC** ppDigimusic, char* file = 0, dword bufferSecs = 4, dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);

		DllExport void		Activate(bool fActivate, bool fPlayAgain = true);
		DllExport bool		IsDSInit() { return (_pDS != 0); }
		DllExport IDirectSound* GetDD() { return _pDS; }
		DllExport IDSB*	GetPrimaryBuffer() { return _pBuffer; }

		DllExport dword	GetFrequency() { return _samplesPerSec; }

		DllExport HRESULT GetCaps(DSCAPS* pDSCaps);

		DllExport HRESULT	EnableSound (bool fSoundEnabled);
		DllExport bool		IsSoundEnabled() { return _fSoundEnabled; }
		DllExport void		SetMaxSound(word fx) { _maxSound = fx; }
		DllExport word		GetMaxSound()		{ return _maxSound; }
		DllExport word		GetSoundPlaying();
		DllExport void		StopSound();

		DllExport HRESULT	EnableMusic(bool fMusicEnabled	);
		DllExport bool		IsMusicEnabled() { return _fMusicEnabled; }
		DllExport void		StopMusic();

		// Lautst�rkefunktionen

		DllExport HRESULT	SetMusicVolume(long volume);
		DllExport HRESULT	GetMusicVolume(long* pVolume);

		DllExport HRESULT	SetSoundVolume(long volume);
		DllExport HRESULT	GetSoundVolume(long* pVolume);

		DllExport HRESULT	SetMixerVolume (char * device, long volume);
		DllExport HRESULT	GetMixerVolume(char * device, long* pVolume, MIXERBOUNDS* pMB = NULL);


		DllExport void		MusicCallback(WPARAM wParam, LPARAM lParam);

		DllExport bool		SetMusicList(char* path, char* files);
		DllExport void		ClearMusicList();

		DllExport void		SwapChannels(bool fSwap)	{ _swapChannels = fSwap; }
		DllExport bool		IsSwapChannels()				{ return _swapChannels; }

		DllExport HWND		GetWindow()						{ return _hWnd; }

	protected:
		SB_CString	GetNextFileFromPlaylist();
		HRESULT	   CreateSoundBuffer (IDSB** ppBuffer, dword size, dword samplesPerSecond, word channels, word bitsPerSample, dword flags);
		HRESULT	   DuplicateSoundBuffer (IDSB* lpDsbOriginal, IDSB** lplpDsbDuplicate);

	protected:
		HWND		_hWnd;
		dword		_samplesPerSec;	// Primary buffer frequency
		word		_channels;			// Kan�le
		word		_bitsPerSample;	// Bits per Sample of mono data
		word		_maxSound;			// Anz. der Samples, die gleichzeitig gespielt werden d�rfen
		bool		_swapChannels;

		IDirectSound* _pDS;			// DirectSound-Object
		IDSB*		_pBuffer;			// Primary buffer
	
		SB_CXList	_soundObjList;		// Liste der FX-Objekte
		SB_CXList	_musicObjList;		// Liste der Midi-Objekte

		bool		_fSoundEnabled;
		bool		_fMusicEnabled;

		MUSIC*	_playingMusicObj;	// Das aktuell gespielte Music-Objekt

		SB_CString	_musicListPath;	// Pfad auf Musik-Dateien
		char*		   _pMusicListFiles;	// Liste der Musik-Dateien (00-terminiert)
		char*		   _pMusicListPos;	// Ptr. auf den aktuellen Eintrag in der Musik-Liste
};

/******************************************************************************\
*
* Class:	DIGITAL
* Desc:	Einheitliche Schnittstelle f�r Samples u.s.w.
*
\******************************************************************************/
class DIGITAL
{
	friend class SSE;

	protected:
		DIGITAL() { ZeroMemory(&_digitalData,sizeof(_digitalData)); }
		virtual	~DIGITAL() {};
		virtual word GetInternalState() { return _digitalData.state; }
		virtual	bool	StopPriority (dword flags) = 0;

	public:
		virtual  long Release() = 0;
		virtual	HRESULT Play(dword dwFlags = 0, long pan = 0) = 0;
		virtual	HRESULT Stop() = 0;
		virtual	HRESULT Pause() { return Stop(); }
		virtual	HRESULT Resume() = 0;
		virtual	HRESULT GetVolume (long* pVolume) = 0;
		virtual	HRESULT SetVolume (long volume) = 0;
		virtual	HRESULT GetPan (long* pPan) = 0;
		virtual	HRESULT SetPan (long pan) = 0;
		virtual	HRESULT Load (char* file = NULL) = 0;
		virtual	HRESULT Free () = 0;

		virtual	HRESULT GetStatus(dword* pStatus) = 0;
		virtual	word	CountPlaying() = 0;
		virtual	void	SetFormat (dword samplesPerSec, word channels, word bitsPerSample) = 0;

	protected:
		DigitalData _digitalData;
};

/******************************************************************************\
*
* Class:		FX (Soundeffekte)
*
\******************************************************************************/
class FX : public DIGITAL
{
	friend class SSE;

	protected:
		FX();
		virtual	~FX();
		HRESULT	Create(SSE* pSSE, char* file, dword samplesPerSec, word channels, word bitsPerSample);
		virtual	bool	StopPriority (dword flags);

	public:
		virtual  long Release();
		virtual	HRESULT Play(dword dwFlags = 0, long pan = 0);
		virtual	HRESULT Stop();
		virtual	HRESULT Resume();
		virtual	HRESULT GetVolume (long* pVolume);
		virtual	HRESULT SetVolume (long volume);
		virtual	HRESULT GetPan (long* pPan);
		virtual	HRESULT SetPan (long pan);
		virtual	HRESULT Load (char* file = NULL);
		virtual	HRESULT Fusion (const FX **Fx, long NumFx);
		virtual	HRESULT Fusion (const FX *Fx, long *Von, long *Bis, long NumFx);
		virtual	HRESULT Tokenize (__int64 Token, long *Von, long *Bis, long &rcAnzahl);
		virtual	FX    **Tokenize (__int64 Token, long &rcAnzahl);
		virtual	HRESULT Free ();

		virtual	HRESULT GetStatus(dword* pStatus);
		virtual	HRESULT IsMouthOpen(long PreTime);
		virtual	word	CountPlaying();
		virtual	void	SetFormat (dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);
      long  GetByteLength (void) { return (_fxData.bufferSize); }

	protected:
		FXData	_fxData;
};

/******************************************************************************\
*
* Class:	MUSIC
* Desc:	Einheitliche Schnittstelle f�r Midi, CD u.s.w.
*
\******************************************************************************/
class MUSIC
{
	friend class SSE;

	protected:
		MUSIC() { ZeroMemory(&_musicData,sizeof(_musicData)); }
		virtual	~MUSIC() {};
		virtual word GetInternalState() { return _musicData.state; }
		virtual	bool	StopPriority (dword flags) = 0;

	public:
		virtual  long Release() = 0;
		DllExport virtual	HRESULT Play(dword dwFlags = 0, long pan = 0) = 0;
		virtual	HRESULT Stop() = 0;
		virtual	HRESULT Pause() { return Stop(); }
		virtual	HRESULT Resume() = 0;
		virtual	HRESULT GetVolume (long* pVolume) = 0;
		virtual	HRESULT SetVolume (long volume) = 0;
		virtual	HRESULT GetPan (long* pPan) = 0;
		virtual	HRESULT SetPan (long pan) = 0;
		virtual	HRESULT Load (char* file = NULL) = 0;
		virtual	HRESULT Free () = 0;

		virtual	HRESULT GetStatus(dword* pStatus) = 0;
		virtual	word	CountPlaying() = 0;

	protected:
		MusicData _musicData;
};

/******************************************************************************\
*
* Class:		MIDI (Hintergrundmusik)
*
\******************************************************************************/
class MIDI : public MUSIC
{
	friend class SSE;

	protected:
		MIDI();
		virtual	~MIDI();
		HRESULT	Create(SSE* pSSE, char* file);
		virtual	bool	StopPriority (dword flags);

	public:
		virtual  long Release();
		DllExport virtual	HRESULT Play(dword dwFlags = 0, long pan = 0);
		virtual	HRESULT Stop();
		virtual	HRESULT Resume();
		virtual	HRESULT GetVolume (long* pVolume);
		virtual	HRESULT SetVolume (long volume);
		virtual	HRESULT GetPan (long* pPan);
		virtual	HRESULT SetPan (long pan);
		virtual	HRESULT Load (char* file = NULL);
		virtual	HRESULT Free ();

		virtual	HRESULT GetStatus(dword* pStatus);
		virtual	word	CountPlaying();

	protected:
		dword		_deviceId;
};
/******************************************************************************\
*
* Class:		DigiMusic (Digitale Hintergrundmusik)
*
\******************************************************************************/

class DIGIMUSIC : public DIGITAL,public MUSIC
{
	friend SSE;
	friend unsigned int WINAPI HandleNotifications(void * lParam); 
	friend unsigned int WINAPI CompatiblePlay(LPVOID ptr);
	friend unsigned int WINAPI CompatibilityCheck(LPVOID ptr);

	protected:
		DIGIMUSIC() { ZeroMemory(&_dmData,sizeof(_dmData)); }

		virtual	~DIGIMUSIC();
		HRESULT	Create(SSE* pSSE, char* file,dword buffersSecs,dword samplesPerSec, word channels, word bitsPerSample);
		HRESULT DIGIMUSIC::CompatibleModeRequired();

		virtual	word	GetInternalState() { return word(_dmData.state); }
		virtual	bool	StopPriority (dword flags);

	public:
		virtual  long Release();
		DllExport virtual	HRESULT Play(dword dwFlags = 0, long pan = 0);
		virtual	HRESULT Stop();
		virtual	HRESULT Pause();
		virtual	HRESULT Resume();
		virtual	HRESULT GetVolume (long* pVolume);
		virtual	HRESULT SetVolume (long volume);
		virtual	HRESULT GetPan (long* pPan);
		virtual	HRESULT SetPan (long pan);
		virtual	HRESULT Load (char* file = NULL);
		virtual	HRESULT Free ();

		virtual	HRESULT GetStatus(dword* pStatus);
		virtual	word	CountPlaying();
		virtual	void	SetFormat (dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);

		virtual void SetCompatibleMode() { _dmData.compatibleMode = true; }
		virtual void SetNormalMode() { _dmData.compatibleMode = false; }

	protected:
		virtual	HRESULT SetupNotifications();
//		dword		HandleNotifications(void * lParam);

	protected:
		DigiMusicData	_dmData;
};

#endif
