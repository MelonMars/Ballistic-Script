//
// Updated: carter 2023-12-03
// Ballistic Language Interpreter (see spec.md)
//

#include "balli.h"
#include <math.h>

int main(int argc, char* argv[]) {
    if (argc > 1) {
        char* program = readProgram(argv[1]);
        if (program == NULL) {
            printf("Error reading file");
            return 1;
        }
        if (valid(program) != 0) {
            printf("Syntax error idgaf where");
            return 1;
        }
        printf("Starting to interpret. Word of warning that there is some weird ass race condition somewhere, idk where, that I can't figure out. If you get a segfault or something try again\n");
        MemoryField memField = constructMemoryField(program);

    } else {
        printf("Usage: balli PROGRAM");
        return 1;
    }
}


char* readProgram(const char* filename) {
        FILE* file = fopen(filename, "r");

        if (file == NULL) {
            fprintf(stderr, "Error opening file: %s\n", filename);
            return NULL;
        }

        // Find the size of the file
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory for the content
        char* content = (char*)malloc(size + 1);

        if (content == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            fclose(file);
            return NULL;
        }

        // Read the content into the buffer
        fread(content, 1, size, file);
        content[size] = '\0'; // Null-terminate the string

        fclose(file);
        return content;
}

int valid(char* program) {
    //make sure it contains a `.memField`, `.cannonCommands`, and `.end` in that order
    return 0;
}

//chat.openai.com
int countPlusSigns(const char* str) {
    int count = 0;

    while (*str != '\n') {
        if (*str == '+') {
            count++;
        }
        str++;
    }

    return count;
}
int convertTo1D(int row, int col, int width) {
    return row * width + col;
}

MemoryField constructMemoryField(char* program) {
    char* memString = extractSubstring(program, ".memField\n", ".cannonCommands");
    printf("%s", memString);
    int width = 0;
    for (int i=0;memString[i]!='\n';i++) {
        if ((int) (char *) memString[i] == '+') { //Oh yeah Jetbrains auto-fix hack!
            width++;
        }
    }
    int height = getHeight(extractSubstring(program, ".memField", ".cannonCommands"));
    //Take off barriers
    width-=2;
    height-=2;
    printf("%d, %d\n", height, width);
    char *field = (char *) malloc((width + 3) * sizeof(char));
    for (int i=0;i<width+2;i++) {
        field[i] = '+';
    }
    field[width+3] = '\0';
    char* mefield = extractSubstring(program, field, field);
    char* mTable = memTable(mefield, height, width);
    char* c = extractSubstring(program, field, "cannonCommands");
    EquivalencyPair* equTable = equivalencyTable(extractSubstring(c, field, "."));
    char* cannonCommands = extractSubstring(program, ".cannonCommands", ".end");

    //commands via extractSubstring(program, ".cannonCommands", ".end"); and then loop through them, and process each one
    //Current project status: working equivalency table, working memory table. Now all I need to do is extract() the fire
    //through modifying the mTable and checking all equivalencies. I also now have to actually come up with my instruction
    //set, which will be a hassle, but whatever. I'll get to that eventually
    int lineStart = 0;
    int currentIndex = 0;
    Cannon mainCannon;
    mainCannon.x = 0;
    mainCannon.y = 0;
    while (cannonCommands[currentIndex] != '\0') {
        if (cannonCommands[currentIndex] == '\n' || cannonCommands[currentIndex] == '\0') {
            int lineLength = currentIndex - lineStart;
            char* line = malloc(lineLength + 1);
            strncpy(line, cannonCommands + lineStart, lineLength);
            line[lineLength] = '\0';

            // Run line
            char typeDet[5];
            for (int i=0;i<4;i++) {
                typeDet[i] = line[i];
            }
            typeDet[4] = '\0';
            if (strcmp(typeDet, "fire") == 0) {
                const char *pastFire = extractSubstring(removeWhitespace(line+4), "(", ")");
                int intensity, rotation, angle;
                sscanf(pastFire, "%d,%d,%d", &intensity, &rotation, &angle);
                double impactX, impactY, impactAngle;
                calculateProjectile(mainCannon.x, mainCannon.y, intensity, 0,angle, 9.8, &impactX, &impactY, &impactAngle);
                if (impactAngle < 10) {
                    mTable[convertTo1D(impactY, impactX, width)] = (char) mainCannon.reg;
                } else if (impactAngle > 10 && impactAngle < 20) {
                    printf("%c", mTable[convertTo1D(impactY, impactX, width)]);
                } else if (impactAngle > 20 && impactAngle < 30) {
                    mainCannon.reg +=  mTable[convertTo1D(impactY, impactX, width)];
                } else if (impactAngle > 30 && impactAngle < 40) {
                    mTable[convertTo1D(impactY, impactX, width)] += mainCannon.reg;
                } else if (impactAngle > 40 && impactAngle < 50) {
                    mainCannon.reg = mTable[convertTo1D(impactY, impactX, width)];
                }
            } else if (strcmp(typeDet, "move") == 0) {
                const char *pastMove = extractSubstring(removeWhitespace(line+4), "(", ")");
                int dX, dY;
                sscanf(pastMove, "%d,%d", &dX, &dY);
                mainCannon.x += dX;
                mainCannon.y += dY;
            }

            free(line);

            if (cannonCommands[currentIndex] != '\0') {
                currentIndex++;
                lineStart = currentIndex;
            }
        } else {
            currentIndex++;
        }
    }
    MemoryField mfield;
    return mfield;
}

