#include <stdio.h>

int search(int key, int frame_items[], int frame_occupied) {
    for (int x = 0; x < frame_occupied; x++)
        if (frame_items[x] == key)
            return 1;
    return 0;
}

void reset(int frame_items[], int max_frames) {
    for (int x = 0; x < max_frames; x++) {
        frame_items[x] = -1;
    }
}

void printOuterStructure(int max_frames) {
    printf("Stream\t\t");
    for (int x = 0; x < max_frames; x++)
        printf("Frame%d\t\t", x+1);
}

void printCurrFrames(int item, int frame_items[], int frame_occupied, int max_frames) {
    printf("\n%d \t\t", item);
    for (int x = 0; x < max_frames; x++) {
        if (x < frame_occupied)
            printf("%d \t\t", frame_items[x]);
        else
            printf("- \t\t");
    }
}

int predict(int ref_str[], int frame_items[], int refStrLen, int index, int frame_occupied) {
    int result = -1, farthest = index;
    for (int x = 0; x < frame_occupied; x++) {
        int y;
        for (y = index; y < refStrLen; y++) {
            if (frame_items[x] == ref_str[y]) {
                if (y > farthest) {
                    farthest = y;
                    result = x;
                }
                break;
            }
        }
        if (y == refStrLen)
            return x;
    }
    return (result == -1) ? 0 : result;
}

void fifoPageReplacement(int ref_str[], int refStrLen, int frame_items[], int max_frames) {
    int frame_occupied = 0, count = 0, y = 0;
    printOuterStructure(max_frames);
    for (int x = 0; x < refStrLen; x++) {
        if (!search(ref_str[x], frame_items, frame_occupied)) {
            frame_items[y] = ref_str[x];
            y = (y + 1) % max_frames;
            count++;
            printCurrFrames(ref_str[x], frame_items, frame_occupied < max_frames ? ++frame_occupied : frame_occupied, max_frames);
        } else {
            printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
        }
    }
    printf("\n\nTotal Page Faults (FIFO): %d\n\n", count);
}

void lruPageReplacement(int ref_str[], int refStrLen, int frame_items[], int max_frames) {
    int frame_occupied = 0, c1, count = 0, k = 0, c2[20];
    printOuterStructure(max_frames);
    for (int x = 0; x < refStrLen; x++) {
        c1 = 0;
        for (int y = 0; y < frame_occupied; y++) {
            if (ref_str[x] != frame_items[y])
                c1++;
        }
        if (c1 == frame_occupied) {
            count++;
            if (frame_occupied < max_frames) {
                frame_items[frame_occupied++] = ref_str[x];
                printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
            } else {
                int b[20];
                for (int r = 0; r < max_frames; r++) {
                    c2[r] = 0;
                    for (int y = x - 1; y >= 0; y--) {
                        if (frame_items[r] != ref_str[y])
                            c2[r]++;
                        else
                            break;
                    }
                }
                for (int r = 0; r < max_frames; r++)
                    b[r] = c2[r];
                int pos = 0;
                for (int r = 1; r < max_frames; r++)
                    if (b[r] > b[pos])
                        pos = r;
                frame_items[pos] = ref_str[x];
                printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
            }
        } else {
            printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
        }
    }
    printf("\n\nTotal Page Faults (LRU): %d\n\n", count);
}

void optimalPageReplacement(int ref_str[], int refStrLen, int frame_items[], int max_frames) {
    int frame_occupied = 0, hits = 0;
    printOuterStructure(max_frames);
    for (int x = 0; x < refStrLen; x++) {
        if (search(ref_str[x], frame_items, frame_occupied)) {
            hits++;
            printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
        } else {
            if (frame_occupied < max_frames) {
                frame_items[frame_occupied++] = ref_str[x];
                printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
            } else {
                int pos = predict(ref_str, frame_items, refStrLen, x + 1, frame_occupied);
                frame_items[pos] = ref_str[x];
                printCurrFrames(ref_str[x], frame_items, frame_occupied, max_frames);
            }
        }
    }
    printf("\n\nTotal Page Faults (Optimal): %d\n\n", refStrLen - hits);
}

int main() {
    int ref_str[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int refStrLen = sizeof(ref_str) / sizeof(ref_str[0]);
    int max_frames = 3;
    int frame_items[max_frames];

    printf("FIFO Replacement Algorithm:\n");
    fifoPageReplacement(ref_str, refStrLen, frame_items, max_frames);
    reset(frame_items, max_frames);

    printf("LRU Replacement Algorithm:\n");
    lruPageReplacement(ref_str, refStrLen, frame_items, max_frames);
    reset(frame_items, max_frames);

    printf("Optimal Replacement Algorithm:\n");
    optimalPageReplacement(ref_str, refStrLen, frame_items, max_frames);
}
