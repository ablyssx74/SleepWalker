# SleepWalker v1.0.5

## Highlights
- **The version comes from the `VERSION` file.** The program's version text (what the update check compares with the `VERSION` file on GitHub), the resource and the package all take it from there now, as hDesktop and Cricket do, so a release changes one file. The number was typed into the code, the Makefile and the resource file before, and one of them could be left behind.
- **The version is one number again.** The v1.0.4 release was made while the program, the Makefile and the `VERSION` file still said 1.0.3, so a v1.0.4 program offered nothing to update from. This release has all of them at 1.0.5.
- The resource's version numbers were in the wrong fields (the third number in the second place); they are right now.

## Install
Install `SleepWalker-1.0.5-1-x86_64.hpkg` on 64-bit Haiku, or `SleepWalker-1.0.5-1-x86_gcc2.hpkg` on 32-bit Haiku, for the whole system.

## Tested on
- Haiku R1/beta6: the 64-bit and 32-bit packages were built (on 64-bit Haiku and on 32-bit Haiku) and show v1.0.5 in the program and in the package. They were not run.
