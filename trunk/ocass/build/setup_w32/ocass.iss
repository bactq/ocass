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
Source: "../run_data/ocass.cfg_sample_dbg"; DestDir: "{app}"
Source: "../../support_libs/rtl/dbghelp.dll"; DestDir: "{app}"
Source: "../../support_libs/rtl/msvcrt.dll"; DestDir: "{app}"
[Dirs]
Name: "{app}\datum"
Name: "{app}\history"
Name: "{app}\log"
