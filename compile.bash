#!/bin/bash
gcc -o gtk_todo_app main.c `pkg-config --cflags --libs gtk+-3.0`
echo "Compiled Successfully to ./gtk_todo_app"
