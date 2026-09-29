CFLAGS = -std=c11 -Iinclude
LIBS = src/dict.c src/editor.c src/highlight.c src/keymap.c src/logger.c src/render.c src/utils.c src/vector.c
BUILD_DIR = build

$(BUILD_DIR)/texture: src/texture.c $(LIBS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -lncurses -o $@

test: tests/*.c src/assert.c $(LIBS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -lncurses -o $(BUILD_DIR)/test_runner
	./$(BUILD_DIR)/test_runner

clean:
	rm -f $(BUILD_DIR)/texture $(BUILD_DIR)/test_runner

run: $(BUILD_DIR)/texture
	./$(BUILD_DIR)/texture

.PHONY: test clean
