# psp5-authentic – own launcher for PSP with ARK-4/ARK-5 – developer notes

**psp5-authentic is a fork of [psp5](https://github.com/08Weber/psp5) by Mille Weber (08Weber), based on psp5 v1.5.3.** The
goal of the fork is to be as authentic as possible to its home console counterpart. These are the developer notes: what each
change touches in the code. The list for users is in [`CHANGELOG.md`](../CHANGELOG.md). The older sections at the bottom
(1.5.2 and earlier) are the original project's own history; they were written in Swedish and have been translated.

A dark launcher with a row of game cards, the game's picture as the background, and tabs for games (Home) and library.
All graphics are its own. The launcher is a normal app in `ms0:/PSP/GAME/PSP5` and never touches flash0, so the XMB is
always still there (HOME in psp5 = back to the XMB).

## psp5 as ARK's launcher
psp5 replaces ARK-5's own launcher (arkMenu): the file `ms0:/PSP/SAVEDATA/ARK_01234/VBOOT.PBP` is psp5.
ARK's "Custom Launcher" in the XMB then starts psp5, and with **Settings → psp5 → Boot into psp5** the PSP starts straight
into psp5 instead of the XMB.

- **Emergency exit:** hold **SELECT** while the PSP starts – ARK then skips SETTINGS.TXT and you land in the XMB.
  Hold **START** to skip all plugins.
- **Exit to XMB** (Settings → Power and USB) turns the launcher mode off for that session.
- **Back to arkMenu:** copy `ARK_01234/VBOOT_ARKMENU.PBP` to `VBOOT.PBP` (a copy is also in `backup/`).
- An ARK update can overwrite `VBOOT.PBP` – then copy `dist/PSP/GAME/PSP5/EBOOT.PBP` in as `VBOOT.PBP` again.

## After 1.0.0 (next version)

*Nothing yet.* All changes after 1.0.0 are noted here (and under *Unreleased* in `CHANGELOG.md`) until the next version is
released.

## Version 1.0.0 (psp5-authentic)

Everything that changed compared with the original's v1.5.3 – see `CHANGELOG.md` for the version written for users.

- **Home and the options menu:** the Games tab is Home: the cards opened last (at most ten, newest first) and the ones
  that were pinned (at most nine, first). Saved in `<data>/home.txt` (`src/home.c`); the cards are named with
  `entry_cover_key` (the folder name for an EBOOT, the game ID for an ISO). START opens a small options menu
  (`draw_menu`) for the selected card: Manage Saved Data, Delete, Information, Remove from Home / Add to Home, Pin /
  Unpin. ✕ chooses, ○ or START closes. Under the card there are the buttons Start and ⋯ (options): ↓ selects Start,
  ← → move between them, ↑ or ○ go back to the card; ✕ on ⋯ opens the same menu. The UMD comes first and does not count
  among the ten.
- **The whole main menu is controlled with the D-pad:** the button hints (L R Tab / △ Settings at the bottom right, and
  ✕ □ ◀▶ in Music) are gone, and △ no longer opens the settings. ↑ from the first thing on a tab (the card in Games, the
  first row in Library, the first track in Music; also an empty tab) selects the top bar (`top_focus` in `ui.c`): the tabs
  and the cog wheel are buttons with the same frame as the Sort button (`popup_choice`), ← → move along it, ✕ on a tab
  opens it (the tab turns bold as before) and ✕ on the cog wheel opens the settings. ↓ goes back to where you came from
  (to the Sort button, if ↑ came from there) and ○ always goes back to the selected game; from the Sort button, ↑ gives
  the cog wheel. L/R switch tabs as before. A held ↑ never moves on into the top bar by itself.
  In the Library, ↑ selects the Sort button only from a card in the first row that sits right under it (the middle of
  the card lies within the button's frame, `sort_above` in `ui.c`; the button is only a stop when there is more than one
  card); from all other cards ↑ goes straight to the top bar, so nothing is in the way. Before, ↑ always went via the Sort
  button. The cog wheel sits 4 px further to the left. Start/⋯ and the Library's title row are on one line (centred on
  y = 243), so that the highlight ends 16 px from the bottom edge, as far as the top bar's text and icons are from the
  top edge.
- **The sort frame no longer flickers:** the frame round the Sort button was switched on and off while you passed through the
  tabs and the cog wheel, although the button was not selected. It now has its own animation (`sort_frame`) that only lights up
  when the button is selected and the top bar is not; `sort_anim` still controls how highlighted the button is (and that the
  card under it is dimmed).
