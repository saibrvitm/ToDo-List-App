#!/bin/bash
if gcc -o gtk_todo_app $1 `pkg-config --cflags --libs gtk+-3.0`; then
    echo "Compiled Successfully to ./gtk_todo_app.exe"
else
    echo "Compilation Failed"
fi

exit 0
