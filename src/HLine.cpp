//============================================================================================
// HLine.cpp : Die HLine-Engine
//============================================================================================
// Link: "HLine.h"
//============================================================================================
#include "StdAfx.h"
#include "HLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------
// Blittet ein HL-Objekt an eine Stelle, bis jetzt aber ohne Clipping
//--------------------------------------------------------------------------------------------
void CHLObj::BlitAt(SB_CBitmapCore *pBitmap, XY Target) {
#ifdef ENABLE_ASM
    static char FirstTime;
    static ULONG JumpBuffer[8]; // Für Wiederholungen
#endif

    SLONG cx = 0;
    SLONG cy = 0;
    SLONG count = 0;

    CRect ClipRect = pBitmap->GetClipRect();

    // Knock-Out für horizontales Clipping?
    if (Target.x >= ClipRect.right || Target.x + Size.x <= ClipRect.left) {
        return;
    }

    // Pool und Sub-Pool vorbereiten:
    pHLPool->Load();

    SB_CBitmapKey Key(*pBitmap);
    if (Key.Bitmap == nullptr) {
        return;
    }
    auto *bm = ((static_cast<UWORD *>(Key.Bitmap)) + Target.x + Target.y * (Key.lPitch >> 1));

    SLONG Min = 0;
    SLONG Max = 0; // Vertikales Clipping
    Min = min(HLineEntries.AnzEntries(), max(0, ClipRect.top - Target.y));
    Max = min(HLineEntries.AnzEntries(), ClipRect.bottom - Target.y);

    ClipRect.left -= Target.x;
    ClipRect.right -= Target.x;

    bm += Key.lPitch / 2 * Min;

    // count für die übersprungenen Zeilen initialisieren
    for (count = cy = 0; cy < Min; cy++) {
        count += HLineEntries[cy];
    }

    for (cy = Min; cy < Max; cy++) {
        for (cx = HLineEntries[cy]; cx > 0; cx--) {
            CHLGene &qHLGene = HLines[count];

            UWORD *target = (static_cast<UWORD *>(bm)) + qHLGene.Offset;
            UBYTE *source = qHLGene.pPixel;
            SLONG anz = qHLGene.Anz;
#ifdef ENABLE_ASM
            UWORD *table = pHLPool->PaletteMapper;
#endif

            if (qHLGene.Anz <= 4) {
                source = reinterpret_cast<UBYTE *>(&qHLGene.pPixel);
            } else {
                source = qHLGene.pPixel;
            }

            if (qHLGene.Offset < ClipRect.left) {
                source += ClipRect.left - qHLGene.Offset;
                target += ClipRect.left - qHLGene.Offset;
                anz -= ClipRect.left - SLONG(qHLGene.Offset);
            }
            if (SLONG(qHLGene.Offset) + anz > ClipRect.right) {
                anz = ClipRect.right - SLONG(qHLGene.Offset);
            }

#ifndef ENABLE_ASM
            if (anz > 0) {
                for (SLONG c = anz - 1; c >= 0; c--) {
                    target[c] = pHLPool->PaletteMapper[static_cast<SLONG>(source[c])];
                }
            }
#else
            if (anz > 0)
                _asm {
                    // Heute schon initialisiert?
                    mov     al, FirstTime
                        test    al, al
                        jnz     AlreadyInitialized

                        // Nein, wir führen also die First-Time Initialisierung durch:
                        lea     edx, JumpBuffer

                        lea     eax, CopyWords08
                        mov     [edx+0],eax
                        lea     eax, CopyWords01
                        mov     [edx+4],eax
                        lea     eax, CopyWords02
                        mov     [edx+8],eax
                        lea     eax, CopyWords03
                        mov     [edx+12],eax
                        lea     eax, CopyWords04
                        mov     [edx+16],eax
                        lea     eax, CopyWords05
                        mov     [edx+20],eax
                        lea     eax, CopyWords06
                        mov     [edx+24],eax
                        lea     eax, CopyWords07
                        mov     [edx+28],eax
                        lea     eax, CopyWords08

                        mov     al, 1
                        mov     FirstTime,1
                    // Die First-Time Initialisierung ist abgeschlossen:

                        AlreadyInitialized:
                        mov     esi, source
                        mov     edi, target
                        mov     eax, anz
                        mov     edx, table

                        xor     ecx, ecx

                        and     eax, 7
                        mov     ebx, JumpBuffer[eax*4]       ;Load Label-Table
                        jmp     ebx

                                     // Gerade Anzahl kopieren:
                        CopyWords08: mov     eax, 8

                        mov     cl, [esi+7]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+6]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+12], ebx
                        CopyWords06: mov     cl, [esi+5]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+4]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+8], ebx
                        CopyWords04: mov     cl, [esi+3]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+2]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+4], ebx
                        CopyWords02: mov     cl, [esi+1]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+0]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+0], ebx

                        lea     esi, [esi+eax]
                        lea     edi, [edi+eax*2]

                        sub     anz, eax
                        jz      game_over

                        mov     eax, anz
                        and     eax, 7
                        mov     ebx, JumpBuffer[eax*4]       ;Load Label-Table
                        jmp     ebx

                                     // Ungerade Anzahl kopieren:
                        CopyWords07: mov     cl, [esi+6]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+5]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+10], ebx
                        CopyWords05: mov     cl, [esi+4]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+3]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+6], ebx
                        CopyWords03: mov     cl, [esi+2]
                        mov     bx, WORD PTR [edx+ecx*2]
                        shl     ebx, 16
                        mov     cl, [esi+1]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+2], ebx
                        CopyWords01: mov     cl, [esi+0]
                        mov     bx, WORD PTR [edx+ecx*2]
                        mov     [edi+0], bx

                        lea     esi, [esi+eax]
                        lea     edi, [edi+eax*2]

                        sub     anz, eax
                        jz      game_over

                        mov     eax, anz
                        and     eax, 7
                        mov     ebx, JumpBuffer[eax*4]       ;Load Label-Table
                        jmp     ebx

                        game_over:
                }