- **Everything is drawn on whole pixels:** nothing drawn at rest has an edge between two pixels (that gave 1 px on one side
  and 2 px on the other, and soft icons). The Library's cards are 80 × 44 px with 8 px between them (before: 78.4 × 43.6 and
  10 px) and the selected card is 6 × 4 px larger (`SELECT_GROW_W/H`); the picture and the ring are drawn on the same
  whole-pixel box (`draw_card`). `gfx_frame` rounds edges and thickness to whole pixels. The battery (the fill 14 px), the
  cog wheel (radius 7), the bolt, the Options dots (radius 2), the switches, the file icon, the padlock, the hint rows, the
  toast and the colour swatches sit on whole pixels; text widths are rounded up with `ceilf`. The check is in `mockge.c`
  (`g_audit` counts drawn rectangles and pictures that have an edge between pixels; `MOCK_AUDIT=1` prints where they come
  from) and is tested in `ui/pixel_test.c` and in the settings, info, files and saves tests. Soft shadows (`gfx_glow`) and
  the background's beam of light and dust are soft on purpose and are not counted. The position of the text is unchanged
  (it cannot be measured without intraFont).
- **Sorting in Manage Saved Data** with the same Sort button and popup as the Library (`set_sort`, `draw_sort_menu` in
  `saves.c`): ↑ from the first row, ✕ opens the choices Last changed / Size / Name, ○ goes back. L/R and the key icons are
  gone.
- **The console-colour themes have a beam of light, golden dust and haze** (`deco_spot` in `theme.c`, `beam` and `dust` in
  `Theme`): a wide beam from the lamp at the top left, a band of dust and bokeh, a cool haze at the bottom left. The picture
  stands still and is the same on all screens. PS1–PS4 keep their own patterns.
- **The start-up screen no longer has a final blue picture:** the old beam of light in `boot.c` (and, for a while, the theme's
  surface) is gone. The dust keeps floating on black until the start-up screen fades out (`T_FADE` 6.6 s, `T_END` 7.15 s)
  and the menu fades in. The start-up sound is 5.2 s from the health notice (2.55 s) and finishes within the 400 ms that
  `sound_wait` waits. The circles and the dust have the theme's colours (`make_palette`): the big dark circles in the theme's
  `intro` colour, the small light ones in the haze's, the dust in the theme's `dust`/`beam` (PS1–PS4 have no `dust`: they use
  the accent colour). The logo and the health notice are grey.
- **PS1–PS4 also have the beam of light and the haze** (`beam` in the theme; `deco_spot` is drawn for all themes under the
  theme's own patterns) but no dust (`dust` = 0).
- **The background's tint from the game's icon is removed** (it cost performance): `accent_target`, `ui->accent`,
  `IconSlot.accent`, `texture_average`, `Theme.tone` and the `tone` parameter of `theme_backdrop` no longer exist.
- **The beam of light covers the whole haze at the bottom, and the background is the same everywhere:** the beam
  (`deco_beam` in `theme.c`) is a mesh of small quads with their own brightness at each corner, not five layers of large
  quads. A quad is split into two triangles and the colour is only blended between a triangle's three corners, so the right
  half of every large quad became almost dark and the beam got a hard edge along the diagonal (it showed as a sharp edge that
  ended at x = 235 at the bottom, while the haze reached x = 430). Now the beam leans to the right and widens downwards: the
  right edge is soft but clear and ends at x = 400 at the bottom, the left side is a long soft slope. The shape is measured
  against the reference's brightness. The haze at the bottom is dimmed so that the beam and the haze together are about as
  bright as the reference. The menu used to be darkened by three black gradients in `draw_background` (top, left, bottom)
  over both the picture and the surface, and had its own darker base tone; the base tone is gone and the gradients now lie
  only over a game's background picture (with the same strength as before, in proportion to how much of the picture is
  visible, so that they fade in and out with the picture). All screens draw the same surface (`surf_top`, `surf_bottom` =
  before `stop`, `sbottom`); the surface is only darkened by the Library and Music (`draw_dim`) and by the popups. The
  `bottom` fields are gone; `top` is now called `intro` and is only used by the start-up screen's circles (so that they look
  as before).
- **The selected card keeps its size when the focus is somewhere else:** the ring round the card (`card_ring` in `Ui`,
  faded over 1/8 s) goes away when the Sort button, Start / Options or the top bar is selected and comes back when the card
  is selected again. The card in the Library no longer shrinks when Sort is selected (it is 86 × 48 all the time); its
  dimming of the brightness remains. `draw_card` has got the parameter `ring` (before, `focus` controlled both the brightness
  and the ring).
