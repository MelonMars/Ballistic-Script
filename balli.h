//
// Created by carter on 12/3/23.
//

#ifndef ESOLANGS_BALLI_H
#define ESOLANGS_BALLI_H

#endif //ESOLANGS_BALLI_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int x;
    int y;
    int integer_value;
    char* string_value;
    char name;
} MemoryCell;

typedef struct {
    MemoryCell* field;
    int width;
    int height;
} MemoryField;

typedef struct {
    char* name;
    char* val;
} EquivalencyPair;

typedef struct {
    int x;
    int y;
    char reg;
} Cannon;

char* readProgram(const char* fname);
int valid(char* program);
//constructMemoryField is the entire fucking program
MemoryField constructMemoryField(char* program);
char* extractSubstring(const char* input, const char* start, const char* end);
int getHeight(const char* multilineString);
int countPlusSigns(const char* str);
char* memTable(const char* input, int height, int width);
EquivalencyPair* equivalencyTable(char* multilineString);
EquivalencyPair parseEquivalencyPair(const char* input);
char* removeWhitespace(const char* input);
void calculateProjectile(double x0, double y0, double v0, double cannonRotation, double launchAngle, double g, double *impactX, double *impactY, double *impactAngle);
int convertTo1D(int row, int col, int width);