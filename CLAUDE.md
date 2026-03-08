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
comms.cpp/.h, timinggenerator.cpp/.h, mips.cpp/.h

## Phase 2 — COMPLETE
Function-level doc comments added to all Phase 1 files.

## Phase 3 — IN PROGRESS

### Branch strategy
- Current branch: `refactor/phase3-cleanup`
  - Branched from `refactor/phase1-cleanup`
  - Merge to `refactor/phase1-cleanup` for testing
  - Only merge to main after hardware testing confirms nothing broken

### Completed in Phase 3
All 13 widget classes extracted from controlpanel.cpp/.h into separate files.
Each file received full Phase 1 + Phase 2 treatment during extraction.

| New file            | Class(es) extracted        | Notes |
|---------------------|---------------------------|-------|
| textlabel.cpp/.h    | TextLabel                 | |
| shutdown.cpp/.h     | Shutdown                  | |
| saveload.cpp/.h     | SaveLoad                  | Fixed eventFilter base-class call |
| cpbutton.cpp/.h     | CPbutton                  | |
| dacchannel.cpp/.h   | DACchannel                | returnPressed, remove toStdString |
| dcbgroups.cpp/.h    | DCBiasGroups              | Fixed missing `: QWidget(parent)`, typos |
| esi.cpp/.h          | ESI                       | returnPressed, remove toStdString |
| ccontrol.cpp/.h     | Ccontrol                  | SKIPCOUNT → skipCount instance var |
| cpanel.cpp/.h       | Cpanel                    | Circular dep resolved via fwd decl |
| statuslight.cpp/.h  | StatusLight, LightWidget, TrafficLightWidget | Header-only helpers |
| textmessage.cpp/.h  | TextMessage               | |
| table.cpp/.h        | Table                     | Fixed SetValues bool/string bug |
| slider.cpp/.h       | Slider                    | Removed dead res variable in Show() |

`controlpanel.cpp/.h` Phase 1+2 cleanup complete:
- `loadConfig()` extracted from the 970-line constructor
- All 33 SIGNAL/SLOT connects modernised
- NULL → nullptr throughout
- Dead commented code removed
- Doc comments on every function (complete coverage)

### Remaining in Phase 3
- `comms.cpp/.h` — Phase 1+2 (deferred structural file)
- `timinggenerator.cpp/.h` — Phase 1+2 (deferred structural file)
- `mips.cpp/.h` — Phase 1+2 (deferred structural file)

Optional (lower priority):
- Split ControlPanel implementation across multiple files:
  - `controlpanel_update.cpp` — UpdateStateMachine, shutdown/restore
  - `controlpanel_save.cpp` — Save, Load, slotSaveCP
  - `controlpanel_command.cpp` — Command, tcpCommand

### Complications to watch for in remaining Phase 3 files
- `comms.cpp/.h`: likely has toStdString().c_str() patterns, SIGNAL/SLOT macros
- `timinggenerator.cpp/.h`: large file, likely many SIGNAL/SLOT macros
- `mips.cpp/.h`: top-level app class, touches most other classes

## Established conventions
- File header: GAA-style border (see any completed file for example)
- Modern Qt5/6 connect syntax throughout — NO SIGNAL/SLOT macros
  - Exception: BlockingQueuedConnection connects to ControlPanel kept as SIGNAL/SLOT
- returnPressed not editingFinished for user-entry line edits
- comms->SendCommand() takes QString directly — no .toStdString().c_str()
- Remove all qDebug() calls
- Remove all commented-out dead code
- Fix typos
- nullptr not NULL
- `/*!` doc comment on every function (Phase 2 style)

## Phase 4 — NOT STARTED
Safety/bounds checks — hardware test required.

## Phase 5 — NOT STARTED
Non-blocking Twave — firmware only.

## Repo
- github.com/GordonAnderson/MIPS_QT6
- Local: ~/Projects/MIPS_QT6