- **The highlights sit in the middle of their text and on whole pixels:** a label looks centred when the baseline is about
  6.45 × the text size below the middle of the frame (like the choices in the popups). `popup_label_mid` and
  `popup_frame_around` in `popup.h` give the frame's top edge and height (the height or one pixel more, so that the edge
  lands on a whole pixel); the tabs' frame follows the text size (it shrinks in long languages), and the same goes for the
  cog wheel and the Sort buttons. `popup_choice` rounds the edges to whole pixels: a frame on parts of a pixel got 1 px on
  one side and 2 px on the other (rectangles are drawn with the pixels whose centre is covered).
- **The same background everywhere, and the same darkening in Library and Music:** Games, Library and Music drew the beam of
  light, the dust and the haze at full strength, while the settings, the information, the file manager and saved data drew
  them at 60 %, so Games looked brighter. `theme_backdrop` now has no parameter for list screens and draws the same
  everywhere: the beam of light (the beam and the lamp, `BEAM_STRENGTH` in `theme.c`) at 60 %, everything else (haze, dust and
  the PS1–PS4 shapes) at full strength. The PS1 symbols sit in a single place (top right, where the lists are not), so the
  theme looks the same on all screens. The Library and Music put the same darkening (`DIM_COLOR`, 8 8 12 with alpha 150, as
  Music had; the Library had 165) over the Games background, a game's picture or the theme's surface.
- **The background is drawn once (FPS):** the surface, the glows, the beam of light with haze and the dust look the same in
  every frame, but were redrawn every time: about 500 calls that touch every pixel three to four times (measured in the test
  environment). `theme_backdrop` now draws that part once into a picture in video memory (`gfx_snapshot_begin/end`, behind
  the two screen buffers, `sceGuDrawBufferList`) and shows it from there as one opaque picture in strips of 64 pixels
  (`gfx_snapshot_draw`). It is redrawn when the theme changes, after `gfx_init` has run again, and after sleep mode
  (`theme_backdrop_forget`). The PS1–PS4 shapes move and are still drawn every frame on top.
