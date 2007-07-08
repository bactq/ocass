/*
 * Copyright (C) 2007 LeXiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 * This file is part of ocass
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "ca_inject.h"
#include "ca_tlhlp.h"
#include "dbghelp.h"

static HMODULE ModuleFromAddress(PVOID pv) 
{
   MEMORY_BASIC_INFORMATION mbi;

   return ((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) 
       ? (HMODULE) mbi.AllocationBase : NULL);
}

// The highest private memory address (used for Windows 98 only)
PVOID sm_pvMaxAppAddr = NULL;
const BYTE cPushOpCode = 0x68;   // The PUSH opcode on x86 platforms

void ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
                             PROC pfnCurrent, PROC pfnNew, 
                             HMODULE hmodCaller) 
{
    // Get the address of the module's import section
    ULONG ulSize;

   PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
      ImageDirectoryEntryToData(hmodCaller, TRUE,
      IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

   if (pImportDesc == NULL)
      return;  // This module has no import section


   // Find the import descriptor containing references to callee's functions
   for (; pImportDesc->Name; pImportDesc++) {
      PSTR pszModName = (PSTR) ((PBYTE) hmodCaller + pImportDesc->Name);
      if (lstrcmpiA(pszModName, pszCalleeModName) == 0)
         break;   // Found
   }

   if (pImportDesc->Name == 0)
      return;  // This module doesn't import any functions from this callee

   // Get caller's import address table (IAT) for the callee's functions
   PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
      ((PBYTE) hmodCaller + pImportDesc->FirstThunk);

   // Replace current function address with new function address
   for (; pThunk->u1.Function; pThunk++) {

      // Get the address of the function address
      PROC* ppfn = (PROC*) &pThunk->u1.Function;

      // Is this the function we're looking for?
      BOOL fFound = (*ppfn == pfnCurrent);

      if (!fFound && (*ppfn > sm_pvMaxAppAddr)) {

         // If this is not the function and the address is in a shared DLL,
         // then maybe we're running under a debugger on Windows 98. In this
         // case, this address points to an instruction that may have the
         // correct address.

         PBYTE pbInFunc = (PBYTE) *ppfn;
         if (pbInFunc[0] == cPushOpCode) {
            // We see the PUSH instruction, the real function address follows
            ppfn = (PROC*) &pbInFunc[1];

            // Is this the function we're looking for?
            fFound = (*ppfn == pfnCurrent);
         }
      }

      if (fFound) {
         // The addresses match, change the import section address
         WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
            sizeof(pfnNew), NULL);
         return;  // We did it, get out
      }
   }
}

CA_DECLARE(void) CA_ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
        PROC pfnCurrent, PROC pfnNew, BOOL fExcludeAPIHookMod) 
{
   HMODULE hmodThisMod = fExcludeAPIHookMod ? 
       ModuleFromAddress(CA_ReplaceIATEntryInAllMods) : NULL;

   // Get the list of modules in this process
   CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());

   MODULEENTRY32 me = { sizeof(me) };
   for (BOOL fOk = th.ModuleFirst(&me); fOk; fOk = th.ModuleNext(&me))
   {
      // NOTE: We don't hook functions in our own module
      if (me.hModule != hmodThisMod) 
      {

         // Hook this function in this module
         ReplaceIATEntryInOneMod(pszCalleeModName, 
             pfnCurrent, pfnNew, me.hModule);
      }
   }
}

CA_DECLARE(CAErrno) CA_ReplaceAPI(const TCHAR *pszModName, 
                const TCHAR *pszAPIName, PROC pNewFunc, PROC *pOldFunc)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    HMODULE hMod = NULL;

    hMod = GetModuleHandle(pszModName);
    if (NULL == hMod)
    {
        funcErr = CA_ERR_FATAL;
        goto EXIT;
    }

    *pOldFunc = GetProcAddress(hMod, pszAPIName);
    if (NULL == *pOldFunc)
    {
        funcErr = CA_ERR_FATAL;
        goto EXIT;
    }

    CA_ReplaceIATEntryInAllMods(pszModName, *pOldFunc, pNewFunc, FALSE);
EXIT:
    return funcErr;
}
