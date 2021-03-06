#
# Makefile for TW.EXE
#

CC = TCC
LINKER = TLINK
LIBPATH = C:\TC\LIB
INCLUDEPATH = C:\TC\INCLUDE;C:\TW\SRC\COMMON;C:\TW\SRC\SCUW\THAISYLS
CCFLAGS = -c -w -DCW -d -O -ml -f- -Z -K -k- -C -I$(INCLUDEPATH) -L$(LIBPATH)

\TW\TW.EXE: \
        ..\COMMON\EDISP.OBJ ..\COMMON\HDISP.OBJ ..\COMMON\ADISP.OBJ \
        ..\COMMON\GRDETECT.OBJ \
        ..\COMMON\CWGRPHC.OBJ ..\COMMON\CWEGRPH.OBJ ..\COMMON\CWAGRPH.OBJ \
        ..\COMMON\EKBD.OBJ \
        ..\COMMON\CSCRN.OBJ ..\COMMON\FILEUTIL.OBJ ..\COMMON\DIR.OBJ \
        ..\COMMON\SOUND.OBJ ..\COMMON\CUSONG.OBJ \
        SCRN.OBJ GETSTR.OBJ TUTIL1.OBJ EDIT.OBJ INS.OBJ DEL.OBJ \
        MENU.OBJ \
        KBD.OBJ STATUS.OBJ MOVE.OBJ MOVEMENT.OBJ WRAP.OBJ \
        ONSCRN.OBJ FILE.OBJ SEARCH.OBJ BLOCK.OBJ CW.OBJ CWSETUP.OBJ
    $(CC) -c ..\COMMON\STAMPDATE.C
    $(LINKER) @SCUW.TLR

SCRN.OBJ: SCRN.C VAR.H
    $(CC) $(CCFLAGS) SCRN.C

GETSTR.OBJ: GETSTR.C VAR.H
    $(CC) $(CCFLAGS) GETSTR.C

TUTIL1.OBJ: TUTIL1.C VAR.H
    $(CC) $(CCFLAGS) TUTIL1.C

EDIT.OBJ: EDIT.C VAR.H
    $(CC) $(CCFLAGS) EDIT.C

INS.OBJ: INS.C VAR.H
    $(CC) $(CCFLAGS) INS.C

DEL.OBJ: DEL.C VAR.H
    $(CC) $(CCFLAGS) DEL.C

MENU.OBJ: MENU.C VAR.H ..\COMMON\CWGRPHC.H
	$(CC) $(CCFLAGS)) MENU.C

KBD.OBJ: KBD.C VAR.H
    $(CC) $(CCFLAGS) KBD.C

STATUS.OBJ:  STATUS.C VAR.H
    $(CC) $(CCFLAGS) STATUS.C

MOVE.OBJ: MOVE.C VAR.H
    $(CC) $(CCFLAGS) MOVE.C

MOVEMENT.OBJ: MOVEMENT.C VAR.H
    $(CC) $(CCFLAGS) MOVEMENT.C

WRAP.OBJ: WRAP.C VAR.H
    $(CC) $(CCFLAGS) WRAP.C

ONSCRN.OBJ: ONSCRN.C VAR.H
    $(CC) $(CCFLAGS) ONSCRN.C

FILE.OBJ: FILE.C VAR.H
    $(CC) $(CCFLAGS) FILE.C

SEARCH.OBJ: SEARCH.C VAR.H
    $(CC) $(CCFLAGS) SEARCH.C

BLOCK.OBJ: BLOCK.C VAR.H
	$(CC) $(CCFLAGS) BLOCK.C

CW.OBJ: CW.C VAR.H ..\COMMON\CWGRPHC.H
	$(CC) $(CCFLAGS) CW.C

CWSETUP.OBJ: CWSETUP.C VAR.H
    $(CC) $(CCFLAGS) CWSETUP.C
