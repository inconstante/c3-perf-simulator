/*
 * Copyright (c) 2003 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string>

#include "base/loader/aout_object.hh"

#include "mem/functional_mem/functional_memory.hh"
#include "base/loader/symtab.hh"

#include "base/trace.hh"	// for DPRINTF

#include "base/loader/exec_aout.h"

using namespace std;

ObjectFile *
AoutObject::tryFile(const string &fname, int fd, size_t len, uint8_t *data)
{
    if (!N_BADMAG(*(aout_exechdr *)data)) {
        return new AoutObject(fname, fd, len, data);
    }
    else {
        return NULL;
    }
}


AoutObject::AoutObject(const string &_filename, int _fd,
                         size_t _len, uint8_t *_data)
    : ObjectFile(_filename, _fd, _len, _data)
{
    execHdr = (aout_exechdr *)fileData;

    entry = execHdr->entry;

    text.baseAddr = N_TXTADDR(*execHdr);
    text.size = execHdr->tsize;

    data.baseAddr = N_DATADDR(*execHdr);
    data.size = execHdr->dsize;

    bss.baseAddr = N_BSSADDR(*execHdr);
    bss.size = execHdr->bsize;

    DPRINTFR(Loader, "text: 0x%x %d\ndata: 0x%x %d\nbss: 0x%x %d\n",
             text.baseAddr, text.size, data.baseAddr, data.size,
             bss.baseAddr, bss.size);
}


bool
AoutObject::loadSections(FunctionalMemory *mem, bool loadPhys)
{
    Addr textAddr = text.baseAddr;
    Addr dataAddr = data.baseAddr;

    if (loadPhys) {
        textAddr &= (ULL(1) << 40) - 1;
        dataAddr &= (ULL(1) << 40) - 1;
    }

    // Since we don't really have an MMU and all memory is
    // zero-filled, there's no need to set up the BSS segment.
    if (text.size != 0)
        mem->prot_write(textAddr, fileData + N_TXTOFF(*execHdr), text.size);
    if (data.size != 0)
        mem->prot_write(dataAddr, fileData + N_DATOFF(*execHdr), data.size);

    return true;
}


bool
AoutObject::loadGlobalSymbols(SymbolTable *symtab)
{
    // a.out symbols not supported yet
    return false;
}

bool
AoutObject::loadLocalSymbols(SymbolTable *symtab)
{
    // a.out symbols not supported yet
    return false;
}
