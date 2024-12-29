# To-Do Application

## Overview
This is a simple GTK-based To-Do list application. The app allows users to manage tasks efficiently through a graphical interface. The operations include adding, editing, and deleting tasks. A real-time clock is displayed in the interface for convenience.

## Run the project
The project can be run as
```bash
./compile.bash todo_app.c && ./gtk_todo_app.exe
```
###### NOTE: It will compile with warnings.

## Features
1. **Add Tasks**  
   - Users can input a task name and description and add it to the task list.
   - The task list updates dynamically.

2. **Edit Tasks**  
   - Users can select an existing task, modify its name or description, and update it in the list.

3. **Delete Tasks**  
   - Users can remove a selected task from the list.

4. **Real-Time Clock**  
   - A clock is displayed in the app, updating every second to show the current time.

## Usage Instructions
1. **Adding a Task**  
   - Enter a task name and description in the respective fields.
   - Click the "Add Task" button to save the task to the list.

2. **Editing a Task**  
   - Select a task from the list.
   - Modify the task name and/or description in the input fields.
   - Click the "Edit Task" button to save changes.

3. **Deleting a Task**  
   - Select a task from the list.
   - Click the "Delete Task" button to remove the task from the list.

4. **View Clock**  
   - The clock at the top of the app updates automatically.

## Limitations
- The app supports a maximum of 100 tasks at a time. 
- Task names and descriptions cannot be empty.

## Requirements
- GTK 3.x must be installed on your system.
- A builder XML file (`builder.xml`) is required to define the UI layout.
