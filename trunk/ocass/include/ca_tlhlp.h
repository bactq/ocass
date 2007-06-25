/**
 *
 */

#ifndef _CA_TLHLP_H_
#define _CA_TLHLP_H_ 1

#include "ca_types.h"
#include <tlhelp32.h>

#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))

class CToolhelp 
{
private:
   HANDLE m_hSnapshot;

public:
   CToolhelp(DWORD dwFlags = 0, DWORD dwProcessID = 0)
   {
       m_hSnapshot = INVALID_HANDLE_VALUE;
       CreateSnapshot(dwFlags, dwProcessID);
   }

   ~CToolhelp(void)
   {
       if (INVALID_HANDLE_VALUE != m_hSnapshot)
       {
           CloseHandle(m_hSnapshot);
       }
   }

   BOOL CreateSnapshot(DWORD dwFlags, DWORD dwProcessID = 0)
   {
       if (INVALID_HANDLE_VALUE != m_hSnapshot)
       {
           CloseHandle(m_hSnapshot);
       }

       if (0 == dwFlags) 
       {
           m_hSnapshot = INVALID_HANDLE_VALUE;
       } 
       else 
       {
           m_hSnapshot = CreateToolhelp32Snapshot(dwFlags, dwProcessID);
       }
       return (m_hSnapshot != INVALID_HANDLE_VALUE);
   }

   BOOL ProcessFirst(PPROCESSENTRY32 ppe) const
   {
       BOOL fOk = Process32First(m_hSnapshot, ppe);
       if (fOk && (ppe->th32ProcessID == 0))
       {
           // Remove the "[System Process]" (PID = 0)
           fOk = ProcessNext(ppe); 
       }
       return (fOk);
   }

   BOOL ProcessNext(PPROCESSENTRY32 ppe) const
   {
       BOOL fOk = Process32Next(m_hSnapshot, ppe);
       if (fOk && (ppe->th32ProcessID == 0))
       {
           // Remove the "[System Process]" (PID = 0)
            fOk = ProcessNext(ppe); 
       }
       return (fOk);
   }

   BOOL ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const
   {
       BOOL fFound = FALSE;
       for (BOOL fOk = ProcessFirst(ppe); fOk; fOk = ProcessNext(ppe)) 
       {
           fFound = (ppe->th32ProcessID == dwProcessId);
           if (fFound) 
           {
               break;
           }
       }
       return (fFound);
   }

   BOOL ModuleFirst(PMODULEENTRY32 pme) const
   {
       return (Module32First(m_hSnapshot, pme));
   }

   BOOL ModuleNext(PMODULEENTRY32 pme) const
   {
       return (Module32Next(m_hSnapshot, pme));
   }

   BOOL ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const
   {
       BOOL fFound = FALSE;
       for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) 
       {
           fFound = (pme->modBaseAddr == pvBaseAddr);
           if (fFound) 
           {
               break;
           }
       }
       return (fFound);
   }

   BOOL ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const
   {
       BOOL fFound = FALSE;
       for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) 
       {
           fFound = (lstrcmpi(pme->szModule,  pszModName) == 0) ||
                    (lstrcmpi(pme->szExePath, pszModName) == 0);
           if (fFound) 
           {
               break;
           }
       }
       return (fFound);
   }

   BOOL ThreadFirst(PTHREADENTRY32 pte) const
   {
       return (Thread32First(m_hSnapshot, pte));
   }

   BOOL ThreadNext(PTHREADENTRY32 pte) const
   {
       return (Thread32Next(m_hSnapshot, pte));
   }

   BOOL HeapListFirst(PHEAPLIST32 phl) const
   {
       return (Heap32ListFirst(m_hSnapshot, phl));
   }

   BOOL HeapListNext(PHEAPLIST32 phl) const
   {
       return (Heap32ListNext(m_hSnapshot, phl));
   }

   int HowManyHeaps() const
   {
       HEAPLIST32 hl = { sizeof(hl) };
       int nHowManyHeaps = 0;
       for (BOOL fOk = HeapListFirst(&hl); fOk; fOk = HeapListNext(&hl))
       {
           nHowManyHeaps++;
       }
       return nHowManyHeaps;
   }

   // Note: The heap block functions do not reference a snapshot and
   // just walk the process's heap from the beginning each time. Infinite
   // loops can occur if the target process changes its heap while the
   // functions below are enumerating the blocks in the heap.
   BOOL HeapFirst(PHEAPENTRY32 phe, DWORD dwProcessID, 
                  UINT_PTR dwHeapID) const
   {
       return (Heap32First(phe, dwProcessID, dwHeapID));
   }

   BOOL HeapNext(PHEAPENTRY32 phe) const
   {
       return Heap32Next(phe);
   }

   int  HowManyBlocksInHeap(DWORD dwProcessID, DWORD dwHeapID) const
   {
       HEAPENTRY32 he = { sizeof(he) };
       int nHowManyBlocksInHeap = 0;

       BOOL fOk = HeapFirst(&he, dwProcessID, dwHeapID);
       for (; fOk; fOk = HeapNext(&he))
       {
           nHowManyBlocksInHeap++;
       }
       return nHowManyBlocksInHeap;
   }

   BOOL IsAHeap(HANDLE hProcess, PVOID pvBlock, PDWORD pdwFlags) const
   {
       HEAPLIST32 hl = { sizeof(hl) };
       for (BOOL fOkHL = HeapListFirst(&hl); fOkHL; fOkHL = HeapListNext(&hl)) 
       {
           HEAPENTRY32 he = { sizeof(he) };
           BOOL fOkHE = HeapFirst(&he, hl.th32ProcessID, hl.th32HeapID);
           for (; fOkHE; fOkHE = HeapNext(&he)) 
           {
               MEMORY_BASIC_INFORMATION mbi;
               VirtualQueryEx(hProcess, (PVOID) he.dwAddress, &mbi, sizeof(mbi));
               if (chINRANGE(mbi.AllocationBase, pvBlock,
                   (PBYTE) mbi.AllocationBase + mbi.RegionSize)) 
               {
                   *pdwFlags = hl.dwFlags;
                   return TRUE;
               }
           }
       }
       return FALSE;
   }

public:
   static BOOL EnableDebugPrivilege(BOOL fEnable = TRUE)
   {
       // Enabling the debug privilege allows the application to see
       // information about service applications
       // Assume function fails
       BOOL fOk = FALSE; 
       HANDLE hToken;
       
       // Try to open this process's access token
       if (OpenProcessToken(GetCurrentProcess(), 
                TOKEN_ADJUST_PRIVILEGES, &hToken)) 
       {
           // Attempt to modify the "Debug" privilege
           TOKEN_PRIVILEGES tp;
           tp.PrivilegeCount = 1;
           LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
           tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
           AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
           fOk = (GetLastError() == ERROR_SUCCESS);
           CloseHandle(hToken);
       }
       return fOk;
   }

   static BOOL ReadProcessMemory(DWORD dwProcessID, LPCVOID pvBaseAddress,
       PVOID pvBuffer, DWORD cbRead, PDWORD pdwNumberOfBytesRead = NULL)
   {
       return Toolhelp32ReadProcessMemory(dwProcessID, pvBaseAddress, pvBuffer,
           cbRead, pdwNumberOfBytesRead);
   }
};

#endif /* !defined(_CA_TLHLP_H_) */