#endif
            count++;
        }
        bm += Key.lPitch / 2;
    }
}

//--------------------------------------------------------------------------------------------
// Blittet und vergrößert:
//--------------------------------------------------------------------------------------------
void CHLObj::BlitLargeAt(SB_CBitmapCore *pBitmap, XY Target) {
    SLONG cx = 0;
    SLONG cy = 0;
    SLONG count = 0;

    CRect ClipRect = pBitmap->GetClipRect();

    // Knock-Out für horizontales Clipping?
    if (Target.x >= ClipRect.right || Target.x + Size.x * 2 <= ClipRect.left) {
        return;
    }

    // Pool und Sub-Pool vorbereiten:
    pHLPool->Load();

    SB_CBitmapKey Key(*pBitmap);
    if (Key.Bitmap == nullptr) {
        return;
    }
    auto *bm = ((static_cast<UWORD *>(Key.Bitmap)) + Target.x + Target.y * (Key.lPitch >> 1));

    SLONG Min = 0;
    SLONG Max = 0; // Vertikales Clipping
    Min = max(0, ClipRect.top - Target.y);
    Max = min(HLineEntries.AnzEntries(), (ClipRect.bottom - Target.y) / 2);

    ClipRect.left -= Target.x;
    ClipRect.right -= Target.x;

    bm += Key.lPitch / 2 * Min;
    count = Min;

    for (cy = Min; cy < Max; cy++) {
        for (cx = HLineEntries[cy]; cx > 0; cx--) {
            CHLGene &qHLGene = HLines[count];

            UWORD *target = (static_cast<UWORD *>(bm)) + qHLGene.Offset * 2;
            UBYTE *source = qHLGene.pPixel;
            SLONG anz = qHLGene.Anz * 2;

            if (qHLGene.Anz <= 4) {
                source = reinterpret_cast<UBYTE *>(&qHLGene.pPixel);
            } else {
                source = qHLGene.pPixel;
            }

            if (qHLGene.Offset * 2 < ClipRect.left) {
                source += (ClipRect.left - qHLGene.Offset * 2) / 2;
                target += ClipRect.left - qHLGene.Offset * 2;
                anz -= ClipRect.left - qHLGene.Offset * 2;
            }
            if (qHLGene.Offset * 2 + anz > ClipRect.right) {
                anz = ClipRect.right - qHLGene.Offset * 2;
            }

            if (anz > 0) {
                for (SLONG c = anz - 1; c >= 0; c--) {
                    target[c + Key.lPitch / 2] = target[c] = pHLPool->PaletteMapper[static_cast<ptrdiff_t>(source[c / 2])];
                }
            }

            count++;
        }
        bm += Key.lPitch / 2 * 2;
    }
}

//--------------------------------------------------------------------------------------------
// Pseudo-Destruktor:
//--------------------------------------------------------------------------------------------
void CHLPool::Destroy() {
    HLObjects.ReSize(0);

    if (pPool != nullptr) {
        delete[] pPool;
        pPool = nullptr;
    }

    PoolSize = 0;
    PoolMaxSize = 0;

    Loaded = 0;

    pHLBasepool1 = nullptr;
    pHLBasepool2 = nullptr;

    BytesReal = BytesCompressed = BytesAdministration = 0;

    Filename.Empty();
}

//--------------------------------------------------------------------------------------------
// Lädt die Bitmap-Daten ohne die eigentlichen Bitmaps...
//--------------------------------------------------------------------------------------------
BOOL CHLPool::PreLoad() {
    // Ggf. Basepool-Objekte laden
    /*if (pHLBasepool1)
      if (!pHLBasepool1->Load ()) return (FALSE);

      if (pHLBasepool2)
      if (!pHLBasepool2->Load ()) return (FALSE);*/

    if (Loaded == 0) {
        CString CompleteFilename = FullFilename((LPCTSTR)Filename, GliPath);

        if (DoesFileExist(CompleteFilename) != 0) {
            TEAKFILE File(CompleteFilename, TEAKFILE_READ);

            File >> PoolSize >> PoolMaxSize;
            File >> BytesReal >> BytesCompressed >> BytesAdministration >> LinesInPool >> LinesRepeated;

            File >> HLObjects;
            for (SLONG c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
                HLObjects[c].pHLPool = this;
            }
        } else {
            HLObjects.ReSize(0);
        }

        Loaded = 1;
    }

    return (FALSE);
}

