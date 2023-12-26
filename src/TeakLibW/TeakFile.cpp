#include "StdAfx.h"

#include <sys/types.h>
#include <sys/stat.h>

#define AT_Log(...) AT_Log_I("TeakFile", __VA_ARGS__)
#define GET_RAW_TEXT_PATH(path, buffer, max_size) snprintf(buffer, max_size, "%s.txt", path)

const char *ExcOpen = "Can't open %s!";
const char *ExcRead = "Can't read %s!";
const char *ExcWrite = "Can't write %s!";
const char *ExcSeek = "Can't seek %s at %li!";

TEAKFILE::TEAKFILE()
    : Ctx(nullptr), Path(nullptr), MemPointer(0), MemBufferUsed(0)

{}

TEAKFILE::TEAKFILE(char const *path, SLONG mode) : Ctx(nullptr), Path(nullptr), MemPointer(0), MemBufferUsed(0) { Open(path, mode); }

TEAKFILE::~TEAKFILE() { Close(); }

void TEAKFILE::ReadLine(char *buffer, SLONG size) const {
    SLONG i = 0;
    for (i = 0; i < size && (IsEof() == 0); i++) {
        char c = SDL_ReadU8(Ctx);

        if (c == '\r') {
            continue;
        }

        if (c == '\n') {
            break;
        }

        buffer[i] = c;
    }

    if (i < size) {
        buffer[i] = '\0';
    }
}

SLONG TEAKFILE::IsEof() const { return static_cast<SLONG>(SDL_RWtell(Ctx) >= SDL_RWsize(Ctx)); }

void TEAKFILE::Close() {
    if (Ctx != nullptr) {
        SDL_RWclose(Ctx);
    }
    Ctx = nullptr;
    if (Path != nullptr) {
        SDL_free(Path);
    }
    Path = nullptr;
}

SLONG TEAKFILE::GetFileLength() const { return static_cast<SLONG>(SDL_RWsize(Ctx)); }

SLONG TEAKFILE::GetPosition() const { return static_cast<SLONG>(SDL_RWtell(Ctx)); }

void TEAKFILE::Open(char const *path, SLONG mode) {
    Ctx = SDL_RWFromFile(path, mode == TEAKFILE_WRITE ? "wb" : "rb");
    if (Ctx == nullptr) {
        TeakLibW_Exception(nullptr, 0, ExcOpen, path);
    }

    Path = SDL_strdup(path);
}

SLONG TEAKFILE::IsOpen() const { return static_cast<SLONG>(Ctx != nullptr); }

void TEAKFILE::Read(unsigned char *buffer, SLONG size) {
    if (MemBuffer.AnzEntries() > 0) {
        SLONG anz = 0;
        if (size >= MemBufferUsed - MemPointer) {
            anz = MemBufferUsed - MemPointer;
        } else {
            anz = size;
        }
        memcpy(buffer, MemPointer + MemBuffer, anz);
        MemPointer += size;
    } else {
        if (SDL_RWread(Ctx, buffer, 1, size) != size) {
            TeakLibW_Exception(nullptr, 0, ExcRead, Path);
        }
    }
}

void TEAKFILE::Write(const unsigned char *buffer, SLONG size) {
    if (MemBuffer.AnzEntries() > 0) {
        if (MemPointer + size > MemBuffer.AnzEntries()) {
            SLONG slack = MemBuffer.AnzEntries() / 10;
            MemBuffer.ReSize(slack + size + MemPointer);
        }
        memcpy(MemBuffer + MemPointer, buffer, size);
        MemPointer += size;
        MemBufferUsed += size;
    } else {
        if (SDL_RWwrite(Ctx, buffer, 1, size) != size) {
            TeakLibW_Exception(nullptr, 0, ExcWrite, Path);
        }
    }
}

void TEAKFILE::ReadTrap(SLONG trap) const {
    if (SDL_ReadLE32(Ctx) != trap) {
        DebugBreak();
    }
}

void TEAKFILE::WriteTrap(SLONG trap) const { SDL_WriteLE32(Ctx, trap); }

void TEAKFILE::SetPosition(SLONG pos) const {
    if (SDL_RWseek(Ctx, pos, RW_SEEK_SET) < 0) {
        TeakLibW_Exception(nullptr, 0, ExcSeek, Path, pos);
    }
}

