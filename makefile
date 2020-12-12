# ------------------------------------------------------------------------
# Generic makefile
# By Kronoman
# Thanks to Schwarzung for the help on making the original makefile system.
# ------------------------------------------------------------------------

# This has the target platform defined, this is modified by fix.bat or fix.sh
#include target.os

TARGET = LINUX

# Suggested by GNU Coding Stardards
SHELL = /bin/sh

# ===============================================
# Target binary name without extension
BINARY = kraptor

# Source directory
SRCDIR = src

# Include directory
INCDIR = include

# Source code suffix (.c, .cpp, etc)
SRCSUF = .c

# Simple source code test file (must be in same dir as makefile for now) :(
# The extension will be taken from SRCSUF, don't put it!
TESTFILE = test
# ===============================================


# -----------------------------
# -- Platform specific stuff --
# -----------------------------

# ------------------
# DJGPP target
# ------------------
ifeq ($(TARGET),DJGPP)

PLATFORMDIR=djgpp

# compiler to invoque
GCC = gcc
# GPP = gxx

# Binary file suffix
BINSUF = .exe
# object suffix
OBJSUF = .o

# If you need extra link options (like more librarys, add to LFLAGS var)
LFLAGS = -s -laldmb -ldumb -lalleg

# Compiler flags
CFLAGS = -I$(INCDIR) -Wall -O2
endif

# ------------------
# MingW32
# ------------------
ifeq ($(TARGET),MINGW32)

PLATFORMDIR=mingw32

GCC = gcc
# GPP = g++

# Binary file suffix
BINSUF = _w32.exe
OBJSUF = .o

# If you need extra link options (like more librarys, add to LFLAGS var)
LFLAGS = -s -mwindows -laldmb -ldumb -lalleg

# Compiler flags
CFLAGS = -I$(INCDIR) -Wall -O2
endif

# ------------------
# Linux
# ------------------
ifeq ($(TARGET),LINUX)

PLATFORMDIR=linux

GCC = gcc
# GPP = g++

# Binary file suffix
BINSUF = _linux.bin
OBJSUF = .o

# If you need extra link options (like more librarys, add to LFLAGS var)
LFLAGS = -laldmb -ldumb `allegro-config --libs`

# Compiler flags
CFLAGS = -I$(INCDIR) -Wall -O2
endif

# ---------------------------------
# -- Platform non-specific stuff --
# ---------------------------------

OBJDIR = obj/$(PLATFORMDIR)
BINDIR = bin

# -- The rules for build are in this file --
include makefile.all