//--------------------------------------------------------------------------------------------
// Lädt den Pool in den Speicher
//--------------------------------------------------------------------------------------------
BOOL CHLPool::Load() {
    // Ggf. Basepool-Objekte laden
    if (pHLBasepool1 != nullptr) {
        if (pHLBasepool1->Load() == 0) {
            return (FALSE);
        }
    }

    if (pHLBasepool2 != nullptr) {
        if (pHLBasepool2->Load() == 0) {
            return (FALSE);
        }
    }

    if (Loaded != 2) {
        CString CompleteFilename = FullFilename((LPCTSTR)Filename, GliPath);

        if (DoesFileExist(CompleteFilename) != 0) {
            SLONG c = 0;
            SBBM TmpBm(10, 10);
            TEAKFILE File(CompleteFilename, TEAKFILE_READ);

            File >> PoolSize >> PoolMaxSize;
            File >> BytesReal >> BytesCompressed >> BytesAdministration >> LinesInPool >> LinesRepeated;

            // Wurden diese Daten schon vorher geladen? Dann reicht einmal!
            if (HLObjects.AnzEntries() != 0) {
                BUFFER_V<CHLObj> DummyHLObjects;

                File >> DummyHLObjects;
            } else {
                File >> HLObjects;
                for (c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
                    HLObjects[c].pHLPool = this;
                }
            }

            if (PoolMaxSize > 0) {
                pPool = new UBYTE[PoolMaxSize];
                File.Read(pPool, PoolSize);
            } else {
                pPool = new UBYTE[1];
            }

            PaletteMapper.ReSize(256);
            File.Read(reinterpret_cast<UBYTE *>(&PaletteMapper[0]), 512);

            DoBaseObjects();

            // Map colors to graphic card bits 555 or 565:
            for (c = 0; c < 256; c++) {
                ULONG p = PaletteMapper[c];

                p = ((p & 31) << (3)) + (((p >> 6) & 31) << (3 + 8)) + (((p >> 11) & 31) << (3 + 16));

                PaletteMapper[c] = (UWORD)TmpBm.pBitmap->GetHardwarecolor(p);
            }
        }

        Loaded = 2;
    }

    return (TRUE);
}

//--------------------------------------------------------------------------------------------
// Speichert das Objekt:
//--------------------------------------------------------------------------------------------
BOOL CHLPool::Save() {
    if (BytesCompressed != 0) {
        TEAKFILE File(FullFilename((LPCTSTR)Filename, GliPath), TEAKFILE_WRITE);

        UnBaseObjects();

        File << PoolSize << PoolMaxSize;
        File << BytesReal << BytesCompressed << BytesAdministration << LinesInPool << LinesRepeated;

        File << HLObjects;

        if (PoolSize != 0) {
            File.Write(pPool, PoolSize);
        }

        File.Write(reinterpret_cast<UBYTE *>(&PaletteMapper[0]), 512);

        DoBaseObjects();
    }

    return (TRUE);
}

//--------------------------------------------------------------------------------------------
// Wirft den gesammten Pool aus dem Speicher raus:
//--------------------------------------------------------------------------------------------
void CHLPool::Unload() {
    if (pPool != nullptr) {
        UnBaseObjects();

        if (pPool != nullptr) {
            delete[] pPool;
            pPool = nullptr;
        }

        Loaded = min(1, Loaded);

        PoolSize = 0;
        PoolMaxSize = 0;
    }
}

//--------------------------------------------------------------------------------------------
// Konstruirt den Pool anhand einer Datei
//--------------------------------------------------------------------------------------------
void CHLPool::ReSize(const CString &Filename, CHLPool *pHLBasepool1, CHLPool *pHLBasepool2) {
    Destroy(); // Altes Objekt zerstören

    CHLPool::Filename = Filename;
    CHLPool::pHLBasepool1 = pHLBasepool1;
    CHLPool::pHLBasepool2 = pHLBasepool2;
}

