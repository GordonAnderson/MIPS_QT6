//
//  MIPS host application revision history:
//
//  1.0, July 6, 2015
//      1.) Initial release.
//  1.1, July 8, 2015
//      1.) Added MIPS firmware download function. Works on PC and Mac.
//      2.) Removed the 1200 baud rate and all rates below 9600.
//  1.2, July 12, 2015
//      1.) Fixed bugs in the firmware download feature.
//      2.) Tested on Mac and PC.
//      3.) Resolved several screen issues transitioning from PC to Mac. Most features tested.
//  1.3, November 30, 2015
//      1.) Added socket support for networked MIPS.
//  1.4, January 10, 2016
//      1.) Added MIPS firmware save and boot bit setting functions.
//  1.5, February 17, 2016
//      1.) Fixed bug in DC bias page that caused control focus to update with old values
//          when returning to the page.
//      2.) Fixed bug that prevented all 16 values from being edited on the DC bias page.
//      3.) Updated range of DC bias page to reflect offset values.
//  1.6, June 27, 2016
//      1.) Started refactoring code. Created the Comms class and moved all communications to it.
//      2.) Added the Twave class and tab.
//      3.) To do: Create classes for each module and finish refactoring.
//  1.7, Sept 16, 2016
//      1.) Updated help files.
//      2.) Added new clock modes to PSG.
//      3.) Added tooltip popup text.
//  1.8, Oct 16, 2016
//      1.) Added fwd/rev to Twave options.
//      2.) Added Sweep functions to Twave tab.
//      3.) Updated the MIPS commands help function.
//      4.) Updated the Twave help data.
//      5.) Added frequency control to DIO page.
//  1.9, March 7, 2017
//      1.) Added the FAIMS tab and CV parking capability.
//      2.) Updated help text.
//      3.) Updated MIPS command summary.
//  1.10, April 9, 2017
//      1.) Added the Single Funnel control panel.
//      2.) Added the Soft Landing control panel.
//      3.) Allowed DC bias module to auto-update.
//      4.) Added filament tab.
//      5.) Changed connection logic to only show supported tabs after connecting.
//  1.11, April 29, 2017
//      1.) Updated ARB dialog to support 4 ARB modules (untested).
//      2.) To do: Make ARB context-sensitive help for multi-pass table functions.
//  1.12, May 9, 2017
//      1.) Updated soft landing form. Added mass units and minor improvements
//          requested by the customer.
//  1.13, June 24, 2017
//      1.) Added support for multiple MIPS systems. New framework allows control
//          panels to communicate with multiple MIPS systems simultaneously.
//      2.) Updated serial port scanning.
//      3.) Changed the disconnection procedure.
//  1.14, July 24, 2017
//      1.) Added host interface commands for file transfer to SD cards and
//          sending/receiving EEPROM module configuration data.
//  1.15, August 22, 2017
//      1.) Added ramp up/down pulse generation to the sequence generator.
//      2.) Fixed minor bugs.
//      3.) Fixed bug in loop timepoint calculation for sequence generator.
//  1.16, November 21, 2017
//      1.) Updated grid fragmentor and added plotting capability.
//      2.) Added the soft landing control panel.
//  1.17, November 25, 2017
//      1.) Added the pulse sequence generator to the grid control panel.
//  1.18, December 12, 2017
//      1.) Added ADC tab and data collection capability.
//      2.) Updated grid PSG.
//  1.19, December 27, 2017
//      1.) Updated the PSG in the Grid control panel:
//             - Enabled Ts1 set to zero to stop the pulse.
//             - Outputs are now driven high when pulses are stopped.
//  1.20, January 1, 2018
//      1.) Inverted shutter signals in the Grid PSG.
//      2.) Added "Ts1=Ts2" and "Ts2=Ts1" checkboxes.
//  1.21, February 18, 2018
//      1.) Added scripting capability to Purdue soft landing control panel.
//  1.22, March 11, 2018
//      1.) Added support for programming ARB Flash memory.
//      2.) Added arrow key value adjustments for DC bias and Twave.
//      3.) Added limits for Twave values, specifically pulse voltage.
//  1.23, March 12, 2018
//      1.) Added Shift+Alt+Arrow to decrease increment by 10x.
//      2.) Fixed limit bug in Twave frequency for arrow keys.
//  1.24, May 12, 2018
//      1.) Added grouping capability for DC bias voltages.
//  1.25, August 2, 2018
//      1.) Added user-definable control panels.
//  1.26, Sept 9, 2018
//      1.) Added documentation and updated help.
//      2.) Corrected PSG label errors and made the PSG box modal.
//      3.) Added JavaScripting.
//  1.27, Oct 5, 2018
//      1.) Fixed start module ID issue.
//      2.) Fixed Ethernet port MIPS discovery.
//  1.28, Oct 8, 2018
//      1.) Fixed DC bias menu crash occurring when CH 9 or above was changed.
//      2.) Added traffic heartbeats to keep Ethernet connections alive.
//  1.29, Nov 1, 2018
//      1.) Added RFamp to the Custom Control Panel.
//  1.30, Nov 17, 2018
//      1.) Added TCP/IP socket communications for control panels.
//      2.) Added DAC channels to control panel.
//      3.) Added ADC channels to control panel.
//      4.) Fixed RFamp shutdown to include DC voltages.
//      5.) Fixed control panel crash on save method.
//  1.31, Nov 20, 2018
//      1.) Added new timing generation.
//      2.) Fixed bugs in control panel.
//      3.) Partial refactor of control panel code.
//  1.32, Nov 27, 2018
//      1.) Fixed bug where RFamp failed to turn off DC supplies.
//      2.) Added DC power supply enable to RFamp.
//      3.) DC offsets now save first when saving method files.
//  1.33, Dec 2, 2018
//      1.) Added new timing generator to control panel.
//      2.) Added TCP/IP updates to control panel.
//      3.) Added external trigger options to FAIMS.
//  1.34, Dec 13, 2018
//      1.) Fixed various bugs in the timing generator control.
//  1.35, Dec 15, 2018
//      1.) Added Save, Load, and Clear All buttons to timing generator.
//      2.) Fixed timing generator bugs.
//  1.36, Dec 26, 2018
//      1.) Improved control panel performance using batch read commands for DC bias and RF drivers.
//      2.) Addressed stability issues causing system crashes.
//      3.) Added properties page for folder settings and startup behavior.
//  1.37, Jan 6, 2019
//      1.) Added versioning to method files saved from control panel.
//      2.) Logged control panel filenames in method files.
//      3.) Queued signals from the acquire program.
//      4.) Disabled trigger button during active events.
//      5.) Added automatic comm port reopening on timeout.
//  1.38, Jan 7, 2019
//      1.) Added logging to properties and implemented log statements.
//  1.39, Jan 11, 2019
//      1.) Added logging for all MIPS and control panel status bar messages.
//      2.) Added delays to the restore function in the control panel.
//  1.40, Jan 12, 2019
//      1.) Fixed minor bugs.
//      2.) Updated logging to include MIPS FW version in control panel logs.
//      3.) Added data log option.
//  1.41, Jan 14, 2019
//      1.) Added serial critical errors to logging.
//  1.42, Jan 16, 2019
//      1.) Updated control panel restore: zero voltages, turn on supplies, then reset voltages.
//      2.) Updated script acquire function to abort if acquisition is already in progress.
//      3.) Added MIPS data validation before populating UI text boxes.
//  1.43, Jan 19, 2019
//      1.) Fixed error in Compressor dialog causing mode/switch radio button failure.
//  1.44, Jan 22, 2019
//      1.) Removed lock/unlock commands due to instability.
//  1.45, Jan 24, 2019
//      1.) Removed "return if busy" logic in Comms.
//  1.46, Jan 25, 2019
//      1.) Redesigned Comms code to block until transactions complete.
//      2.) Added file path to acquire dialog.
//  1.47, Feb 1, 2019
//      1.) Added processEvents to control panel update routine.
//      2.) Set processEvents interval to 10ms in script processor.
//      3.) Switched to non-queued event processing in console window.
//  1.48, Feb 2, 2019
//      1.) Removed lock code from control panel.
//      2.) Removed queue options from timing class connectors.
//      3.) Removed ring-buffer "wait for line" function dependency.
//  1.49, Feb 5, 2019
//      1.) Added "wait for start" to cmdlineapp process start.
//      2.) Fixed memory leak in the acquire class.
//      3.) Fixed ARB directory bug in control panel code.
//  1.50, Feb 14, 2019
//      1.) Fixed bug in control panel ARB waveform editor.
//      2.) Removed doEvents from control panel update to prevent script-interruption freezes.
//  1.51, Feb 21, 2019
//      1.) Added update rate adjustments to properties page.
//      2.) Added MIPS box names to method files.
//      3.) Added "wait for update" function for scripting.
//      4.) Added MIPS names to comm errors.
//  1.52, May 19, 2019
//      1.) Added uptime to method file saves.
//      2.) RF drive now zeros before auto-tune with 1s delay (fixes high-power tune bug).
//      3.) Temporary fix for ARB edit dependency on PPP.
//      4.) Added support for child control panels (nested popups).
//      5.) Added path validation at experiment start.
//      6.) Added closed-loop option to RF driver in control panel.
//      7.) Added AMPS support with baud rate selection and specific serial settings.
//      8.) Added Group Box commands for UI organization.
//      9.) Added Script Button for automated script calls.
//      10.) Added plotting capability to control panel.
//      11.) Linked timing generator start/width values to other event values.
//  1.53, June 18, 2019
//      1.) Added .Start and .Width selection fields to timing generator.
//      2.) Added rename function to timing event editor.
//  1.54, July 5, 2019
//      1.) Added 4, 5, and 6-bit Hadamard multiplexing to PSG.
//      2.) Added heatmap plotting for FAIMSFB work.
//  1.55, July 13, 2019
//      1.) Added serial device capability to control panels.
//  1.56, July 18, 2019
//      1.) Added Mux bits 7, 8, and 9.
//      2.) Added char-send delays to MIPS to support long tables.
//  1.57, July 23, 2019
//      1.) Added socket commands for timing trigger, edit, and abort.
//      2.) Fixed 1.56 bug where 10ms delay applied to every command incorrectly.
//  1.58, August 6, 2019
//      1.) Fixed bug in multiplexing file generation.
//  1.59, August 27, 2019
//      1.) Fixed bugs in control panel CSV functions.
//      2.) Added command processing for script buttons.
//  1.60, Sept 2, 2019
//      1.) Added -A option for Accumulations in the acquire app.
//      2.) Implemented save/restore for timing generator Mux state.
//  1.61, Sept 25, 2019
//      1.) Added shutdown and restore for ARB channels in control panel.
//  1.62, Oct 17, 2019
//      1.) Added "time in ms" mode to timing generator.
//      2.) Added external trigger mode to timing generator.
//      3.) Fixed bug in DIO process command.
//  1.63, Oct 18, 2019
//      1.) Fixed bug in timing generation upgrades for time-based operations.
//  1.64, Oct 24, 2019
//      1.) Fixed bug where acquire app returned to local mode; now remains in table mode.
//      2.) Fixed 1.60 bug regarding total TOFs sent to acquire app.
//  1.65, Oct 25, 2019
//      1.) Fixed external trigger bug in timing generator.
//      2.) Added support for multiple timing generators; last in list controls the acquire app.
//      3.) Updated TCP server connection method.
//      4.) Updated plot scripting functions.
//      5.) Fixed plotting crash on invalid entries.
//  1.66, Nov 1, 2019
//      1.) Added low-level MIPS comms to TCP/IP server.
//      2.) Added EventControl to edit table parameters on the fly.
//      3.) Added external clock support for timing generator.
//      4.) Added Savitzky-Golay filter to plot function.
//  1.67, Nov 7, 2019
//      1.) Fixed TCP server interface to timing control.
//      2.) Fixed EventControl to update values in the timing control.
//  1.68, Nov 10, 2019
//      1.) Added isTblMode flag to timing generator.
//      2.) Changed EventControl options to .Value and .ValueOff.
//  1.69, Nov 18, 2019
//      1.) Updated TCP server read routine to respect ring buffer capacity.
//  1.70, Dec 13, 2019
//      1.) Updated RFamp interface for pole bias and resolving DC bias enable/disable.
//  1.71, Jan 13, 2020
//      1.) Added SendCommand to control panel for raw MIPS command execution.
//  1.72, Jan 23, 2020
//      1.) Added CheckBox to custom controls.
//      2.) Added arrow key value adjustments to DAC control.
//      3.) Allowed script buttons to be called on every update cycle.
//  1.73, Feb 5, 2020
//      1.) Added timing generator commands to return start time and channel number.
//  1.74, Feb 17, 2020
//      1.) Fixed control panel bug when loading settings with overlapping names.
//      2.) Added arrow key value control to ARB in control panel.
//  1.75, March 11, 2020
//      1.) Added backspace (0x08) support to console.
//  1.76, March 30, 2020
//      1.) Added validity testing for Mux timing generation.
//      2.) Fixed sign bug in timing generator.
//  1.77, April 25, 2020
//      1.) Fixed PC-specific automatic serial port reconnection bug.
//      2.) Added SerialWatchDog support.
//  1.78, May 10, 2020
//      1.) Added Shutter timing control to the control panel.
//  1.79, May 14, 2020
//      1.) Redesigned Shutter timing control to fix significant bugs.
//  1.80, Sept 11, 2020
//      (2nd Gen Soft Landing Updates)
//      1.) Updated ADC to use LOG output on gauges; added "OFF" threshold.
//      2.) Disabled input on DIO channels.
//      3.) Allowed DIO disable in control panel.
//      4.) Fixed script button bug.
//      5.) Fixed RF quad enable state restoration.
//      6.) Added "On Exit" script button.
//  1.81, Dec 6, 2020
//      1.) Added time sweep function to shutter timing generator.
//  1.82, Jan 26, 2021
//      1.) Added 57600 baud support to serial object.
//      2.) Added .Update command for spectrum scripts.
//  1.83, Feb 4, 2021
//      1.) Updated acquire system to support persistent running/restarting.
//  1.84, Feb 28, 2021
//      1.) Updated FAIMS to include auto-tune and curtain supply control.
//  1.85, May 2, 2021
//      1.) Added auto-reconnect when control panel is not running.
//      2.) Comms object now reads and stores MIPS major.minor version.
//      3.) Updated FAIMS tab to toggle options based on MIPS version.
//      4.) Added negative peak tune and curtain supply options.
//      5.) Added Tab control to custom panels.
//  1.86, May 7, 2021
//      1.) Added "Load Control Panel" button option.
//      2.) App now reads MIPSname.ini from the application directory for serial connections.
//  1.87, May 29, 2021
//      1.) Resolved minor control panel bugs.
//      2.) Improved folder searching for config files.
//      3.) UI/UX usability improvements.
//  1.88, July 31, 2021
//      1.) Added features to control panel plot command.
//      2.) Fixed minor bugs in custom control updates during load.
//      3.) Updated terminal serial read function.
//  1.89, Sept 10, 2021
//      1.) Updated property load/save priority: app location first, then home directory.
//      2.) Added deployment processing to dev environment.
//      3.) Added app signing.
//  1.90, Oct 12, 2021
//      1.) Fixed scripting bug where partial string matches occurred (e.g., CH 1 matching CH 10).
//      2.) Fixed heatmap to use full window when only one graph is active.
//  1.91, Nov 21, 2021
//      1.) Added "Repeat" event feature for timing tables.
//  1.92, Dec 11, 2021
//      1.) Added pulse sequence ramps (Init/Ramp parameters).
//      2.) Added functions to change signal names in timing generator.
//  1.93, Jan 15, 2022
//      1.) Sub-control panels now inherit timing generator controls from parent.
//      2.) Synced sub-control panel save/load settings with parent.
//  1.94, March 1, 2022
//      1.) Fixed bug in time mode checkbox serialization.
//  1.95, March 11, 2022
//      1.) Fixed PlotCommand bug for non-integer x-axis steps.
//      2.) Event control now ignores "?" returns from table updates.
//  1.96, June 22, 2022
//      1.) Fixed RFChannel SetValue bug where Setpoint and Freq were reversed.
//  1.97, July 25, 2022
//      1.) Added text message options to control panel functions.
//  1.98, Aug 7, 2022
//      1.) Added TCP client support to control panel.
//  1.99, Oct 2, 2022
//      1.) Fixed crash in plot AddPoint function when overflowed with values.
//      2.) Added port address to programming function.
//      3.) Added RFmega (ItsyBitsy M4) programming support.
//  1.100, Nov 3, 2022
//      1.) Exposed SendSerial function in MIPS.
//      2.) Script buttons now take focus to improve UI feedback.
//      3.) Minor bug fixes.
//  1.101, Nov 19, 2022
//      1.) Fixed plot bug occurring when multiple scans were added.
//  1.102, Dec 23, 2022
//      1.) Added Modbus support (specifically for Novus temperature controllers).
//  1.103, Dec 29, 2022
//      1.) Improved algorithm for long-time calculations.
//      2.) Fixed ARB compressor trigger bug from the table.
//  1.104, April 29, 2023
//      1.) Added STRINGL to custom controls for long string support.
//  1.105, May 30, 2023
//      1.) General updates to plotting functions.
//  1.106, Sept 19, 2023
//      1.) Added arrow key value adjustments to custom controls.
//      2.) Added STEP command to config to set arrow increment sizes.
//      3.) Added system font size adjustments.
//  1.107, Nov 11, 2023
//      1.) Added "Define" and subroutines to control panel.
//      2.) SendMessage can now return empty strings.
//      3.) Added commands to programmatically press control panel buttons.
//      4.) Stripped whitespace from tokens in config files.
//  1.108, Nov 20, 2023
//      1.) Added RF driver control support from the PSG in the control panel.
//  1.109, Mar 1, 2024
//      1.) Fixed DC bias channel grouping bug (previously only worked with arrow keys).
//  1.110, April 14, 2024
//      1.) Added delete option to plot commands.
//      2.) Added GenerateUniqueFileName scripting command.
//
//  2.0, June 23, 2024
//      1.) Completed port to Qt 6.7.1.
//      2.) Resolved most warnings; Modbus remains for future deprecation cleanup.
//      3.) Added "RemoveLast" plot command for error recovery.
//      4.) Fixed various porting-related bugs.
//  2.1beta, 2024
//      1.) Added mouse wheel value adjustments; refactored arrow key logic.
//      2.) Added support for loading/displaying comments with settings.
//      3.) Implemented drag-and-drop for UI controls (left-click top-left).
//      4.) Cfg files now auto-update with new position data.
//      5.) Added file association support for starting app with config files.
//      6.) Added ImageBox for dynamic image loading.
//      7.) Removed legacy static control panel menu options.
//      8.) Discontinued IFT support.
//      9.) Renamed "Select..." to "Load configuration...".
//      10.) Removed shutter from control panel.
//      11.) Added embedded plots for forms with script data access.
//      12.) Set processEvents to false in control panel to fix threading-related scripting issues.
//      13.) Fixed UpdateHalted flag reset on script abort.
//      14.) Added Abort command to script buttons.
//      15.) Fixed heatmap averaging bug.
//      16.) Resolved plot resource cleanup issues.
//      17.) Reduced serial receive signal delay from 10ms to 2ms.
//      18.) Added "plotonly" command.
//      19.) Added OpenFile and ClearComments plot features.
//      20.) Fixed long-standing checkbox focus-change signal bug.
//      21.) Optimized plot loading by preventing unnecessary message object creation.
//  2.2, December 21, 2024
//      1.) Removed beta tag; version fully tested.
//      2.) Added capability to read MIPS option files to modify control panels.
//      3.) Fixed bug in subroutine call function.
//  2.3, January 4, 2025
//      1.) Renamed READSD command to avoid MIPS command conflict.
//      2.) Added support for ScriptButton parameters (using double-quoted calls).
//      3.) Optimized "Find MIPS" by removing slow Bluetooth search on exit.
//      4.) Improved connection reporting and tab pruning.
//      5.) Fixed terminal window vertical sizing/line cutoff bug.
//  2.4, February 2, 2025
//      1.) Added file I/O functions to scripting.
//  2.5, February 23, 2025
//      1.) Added "Common Script" (appended to all control panel scripts).
//      2.) Restricted MIPS comm buttons on child panels.
//      3.) Added sysUpdating invokable function (always targets parent).
//      4.) Updated UpdateHalted and WaitForUpdate to target parent panel.
//  2.6, March 13, 2025
//      1.) Fixed RFamp container display bug.
//      2.) Added ProcessCommand to RFamp for parameter access.
//      3.) Fixed disappearing controls in control panel.
//      4.) Fixed plot memory leak.
//      5.) Prevented ESC key from accidentally unloading the control panel.
//      6.) MIPS info area now clears on MIPS change.
//  2.7, April 13, 2025
//      1.) Redesigned script processing to reuse threads and engines (improved stability).
//      2.) Added scriptObj class for attaching scripts to Ccontrol events.
//      3.) Serial "wait for line" updated to remove processEvent calls, fixing random crashes.
//      4.) Stopped serial port reconnect on timeout errors.
//      5.) Added color options for DC bias and RF driver controls via script commands.
//      6.) Fixed control panel save bug.
//      7.) Added read/write access for script buttons and objects.
//      8.) Added invokable function to read text files into QString.
//      9.) Updated arrow key logic to use correct multipliers.
//      10.) Disabled value adjustment on STRING type Ccontrols.
//      11.) Implemented reduced-rate setpoint readback updates (DCB and Ccontrol).
//      12.) Added SkipCount config command for update throttling.
//  2.8, July 17, 2025
//      1.) Removed mutex from Comms; added logging for re-entrant comms calls.
//  2.9, July 22, 2025
//      1.) Added Table control to the control panel.
//  2.10, August 14, 2025
//      1.) Further removed processEvents calls to boost performance and script stability.
//      2.) Added a state machine to prevent update functions from blocking the UI.
//      3.) Added channel/mips commands to DCbias class.
//      4.) Added "on change" scripts for table cells.
//      5.) Added color and hide options for group boxes.
//      6.) Comms delays now use sleep instead of processEvents.
//      7.) Added tooltip and color control scripts for custom controls.
//      8.) Added async serial port processing commands.
//      9.) Plotting: Added addpoints (CSV support), auto-scale, and stability fixes.
//      10.) Added scrolling groupbox option.
//      11.) Fixed nested tab control naming/nesting.
//  2.11, August 22, 2025
//      1.) Throttled UI updates (10 controls per step) to improve responsiveness.
//      2.) Fixed minor bugs.
//  2.12, Sep 5, 2025
//      1.) Added argument support for custom control and table events.
//      2.) Fixed crash when scripting to incorrectly defined custom control objects.
//  2.13, Sep 14, 2025
//      1.) Added "abort script" for script buttons to handle cleanup on interruption.
//  2.14, Sept 27, 2025
//      1.) Added Slider control to control panels.
//  2.15, Oct 5, 2025
//      1.) Fixed vertical slider bug and settings loading error.
//      2.) Added .WidthT command to timing generator.
//      3.) Fixed crash when triggering a disconnected timing generator.
//  2.16, Oct 17, 2025
//      1.) Added "Load Plot" option to file menu.
//  2.17, Oct 25, 2025
//      1.) Added setValue and getValue to invokable scripting functions.
//      2.) Automated MIPS firmware help generation via Python.
//      3.) Added lasso-type zooming to plots (optional build flag).
//  2.18, Nov 22, 2025
//      1.) Added ability to run an external program from the control panel
//      2.) In void Comms::readData2RingBuffer(void) changed .waitForReadyRead(0) from
//          .waitForReadyRead(2); this should improve serial port read performance.
//      3.) Fixed a bug in the comms delay, it was change to sleep and the file transfer
//          to MIPS failed because of this. Added wait for buffer to transmit before calling
//          the delay function.
//  2.19, Feb 10, 2026
//      1.) Improved the plot lasso zooming.
//      2.) Added change script to the timing generator when a parameter is changed.
//  2.20, Feb 12, 2026
//      1.) Allow multiple status lights in control panel
//      2.) Allow multiple text messages in control panel
//      3.) Added the arb aux controls to timing generator
//  2.21, Feb 27, 2026
//      1.) Fixed the slow down bug in the plot function when acquiring lots of scans.
//  2.22, March 4, 2026
//      1.) Added ZMQ interface to scripting system.
//  2.23, March 20, 2026
//      1.) Fixed a crash in the scripting system cuased by where the 
//          engine was created.
//  2.23-dev updates, in progress
//	1.) Added properties options for the following:
//	    - mouse wheel editing
//	    - Control panel editing
//	    - lasso zoom mode enable
//	2.) Fixed the lasso zoom out phatom dotted box
//	3.) Improved the line end box data entry. Enter now forces
//	    an update.


