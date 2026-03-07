# MIPS Qt Host App — Refactoring Project

## Active branch: refactor/phase1-cleanup
## Current phase: Phase 2 (function-level doc comments on completed files)

## Established conventions
- File header: GAA-style border (see any completed file for example)
- Modern Qt5/6 connect syntax throughout — NO SIGNAL/SLOT macros
  - Exception: BlockingQueuedConnection connects to ControlPanel kept as SIGNAL/SLOT
- returnPressed not editingFinished for user-entry line edits
- comms->SendCommand() takes QString directly — no .toStdString().c_str()
- Remove all qDebug() calls
- Remove all commented-out dead code
- Fix typos

## Phase plan
- Phase 1: dead code, magic numbers, typos, file headers (COMPLETE)
- Phase 2: function-level doc comments — same branch, zero risk
- Phase 3: structural (new branch after Phase 1+2 merge to main)
  - Split controlpanel.cpp into 14 classes
  - Decompose Comms constructor
  - faims.cpp arc detection extraction
- Phase 4: safety/bounds checks — hardware test required
- Phase 5: non-blocking Twave (firmware only)

## Repo
- github.com/GordonAnderson/MIPS_QT6
- Local: ~/Projects/MIPS_QT6

## Phase 1 complete — committed files
arb.cpp/.h, faims.cpp/.h, cmdlineapp.cpp/.h, scriptingconsole.cpp/.h,
rfdriver.cpp/.h, mipscomms.cpp/.h, settingsdialog.cpp/.h, 
zmqworker.cpp/.h, compressor.cpp/.h, psviewer.cpp/.h,
arbwaveformedit.cpp/.h, pse.cpp/.h, psg.cpp/.h, twave.cpp/.h,
rfamp.cpp/.h, dcbias.cpp/.h, filament.cpp/.h, tcpserver.cpp/.h,
modbus.cpp/.h, program.cpp/.h, properties.cpp/.h, dio.cpp/.h,
adc.cpp/.h, device.cpp/.h, help.cpp/.h, console.cpp/.h,
ringbuffer.cpp/.h, cdirselectiondlg.cpp/.h, Utilities.cpp/.h,
main.cpp, plot.cpp/.h

## Phase 1 deferred to Phase 3 (structural)
comms.cpp/.h, timinggenerator.cpp/.h, mips.cpp/.h, controlpanel.cpp/.h
```
