;
; Copyright (C) 2007 LeXiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
; This file is part of ocass
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either
; version 2 of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
Source: "../../support_libs/rtl/dbghelp.dll"; DestDir: "{app}"
Source: "../../support_libs/rtl/msvcrt.dll"; DestDir: "{app}"

[Dirs]
Name: "{app}\datum"
Name: "{app}\history"
Name: "{app}\log"

[Icons]
Name: "{group}\MS Office Communicator Assistant\Assistant control"; Filename: "{app}\ocassw.exe"; WorkingDir: "{app}"
Name: "{group}\MS Office Communicator Assistant\Assistant control (Safe)"; Filename: "{app}\ocassw.exe"; WorkingDir: "{app}"; Parameters: "-S"
Name: "{group}\MS Office Communicator Assistant\Uninstall"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "ocass_autorun"; ValueData: "{app}\ocassw.exe"; Flags: uninsdeletekeyifempty uninsdeletevalue

