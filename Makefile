# ################################################################################################ #
#
# Copyright (C) 2012 Philip Cronje. Distributed under the terms of the BSD 2-Clause License. See
# LICENSE.txt in the source distribution for the full terms of the license.
#
# ################################################################################################ #
.PHONY: clean debug release

release: CXXFLAGS += -O2
release: CreepWrap.exe
	$(STRIP) CreepWrap.exe

debug: CXXFLAGS += -g
debug: CreepWrap.exe

CreepWrap.exe: CreepWrap.o CreepWrap.res JVMInvoke.o JVMInvokeWindows.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

CreepWrap.res: CreepWrap.rc
	$(WINDRES) $< -o $@ -O coff

CreepWrap.rc: AppIcon.ico

clean:
	$(RM) *.exe
	$(RM) *.o
	$(RM) *.res