void TEAKFILE::Skip(SLONG size) const {
    if (SDL_RWseek(Ctx, size, RW_SEEK_CUR) < 0) {
        TeakLibW_Exception(nullptr, 0, ExcSeek, Path, size);
    }
}

void TEAKFILE::Announce(SLONG size) { MemBuffer.ReSize(size); }

CRLEReader::CRLEReader(const char *path) : Ctx(nullptr), SeqLength(0), SeqUsed(0), IsSeq(false), Sequence(), IsRLE(false), Size(0), Key(0), Path(path) {
    Ctx = SDL_RWFromFile(path, "rb");
    if (Ctx != nullptr) {
        char str[6];
        SDL_RWread(Ctx, str, sizeof(str), 1);
        if (strcmp(str, "xtRLE") == 0) {
            IsRLE = true;
            SLONG version = SDL_ReadLE32(Ctx);
            if (version >= 0x102) {
                Key = 0xA5;
            }
            if (version >= 0x101) {
                Size = SDL_ReadLE32(Ctx);
            }
        } else {
            Size = static_cast<SLONG>(SDL_RWsize(Ctx));
            SDL_RWseek(Ctx, 0, RW_SEEK_SET);
        }
    }
}

CRLEReader::~CRLEReader() { Close(); }
bool CRLEReader::AlwaysSaveAsPlainText = false;
bool CRLEReader::UpdateDataBeforeOpening = false;

bool CRLEReader::Close() {
    if (Ctx == nullptr) {
        return false;
    }
    return SDL_RWclose(Ctx) == 0;
}

void CRLEReader::SaveAsPlainText() {
    if (Ctx == nullptr) {
        return;
    }

    BUFFER_V<BYTE> buffer(GetSize());
    if (Read(buffer.getData(), buffer.AnzEntries(), true)) {
        char fn[255];
        GET_RAW_TEXT_PATH(Path, fn, 255);
        AT_Log("Saving as plain text file to %s", fn);

        // TEAKFILE file(fn, TEAKFILE_WRITE);
        FILE *fp = fopen(fn, "wb");
        for (SLONG i = 0; i < buffer.AnzEntries(); ++i) {
            fputc(buffer[i], fp);
        }
        fclose(fp);
    }
}

bool CRLEReader::Buffer(void *buffer, SLONG size) { return SDL_RWread(Ctx, buffer, size, 1) > 0; }

bool CRLEReader::NextSeq() {
    char buf = 0;
    if (!Buffer(&buf, 1)) {
        return false;
    }
    SeqLength = buf;

    if ((SeqLength & 0x80) != 0) {
        SeqLength &= 0x7FU;
        SeqUsed = 0;
        IsSeq = true;
        if (!Buffer(Sequence, SeqLength)) {
            return false;
        }
        for (SLONG i = 0; i < SeqLength; i++) {
            Sequence[i] ^= Key;
        }
    } else {
        IsSeq = false;
        if (!Buffer(Sequence, 1)) {
            return false;
        }
    }
    return true;
}

bool CRLEReader::Read(BYTE *buffer, SLONG size, bool decode) {
    if (!decode || !IsRLE) {
        return Buffer(buffer, size);
    }

    for (SLONG i = 0; i < size; i++) {
        if ((SeqLength == 0) && !NextSeq()) {
            return false;
        }

        if (IsSeq) {
            buffer[i] = Sequence[SeqUsed++];
            if (SeqUsed == SeqLength) {
                SeqLength = 0;
            }
        } else {
            buffer[i] = Sequence[0];
            SeqLength--;
        }
    }
    return true;
}

BOOL DoesFileExist(char const *path) {
    SDL_RWops *ctx = SDL_RWFromFile(path, "rb");
    if (ctx != nullptr) {
        SDL_RWclose(ctx);
        return 1;
    }
#ifdef _DEBUG
    AT_Log("File not found: %s", path);
#endif
    return 0;
}

BOOL DoesDirectoryExist(char const *path) {
    struct stat info{};

    if (stat(path, &info) == 0 && (info.st_mode & S_IFMT) == S_IFDIR) {
        return 1;
    }

#ifdef _DEBUG
    AT_Log("Directory not found: %s", path);
#endif
    return 0;
}

