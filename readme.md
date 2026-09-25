Texture - ncurses based text editor with vi-style modal editing and a table-driven keymap system

Extends the kilo "Build Your Own Text Editor" tutorial with vi-style modal editing (normal/insert/visual/command) and a table-driven keymap system supporting multi-key commands

Bugs:
Currently exiting a mode can refuse and eventually cause a crash (likely a mem issue)