- **Fixes from the original project (psp5 1.5.4, the issues on GitHub):**
  - **.DAX games** (`scan_iso_dir` in `scan.c`, the delete guard in `gameops.c`, the colour in the file manager): libcisoread
    reads DAX and ARK starts it, so it was enough for the scanner to include the extension. The Information screen recognises
    DAX (`DAX_MAGIC`) and reads the unpacked size as 32 bits at position 4 in the header (CSO/ZSO have 64 bits at position 8).
  - **USB mode and the file manager jumped back at once:** `usb_mode` and `files_mode` in `main.c` only ignored a ✕ that was
    held down when they opened; with ○ as the confirm button they exited immediately. Now all buttons are ignored until they
    have been released (`old = ~0u`). `sys_usb_start` also does not give up if `sceUsbActivate` answers with an error, like
    arkMenu.
  - **PSP Go, psp5 started from the internal storage:** ARK then redirects `ms0:` to `ef0:`, but only during psp5's own
    session, so the games got the path `ms0:` and were started as if from a memory stick (PS1 games: error 800201A8, ISO:
    froze). `scan_all` now always scans `ef0:` and `ms0:` only when it is a real stick (`sys_ms_is_ef`), and `launch.c` then
    picks the Go modes (`on_ef0`). Home and the covers use the game's ID or folder name, not the drive, so nothing is lost; the
    scan cache is rebuilt once.
  - **Homebrew on the PSP Go's internal storage** (the GTA III and Super Smash Bros ports froze or switched the PSP off): is
    now started with apitype 0x152 (`HOMEBREW_RUNLEVEL_GO` in `launch.c`) like arkMenu, so that ARK redirects `ms0:` to `ef0:`
    and ports that read their data files from `ms0:` find them. Homebrew on a memory stick is started as before with 0x141.
    The test `host-tests/launch` checks the apitype for every kind of game and drive.
  - **The background picture is drawn in narrow strips** (`gfx_tex` in `gfx.c`): a picture wider than 64 texels is drawn as one
    sprite per 64 texels in the same call. The GE's texture cache copes with that much better than with a sprite as wide as
    the screen, and the games' pictures behind the cards are the biggest thing drawn. The strips' borders are moved to whole
    pixels and get the texture coordinate that belongs to the moved border, so that the picture looks the same and the strips
    meet without gaps or overlap.
  - **"Boot psp5" showed off** until the first game start: a paused launcher mode is switched on by `start_confirmed()` only
    after the menu has worked for a while, but the settings read the mode as soon as they opened. `main.c` now calls
    `start_confirmed()` before `settings_open()`.
  - **A crackle when the settings opened:** the sound was played in `ui.c` before the settings were read in (the font from
    flash0, ARK's files), and the heavy reading disturbed the mixer. `ui.c` no longer plays anything; `main.c` plays
    `SND_SELECT` after `settings_open()`.
  - **PSP minis' square icons are no longer stretched** (`fit_icon` in `art.c`): an icon narrower than 1.5 : 1 keeps its shape
    in the middle of the card, on a darker (55 %), blurred copy of itself (blur 8) that fills the space beside it. Other icons
    are scaled as before. The icons that were already in `cache/` are stretched, so `art_init` removes all `*.ico` there once
    (the marker `cache/icons2`, the same name as in psp5 1.6.0); `covers/` is not touched.
- **SELECT opens a power popup** (`MENU_POWER`, `menu_build_power`, `open_power_menu` in `ui.c`): on all tabs (also with the top
  bar selected) the same dark panel as the options menu is shown in the middle of the screen, 200 px wide, with Sleep,
  Shut down, Restart and Recovery menu – the same texts (`S_SLEEP`, `S_SHUTDOWN`, `S_RESTART`, `S_RECOVERY`), the same order
  and the same function as at the bottom of Settings → Power and USB. ✕ (or ○ with that setting) does the choice: `ui_update`
  returns `UI_SLEEP`, `UI_SHUTDOWN`, `UI_RESTART` or `UI_RECOVERY` and `main.c` runs the same `run_action` as the settings. ○,
  △, START or SELECT close the popup without doing anything.
- **The confirmation step "Press again to confirm" is gone:** `confirm` in `Item`, `CONFIRM_TIME`, `confirm_item`/`confirm_time`,
  the parameter of `action()` and the string `S_CONFIRM` (all 11 languages) no longer exist; Sleep, Restart, Shut down and
  Recovery menu run on the first ✕. The order in Power and USB is now Sleep, **Shut down**, **Restart**, Recovery menu.
  `tools/tests/settings.txt` no longer presses ✕ on Sleep (it would put PPSSPP to sleep).
- **Version 1.0.0** (`settings.c`, `tools/make_release.py`, `INSTALL.md`): the zips are called
  `psp5-authentic-v<version>-…`. `CHANGELOG.md` has all changes compared with the original's v1.5.3.

## Version 1.5.3 (the original – the base of the fork)

- **Category folders:** a folder in `PSP/GAME` without `EBOOT.PBP` (e.g. `GAME/PS1/<game>` or
  `GAME/Emulators/TempGBA`) is searched one level down, as arkMenu does (`scan_game_dir`, `depth`). Before, PS1 games and
  apps in such folders did not show up at all.

## Version 1.5.2

- **Move between Games and Apps:** □ on a card in Games or Apps opens a small options menu (`draw_menu`) with the card's name
  and *Move to Games/Apps*. ✕ chooses, ○ or □ closes, so nothing is moved by mistake. Saved in `<data>/moved.txt`, one line
  per card with `entry_cover_key` (the folder name for an EBOOT, the game ID for an ISO). The UMD has no menu. The hint
  "□ Options" is shown faintly next to the Start button.
- **File manager** (`src/files.c`, Settings → Power and USB): runs like the USB mode, with the rest of psp5 closed
  (`files_mode` in `main.c`), and the game list is read again afterwards. ms0: and, on the PSP Go, also ef0: (drive list at
  the top), never flash0. Folders first, sorted by name; □ marks, △ (or ✕ on a file) opens the menu: Paste here, Copy, Move,
  Rename, Delete, New folder. Copy/move/delete run in their own thread (`worker`) with progress and ○ to cancel; a cancelled
  or failed copy is cleaned up, and the free space is checked first (`sceIoDevctl` 0x02425818). A move on the same drive is a
  `sceIoRename`, otherwise copy + delete. If the name already exists it becomes "name (2)". Delete always asks, with Cancel
  preselected. Names are typed with the PSP's keyboard (`sceUtilityOsk*`, via `gfx_end_with`). The free space is shown with a
  thin bar at the top right.

## Version 1.5.1

- **Volume indicator** (`src/osd.c`): the volume buttons go straight to the system, so psp5 reads the volume (0–30) and
  sound on/off with the kernel's `sceImposeGetParam` – ARK finds the function (`sctrlHENFindFunction`, ARK translates the NID
  0x531C9778 for 6.60/6.61, otherwise 0xDC3BECFF is tried) and kubridge's `kuKernelCall` calls it. When the value changes, a
  bar slides in at the top for 1.6 s, in all tabs, the settings and the USB mode. Without ARK (PPSSPP) nothing is shown.
