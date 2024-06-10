#!/bin/sh

# Build the project, watcom DOS real mode executable

# Compile the source code release mode 
wcl -ms -bt=dos -d0 -lr -s -fe=demo.exe main.c 

