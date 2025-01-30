#!/bin/bash
OUTPUT=ToDo_Application.exe
echo "Starting Linking & Compiling"
if gcc todo_app.c task_manager.c user_auth.c clock_displaying.c file_operations.c button_callbacks.c -o $OUTPUT `pkg-config --cflags --libs gtk+-3.0`; then
    echo "Compiled Successfully to $OUTPUT. Run it using \"./$OUTPUT\""
else
    echo "Compilation Failed"
fi

exit 0