- **Seeking in the music** (`seek_to` in `src/music.c`): a short press of ← / → changes track when the button is released,
  hold for 0.35 s to seek (8, then 20 and 45 seconds per second). The decoder is restarted at the right byte in the file: the
  Xing table for VBR, otherwise proportionally, and then the first valid frame (the same version, layer and frequency as the
  track's first, and a valid frame right after it). The first 8 frames after a jump may fail without the track being changed:
  the PSP's decoder answers `MP3_END` (0x80671402) instead of an error for a frame that lacks data from the frames before it
  (the bit reservoir). Seeking stops one second before the end.
- **Covers in the music** (`art_load_cover`, `music_cover_data`): from the ID3 tag's APIC/PIC (preferably the front cover,
  v2.2–2.4 with unsynchronisation), otherwise `cover.jpg`, `folder.jpg`, `front.jpg`, `album.jpg`, `cover.png` or
  `folder.png` in the album folder. JPEG is decoded with libjpeg-turbo (scaled down already in the decoder), PNG with libpng.
  64×64 with rounded corners, saved in `cache/` per album and loaded in the loader thread. `loader_stop` now runs before
  `music_term`, because the loader reads the music list.
- **Charging animation:** while the battery charges, the fill sweeps from the level up to full, with a bolt beside it.
- Test script `tools/tests/media.txt` and new test commands: `hold BUTTON:N`, `volume N`, `charging 0/1`, `battery N`.

## Version 1.5.0

- **Themes** (`src/theme.c`, new *Theme* tab in the settings): the default colour theme (as before), the console-cover
  colours (Midnight Black, Cosmic Red, Nova Pink, Starlight Blue, Galactic Purple, the Deep Earth series Volcanic Red/Cobalt
  Blue/Sterling Silver and Chroma Indigo/Teal/Pearl) and PS1–PS4 as a setting for the *Appearance*: PS1 grey with the
  controller's symbols, PS2 dark with blue towers and orbs, PS3 with the XMB wave and PS4 blue with waves and particles.
  Everything is drawn in code. The highlighted theme is previewed in the settings, ✕ chooses it. The theme colours the
  background (when the game has no picture), the settings, the USB mode, the charging screen, the placeholder cards, the
  switches, the music's progress and the charging dots. *Game art in the background* can be switched off – then the theme is
  always visible and no background pictures are loaded. Saved in `psp5.cfg` (`theme=`, `themeart=`).
- **Faster drawing:** the settings re-wrapped the category description (many `text_width` calls) for every row in every
  frame – now only when the text changes. The widths of the tabs and the hints are only measured again when something
  changes. `gfx.c` does not rebind the same texture (and does not flush the texture cache) for every shape in a row. The
  surface under the game's picture is not drawn when the picture covers the whole screen. The icon's average colour is
  calculated directly from the texture instead of on a copy (46 KB less to copy per icon).
- **Fixed:** a background picture that finished loading just as the highlight moved was thrown away and then never loaded
  again if you went back to the game (`s_bg_last` in `src/loader.c`). A broken directory entry in an ISO (too short an
  entry or too long a name) could be read outside the sector buffer (`src/iso.c`).
- Test script `tools/tests/theme.txt`; `settings.txt` and `lang.txt` have one more step for the new tab.

## Version 1.4.0