//--------------------------------------------------------------------------------------------
// Fügt dem Pool eine neue Bitmap hinzu:
// * Quality : Bildqualität der Kompression in % (bis zu 100%)
// * Speed   : Entpackgeschindigkkeit, 100% ist Maximum
//--------------------------------------------------------------------------------------------
void CHLPool::AddBitmap(__int64 graphicID, SB_CBitmapCore *pBitmap, PALETTE *Pal, SLONG Quality, SLONG /*Speed*/) {
    // Zielobjekt vorbereiten:
    HLObjects.ReSize(HLObjects.AnzEntries() + 1);
    CHLObj &qObj = HLObjects[HLObjects.AnzEntries() - 1];
    qObj.HLines.ReSize(pBitmap->GetYSize() * 8);
    qObj.graphicID = graphicID;
    qObj.pHLPool = this;
    qObj.Size = XY(pBitmap->GetXSize(), pBitmap->GetYSize());

    if (pHLBasepool1 != nullptr) {
        pHLBasepool1Pool = pHLBasepool1->pPool;
        HLBasepool1Size = pHLBasepool1->PoolSize;
    } else {
        pHLBasepool1Pool = nullptr;
        HLBasepool1Size = 0;
    }

    if (pHLBasepool2 != nullptr) {
        pHLBasepool2Pool = pHLBasepool2->pPool;
        HLBasepool2Size = pHLBasepool2->PoolSize;
    } else {
        pHLBasepool2Pool = nullptr;
        HLBasepool2Size = 0;
    }

    SLONG MaxDelta = (100 - Quality) * 768 / 100;

    SLONG AnzObjHLines = 0;
    BUFFER_V<UBYTE> Equal(65536);

    BytesReal += pBitmap->GetXSize() * pBitmap->GetYSize() * 2;
    BytesAdministration += pBitmap->GetYSize(); // Anz Genes für jede Scanline
    BytesCompressed += pBitmap->GetYSize();     // Anz Genes für jede Scanline

    qObj.HLineEntries.ReSize(pBitmap->GetYSize());

    SB_CBitmapKey Key(*pBitmap);

    // ggf. Palette übernehmen:
    if (PaletteMapper.AnzEntries() == 0) {
        PaletteMapper.ReSize(256);

        for (SLONG c = 0; c < 256; c++) {
            PaletteMapper[c] = UWORD((Pal->Pal[c].b >> 3) + (Pal->Pal[c].g >> 2 << 5) + (Pal->Pal[c].r >> 3 << 11));
        }
    }

    // Bitmaps runterrechnen:
    if ((reinterpret_cast<__int64 *>((static_cast<UWORD *>(Key.Bitmap)) + pBitmap->GetYSize() * Key.lPitch / 2))[-1] !=
        *reinterpret_cast<const __int64 *>("CONVERTD")) {
        SLONG x = 0;
        SLONG y = 0;
        SLONG c = 0;
        SLONG d = 0;
        SLONG dc = 0;
        SLONG n = 0;

        for (y = 0; y < pBitmap->GetYSize(); y++) {
            for (x = 0; x < pBitmap->GetXSize(); x++) {
                n = x + y * Key.lPitch / 2;

                SLONG r = 0;
                SLONG g = 0;
                SLONG b = 0;
                UWORD p = *((static_cast<UWORD *>(Key.Bitmap)) + n);

                if (p != 0) {
                    b = (p & 31) << 3;
                    g = ((p >> 5) & 63) << 2;
                    r = ((p >> 11) & 31) << 3;

                    d = 999;

                    for (c = 1; c < 256; c++) {
                        if (d > abs(Pal->Pal[c].b - b) + abs(Pal->Pal[c].g - g) + abs(Pal->Pal[c].r - r)) {
                            d = abs(Pal->Pal[c].b - b) + abs(Pal->Pal[c].g - g) + abs(Pal->Pal[c].r - r);
                            dc = c;
                        }
                    }

                    *((static_cast<UBYTE *>(Key.Bitmap)) + n) = static_cast<UBYTE>(dc);
                } else {
                    *((static_cast<UBYTE *>(Key.Bitmap)) + n) = 0;
                }
            }
        }

        // Bitmap als bekehrt markieren:
        (reinterpret_cast<__int64 *>((static_cast<UWORD *>(Key.Bitmap)) + pBitmap->GetYSize() * Key.lPitch / 2))[-1] =
            *reinterpret_cast<const __int64 *>("CONVERTD");
    }

    //Ähnlichkeitstabelle berechnen:
    {
        SLONG x = 0;
        SLONG y = 0;

        for (y = 0; y < 256; y++) {
            for (x = 0; x <= y; x++) {
                if (abs(Pal->Pal[x].b - Pal->Pal[y].b) + abs(Pal->Pal[x].g - Pal->Pal[y].g) + abs(Pal->Pal[x].r - Pal->Pal[y].r) <= MaxDelta) {
                    Equal[x + (y << 8)] = Equal[y + (x << 8)] = static_cast<UBYTE>(
                        min(255, abs(Pal->Pal[x].b - Pal->Pal[y].b) + abs(Pal->Pal[x].g - Pal->Pal[y].g) + abs(Pal->Pal[x].r - Pal->Pal[y].r)));
                } else {
                    Equal[x + (y << 8)] = Equal[y + (x << 8)] = 255;
                }
            }
        }
    }

    // Datenkompression beginnt hier:
    SLONG x = 0;
    SLONG y = 0;
    SLONG cx = 0;
    SLONG Pass = 0;

    auto *bm = static_cast<UBYTE *>(Key.Bitmap);

    // Alle Pixel der Bitmap zeilenweise durchgehen:
    for (y = 0; y < pBitmap->GetYSize(); y++) {
        qObj.HLineEntries[y] = 0;
        for (x = 0; x < pBitmap->GetXSize(); x++) {
            //...und nach nicht.transparenten Pixeln suchen:
            if (bm[x] != 0) {
                qObj.HLineEntries[y]++; // Erhöhe, die Zahl der Gen-Strings für diese Zeile

                // Suchen, wie weit die nicht-transparente HLine geht...
                for (cx = 0; cx + x < pBitmap->GetXSize() && (bm[cx + x] != 0U); cx++) {
                    ;
                }

                if (cx <= 4) {
                    // Für HLines mit einer Länge <=2 gilt eine Sonderregelung:
                    qObj.HLines[AnzObjHLines].Offset = static_cast<UBYTE>(x);
                    qObj.HLines[AnzObjHLines].Anz = static_cast<UBYTE>(cx);

                    qObj.HLines[AnzObjHLines].pPixel = (UBYTE *)(*(reinterpret_cast<ptrdiff_t *>(bm + x)));

                    AnzObjHLines++;

                    BytesCompressed += sizeof(CHLGene);
                    BytesAdministration += sizeof(CHLGene);
                } else {
                    UBYTE *BestP = nullptr;
                    SLONG BestDist = 999;

                    // In drei verschiedenen Pools suchen:
                    for (Pass = 1; Pass <= 3; Pass++) {
                        UBYTE *p = nullptr;
                        CHLPool *pCHLPool = nullptr;

                        // Die Basepools der Basepools werden bis jetzt übersehen:
                        pCHLPool = nullptr;
                        if (Pass == 1) {
                            pCHLPool = pHLBasepool1;
                        }
                        if (Pass == 2) {
                            pCHLPool = pHLBasepool2;
                        }
                        if (Pass == 3) {
                            pCHLPool = this;
                        }

                        if (pCHLPool == nullptr && Pass < 3) {
                            continue;
                        }

                        if ((pCHLPool != nullptr) || Pass == 3) {
                            p = pCHLPool->pPool;

                            // Den Pool nach der HLine durchsuchen:
                            while (p + cx - 1 < pCHLPool->pPool + pCHLPool->PoolSize) {
                                if (Equal[p[0] + (bm[x] << 8)] < 255 && Equal[p[cx - 1] + (bm[x + cx - 1] << 8)] < 255) {
                                    SLONG cmp = 0;
                                    SLONG Dist = 0;

                                    for (cmp = cx - 1; cmp >= 0; cmp--) {
                                        if (Equal[p[cmp] + (bm[x + cmp] << 8)] >= 255) {
                                            break;
                                        }
                                        Dist = max(Dist, Equal[p[cmp] + (bm[x + cmp] << 8)]);
                                        if (Dist >= BestDist) {
                                            break;
                                        }
                                    }

                                    // Haben wir eine bessere Alternative gefunden?
                                    if (cmp < 0 && Dist < BestDist) {
                                        BestP = p;
                                        BestDist = Dist;

                                        if (Dist == 0) {
                                            break;
                                        }
                                    }
                                }

                                p++;
                            }

                            if (Pass == 3 && (BestP != nullptr) && BestDist < MaxDelta) {
                                // Erfolg! Eine Kopie wurde gefunden:
                                qObj.HLines[AnzObjHLines].Offset = UBYTE(x);
                                qObj.HLines[AnzObjHLines].Anz = UBYTE(cx);

                                qObj.HLines[AnzObjHLines].pPixel = BestP;

                                if (qObj.HLines[AnzObjHLines].Anz > 4) {
                                    if (qObj.HLines[AnzObjHLines].pPixel >= pPool && qObj.HLines[AnzObjHLines].pPixel <= pPool + PoolSize) {
                                        ;
                                    } else if ((pHLBasepool1 != nullptr) && qObj.HLines[AnzObjHLines].pPixel >= pHLBasepool1->pPool &&
                                               qObj.HLines[AnzObjHLines].pPixel <= pHLBasepool1->pPool + pHLBasepool1->PoolSize) {
                                        ;
                                    } else if ((pHLBasepool2 != nullptr) && qObj.HLines[AnzObjHLines].pPixel >= pHLBasepool2->pPool &&
                                               qObj.HLines[AnzObjHLines].pPixel <= pHLBasepool2->pPool + pHLBasepool2->PoolSize) {
                                        ;
                                    } else {
                                        DebugBreak();
                                    }
                                }

                                AnzObjHLines++;

                                BytesCompressed += sizeof(CHLGene);
                                BytesAdministration += sizeof(CHLGene);
                                LinesRepeated++;
                                break;
                            }

                            // if (p+cx-1<pCHLPool->pPool+pCHLPool->PoolSize) break;

                            if (Pass == 3) {
                                // Gen-String wurde nicht gefunden! Also in den Pool einfügen:
                                if (pCHLPool->pPool == nullptr) {
                                    // Erst einmal den Pool anlegen:
                                    pCHLPool->pPool = new UBYTE[1000];

                                    pCHLPool->PoolSize = 0;
                                    pCHLPool->PoolMaxSize = 1000;
                                }

                                // Pool ggf. vergrößern?
                                if (PoolSize + cx > PoolMaxSize) {
                                    /*{
                                    //Für alle HLines eines Objektes...
                                    for (SLONG d=AnzObjHLines-1; d>=0; d--)
                                    {
                                    //Falls die HLine aus dem aktuellen Pool stammt...
                                    if (qObj.HLines[d].Anz>4)
                                    if (qObj.HLines[d].pPixel>=pPool && qObj.HLines[d].pPixel<=pPool+PoolSize)
                                    ;
                                    else if (pHLBasepool1 && qObj.HLines[d].pPixel>=pHLBasepool1->pPool &&
                                    qObj.HLines[d].pPixel<=pHLBasepool1->pPool+pHLBasepool1->PoolSize)
                                    ;
                                    else if (pHLBasepool2 && qObj.HLines[d].pPixel>=pHLBasepool2->pPool &&
                                    qObj.HLines[d].pPixel<=pHLBasepool2->pPool+pHLBasepool2->PoolSize)
                                    ;
                                    else DebugBreak();
                                    }
                                    }*/

                                    auto *pNew = new UBYTE[PoolMaxSize + 1000];
                                    ReBaseObjects(pCHLPool->pPool, pNew);

                                    memcpy(pNew, pCHLPool->pPool, PoolSize);

                                    delete[] pCHLPool->pPool;

                                    pCHLPool->pPool = pNew;
                                    PoolMaxSize += 1000;

                                    /*{
                                    //Für alle HLines eines Objektes...
                                    for (SLONG d=AnzObjHLines-1; d>=0; d--)
                                    {
                                    //Falls die HLine aus dem aktuellen Pool stammt...
                                    if (qObj.HLines[d].Anz>4)
                                    if (qObj.HLines[d].pPixel>=pPool && qObj.HLines[d].pPixel<=pPool+PoolSize)
                                    ;
                                    else if (pHLBasepool1 && qObj.HLines[d].pPixel>=pHLBasepool1->pPool &&
                                    qObj.HLines[d].pPixel<=pHLBasepool1->pPool+pHLBasepool1->PoolSize)
                                    ;
                                    else if (pHLBasepool2 && qObj.HLines[d].pPixel>=pHLBasepool2->pPool &&
                                    qObj.HLines[d].pPixel<=pHLBasepool2->pPool+pHLBasepool2->PoolSize)
                                    ;
                                    else DebugBreak();
                                    }
                                    }*/
                                }

                                // Im Pool referenzieren:
                                qObj.HLines[AnzObjHLines].Offset = UBYTE(x);
                                qObj.HLines[AnzObjHLines].Anz = UBYTE(cx);
                                qObj.HLines[AnzObjHLines].pPixel = pCHLPool->pPool + PoolSize;
                                AnzObjHLines++;
                                LinesInPool++;

                                // Und jetzt endlich in den Pool eintragen:
                                memcpy(pCHLPool->pPool + PoolSize, bm + x, cx);
                                PoolSize += cx;

                                BytesCompressed += sizeof(CHLGene) + cx;
                                BytesAdministration += sizeof(CHLGene);
                            }
                        }
                    }
                }

                x += cx - 1;
            }
        }

        bm += Key.lPitch / 2;
    }

    // HLines zurechtstutzen, wir wollen nichts verschwenden:
    if (AnzObjHLines >= qObj.HLines.AnzEntries()) {
        DebugBreak();
    }

    qObj.HLines.ReSize(AnzObjHLines);
}

