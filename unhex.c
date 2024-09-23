
// UNix HEX-editor
// compile with `gcc -o hexed hexed.c -lncurses`

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>

// Constants for editor layout
#define BYTES_PER_LINE 16
#define MAX_ROWS 24

// Function to load file content into memory
unsigned char* load_file(const char* filename, long* file_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* data = malloc(*file_size);
    if (!data) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    fread(data, 1, *file_size, file);
    fclose(file);

    return data;
}

// Function to save edited data back to the file
void save_file(const char* filename, unsigned char* data, long file_size) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error saving file");
        exit(EXIT_FAILURE);
    }

    fwrite(data, 1, file_size, file);
    fclose(file);
}

// Function to convert a byte to its ASCII equivalent or '.' if non-printable
char byte_to_ascii(unsigned char byte) {
    return isprint(byte) ? byte : '.';
}

// Function to display the hex editor view
void display_hex_editor(WINDOW* win, unsigned char* data, long file_size, int selected_byte) {
    wclear(win);

    int rows, cols;
    getmaxyx(win, rows, cols);
    int max_bytes_per_row = BYTES_PER_LINE;
    int max_visible_rows = MAX_ROWS;

    int start_line = selected_byte / max_bytes_per_row - (max_visible_rows / 2);
    if (start_line < 0) start_line = 0;

    for (int row = 0; row < max_visible_rows; row++) {
        int offset = (start_line + row) * max_bytes_per_row;
        if (offset >= file_size) break;

        wprintw(win, "%08x - ", offset);
        for (int col = 0; col < max_bytes_per_row; col++) {
            if (offset + col < file_size) {
                if (offset + col == selected_byte) {
                    wattron(win, A_REVERSE);
                    wprintw(win, "%02x", data[offset + col]);
                    wattroff(win, A_REVERSE);
                } else {
                    wprintw(win, "%02x", data[offset + col]);
                }
            } else {
                wprintw(win, "  ");
            }
            wprintw(win, " ");
        }

        wprintw(win, "- ");
        for (int col = 0; col < max_bytes_per_row; col++) {
            if (offset + col < file_size) {
                wprintw(win, "%c", byte_to_ascii(data[offset + col]));
            } else {
                wprintw(win, " ");
            }
        }

        wprintw(win, "\n");
    }

    wrefresh(win);
}

// Main function for the hex editor
void hex_editor(const char* filename) {
    // Initialize curses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Load file
    long file_size;
    unsigned char* data = load_file(filename, &file_size);

    // Editor state
    int selected_byte = 0;
    int edit_mode = 0;  // 0: not editing, 1: editing the first digit, 2: editing the second digit
    unsigned char edit_value = 0;

    // Main loop
    while (1) {
        display_hex_editor(stdscr, data, file_size, selected_byte);

        int ch = getch();
        if (ch == KEY_UP) {
            if (selected_byte >= BYTES_PER_LINE) selected_byte -= BYTES_PER_LINE;
        } else if (ch == KEY_DOWN) {
            if (selected_byte + BYTES_PER_LINE < file_size) selected_byte += BYTES_PER_LINE;
        } else if (ch == KEY_LEFT) {
            if (selected_byte > 0) selected_byte--;
        } else if (ch == KEY_RIGHT) {
            if (selected_byte < file_size - 1) selected_byte++;
        } else if (ch == 'q') {
            break;  // Quit on 'q'
        } else if (isxdigit(ch)) {
            int hex_value = isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
            if (edit_mode == 0) {
                edit_value = hex_value << 4;
                edit_mode = 1;
            } else {
                edit_value |= hex_value;
                data[selected_byte] = edit_value;
                selected_byte++;
                edit_mode = 0;
            }
        }
    }

    // Save file on exit
    save_file(filename, data, file_size);

    // Clean up
    endwin();
    free(data);
}

// Entry point
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    hex_editor(argv[1]);
    return EXIT_SUCCESS;
}