- **PSP Go** – the same EBOOT on all models: `sys_set_root` (`src/system.c`) picks `g_root` from argv[0]. If psp5 is started
  from `ef0:` (the PSP Go's internal storage), psp5's files are in `ef0:/PSP5`, ARK's default folder and `SEPLUGINS` are
  there, and the hint texts show `ef0:/`. Games (`ISO`, `PSP/GAME`) and music (`MUSIC`) are always searched on both `ms0:` and
  `ef0:`, like arkMenu – so also a Memory Stick Micro in a PSP Go. Games on `ef0:` are started with ARK's apitypes for the
  internal storage (0x125/0x126 ISO and PSN, 0x155 PS1; homebrew as usual 0x141), and updates (`PBOOT.PBP`) are searched on
  both. No UMD thread on the PSP Go (model 4). Before, psp5 only looked on `ms0:`, so games in `ef0:/PSP/GAME` and music in
  `ef0:/MUSIC` did not show up on the PSP Go. The release zips `psp5-v1.4.0-PSPGo-ARK5.zip` and `-PSPGo-ARK4.zip` have the
  same EBOOT as the ARK5/ARK4 zips, only with a READ ME for the PSP Go (`tools/make_release.py`).
- **Settings on the PSP Go** (`src/settings.c`, model 4): *Disable Pause Game* (`disablepause`) and *Keep the paused game*
  (`hibblock`) are shown, *Disable UMD drive* is hidden – as in arkMenu.
- **Build in GitHub Actions** (`.github/workflows/build.yml`): both versions and all four zips.

## Version 1.3.3

- **Start-up guard against semi-bricks** (`src/main.c`): `ms0:/PSP5/starting` is created first of all at start-up and
  contains the number of starts in a row that did not finish. The file is removed when the menu has run for 5 s and the music
  list has been read, or when a game is started or something is chosen in the settings.
  - 1 failed start: **safe start** – no start-up screen, no music, no UMD thread, `ms0:/PSP5/cache` is emptied and a toast is
    shown.
  - 2 in a row as ARK's launcher: psp5 goes straight to the XMB (`sys_exit_xmb`, the launcher mode is paused) and writes 1 to
    the file, so that the next start as a launcher is a safe one instead of the XMB again.
  - A paused launcher mode is only switched on again when a start has finished (`sys_resume_launcher`), not directly in
    `sys_init`.
- **Sleep mode without graphics:** when the PSP falls asleep, two black frames are drawn, then psp5 waits with
  `sceKernelDelayThread` instead of drawing all the time while the PSP falls asleep and wakes up. No start-up screen after
  sleep mode in safe mode.
- `sctrlArkSetConfig` is only called with a configuration that could be read (`IS_ARK_CONFIG`).
- The HOME menu in games: psp5 is not in memory then (LoadExec restarts the system) and starts games with exactly the same
  call as arkMenu. Help text about plugins and ARK's combination L + R + START + ↓ in INSTALL.md.

## Version 1.3.2

- **No menu before the start-up screen after sleep mode:** "finished waking up" (`PSP_POWER_CB_RESUME_COMPLETE`) arrives about a
  second after the screen has lit up, and until then the menu was visible. Now the screen is drawn black already when the PSP
  falls asleep (`PSP_POWER_CB_SUSPENDING`) and is kept black until it has finished waking up, then the start-up screen
  (`sleep_and_wake` in `src/main.c`, at most 4 s awake without a signal). The music is paused when the PSP falls asleep.
  Test script: `tools/tests/resume.txt` (`sleep 1`, `resume 1`).

## Version 1.3.1

- **No start-up screen after a game:** as ARK's launcher, psp5 is restarted when a game ends. Before a game starts,
  `ms0:/PSP5/ingame` is created; if the file exists when psp5 starts, it is removed and the start-up screen is skipped. As a
  normal app the file is not created (the system goes to the XMB after the game).
- **Start-up screen after sleep mode:** a power callback (`scePowerRegisterCallback`, `PSP_POWER_CB_RESUME_COMPLETE`) shows
  the start-up screen again when the PSP is woken with the side button. The start-up sound is recalculated in the sound
  thread (`sound_prepare_boot`), music is paused in the meantime and the cards slide in again (`ui_replay_intro`).
  Test script: `tools/tests/resume.txt` (`resume 1` pretends the PSP woke up).
- **The UMD first under Games** (`src/umd.c`): a thread looks for the disc as arkMenu does (sceUmdCheckMedium,
  sceUmdActivate, wait for PSP_UMD_READY) and reads `disc0:/PSP_GAME/PARAM.SFO`. The disc is put first in the list when it is
  inserted and removed when it is taken out, and the highlight stays on the same game. It is never saved in the list cache.
  Start: apitype 0x120 with `disc0:/PSP_GAME/SYSDIR/EBOOT.BIN`, or 0x160 with `ms0:/PSP/GAME/<DISC_ID>/PBOOT.PBP` if an
  update exists. In the test build, `ms0:/PSP5/fakeumd/` works as a disc (test script `tools/tests/umd.txt`).

## Version 1.3

- **ARK-4 and ARK-5, PSP-1000/2000/3000:** two builds – `tools/build.sh` (ARK-5, links ArkCtrl) and `tools/build.sh ark4`
  (ARK-4, `-DPSP5_ARK4`, without ArkCtrl → `dist-ark4/`). Both detect the ARK version (`sctrlSEGetVersion()`: 4/5) and the model
  (`kuKernelGetModel()`: 0 = PSP-1000) at start-up. On ARK-4 the ARK configuration is read through a separate
  SystemCtrlForUser import (`src/ark4stub.S`) and SETTINGS.TXT is written in ARK-4's format (overclock/defaultclock/powersave,
  highmem/mscache on/off). The PSP-1000 hides extra memory and USB charging.
