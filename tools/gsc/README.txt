gsc.py -- the GAACE_Script compiler, run by the Device Scripts dialog.

This is an unmodified copy of tools/gsc.py from
https://github.com/GordonAnderson/GAACE_Script (version in __version__ near
the top of the file; the app requires 1.2.0 or newer). Do not edit it here:
fix it in the GAACE_Script repository and copy it over, so there is one
compiler. The build copies it next to the application: Contents/Resources on
macOS, beside MIPS.exe on Windows (see MIPS.pro).

It needs only the Python standard library. Until a Python runtime is bundled
with the app, the app uses a system python3 (see gscrunner.cpp for the search).
