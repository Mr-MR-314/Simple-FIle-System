# Linux File System with C

This project implements a Linux-like file system using C. It provides ⚙️ functionalities for creating, managing, and manipulating 📂 files and 📁 directories. Users can interact with this file system through a 🔧 command-line interface.

## **About the Project**

The project mimics a 🔬 hierarchical file system where 📂 directories and 📁 files are represented as nodes in a 🌲 tree structure. It supports a wide range of operations, including 🌟 file creation, 🗑 deletion, moving 📂 files, handling symbolic links 🔗, directory traversal, and even compressing 🔐 and decompressing the entire file system.

### **Features**

- 🔄 Creation of 📁 files and 📂 directories.
- 🔎 Recursive listing of directory contents.
- ➔ Move, 🔁 rename, and 🗑 delete files and directories.
- 📝 Save and load the directory structure.
- 🔗 Symbolic link creation.
- 📃 Directory sorting by name or date 🕒.
- 🔐 Merge directories with conflict resolution.
- 🏎️ Compression and decompression of the directory structure.

## **How to Use**

### **Compilation**

Ensure you have the required development tools and libraries installed (e.g., `gcc` and `zlib`). Then compile the project:

```bash
gcc -o linux_file_system main.c -lz
```

### **Running the Program**

Run the compiled executable:

```bash
./linux_file_system
```

You will enter a ⚖️ command-line interface where you can execute various commands to interact with the 🏢 file system.

## **Available Commands**

| **Command**               | **Description**                                                              | **Example Usage**                                                 |               |
| ------------------------- | ---------------------------------------------------------------------------- | ----------------------------------------------------------------- |
| `mkdir <name>`            | Creates a new 📂 folder in the current directory.                               | `mkdir documents`                                                 |
| `touch <name>`            | Creates a new 📁 file in the current directory.                                 | `touch notes.txt`                                                 |
| `ls`                      | Lists all 📂 files and folders in the current directory.                        | `ls`                                                              |  
| `lsrecursive`             | Recursively lists all files and folders starting from the current directory. | `lsrecursive`                                                     |
| `cd <folder>`             | Changes the current 🏢 directory to the specified folder.                       | `cd documents`                                                    |
| `cdup`                    | Moves to the 🔼 parent directory of the current folder.                         | `cdup`                                                            |
| `rm <name>`               | Deletes the specified 🗑 file or folder from the current directory.             | `rm notes.txt`                                                    |
| `mov <src> <dest>`        | Moves a file or folder to another directory.                                 | `mov notes.txt projects`                                          |
| `countFiles`              | Counts the total number of 📁 files in the entire directory tree.               | `countFiles`                                                      |
| `save <filename>`         | 📝 Saves the current directory structure to a file.                             | `save filesystem.txt`                                             |
| `load <filename>`         | Loads a directory structure from a previously saved file.                    | `load filesystem.txt`                                             |   
| `merge <src> <dest>`      | 🌐 Merges two directories, resolving any conflicts interactively.               | `merge src_folder dest_folder`                                    | 
| `symlink <target> <link>` | Creates a symbolic 🔗 link to an existing file or folder.                       | `symlink notes.txt shortcut`                                      |
| `sortBy <name \| date>`                                                                      | Sorts files and folders in the current directory by name or date. | `sortBy name` |
| `compress <filename>`     | 🔐 Compresses the entire directory structure into a file.                       | `compress archive.gz`                                             | 
| `decompress <filename>`   | 🔋 Decompresses a file and restores the directory structure.                    | `decompress archive.gz`                                           | 
| `rename <old> <new>`      | Renames a file or folder in the current directory.                           | `rename oldname.txt newname.txt`                                  |  
| `fullpath`                | Displays the full 🔍 path of the current directory.                             | `fullpath`                                                        |  

---

## **Examples**

### 1. Create Folders and Files

Commands:

```bash
mkdir documents
cd documents
touch notes.txt
mkdir projects
ls
```

Expected Output:

```bash
1 items   01 Dec 12:00   notes.txt
1 items   01 Dec 12:01   projects
```

### 2. Traverse Directory Recursively

Commands:

```bash
lsrecursive
```

Expected Output:

```bash
1 items   01 Dec 12:00   notes.txt
|_1 items   01 Dec 12:01   projects
|___Empty___
```

### 3. Save and Load Directory Structure

Commands:

```bash
save filesystem.txt
load filesystem.txt
```

Expected Output:

```bash
Directory structure 📝 saved to filesystem.txt
Directory structure loaded from filesystem.txt
```

### 4. Merge Directories with Conflict Handling

Commands:

```bash
merge dir1 dir2
```

Interactive Prompts:

```bash
Conflict detected: file1.txt already exists. Choose an option:
1. Skip
2. Rename
3. Overwrite
```

Expected Output (after resolving conflicts):

```bash
🌐 Directories merged.
```

### 5. Handle Symbolic Links

Commands:

```bash
symlink notes.txt shortcut_to_notes
ls
```

Expected Output:

```bash
1 items   01 Dec 12:00   notes.txt
1 items   01 Dec 12:05   shortcut_to_notes -> notes.txt
```

### 6. Sort by Name and Date

Commands:

```bash
sortBy name
ls
sortBy date
ls
```

Expected Output:

```bash
Sorted by name:
notes.txt
projects

Sorted by date:
projects
notes.txt
```

### 7. Compress and Decompress Directories

Commands:

```bash
compress archive.gz
decompress archive.gz
```

Expected Output:

```bash
🔐 Directory compressed to archive.gz
🔋 Directory decompressed from archive.gz
```

---

## **Notes**

- Commands are ✌️ case-sensitive.
- Conflicts (e.g., file with the same name) are resolved interactively unless automated handling is implemented.

---

## **Contributing**

📚 Contributions are welcome! If you have suggestions or bug fixes, please feel free to submit a pull request or raise an issue.

## **License**

This project is open-source and available under the 🔒 MIT License.

