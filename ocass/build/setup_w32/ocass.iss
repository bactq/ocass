;
; OCASS - Microsoft Office Communicator Assistant 
;         (http://code.google.com/p/ocass/)
;
; Copyright (C) 2007 Le Xiongjia 
; 
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
; Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
;

[Setup]
AppName=MS Office Communicator Assistant
AppVerName=MS Office Communicator Assistant 0.0.0.1 alpha
DefaultDirName={pf}\LSoft\ocass
DefaultGroupName=LSoft

[Files]
Source: "../msdev6/Release/ocassw.exe"; DestDir: "{app}"
Source: "../msdev6/Release/liboca.dll"; DestDir: "{app}"
Source: "../msdev6/Release/libocc.dll"; DestDir: "{app}"
Source: "../msdev6/Release/liboch.dll"; DestDir: "{app}"
Source: "../msdev6/Release/libocs.dll"; DestDir: "{app}"
Source: "../run_data/msg_style.xsl"; DestDir: "{app}\template"
Source: "../run_data/ocass.cfg_sample_dbg"; DestDir: "{app}\cfg_sample"
Source: "../run_data/ocass.cfg_sample"; DestDir: "{app}\cfg_sample"
Source: "../run_data/license.txt"; DestDir: "{app}"
Source: "../../support_libs/rtl/dbghelp.dll"; DestDir: "{app}"
Source: "../../support_libs/rtl/msvcrt.dll"; DestDir: "{app}"

[Dirs]
Name: "{app}\datum"
Name: "{app}\history"
Name: "{app}\log"

[Icons]
Name: "{group}\MS Office Communicator Assistant\Assistant control"; Filename: "{app}\ocassw.exe"; WorkingDir: "{app}"
Name: "{group}\MS Office Communicator Assistant\Assistant control (Safe)"; Filename: "{app}\ocassw.exe"; WorkingDir: "{app}"; Parameters: "-S"
Name: "{group}\MS Office Communicator Assistant\License"; Filename: "{app}\license.txt"; 
Name: "{group}\MS Office Communicator Assistant\Uninstall"; Filename: "{uninstallexe}"

;
; remove auto startup
; [Registry]
; Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "ocass_autorun"; ValueData: "{app}\ocassw.exe"; Flags: uninsdeletekeyifempty uninsdeletevalue
;
