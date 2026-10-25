# Level Stats Editor (Geode mod)

Right-click the (i) button on a level to edit the **Attempts** and **Jumps** numbers shown in that
level's info popup. Left-click still works exactly like vanilla.

## Folder structure

```
LevelStatsEditor/
├── CMakeLists.txt        build recipe
├── mod.json              mod metadata + one setting
├── about.md              text shown on the mod's page in Geode
├── changelog.md
├── logo.png              mod icon
├── .gitignore
├── README.md             this file
└── src/
    ├── main.cpp              hooks LevelInfoLayer + LevelPage; shows edited numbers in the vanilla popup
    ├── RightClickLayer.hpp/.cpp   detects right-click on the (i) button
    ├── EditStatsPopup.hpp/.cpp    the editor menu (Attempts, Jumps, Done, Reset)
    └── StatsStore.hpp/.cpp        per-level keys, saving, loading, validation
```

## How it works (short)

* **Right-click:** each level page gets an invisible, mouse-only layer. When it receives the game's
  right-click event it checks whether the cursor is over an (i) button. Because it is not touch-enabled
  it can never trigger the left-click action.
* **Display only:** your real game statistics are **never** modified. Your numbers are stored in the
  mod's own save file. When the vanilla info popup is created, the level's numbers are swapped to your
  edited ones for that single function call and put back immediately afterwards.
* **Reset** removes your edit, so the popup shows the real, original numbers again. It does not set
  anything to zero.
* **Per level:** each edit is stored under a key made of the level type and level ID (for example
  `t1:1` for an official level, `t4:128` for an online level), so levels cannot overwrite each other.
* **Persistence:** stored with Geode's saved-value API (`Mod::setSavedValue`), which Geode writes to disk.
  Missing or corrupted entries are ignored (the real numbers are shown).

## Build on Windows (beginner steps)

I could not run a build in my environment, so these steps follow Geode's official workflow but are
untested by me. If any command or menu name differs, the official guide wins:
https://docs.geode-sdk.org/getting-started/

1. **Install Geometry Dash** from Steam and **install Geode** with the official installer from
   https://geode-sdk.org. Start the game once and make sure the Geode button appears on the main menu.
2. **Install the build tools.** Install *Visual Studio 2022 Community* (free) and, in its installer,
   tick **Desktop development with C++**. Also install **CMake** and **Git** if the Geode guide asks for them.
   Geode v5 needs a C++23-capable compiler, so keep Visual Studio fully updated.
3. **Install the Geode CLI** (the `geode` command) from https://github.com/geode-sdk/cli/releases
   and make sure typing `geode --version` in a new Command Prompt works.
4. **Set up Geode.** In Command Prompt run these, one at a time:
   ```
   geode config setup
   geode sdk install
   geode sdk install-binaries
   ```
   When asked, point it to your Geometry Dash folder (usually
   `C:\Program Files (x86)\Steam\steamapps\common\Geometry Dash`).
5. **Check the SDK version.** Run `geode sdk version`. This project expects **5.11.2 or newer**.
   If it is older, run `geode sdk update`.
6. **Edit `mod.json`:** change `yourname` in `"id"` and `"developer"` to your own name
   (lowercase letters, numbers and dashes for the id, for example `bob.level-stats-editor`).
7. **Open a Command Prompt inside this folder** (open the folder in File Explorer, click the address
   bar, type `cmd`, press Enter).
8. **Build:**
   ```
   geode build
   ```
   The first build can take several minutes.
9. **Find the result:** the finished file is in the `build` folder and is named after your mod id,
   e.g. `build\bob.level-stats-editor.geode`.

## Install

1. Close Geometry Dash.
2. Copy the `.geode` file into `Geometry Dash\geode\mods\`. (`geode build` may already have done this for you.)
3. Start the game. In the Geode menu (Mods list) make sure *Level Stats Editor* is enabled.

## Use

1. Go to a level page (official level select, or an online/saved level's page).
2. **Right-click** the (i) button. Type your numbers. Press **Done** (saves) or **Reset** (back to real numbers).
3. **Left-click** the (i) button to see the normal info popup, now showing your edited numbers.

Settings: *Show edited values* in the mod's settings shows your edits in the info popup (default on).
Turn it off to see the real numbers without deleting your edits.

## Known limitations

* **Windows desktop only** by design (right-click does not exist on phones). `mod.json` lists Windows only.
* Edited numbers are fixed values. If you keep playing, the real counters go up but the popup keeps
  showing your edited numbers until you press Reset.
* Newly unsaved editor levels have no level ID, so they are keyed by type and name (not used by the screens this mod hooks).
* Written for Geometry Dash **2.2081**. See the notes below about 2.209.
