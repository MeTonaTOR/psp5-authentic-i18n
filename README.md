# psp5-authentic

**psp5-authentic is a fork of [psp5](https://github.com/08Weber/psp5) by Mille Weber (08Weber), based on psp5 v1.5.3.**
It keeps what psp5 is – a launcher for PSPs with the **ARK-4 or ARK-5** custom firmware that recreates the look and feel
of a modern **home console's user interface** on a 480×272 screen from 2004 – and has one goal of its own: to be **as
authentic as possible to its home console counterpart**. Where the console does something, this fork does it the same
way: the Home screen with the games you played last and the ones you pinned, a menu you move through with the D-pad
alone, the same popups everywhere, the console's own light beam, golden dust and haze behind everything, and a start-up
that follows the console's.

Version 1.0.0 is the first release of the fork. Everything that changed compared with psp5 v1.5.3 is listed in the
[release notes](CHANGELOG.md). Many thanks to Mille Weber for psp5 itself: all of this stands on that work.

## About this fork

psp5-authentic is **Claude's work**: the code and the documentation are written by Claude, an AI assistant made by
Anthropic. The person who runs this fork has almost no coding experience. They do what they can – they test every update
on their own PSP and report back what they see, and the two of them work out the solutions together. So there will be
bugs; if you find one, please open an issue and say what you saw (and which PSP and ARK you have), which is exactly
how this project gets better.

psp5-authentic runs on PSPs with the **ARK-4 or ARK-5** custom firmware (PSP-1000, 2000, 3000 and the PSP Go). You can
start it like any other homebrew, or let it take over as ARK's own launcher so your PSP boots straight into it.

Everything you see is drawn by psp5 itself. No logos, icons, sounds or other assets from Sony are used –
it's an own take on that style.

## What it does

psp5 starts the way a modern home console does: a small grey logo on black, a short health notice over large, dark blue
out-of-focus circles, then dust that swirls up, with a soft, airy sound underneath, before the intro fades out and your
games fade in. The circles and the dust take the colours of your theme, so a Cosmic Red console boots in red. It plays
when the PSP starts and when it wakes up from sleep, but not when you come back from a game. Any button skips it, and
you can turn it off in settings. The
logo is psp5's own and everything is drawn and synthesized in code – no borrowed logos or sounds.

Then you get your games in a row along the top. The selected one is large with a thin light
frame around it, and its artwork fades in behind everything. Under it are two buttons, **Start** and a round
**⋯** button for its options: press ↓ to reach them, ← → to move between them, and ↑ or ○ to go back to the game.
**Games** is your home screen: the games and apps you opened last (newest first, at most ten), plus the ones you
pinned – pinned cards come first, and up to nine can be pinned. A UMD in the drive shows up first, and it appears or
disappears as soon as you put a disc in or take it out. The options (START, or ✕ on the ⋯ button) let you pin a card,
take it off Home or add it, look at its information, manage its saved data or delete it.
Next to it is the **Library**: everything you have, games and apps together, in a grid that's easy to scan
through. Press ↑ from a card in the first row that sits right under the **Sort** button to reach it, and ✕ to sort by
name or by what you added most recently. From every other card of the first row ↑ goes straight to the top bar.

There are no button hints on screen: the whole menu is navigated with the D-pad. Press ↑ from the first thing on a
tab (the game, the first row of the Library, the first track) to select the top bar. **Games**, **Library**, **Music**
and the **cog wheel** are buttons there: ← → move between them, ✕ on a tab opens it, and ✕ on the cog wheel opens the
settings. ↓ takes you back down to where you were (the Sort button, if you came from there) and ○ to the game that is
selected. L and R still switch tabs at any time. The card you have selected stays large when the focus moves to the top bar,
the Sort button or the Start and ⋯ buttons; only its white ring goes away, so it's clear what has the focus.

The last tab is **Music**, like the music player in the XMB: it lists the MP3 files in `ms0:/MUSIC` (folders
too) with their titles and artists, and keeps playing while you browse your games. The track that's playing
shows up as a little note at the top of the other tabs. "Now playing" shows the album art – from the MP3 itself
or a `cover.jpg` / `folder.jpg` in the album folder – and holding ← or → seeks through the track (the longer you
hold, the faster it goes).

Pressing ✕ starts the game the same way ARK's own menu does, so updates, patched ISOs and
PS1 games all work. Before a game launches psp5 shuts itself down completely and frees all of its memory,
so the game gets the whole PSP.

The cog wheel at the top right opens the settings, laid out like a console's settings pages. From there you can change ARK's CFW
options (CPU speed, extra memory, caches, USB charging and so on), turn plugins on and off, connect over
USB, restart, shut down, jump to the XMB or open ARK's recovery menu.
On any of the three tabs, **SELECT** opens a popup in the middle of the screen with the same four choices as at the
bottom of *Settings → Power and USB*: **Sleep**, **Shut down**, **Restart** and **Recovery menu**. ✕ does the chosen one
at once (there is no "press again" step), ○ or SELECT closes the popup.

A few other things I cared about:

- **It's quick.** Only the tiles near your selection are loaded, in the background, and all artwork is cached
  as small ready-made textures, so after the first start it opens almost instantly.
- **It has sounds.** A tick when you move around or change tab, a chime when you select something or start a
  game, and one for going back, for errors and for the startup. They are WAV files in `PSP5/sounds/` on the
  card (the zips include them). If a file is missing, psp5 plays the built-in sound of the same name, which it
  synthesizes in code – you can listen to those in [`docs/sounds/`](docs/sounds). Turn all sounds off in settings.
- **It speaks your language.** psp5 follows the PSP's system language: English, German, French, Spanish,
  Italian, Dutch, Portuguese, Russian and Japanese. Swedish and Czech are there too – the PSP itself doesn't
  offer them, so you pick them under *Settings → psp5 → Language*.
- **It has themes.** *Settings → Theme* has the colours of the console covers (Midnight Black, Cosmic Red,
  Nova Pink, Starlight Blue, Galactic Purple, Volcanic Red, Cobalt Blue, Sterling Silver and the Chroma
  colours), each with the console's own scene behind your games: a wide beam of light from the top left, a band of
  golden dust and a cool haze at the bottom (the scene stands still). The
  background is the same on every screen; the Library and Music show it under a darkening. And the classic
  looks of the older consoles, PS1 to PS4: the grey console with its button symbols,
  the dark towers and glowing orbs, the XMB wave and the blue PS4 waves – with the same beam of light and haze behind
  them, but no dust.
  Highlight a theme to preview it and press ✕ to use it. You can also turn the game art in the background off,
  so the theme is always visible.
- **It shows the volume.** Press the volume buttons anywhere in psp5 and a bar slides in at the top,
  with the level and a crossed-out speaker when the sound is off. While the PSP charges, the battery icon fills up
  in a loop next to a little lightning bolt.
- **It has a file manager**, like ARK's own launcher: *Settings → Power and USB → File manager*. Browse the
  Memory Stick (and the internal storage on a PSP Go), mark files with □, and copy, move, rename or delete them
  or make a new folder from the △ menu. Copying shows a progress bar and can be cancelled with ○, and deleting
  always asks first. *Manage Saved Data* (from a game's options) sorts with the same **Sort** button and popup
  as the Library: ↑ from the first row, then ✕.
- **It never touches flash0**, and you can always get back to the normal XMB.

## Installing

Everything you need to get psp5 onto your PSP – as a normal app or as ARK's launcher – is in
**[INSTALL.md](INSTALL.md)**. The download is on the [Releases](../../releases) page.

## Buttons

| Button | What it does |
|---|---|
| ← → or the analog stick | move between games (hold to go faster) |
| ↑ ↓ | move between rows in the Library · in Games, ↓ selects the Start / ⋯ buttons under the card and ↑ goes back |
| ↑ from the first thing on a tab | select the top bar (the tabs and the cog wheel) · ← → move along it · ↓ back down to where you were · ○ back to the selected game |
| L / R | switch between Games, Library and Music |
| START | options of the selected game (Games and Library) |
| SELECT | power popup: Sleep, Shut down, Restart, Recovery menu (on every tab) |
| ✕ | start the selected game (or the Start button) · on the ⋯ button: options · on the Sort button: sort the Library (or the saved data) · on a tab in the top bar: open it · on the cog wheel: settings |
| □ in Music | pause/resume |
| ✕ in Music | play the selected track (or pause it) |
| ← → in Music | previous / next track · hold to seek |
| any button | skip the startup screen |
| ○ | back (also from the Start / ⋯ buttons and the Sort button) |
| HOME | quit to the XMB |

If something ever goes wrong, hold **SELECT** while the PSP boots – ARK then skips its settings and you land
in the normal XMB. psp5 also looks after itself. If a start doesn't finish (the PSP froze or was switched off
while psp5 was starting), the next start is a *safe start* without the startup screen, music and UMD. If that
doesn't finish either, psp5 sends the PSP to the XMB.

If the HOME menu doesn't respond inside a game, see *Help!* in [INSTALL.md](INSTALL.md).
**L + R + START + ↓** always quits a game. That one is built into ARK.

## Covers

psp5 uses each game's own icon and background automatically. Square icons (PSP minis) keep their shape: they sit
in the middle of the card on a darker, blurred copy of themselves. If you'd like your own covers, there's a small
tool for your computer that prepares them – see the covers section in [INSTALL.md](INSTALL.md).

## Status

psp5-authentic runs on the **PSP-1000, PSP-2000, PSP-3000 and PSP Go** with **ARK-4 or ARK-5** – there's a download for
each ARK, on the PSP Go too (see [INSTALL.md](INSTALL.md)), and it adapts the settings to your model and ARK version
(for example, the PSP-1000 has no extra memory or USB charging, and ARK-4 has fewer CPU speeds).

**What has been tested:** every update of this fork is tested on a **PSP-1000**, which is the only device used for
testing. The PSP-2000, PSP-3000 and PSP Go, and the two ARK versions, are not tested separately. They are expected to work
(the original psp5 was developed and tried on a PSP-2000 with ARK-5), but the PSP Go fixes in particular could not be
tried on a PSP Go. Besides testing on the PSP-1000, Claude runs the code through automated checks on a PC (a mock of the
PSP's graphics chip and fake folder trees); those checks are not part of this repository. Please try it on your own PSP
and report anything that behaves oddly. The PS Vita isn't supported.

## Building it yourself

(What each change touches in the code is written down in [docs/DEVELOPER-NOTES.md](docs/DEVELOPER-NOTES.md).)

psp5 is written in C and built with the [pspdev](https://github.com/pspdev/pspdev) toolchain plus
[psp-cfw-sdk](https://github.com/pspdev/psp-cfw-sdk), which provides ARK's headers and libraries. With pspdev
installed it's just:

```sh
make
```

On Windows I build it inside WSL – `tools/setup-wsl.sh` installs the toolchain and `tools/build.sh` builds
`dist/PSP/GAME/PSP5/EBOOT.PBP` (`tools/build.sh ark4` or `make ARK=4` builds the ARK-4 version; both also run
on the PSP Go).

## Thanks

- **Mille Weber (08Weber)**, who made [psp5](https://github.com/08Weber/psp5), the project this fork is based on (v1.5.3).
- The [ARK-5](https://github.com/PSP-Arkfive) and [ARK-4](https://github.com/PSP-Archive/ARK-4) teams – psp5
  starts games and stores settings the same way their arkMenu does, and uses ARK's libraries through
  psp-cfw-sdk.
- [pspdev](https://github.com/pspdev) for the toolchain, PSPSDK and packages.
- intraFont, libpng, zlib, LZ4 and libcisoread.

## License

GPL-3.0 – see [LICENSE](LICENSE).

psp5-authentic is a modified version of psp5 v1.5.3 by Mille Weber, changed in 2026. The copyright notices of the original
are kept in the source files; what was changed is listed in the [release notes](CHANGELOG.md).

psp5 and psp5-authentic are unofficial fan projects (by Mille Weber, and a fork of it). They are not affiliated with or endorsed by Sony Interactive
Entertainment. "PSP" and "PlayStation" are trademarks of Sony Interactive Entertainment Inc.
