set execPath=%~dp0
set fileName=%~n1
/masm32/bin/ml /c /coff "%1"
/masm32/bin/link /SUBSYSTEM:CONSOLE %fileName%.obj