Revolution SDK (Dec. 11, 2009)
=============

A work-in-progress decompilation of the December 11, 2009 version of the Revolution SDK, and the February 24, 2010 version of the Home Button Menu library.

This repository does not contain any of the original objects. An existing copy of the game [`[SC5PGN]`](https://wiki.dolphin-emu.org/index.php?title=SC5PGN) *Challenge Me: Word Puzzles* is required. (See [Building](#building) below.)

Supported versions:

- Debug
- Release

Dependencies
============

Windows
--------

On Windows, it's recommended to use native tooling. WSL or msys2 are not required.  
When running under WSL, [objdiff](#diffing) is unable to get filesystem notifications for automatic rebuilds.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
  - Also available from the [Windows Store](https://apps.microsoft.com/store/detail/python-311/9NRWMJP3717K).
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

macOS
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

- Install [wine-crossover](https://github.com/Gcenx/homebrew-wine):

  ```sh
  brew install --cask --no-quarantine gcenx/wine/wine-crossover
  ```

After OS upgrades, if macOS complains about `Wine Crossover.app` being unverified, you can unquarantine it using:

```sh
sudo xattr -rd com.apple.quarantine '/Applications/Wine Crossover.app'
```

Linux
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- For non-x86(\_64) platforms: Install wine from your package manager.
  - For x86(\_64), [wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

Building
========

- Clone the repository:

  ```sh
  git clone https://github.com/muff1n1634/sdk_2009-12-11.git
  ```

- Extract the SDK objects:
  - Replace `SC5PGN.iso` with the path to your copy of *Challenge Me*.

  ```sh
  dtk vfs cp SC5PGN.iso/build/libs/Debug/slamWiiD.a SC5PGN.iso/build/libs/Release/slamWii.a orig/
  ```

  - Only extract the archives, *not* the objects within the archives. The extraction of the objects from the archives are handled as part of the build.
  - After the initial build, the original archives can be deleted to save space.

- Configure:

  ```sh
  python configure.py
  ```

  Both debug and release objects are configured to build at the same time, so there is no need to configure this with command-line options. (Thanks [@robojumper](https://github.com/robojumper)!)

- Build:

  ```sh
  ninja
  ```

Diffing
=======

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, or `configure.py`.
