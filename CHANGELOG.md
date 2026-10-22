# Changelog

All notable changes of **psp5-authentic**, a fork of [psp5](https://github.com/08Weber/psp5) by Mille Weber (08Weber).
The newest version is on top. Changes that are not in a release yet are listed under *Unreleased*.

## Unreleased

- **Settings:** new switch **Show battery percentage** (Settings → psp5, under *Startup animation*), off by default. When it is on,
  the real battery level is shown as a number (for example *87%*), in the same font as the clock, between the clock and the
  battery. The battery and the cog wheel move to the left to make room for it, and the tabs make room too (long languages
  shrink their tab names a little more). If the PSP reports no battery, no number is shown and nothing moves. The switch is saved
  like the other psp5 settings.
- **Manage Saved Data:** the **Sort** button and the popup that opens under it moved 4 px to the left. The frame now starts at
  the same edge as the highlight of a save, and the text sits right above the circles of the saves.

## 1.0.0 – the first release of psp5-authentic

psp5-authentic is based on **psp5 v1.5.3** by Mille Weber (08Weber). Its goal is to be **as authentic as possible to its home
console counterpart**. Everything below is what changed compared with psp5 v1.5.3. Several fixes that
psp5 1.5.4 and 1.6.0 made on their side are in too (they are marked *also fixed in psp5 1.5.4/1.6.0*); they were written
again for this code, they are not copied.

psp5-authentic is **Claude's work**: the code and the documentation are written by Claude, an AI assistant made by Anthropic.
The person who runs the fork has almost no coding experience; they test every update on their own PSP, report back what they
see, and the two of them work out the solutions together.

> **Please read:** every update is tested on a **PSP-1000**, the only device used for testing. The PSP-2000, PSP-3000 and
> PSP Go, and the two ARK versions, are not tested separately, and the PSP Go fixes below could not be tried on a PSP Go.
> If something behaves oddly on your PSP, please open an issue and say which PSP and ARK you have.

### Home screen and menu, the console way

- **Games is now Home.** The Apps tab is gone, as on the console. Home shows the games and apps you opened last (newest first,
  at most ten) and the ones you pinned (pinned cards come first, at most nine). A UMD in the drive is always the first card
  and does not count. What Home remembers is kept in `home.txt` in psp5's data folder; on the first start it is filled with
  the ten newest.
- **Library** shows everything in a grid (80 × 44 cards). Its **Sort** button is reached with ↑ and opens a popup with the two
  sortings (name / recently added); the old white square icon is gone.
- **Start and Options buttons** under the big card in Games are real buttons now: ↓ selects **Start**, ← → move between Start
  and the round **⋯ Options** button, ↑ or ○ goes back to the card. START (or ✕ on ⋯) opens the options popup of the card; START
  closes it again. START no longer starts a game, and it no longer plays a track in Music.
- **The whole main menu works with the D-pad alone.** The hint pills are gone. ↑ from the first thing on a tab selects the
  **top bar**: ← → move over **Games, Library, Music** and the **cog wheel**, ✕ opens a tab (or the settings on the cog wheel),
  ↓ goes back down, ○ goes back to the selected game. L and R still switch tabs. △ no longer opens the settings.
- In the Library, ↑ stops at the Sort button only from a card that sits right under it; from every other first-row card it goes
  straight to the top bar.
- **The selected card keeps its size** when the focus is somewhere else (Sort, Start, Options, the top bar); only its white
  ring fades out. The ring fits the card's picture exactly (no gap, no corners poking out).
- The **type tag** (PSP / APP / PSN / PS1 / UMD) sits in front of the game's name next to the big card, as on the console; the game
  id is no longer shown on the launcher (it is on the Information screen).
- **Options popup** of a card: *Manage Saved Data*, *Information*, *Delete*, *Remove from Home* / *Add to Home*, *Pin to Home* /
  *Unpin from Home*.
- **New: power popup on SELECT.** On every tab, SELECT opens a popup in the middle of the screen, drawn like the options popup,
  with **Sleep, Shut down, Restart, Recovery menu** – the same names, the same order and the same functions as at the bottom
  of *Settings → Power and USB*. ✕ does the chosen one, ○ or SELECT closes the popup.
- **The "press again to confirm" step is gone** from Sleep, Restart, Shut down and Recovery menu: they run on the first ✕.
  In *Settings → Power and USB* the order is now Sleep, **Shut down**, **Restart**, Recovery menu.

### Information, Delete and saved data

- **Information** (options popup): where the game is and how much room is left there, file name, size, date, volume name, the
  size of an EBOOT game's whole folder, the unpacked size and ratio of CSO/ZSO/DAX images, and four values from the game's
  PARAM.SFO (DISC_ID, DISC_VERSION, APP_VER, PSP_SYSTEM_VER). The UMD gets it too.
- **Delete** (options popup): asks first, with *Cancel* preselected, shows a progress panel that can be cancelled, then takes the
  card out of the list. Only ISO/CSO/ZSO/DAX files and game folders can be deleted – never the UMD, psp5's own folder,
  SAVEDATA, or anything else. A cancelled or failed delete never leaves a half game in the list.
- **Manage Saved Data** (options popup): lists every folder in `PSP/SAVEDATA` (also on a PSP Go's internal storage) with the
  game it belongs to, title, description, size and date. Mark saves (□), select all (△), sort with the same Sort button and popup as the
  Library (last changed / size / name), and delete after a question. `ARK_*` (ARK's settings and `VBOOT.PBP`) is locked and can
  never be marked or deleted.
- The **file manager** and Manage Saved Data share the look and the questions of the rest of the app.

### Backgrounds, themes and the start-up

- **The console-cover colours have the console's scene:** a wide beam of light from a lamp at the top left that leans to the right, a band
  of fine golden dust with soft bokeh, and a cool haze along the bottom left; each colour tints them. **PS1–PS4** get the beam
  and the haze behind their own decorations (but no dust).
- **One background on every screen.** Games, Library, Music, Settings, Information, the file managers and the USB screen draw the
  same surface; the beam is a little softer (60 %) than the haze, the dust and the shapes. Library and Music lay **one and the
  same darkening** over it. A game's own picture is shaded at the top, the left and the bottom so the tabs, the cards and the
  text stay readable. The old tint that a game's icon gave the background is gone.
- The **PS1 symbols** sit in one place (top right) on every screen.
- **Start-up** (the intro): the grey logo, the health notice (now one larger paragraph) over dark out-of-focus circles, then
  floating dust – and then it fades straight into the menu; the still picture at the end is gone. The circles and the dust take
  the colours of your theme (a Cosmic Red console boots in red).
- The **volume popup** sits 12 px from the top.
- **Faster background:** the part of the background that never changes (surface, glows, beam, haze, dust) is drawn once into a
  picture in video memory and shown from there; only the moving shapes of PS1–PS4 are drawn every frame.

### A cleaner, smoother picture

- **Everything at rest is drawn on whole pixels:** no more borders that are 1 px on one side and 2 px on the other, no soft
  icons. The selection frames are centred on their text (also in long languages).
- **Smooth icons and shapes:** the button symbols, the L/R key caps, the cog wheel, the charging bolt, the play/seek triangles,
  the music note and the speaker are pixel-exact anti-aliased images; rounded rectangles and circles are anti-aliased; drawn
  lines (check marks, the PS1 symbols, the waves, the boot logo) have soft edges.
- **Popups look the same everywhere:** one dark, square panel with a soft shadow, a faint light fill and a thin frame round the
  chosen entry. The question before a delete is drawn in the middle of the screen; progress panels and toasts use the same
  style in a lighter colour. The look lives in one place (`popup.c`).
- **Czech** is drawn correctly: the hacek and ring marks are drawn by psp5 itself, because the PSP's fonts do not have them.

### Languages and sounds

- **Czech** is a new language (select it under *Settings → psp5 → Language*), so there are now 11: English, Swedish, German,
  French, Spanish, Italian, Dutch, Portuguese, Russian, Japanese and Czech.
- Titles that are capitalised in English (*Manage Saved Data*, *Disable Pause Game*, *System Storage*) are capitalised the same
  way in the other languages, and 64 texts were changed in all (those capitals, and shorter wording where a text was much wider than the English one).
- **Interface sounds** are WAV files in `PSP5/sounds` (the zips include them): *move*, *select*, *back*, *error* and *boot*. The
  nine old sounds were merged into these five. If a file is missing, psp5 plays the built-in sound of the same name.

### Settings

- The **analog stick** works in the Settings menu: up/down move, sideways acts like ← / → (enter, go back, change a value).
- The **Version** row in *Settings → psp5* shows 1.0.0.
- Strings and code that nothing uses any more are removed (among them *Press again to confirm*).

### Fixes

*Also fixed in psp5 1.5.4 / 1.6.0:*

- **.DAX games** are listed like ISO, CSO and ZSO (also in the file manager, the delete guard and the Information screen).
- **USB mode and the file manager closed at once** when they were opened with the button that is the confirm button of your
  setup; they now wait until every button is released. Starting USB no longer gives up when the system reports an error.
- **PSP Go, psp5 started from the internal storage:** games were started as if they were on a Memory Stick (PS1: error
  800201A8, ISO: freeze). `ef0:` is now always scanned and `ms0:` only when it is a real stick, so the games start with the right
  run levels.
- **PSP Go homebrew on the internal storage** (for example the GTA III and Super Smash Bros ports) is started with the run level
  that ARK's own menu uses, so ports that read their files from `ms0:` find them.
- **"Boot psp5"** showed as off until the first game was started.
- **A crackle when Settings opened** (the sound played while Settings was still being read).
- **PSP minis icons** are no longer stretched: a square icon keeps its shape on a darker, blurred copy of itself that fills the
  space beside it.
- **Wide pictures are drawn in narrow strips**, which the PSP's graphics chip handles much faster (the artwork behind the cards).

*Other fixes:*

- Behind the Settings, Information and delete screens the menu got a stick that was pushed all the way left and up, so the
  selection slid to the first card with move sounds. It now gets a centred stick.
- A picture that claims a size such as 65537 × 65537 could pass the size check (the product wrapped around in 32 bits); the check is
  done in 64 bits now.
- Cancelling a folder copy left the half-made folder behind; the 1000th copy of a file could overwrite the existing one.
- `SETTINGS.TXT` and `PLUGINS.TXT` could lose lines on saving: game-specific lines past the 64th were deleted, and a file that
  did not fit was cut. Long files are now kept in full, and a file that could not be read completely is never written over.
- The loader no longer writes through a null pointer when memory runs out; the cards just show no pictures.
- Games and apps in category folders (`PSP/GAME/PS1/<game>` and the like) show up (this is psp5 v1.5.3's own change).

### Faster

- Opaque fills, gradients and pictures are drawn without blending; the battery and clock are read once a second; text widths
  are remembered; the blur and the JPEG decoding are cheaper; the music tags are cached on the card so later starts only read
  new or changed files.

### Removed

- The Apps tab and *Move to Apps / Games*, the on-screen hint pills (L R Tab, Settings), the △ shortcut to Settings, the tint a
  game's icon gave the background, and the code and strings that went with them.

### Downloads and installing

`psp5-authentic-v1.0.0-ARK5.zip`, `-ARK4.zip`, `-PSPGo-ARK5.zip` and `-PSPGo-ARK4.zip` – see [INSTALL.md](INSTALL.md). They use
the same folders as psp5 (`PSP/GAME/PSP5`, `PSP5`), so they replace an installed psp5 and keep its settings and covers (the
image cache is rebuilt once).

### Credits and licence

Based on [psp5](https://github.com/08Weber/psp5) v1.5.3 by Mille Weber (08Weber). Written with Claude (Anthropic). GPL-3.0, like
the original; the original copyright notices are kept in the source files. Not affiliated with or endorsed by Sony Interactive Entertainment.
