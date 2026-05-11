# 🎮 WinEventHook

A lightweight **Windows console application** written in **C** using the **Win32 API**.

WinEventHook records and replays keyboard and mouse input using low-level Windows hooks and `SendInput()`.


---

# ✨ Features

- ✅ Global keyboard & mouse hooks
- ✅ Real-time input recording
- ✅ Replay saved recordings
- ✅ Binary event storage
- ✅ Replay file viewer
- ✅ Ignores injected/replayed input
- ✅ Mouse movement filtering
- ✅ Hotkey-based controls
- ✅ Multi-monitor support

---

# ⌨️ Hotkeys

| Key | Action |
|------|---------|
| `F8`  | View recording |
| `F9`  | Start / Stop recording |
| `F10` | Replay recording |
| `F11` | Exit application |

---


# ⚙️ How It Works

## Recording

The application installs global Windows hooks:

- `WH_KEYBOARD_LL`
- `WH_MOUSE_LL`

to capture keyboard and mouse events system-wide.

Recorded events include:

- Keyboard key up/down
- Mouse movement
- Mouse clicks
- Mouse wheel scrolling
- Timing data

---

## Saving

Captured events are converted into native Win32 `INPUT` structures and saved as:

```text
data/events_YYYYMMDD_HHMMSS.bin
```

---

## Replay

Replay loads saved `INPUT` events and replays them using:

```c
SendInput()
```

Replay timing is preserved for accurate automation.

---

# 🖱️ Mouse Filtering

Mouse movement spam is reduced using thresholds:

```c
#define MOUSE_MOVE_MIN_DT   100
#define MOUSE_MOVE_MIN_DIST 50
```

This improves performance and reduces file size.

---

# 🔨 Building

## Requirements

- Windows
- GCC / MinGW or MSVC
- Win32 API

---

## Build Using Makefile

```bash
make
```

---

# 🚀 Usage

1. Launch the executable
2. Press `F9` to start recording
3. Perform actions
4. Press `F9` again to save
5. Press `F10` to replay
6. Press `F8` to inspect recordings

---

# 📌 Notes

- Supports multi-monitor setups
- Uses absolute mouse coordinates
- Ignores replay-generated events
- Stores data in Win32 `INPUT` format

---

# 🛠️ Future Improvements

- Adjustable replay speed
- Loop replay
- Pause / Resume
- GUI version
- JSON export/import
- Macro editor

---

# 📄 License

MIT License

---

# ⚠️ Disclaimer

This project is intended for:

- Automation
- Testing
- Accessibility tools
- Educational purposes

Use responsibly.