Build/main.exe: Src/*.cpp
	powershell "New-Item -ErrorAction Ignore -ItemType directory $(dir $@); $$null"
	cl.exe /Zi /EHsc /Fe: $@ Src/main.cpp /link /LIBPATH Lib/x86/LogitechLEDLib.lib

.PHONY: clean
clean:
	powershell "Remove-Item -Recurse -Force -ErrorAction Ignore Build; $$null"