EquivalencyPair parseEquivalencyPair(const char* input) {
    EquivalencyPair pair;
    pair.name = malloc(2); // Allocate space for the character and null terminator
    pair.name[0] = input[0];
    pair.name[1] = '\0'; // Null terminator

    // Assuming the input format is "<character>: \"<string>\""
    const char* start = strchr(input, '\"');
    const char* end = strrchr(input, '\"');

    if (start != NULL && end != NULL && start < end) {
        size_t len = end - start - 1; // Length of the string without quotes
        pair.val = malloc(len + 1); // Allocate space for the string and null terminator
        strncpy(pair.val, start + 1, len);
        pair.val[len] = '\0'; // Null terminator
    } else {
        // Handle error: invalid input format
        pair.val = NULL;
    }

    return pair;
}

EquivalencyPair* equivalencyTable(char* multilineString) {
    int lineStart = 0;
    int currentIndex = 0;
    int memRequired = 0;

    // Calculate the memory required for EquivalencyPair structures
    while (multilineString[currentIndex] != '\0') {
        if (multilineString[currentIndex] == '\n' || multilineString[currentIndex] == '\0') {
            int lineLength = currentIndex - lineStart;
            char* line = malloc(lineLength + 1);
            strncpy(line, multilineString + lineStart, lineLength);
            line[lineLength] = '\0';

            // Update memory requirement based on parseEquivalencyPair result
            memRequired += sizeof(EquivalencyPair);

            free(line);

            if (multilineString[currentIndex] != '\0') {
                currentIndex++;
                lineStart = currentIndex;
            }
        } else {
            currentIndex++;
        }
    }

    // Allocate memory for EquivalencyPair structures
    EquivalencyPair* equTable = malloc(memRequired);

    int i = 0;
    currentIndex = 0;
    lineStart = 0;

    while (multilineString[currentIndex] != '\0') {
        if (multilineString[currentIndex] == '\n' || multilineString[currentIndex] == '\0') {
            int lineLength = currentIndex - lineStart;
            char* line = malloc(lineLength + 1);
            strncpy(line, multilineString + lineStart, lineLength);
            line[lineLength] = '\0';

            EquivalencyPair parsedLine = parseEquivalencyPair(line);
            equTable[i] = parsedLine;

            i++;

            free(line);

            if (multilineString[currentIndex] != '\0') {
                currentIndex++;
                lineStart = currentIndex;
            }
        } else {
            currentIndex++;
        }
    }

    return equTable;
}

