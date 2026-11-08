# Python runtime for the Device Scripts compiler

Tools -> Device Scripts compiles GAACE_Script by running `tools/gsc/gsc.py` with
Python. End users should not have to install Python, so the application ships
its own copy. It is **not** committed to this repository (about 50 MB per
platform); a script fetches it, and the build copies it into the application.

Frozen executables (PyInstaller and the like) are deliberately not used: their
launchers are commonly flagged as malware. This is the real Python interpreter
plus `gsc.py` as a plain text file.

## Getting the runtime

| Platform | Run | Result |
| --- | --- | --- |
| macOS (Apple silicon) | `tools/python/fetch_python.sh` | `tools/python/macos/python/` |
| Windows (x64) | `powershell -ExecutionPolicy Bypass -File tools\python\fetch_python.ps1` | `tools\python\windows\python\` |

Both read `versions.txt`, check the download against the pinned SHA-256, and
run a quick import test before keeping the result. The macOS one also removes
what `gsc.py` never uses (tkinter, tests, pip, headers), which takes the runtime
from about 100 MB to 47 MB. Run them again after changing `versions.txt`.

* macOS: [python-build-standalone](https://github.com/astral-sh/python-build-standalone)
  (`install_only_stripped`, aarch64).
* Windows: python.org's *embeddable package*. Its `python.exe` and DLLs are
  already signed by the Python Software Foundation.

## How it gets into the application

`MIPS.pro` copies the runtime when it builds, if the fetch has been run
(otherwise the app falls back to a `python3` on the PATH and prints a note):

* macOS: `MIPS.app/Contents/Resources/python` and `Contents/Resources/gsc.py`.
  **It must be in `Contents/Resources`**: `codesign --deep` fails ("bundle format
  unrecognized, invalid, or unsuitable") if the runtime is in `Contents/MacOS` or
  `Contents/Frameworks`.
* Windows: `release\python\` and `release\gsc.py` beside `MIPS.exe`. The release
  workflows copy both into the `deploy` folder.

`gscrunner.cpp` looks for them there first. The app runs Python with `-I -B`:
isolated from the user's Python settings, and never writing `.pyc` files, which
would change the signed bundle.

## macOS signing and notarization

`sign_macos_python.sh MIPS.app [IDENTITY]` signs every Mach-O file in the
runtime (hardened runtime; with a Developer ID identity it also adds the secure
timestamp notarization needs). Run it **before** signing the app bundle. The
workflows do this:

* `release.yml`, `pre-release.yml`: ad-hoc (`-`), before "Sign app bundle".
* `MACsigning.yml`: with the Developer ID, before the app is signed and
  notarized.

Tested locally with an ad-hoc signature: the signed bundle passes
`codesign --verify --deep --strict`, the bundled Python runs under the hardened
runtime with no extra entitlements, and the bundle stays valid after compiling.
**Not tested:** the Developer ID signature and the notarization itself (they
need the certificate and Apple credentials in the workflow secrets). If
notarization complains about the Python binaries, the usual fix is an
entitlements file passed to `codesign`.

## Updating Python

1. Pick the new version; get the new checksums (macOS: the release's published
   SHA-256; Windows: `Get-FileHash` of the embeddable zip) and edit `versions.txt`.
2. Run the fetch script for each platform.
3. Run `python3 -m unittest tools/test_gsc.py` from the GAACE_Script repository
   with the new interpreter (it passes on 3.9, 3.12 and 3.14.10).
4. Build, and check About shows the new version.
