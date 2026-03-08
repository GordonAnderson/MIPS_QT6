# MIPS Qt Host App — Refactoring Project

## Phase 1 — COMPLETE
Dead code removal, magic numbers, typos, file headers, modern connect syntax,
returnPressed, remove toStdString casts.

### Refactored files
arb.cpp/.h, faims.cpp/.h, cmdlineapp.cpp/.h, scriptingconsole.cpp/.h,
rfdriver.cpp/.h, mipscomms.cpp/.h, settingsdialog.cpp/.h,
zmqworker.cpp/.h, compressor.cpp/.h, psviewer.cpp/.h,
arbwaveformedit.cpp/.h, pse.cpp/.h, psg.cpp/.h, twave.cpp/.h,
rfamp.cpp/.h, dcbias.cpp/.h, filament.cpp/.h, tcpserver.cpp/.h,
modbus.cpp/.h, program.cpp/.h, properties.cpp/.h, dio.cpp/.h,
adc.cpp/.h, device.cpp/.h, help.cpp/.h, console.cpp/.h,
ringbuffer.cpp/.h, cdirselectiondlg.cpp/.h, Utilities.cpp/.h,
main.cpp, plot.cpp/.h

### Deferred to Phase 3 (structural)
comms.cpp/.h, timinggenerator.cpp/.h, mips.cpp/.h, controlpanel.cpp/.h

## Phase 2 — COMPLETE
Function-level doc comments added to all Phase 1 files.

## Phase 3 — NOT STARTED

### Branch strategy
- Current branch: `refactor/phase1-cleanup` (Phase 1+2 complete)
- Phase 3 branch: `refactor/phase3-structural`
  - Branch FROM `refactor/phase1-cleanup` (not main)
  - Merge Phase 3 back to `refactor/phase1-cleanup` for testing
  - Only merge to main after hardware testing confirms nothing broken

### controlpanel.cpp restructuring plan
Extract 13 widget classes into separate `.cpp/.h` files named after
the class they contain:
- textlabel.cpp/.h
- shutdown.cpp/.h
- saveload.cpp/.h
- cpbutton.cpp/.h
- dacchannel.cpp/.h
- dcbgroups.cpp/.h
- esi.cpp/.h
- ccontrol.cpp/.h
- cpanel.cpp/.h
- statuslight.cpp/.h
- textmessage.cpp/.h
- table.cpp/.h
- slider.cpp/.h

Extract `loadConfig()` from the 970-line constructor.

Optionally split ControlPanel implementation across:
- `controlpanel.cpp` — constructor, destructor, core
- `controlpanel_update.cpp` — UpdateStateMachine, shutdown/restore
- `controlpanel_save.cpp` — Save, Load, slotSaveCP
- `controlpanel_command.cpp` — Command, tcpCommand

### Complications to watch for in Phase 3
- All new widget headers need `#include "Utilities.h"` (moveWidget, adjustValue)
- Convert SIGNAL/SLOT macros to modern connect syntax during extraction
- Fix `.toStdString().c_str()` → direct QString during extraction
- `controlpanel.h` remains the single external include — new widget headers
  are included from it, no other files need updating
- `Cpanel` has circular dependency with `ControlPanel` — resolve with forward
  declaration in cpanel.h; cpanel.cpp includes controlpanel.h
- `SKIPCOUNT` macro only lives in controlpanel.h — replace with instance
  variable `skipCount` in Ccontrol::Update()

## Established conventions
- File header: GAA-style border (see any completed file for example)
- Modern Qt5/6 connect syntax throughout — NO SIGNAL/SLOT macros
  - Exception: BlockingQueuedConnection connects to ControlPanel kept as SIGNAL/SLOT
- returnPressed not editingFinished for user-entry line edits
- comms->SendCommand() takes QString directly — no .toStdString().c_str()
- Remove all qDebug() calls
- Remove all commented-out dead code
- Fix typos

## Phase 4 — NOT STARTED
Safety/bounds checks — hardware test required.

## Phase 5 — NOT STARTED
Non-blocking Twave — firmware only.

## Repo
- github.com/GordonAnderson/MIPS_QT6
- Local: ~/Projects/MIPS_QT6