BOOL isCRLE(char const *path) {
    CRLEReader reader(path);
    return reader.getIsRLE();
}

BUFFER_V<BYTE> LoadCompleteFile(char const *path) {
    BOOL _isCRLE = isCRLE(path);

    if (_isCRLE && CRLEReader::UpdateDataBeforeOpening) {
        CRLEWriter writer(path);
        writer.UpdateFromPlainText();
        writer.Close();
    }

    if (_isCRLE && CRLEReader::AlwaysSaveAsPlainText) {
        CRLEReader converter(path);
        converter.SaveAsPlainText();
        converter.Close();
    }
    
    // Read file and return
    CRLEReader reader(path);
    BUFFER_V<BYTE> buffer(reader.GetSize());
    if (!reader.Read(buffer.getData(), buffer.AnzEntries(), true)) {
        return buffer;
    }
    return buffer;
}


CRLEWriter::CRLEWriter(const char *path) : Ctx(nullptr), Version(0x102), Key(0xA5), Magic("xtRLE"), Path(path), Sequence() {}
CRLEWriter::~CRLEWriter() { Close(); }
bool CRLEWriter::Close() {
    if (Ctx == nullptr) {
        return false;
    }
    return SDL_RWclose(Ctx) == 0;
}

SLONG CRLEWriter::GetNextSequence(const unsigned char *buffer, SLONG size, SLONG consumed) {
    SLONG length = 0;
    unsigned char previousChar = buffer[consumed];
    Sequence[length++] = previousChar;

    // chain of same characters - compress them
    while (consumed + length < size && previousChar == buffer[consumed + length]) {
        length += 1;
        // length is limited to 127
        if (length == 127) {
            return length;
        }
        if (consumed + length < size && previousChar != buffer[consumed + length]) {
            return length;
        }
    }

    // create a sequence
    while (consumed + length < size && previousChar != buffer[consumed + length]) {
        if (consumed + length + 1 < size) {
            // two same character - we shouldn't include them in the sequence
            if (buffer[consumed + length] == buffer[consumed + length + 1]) {
                break;
            }
        }
        previousChar = buffer[consumed + length];
        Sequence[length] = buffer[consumed + length];
        length += 1;
        // length is limited to 127
        if (length == 127) {
            break;
        }
    }

    return length | 0x80;
}

void CRLEWriter::Write(const unsigned char *buffer, SLONG size) {
    SLONG consumed = 0;
    SLONG cursor = 0;
    // This should theoratically be enough
    unsigned char *output = new unsigned char[size * 4];

    while (consumed < size) {
        SLONG length = GetNextSequence(buffer, size, consumed);
        // it's a sequence
        if (length & 0x80) {
            length &= 0x7FU;
            output[cursor++] = length | 0x80;
            for (SLONG j = 0; j < length; ++j) {
                output[cursor++] = Sequence[j] ^ Key;
            }
        } else {
            output[cursor++] = length;
            output[cursor++] = Sequence[0];
        }
        consumed += length;
    }

    // Finally: Write header + content
    TEAKFILE file(Path, TEAKFILE_WRITE);
    file.Write(reinterpret_cast<const unsigned char *>(Magic), 6);
    file.WriteTrap(Version);
    // size
    file.WriteTrap(consumed);
    file.Write(output, cursor);
    file.Close();
}

void CRLEWriter::UpdateFromPlainText() {
    char rawPath[255];
    GET_RAW_TEXT_PATH(Path, rawPath, 255);
    if (DoesFileExist(rawPath) == 0) {
        AT_Log("CRLEWriter: Unable to find file %s to update data file. Skipping.", rawPath);
        return;
    }
    AT_Log("Saving as compressed text file to %s", Path);

    // Read the buffer
    CRLEReader reader(rawPath);
    std::vector<char> buffer;
    SLONG bufferSize = reader.GetSize();
    buffer.resize(bufferSize);
    reader.Buffer(buffer.data(), bufferSize);

    Write(reinterpret_cast<const unsigned char *>(buffer.data()), bufferSize);
    reader.Close();
}