//chat.openai.com
char* extractSubstring(const char* input, const char* start, const char* end) {
    const char* startPtr = strstr(input, start);

    if (startPtr == NULL) {
        // Starting string not found
        return NULL;
    }

    startPtr += strlen(start); // Move to the end of the starting string

    const char* endPtr = strstr(startPtr, end);

    if (endPtr == NULL) {
        // Ending string not found
        return NULL;
    }

    // Calculate the length of the substring
    size_t length = endPtr - startPtr;

    // Allocate memory for the substring
    char* result = (char*)malloc(length + 1);

    if (result == NULL) {
        // Memory allocation failed
        return NULL;
    }

    // Copy the substring to the result buffer
    strncpy(result, startPtr, length);

    // Null-terminate the result string
    result[length] = '\0';

    return result;
}

char* memTable(const char* input, int height, int width) {
    char* table = (char*)malloc(width * height * sizeof(char) + 1);
    int latestIndex = 0;
    for (int i=0;input[i]!='\0';i++) {
        if (input[i] != '+' && input[i] != '\n') {
            table[latestIndex] = input[i];
            latestIndex++;
        }
    }
    table[width * height * sizeof(char) + 1] = '\0';
    return table;
}

int getHeight(const char* multilineString) {
    if (multilineString == NULL) {
        printf("Error: Input string is NULL\n");
        return 0;
    }

    // Initialize variables
    int lineStart = 0;
    int currentIndex = 0;
    int inField = 0;
    int height = 0;
    // Loop through each character in the string
    while (multilineString[currentIndex] != '\0') {
        // Check for the end of the line
        if (multilineString[currentIndex] == '\n' || multilineString[currentIndex] == '\0') {
            // Calculate the length of the line
            int lineLength = currentIndex - lineStart;

            // Allocate memory for the line
            char* line = malloc(lineLength + 1);
            if (line == NULL) {
                printf("Error: Memory allocation failed\n");
                return 0;
            }

            // Copy the line from the multiline string
            strncpy(line, multilineString + lineStart, lineLength);
            line[lineLength] = '\0';  // Null-terminate the line

            // Process the line (you can replace this with your own logic)
            if (countPlusSigns(line) > 2) {
                if (inField == 0) {
                    inField = 1;
                } else {
                    free(line);
                    break;
                }
            }
            height++;

            // Free the memory allocated for the line
            free(line);

            // Move to the next line (if not at the end of the string)
            if (multilineString[currentIndex] != '\0') {
                currentIndex++;
                lineStart = currentIndex;
            }
        } else {
            // Move to the next character
            currentIndex++;
        }
    }
    return height;
}

char* removeWhitespace(const char* input) {
    if (input == NULL) {
        return NULL;
    }

    // Count non-whitespace characters
    size_t length = 0;
    for (const char* p = input; *p != '\0'; ++p) {
        if (!isspace((unsigned char)*p)) {
            length++;
        }
    }

    // Allocate memory for the new string
    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        // Allocation failed
        return NULL;
    }

    // Copy non-whitespace characters to the new string
    char* pResult = result;
    for (const char* p = input; *p != '\0'; ++p) {
        if (!isspace((unsigned char)*p)) {
            *pResult++ = *p;
        }
    }

    // Null-terminate the new string
    *pResult = '\0';

    return result;
}

void calculateProjectile(double x0, double y0, double v0, double cannonRotation, double launchAngle, double g, double *impactX, double *impactY, double *impactAngle) {
    // Convert angles to radians
    double thetaCannon = cannonRotation * M_PI / 180.0;
    double thetaLaunch = launchAngle * M_PI / 180.0;

    // Calculate components of initial velocity with cannon rotation
    double v0x = v0 * cos(thetaCannon) * cos(thetaLaunch);
    double v0y = v0 * sin(thetaCannon) * cos(thetaLaunch);

    // Calculate time of flight
    double tFlight = (2.0 * v0y) / g;

    // Calculate impact position
    *impactX = x0 + v0x * tFlight;
    *impactY = y0 + v0y * tFlight - 0.5 * g * tFlight * tFlight;

    // Calculate impact angle
    *impactAngle = atan((v0y - g * tFlight) / v0x) * 180.0 / M_PI;
}