//--------------------------------------------------------------------------------------------
// Gibt allen Objekten und Unterobjekten eine Basis:
//--------------------------------------------------------------------------------------------
void CHLPool::DoBaseObjects() {
    SLONG c = 0;
    SLONG d = 0;

    if (pHLBasepool1 != nullptr) {
        pHLBasepool1Pool = pHLBasepool1->pPool;
        HLBasepool1Size = pHLBasepool1->PoolSize;
    } else {
        pHLBasepool1Pool = nullptr;
        HLBasepool1Size = 0;
    }

    if (pHLBasepool2 != nullptr) {
        pHLBasepool2Pool = pHLBasepool2->pPool;
        HLBasepool2Size = pHLBasepool2->PoolSize;
    } else {
        pHLBasepool2Pool = nullptr;
        HLBasepool2Size = 0;
    }

    // Für alle Objekte...
    for (c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
        CHLObj &qObj = HLObjects[c];

        // Für alle HLines eines Objektes...
        for (d = qObj.HLines.AnzEntries() - 1; d >= 0; d--) {
            // Falls die HLine aus dem aktuellen Pool stammt...
            if (qObj.HLines[d].Anz > 4) {
                if (qObj.HLines[d].pPixel >= (UBYTE *)nullptr && qObj.HLines[d].pPixel <= (UBYTE *)nullptr + PoolSize) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - static_cast<ptrdiff_t>(0x00000000) + (ptrdiff_t)pPool);
                } else if ((pHLBasepool1 != nullptr) && qObj.HLines[d].pPixel >= (UBYTE *)0x10000000 &&
                           qObj.HLines[d].pPixel <= (UBYTE *)0x10000000 + pHLBasepool1->PoolSize) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - static_cast<ptrdiff_t>(0x10000000) + (ptrdiff_t)pHLBasepool1->pPool);

                } else if ((pHLBasepool2 != nullptr) && qObj.HLines[d].pPixel >= (UBYTE *)0x20000000 &&
                           qObj.HLines[d].pPixel <= (UBYTE *)0x20000000 + pHLBasepool2->PoolSize) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - static_cast<ptrdiff_t>(0x20000000) + (ptrdiff_t)pHLBasepool2->pPool);

                } else {
                    DebugBreak();
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Nimm allen Objekten und Unterobjekten die Basis weg:
//--------------------------------------------------------------------------------------------
void CHLPool::UnBaseObjects() {
    SLONG c = 0;
    SLONG d = 0;

    // Für alle Objekte...
    for (c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
        CHLObj &qObj = HLObjects[c];

        // Für alle HLines eines Objektes...
        for (d = qObj.HLines.AnzEntries() - 1; d >= 0; d--) {
            // Falls die HLine aus dem aktuellen Pool stammt...
            if (qObj.HLines[d].Anz > 4) {
                if (qObj.HLines[d].pPixel >= pPool && qObj.HLines[d].pPixel <= pPool + PoolSize) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - (ptrdiff_t)pPool + static_cast<ptrdiff_t>(0x00000000));

                } else if ((pHLBasepool1 != nullptr) && qObj.HLines[d].pPixel >= pHLBasepool1Pool &&
                           qObj.HLines[d].pPixel <= pHLBasepool1Pool + HLBasepool1Size) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - (ptrdiff_t)pHLBasepool1Pool + static_cast<ptrdiff_t>(0x10000000));

                } else if ((pHLBasepool2 != nullptr) && qObj.HLines[d].pPixel >= pHLBasepool2Pool &&
                           qObj.HLines[d].pPixel <= pHLBasepool2Pool + HLBasepool2Size) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - (ptrdiff_t)pHLBasepool2Pool + static_cast<ptrdiff_t>(0x20000000));

                } else {
                    DebugBreak();
                }
            }
        }
    }

    pHLBasepool1Pool = nullptr;
    HLBasepool1Size = 0;
    pHLBasepool2Pool = nullptr;
    HLBasepool2Size = 0;
}

