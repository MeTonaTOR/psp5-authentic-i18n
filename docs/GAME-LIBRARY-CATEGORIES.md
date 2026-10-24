# Game library categories

This change introduces the category model and persistent manual overrides as a separate layer from the scanner's launch types.

## Categories

- **PSP** — ISO/CSO/ZSO/DAX, UMD, PSP homebrew and PSN EBOOTs.
- **PS1** — entries scanned with the existing `ENTRY_POPS` classification.
- **Emulators** — entries whose paths are inside an `Emulator` or `Emulators` directory.
- **Other** — a fallback for entries that do not match a known group.

The inference intentionally does not alter `Entry.type`; launch behavior, ARK compatibility, and PSP Go's `ms0:` / `ef0:` scanning remain separate.

## Manual overrides

`categories_set()` stores a category override keyed by the full scanned path. `categories_save()` writes tab-separated path/category records to a caller-provided file, and `categories_load()` restores them after restart. The caller should place this file in the launcher's existing data directory, rebuild filtered views after a change, and call `categories_save()` after a successful move. The caller should also call `categories_forget_path()` when an entry is permanently deleted.

## Integration status

This is the category/persistence foundation only. The UI still needs to expose the PSP, PS1 and Emulators tabs, hide empty optional tabs, and add the “Move to…” menu action. Those UI changes should use these helpers rather than changing launch types or moving game files on disk.