- **The "ARK not found" error:** `sctrlHENGetVersion()` only gives the middle version (0 on ARK 5.0.x) – now the major version
  is used.
- **Music:** the Music tab plays MP3 from `ms0:/MUSIC` (sceMp3, its own thread, `src/music.c`), like the XMB's player.
- **A start-up screen like the console's:** grey logo on black → health notice with dark blue bokeh → golden dust → beam of
  light.
- The heap leaves 7 MB free (ARK's RECOVERY.PRX needs 4 MB).

## Start-up screen and library (1.2.0)

- **Start-up screen:** particles of light swirl in towards the middle, meet in a flash and leave the psp5 logo behind while a
  chord of its own is played. Everything is drawn and calculated in `src/boot.c` / `src/sound.c`. Any button skips it; it is
  turned off under Settings → psp5 → Startup animation (`bootanim=0` in `ms0:/PSP5/psp5.cfg`).
- **Library:** the second tab (L/R). All games and apps in a grid with 5 columns, ↑↓←→ move. ↑ from the first row that sits
  under the button selects the Sort button (from the others ↑ goes straight to the top bar), ✕ opens the sorting (name /
  recently added), ○ or ↓ back to the cards. START opens the options menu for the selected card.

## Languages
All texts are in `src/lang.c` (English, Swedish, German, French, Spanish, Italian, Dutch, Portuguese, Russian, Japanese,
Czech). The language follows the PSP's system language; Swedish and Czech are chosen manually under
Settings → psp5 → Language (saved in `ms0:/PSP5/psp5.cfg`).

## Sounds
The interface sounds are WAV files in `PSP5/sounds/` on the storage (`move`, `select`, `back`, `error`, `boot`; 44.1 kHz,
16 bit; the zips contain them). If a file is missing, psp5 plays the built-in sound with the same name – they are calculated
in `src/sound.c`, no sound files are needed. `tools/convert_sounds.py` converts your own recordings to the right format.
On/off under Settings → psp5 → Interface sounds (saved in `ms0:/PSP5/psp5.cfg`).

## Settings (the cog wheel)
Laid out like a console's settings pages, and reads/writes the same files as arkMenu:
- **psp5:** autoboot, interface sounds, startup animation, language, clear image cache, version
- **Performance:** CPU in games / XMB, extra memory, memory stick cache, ISO cache
- **System:** USB charging, WPA2, logos, hide pictures/MAC/DLC, lights, UMD, analog stick, regions and more
- **Plugins:** switch rows in `ARK_01234/PLUGINS.TXT` and `SEPLUGINS/PLUGINS.TXT` on/off
- **Power and USB:** USB mode, file manager, XMB, sleep, shut down, restart, recovery menu (the same four as in the SELECT
  popup; no confirmation)

The CFW settings (`SETTINGS.TXT`) are written in exactly the same format as arkMenu and take effect after a restart.
Unknown lines and comments are kept.

## Installing on the PSP
1. Connect the PSP with USB.
2. Copy `dist/PSP` to the root of the memory stick (so that the file ends up in `PSP/GAME/PSP5/EBOOT.PBP`).
3. As ARK's launcher: copy the same file to `PSP/SAVEDATA/ARK_01234/VBOOT.PBP` (save arkMenu as `VBOOT_ARKMENU.PBP` first).
4. (Optional) Prepare covers: `py tools\covers.py E:\` (E: = the PSP).
5. Start "psp5" from the XMB → Game → Memory Stick, or via ARK's Custom Launcher.

## Controls
| Button | Function |
|---|---|
| ← → / the stick | browse (hold to go faster) |
| L / R | switch tab (Games / Library / Music) |
| ↑ from the first thing on a tab | select the top bar: ← → between the tabs and the cog wheel, ✕ opens the tab (or the settings on the cog wheel), ↓ or ○ back |
| ✕ (○ with the Japanese button setting) | start (on the card or the Start button) |
| START | options menu for the selected card (Games and Library) |
| SELECT | power popup in the middle of the screen: Sleep, Shut down, Restart, Recovery menu (all tabs) |
| ↓ in Games | select the Start and ⋯ buttons under the card (← → between them, ↑ or ○ back) |
| HOME | back to the XMB (the system's own question) |

## What is shown
- `ms0:/ISO` (+ one level of sub-folders): `.iso`, `.cso`, `.zso`, `.dax`
- `ms0:/PSP/GAME/*/EBOOT.PBP`: PSN and PS1 games and apps
- Everything shows up in the **Library**; **Games** (Home) shows the cards opened last and the pinned ones
- The UMD in the drive: first under **Games** and in the **Library**

Games are started with the same systemctrl calls as ARK-5's arkMenu (`src/launch.c`), including installed updates
(`PBOOT.PBP`), patched ISOs (`EBOOT.OLD`) and an unencrypted `BOOT.BIN`. Before the start everything is released: the loader
thread, textures, fonts, GU and the game list.

## Pictures and cache
Card and background pictures are looked for in this order:
1. `ms0:/PSP5/covers/<key>.ico|.bg` – from `tools/covers.py` (key = DISC_ID for ISO and UMD, folder name for EBOOT)
2. `ms0:/PSP5/cache/` – created by the launcher itself the first time a game is shown
3. ICON0/PIC1 inside the game

Only the cards near the highlight are loaded (in a background thread), and the background is loaded only when the highlight
has stood still for a short while. The game list is cached in `ms0:/PSP5/cache/list.bin`, so unchanged files are not read
again. Delete `ms0:/PSP5/cache` (or Settings → Clear image cache) if something looks wrong – it is rebuilt.

### Your own covers
Put pictures in `tools/covers-src/` (or give `--src`), named after the game ID or title:
`ULUS10041.jpg` or `My Game.png` → card picture, `ULUS10041_bg.jpg` → background. The pictures are cropped automatically.
Flags: `--force` (rewrite all), `--clean` (remove covers for games that no longer exist), `--dry-run`.

## Building (Windows + WSL1)
The toolchain (pspdev + psp-cfw-sdk) is in the WSL1 distro `pspdev` (`C:\WSL\pspdev`). WSL1 is used on purpose – WSL2 turns on
the hypervisor, which clashes with VMware.

```
wsl -d pspdev --exec bash /mnt/c/path/to/psp5/tools/build.sh
powershell -ExecutionPolicy Bypass -File tools\run-ppsspp.ps1      # test in PPSSPP
```

| Script | What it does |
|---|---|
| `tools/setup-wsl.sh` | installs pspdev + packages in WSL (run as root) |
| `tools/build.sh` | builds to `dist/PSP/GAME/PSP5/EBOOT.PBP` |
| `tools/make_icon.py` | draws psp5's own ICON0/PIC1 into `res/` |
| `tools/make_testdata.py` | made-up test games (iso/cso/zso/EBOOT) in `testdata/` |
| `tools/run-ppsspp.ps1` | copies to PPSSPP's memory stick and starts it |
| `tools/ppsspp-capture.ps1` | sends keys to PPSSPP and takes a screenshot |
| `tools/covers.py` | prepares covers on the memory stick |
| `tools/build.sh test` | test build with a scripted test mode (`src/testrun.c`) to `dist-test/` |
| `tools/ppsspp-test.ps1` | runs a test script (`tools/tests/*.txt`) in PPSSPP and saves screenshots – without key presses |

PPSSPP does not have ARK's systemctrl, so "ARK not found" is shown there when you press ✕. The game start itself has to be
tried on the PSP.

## Source code
| File | Contents |
|---|---|
| `src/main.c` | start-up, main loop, releasing before a game starts |
| `src/ui.c` | layout, animations, tabs |
| `src/gfx.c` | a small GU renderer + intraFont text |
| `src/loader.c` | background thread that loads pictures |
| `src/art.c`, `src/image.c` | covers/cache, PNG, scaling, blur, NTX format |
| `src/scan.c`, `src/iso.c`, `src/sfo.c` | scanning, ISO/CSO/ZSO/DAX, PARAM.SFO |
| `src/launch.c` | start through ARK-5's systemctrl |
| `src/settings.c` | the settings screen |
| `src/arkcfg.c`, `src/plugins.c` | ARK's SETTINGS.TXT and PLUGINS.TXT |
| `src/system.c` | USB, power, XMB, recovery menu |