//--------------------------------------------------------------------------------------------
// Gibt allen Objekten eine neue Basis:
//--------------------------------------------------------------------------------------------
void CHLPool::ReBaseObjects(UBYTE *pOldPool, UBYTE *pNewPool) {
    SLONG c = 0;
    SLONG d = 0;

    // Für alle Objekte...
    for (c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
        CHLObj &qObj = HLObjects[c];

        // Für alle HLines eines Objektes...
        for (d = qObj.HLines.AnzEntries() - 1; d >= 0; d--) {
            // Falls die HLine aus dem aktuellen Pool stammt...
            if (qObj.HLines[d].Anz > 4) {
                if (qObj.HLines[d].pPixel >= pOldPool && qObj.HLines[d].pPixel <= pOldPool + PoolSize) {
                    qObj.HLines[d].pPixel = ((qObj.HLines[d].pPixel) - (ptrdiff_t)pOldPool + (ptrdiff_t)pNewPool);
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------
// Sucht ein HLObj anhand seiner id heraus:
//--------------------------------------------------------------------------------------------
CHLObj *CHLPool::GetHLObj(__int64 graphicID) {
    SLONG c = 0;

    for (c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
        if (HLObjects[c].graphicID == graphicID) {
            return (&HLObjects[c]);
        }
    }

    return (nullptr);
}

//--------------------------------------------------------------------------------------------
// Sucht ein HLObj anhand seiner id heraus:
//--------------------------------------------------------------------------------------------
CHLObj *CHLPool::GetHLObj(const CString &String) {
    SLONG c = 0;
    __int64 graphicId = 0;

    graphicId = 0;
    for (c = 0; c < static_cast<SLONG>(strlen(String)); c++) {
        graphicId += __int64(String[static_cast<SLONG>(c)]) << (8 * c);
    }

    for (c = HLObjects.AnzEntries() - 1; c >= 0; c--) {
        if (HLObjects[c].graphicID == graphicId) {
            return (&HLObjects[c]);
        }
    }

    return (nullptr);
}

//--------------------------------------------------------------------------------------------
// CHLBms::
//--------------------------------------------------------------------------------------------
// Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize(CHLPool *pHLPool, __int64 graphicID, ...) {
    SLONG count = 0;
    __int64 i = graphicID;
    va_list marker;
    BUFFER_V<__int64> graphicIds;

    // Anzahl ermitteln:
    va_start(marker, graphicID);
    while ((i & 0xffffffff) != 0) {
        count++;
        i = va_arg(marker, __int64);
    }
    va_end(marker);

    graphicIds.ReSize(count);

    // Und initialisieren:
    count = 0, i = graphicID;

    va_start(marker, graphicID);
    while ((i & 0xffffffff) != 0) {
        graphicIds[count++] = i;
        i = va_arg(marker, __int64);
    }
    va_end(marker);

    ReSize(pHLPool, graphicIds);
}

//--------------------------------------------------------------------------------------------
// Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize(CHLPool *pHLPool, const BUFFER_V<__int64> &graphicIds) {
    SLONG c = 0;

    Bitmaps.ReSize(graphicIds.AnzEntries());

    for (c = 0; c < graphicIds.AnzEntries(); c++) {
        Bitmaps[c].ReSize(pHLPool, graphicIds[c]);
    }
}

//--------------------------------------------------------------------------------------------
// Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize(CHLPool *pHLPool, const CString &graphicstr) {
    SLONG Anz = 0;
    char *Texts[100];
    BUFFER_V<__int64> graphicIds;
    BUFFER_V<char> Str(graphicstr.GetLength() + 1);

    strcpy(Str.data(), graphicstr);

    for (Anz = 0;; Anz++) {
        if (Anz == 0) {
            Texts[Anz] = strtok(Str.data(), " ");
        } else {
            Texts[Anz] = strtok(nullptr, " ");
        }
        if (Texts[Anz] == nullptr) {
            break;
        }
    }

    graphicIds.ReSize(Anz);

    for (SLONG c = 0; c < Anz; c++) {
        graphicIds[c] = 0;
        for (SLONG d = 0; d < static_cast<SLONG>(strlen(Texts[c])); d++) {
            graphicIds[c] += __int64((Texts[c])[d]) << (8 * d);
        }
    }

    ReSize(pHLPool, graphicIds);
}

//--------------------------------------------------------------------------------------------
// Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize(CHLPool *pHLPool, const CString &graphicstr, SLONG Anzahl) {
    BUFFER_V<__int64> graphicIds;
    BUFFER_V<char> Str(graphicstr.GetLength() + 1);

    strcpy(Str.data(), graphicstr);

    graphicIds.ReSize(Anzahl);

    for (SLONG c = 0; c < Anzahl; c++) {
        graphicIds[c] = 0;
        SLONG len = strlen(Str.data());
        for (SLONG d = 0; d < len; d++) {
            graphicIds[c] += __int64(Str[d]) << (8 * d);
        }

        Str[len - 1]++;
        if (Str[len - 1] > '9') {
            Str[len - 1] = '0';
            Str[len - 2]++;
        }
    }

    ReSize(pHLPool, graphicIds);
}

//--------------------------------------------------------------------------------------------
// Bringt alle HLine-Pool auf den neusten Stand:
//--------------------------------------------------------------------------------------------
void UpdateHLinePool() {
    SLONG c = 0;
    SLONG d = 0;
    SLONG e = 0;
    PALETTE AnimPal;
    PALETTE SkelPal;

    BOOL DontSaveSkeletons = FALSE;

    SkelettPool.ReSize(bprintf("skelett.pol", c), nullptr, nullptr);
    SkelPal.RefreshPalFromLbm(const_cast<char *>((LPCTSTR)FullFilename("skel_pal.lbm", "clan\\lbm\\%s")));

    for (c = Clans.AnzEntries() - 1; c >= 0; c--) {
        if ((Clans.IsInAlbum(c) != 0) && (Clans[c].UpdateNow != 0U)) {
            hprintf("Compressing %li", c);

            AnimPal.RefreshPalFromLbm(const_cast<char *>((LPCTSTR)FullFilename(Clans[c].PalFilename, "clan\\lbm\\%s")));

            if (c < SLONG(Clans.AnzEntries() - 1) && (Clans.IsInAlbum(c + 1) != 0) && Clans[c].PalFilename == Clans[SLONG(c + 1)].PalFilename &&
                Clans[c].Type != 30) {
                Clans[c].ClanPool.ReSize(bprintf("clan%li.pol", c), &Clans[SLONG(c + 1)].ClanPool, nullptr);
                Clans[c].ClanGimmick.ReSize(bprintf("clang%li.pol", c), &Clans[SLONG(c + 1)].ClanPool, &Clans[c].ClanPool);
                Clans[c].ClanWarteGimmick.ReSize(bprintf("clanw%li.pol", c), &Clans[SLONG(c + 1)].ClanPool, &Clans[c].ClanPool);
            } else {
                Clans[c].ClanPool.ReSize(bprintf("clan%li.pol", c), nullptr, nullptr);
                Clans[c].ClanGimmick.ReSize(bprintf("clang%li.pol", c), &Clans[c].ClanPool, nullptr);
                Clans[c].ClanWarteGimmick.ReSize(bprintf("clanw%li.pol", c), &Clans[c].ClanPool, nullptr);
            }

            for (d = 0; d < 14; d++) {
                if (d != 4 && d != 9) { // Kein Gimmick hier:
                    for (e = 0; e < Clans[c].PhasenIds[d].AnzEntries(); e++) {
                        if ((Clans[c].ClanPool.GetHLObj((Clans[c].PhasenIds[d])[e]) == nullptr) && ((Clans[c].Phasen[d])[e].Size.x != 0)) {
                            Clans[c].ClanPool.AddBitmap((Clans[c].PhasenIds[d])[e], (Clans[c].Phasen[d])[e].pBitmap, &AnimPal, 95, 100);
                        }
                    }
                }
            }

            for (d = 4; d <= 4; d++) { // Gimmick:
                for (e = 0; e < Clans[c].PhasenIds[d].AnzEntries(); e++) {
                    if ((Clans[c].ClanGimmick.GetHLObj((Clans[c].PhasenIds[d])[e]) == nullptr) && ((Clans[c].Phasen[d])[e].Size.x != 0)) {
                        Clans[c].ClanGimmick.AddBitmap((Clans[c].PhasenIds[d])[e], (Clans[c].Phasen[d])[e].pBitmap, &AnimPal, 95, 100);
                    }
                }
            }

            for (d = 9; d <= 9; d++) { // Wartegimmick:
                for (e = 0; e < Clans[c].PhasenIds[d].AnzEntries(); e++) {
                    if ((Clans[c].ClanWarteGimmick.GetHLObj((Clans[c].PhasenIds[d])[e]) == nullptr) && ((Clans[c].Phasen[d])[e].Size.x != 0)) {
                        Clans[c].ClanWarteGimmick.AddBitmap((Clans[c].PhasenIds[d])[e], (Clans[c].Phasen[d])[e].pBitmap, &AnimPal, 95, 100);
                    }
                }
            }

            for (d = 0; d < 14; d++) {
                for (e = 0; e < Clans[c].SkelettIds[d].AnzEntries(); e++) {
                    if ((SkelettPool.GetHLObj((Clans[c].SkelettIds[d])[e]) == nullptr) && ((Clans[c].Skelett[d])[e].Size.x != 0)) {
                        SkelettPool.AddBitmap((Clans[c].SkelettIds[d])[e], (Clans[c].Skelett[d])[e].pBitmap, &SkelPal, 98, 100);
                    }
                }
            }

            Clans[c].ClanWarteGimmick.Save();
            Clans[c].ClanGimmick.Save();
            Clans[c].ClanPool.Save();
        } else if (Clans.IsInAlbum(c) != 0) {
            DontSaveSkeletons = TRUE;
        }
    }

    if (DontSaveSkeletons == 0) {
        SkelettPool.Save();
    }
}
