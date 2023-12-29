/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "ProfanityFilter.h"
#include "LinuxStrings.h"
#include "RakAssert.h"
#include "Rand.h"

#if defined(_WIN32)
#include <malloc.h> // alloca
#elif (defined(__GNUC__) || defined(__GCCXML__))
#include <alloca.h>
#else
#endif

using namespace RakNet;

char ProfanityFilter::BANCHARS[] = "!@#$%^&*()";
char ProfanityFilter::WORDCHARS[] = "abcdefghijklmnopqrstuvwxyz0123456789";

ProfanityFilter::ProfanityFilter() = default;

ProfanityFilter::~ProfanityFilter() = default;

char ProfanityFilter::RandomBanChar() { return BANCHARS[randomMT() % (sizeof(BANCHARS) - 1)]; }

bool ProfanityFilter::HasProfanity(const char *str) { return FilterProfanity(str, nullptr, false) > 0; }

int ProfanityFilter::FilterProfanity(const char *input, char *output, bool filter) {
    if (input == nullptr || input[0] == 0) {
        return 0;
    }

    int count = 0;
    char *b = static_cast<char *>(alloca(strlen(input) + 1));
    strcpy(b, input);
    _strlwr(b);
    char *start = b;
    if (output != nullptr) {
        strcpy(output, input);
    }

    start = strpbrk(start, WORDCHARS);
    while (start != nullptr) {
        size_t len = strspn(start, WORDCHARS);
        if (len > 0) {
            // we a have a word - let's check if it's a BAAAD one
            char saveChar = start[len];
            start[len] = '\0';

            // loop through profanity list
            for (unsigned int i = 0, size = words.Size(); i < size; i++) {
                if (_stricmp(start, words[i].C_String()) == 0) {
                    count++;

                    // size_t len = words[i].size();
                    if (filter && (output != nullptr)) {
                        for (unsigned int j = 0; j < len; j++) {
                            output[start + j - b] = RandomBanChar();
                        }
                    }
                    break;
                }
            }
            start[len] = saveChar;
        }

        start += len;
        start = strpbrk(start, WORDCHARS);
    }

    return count;
}

int ProfanityFilter::Count() { return words.Size(); }
void ProfanityFilter::AddWord(const RakNet::RakString &newWord) { words.Insert(newWord, _FILE_AND_LINE_); }
