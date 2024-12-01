#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <zlib.h> // For compression and decompression

enum nodeType {File, Folder, Symlink};

typedef struct node {
    enum nodeType type;
    char* name;
    int numberOfItems;
    size_t size;
    time_t date;
    char* content;
    struct node* previous;
    struct node* parent;
    struct node* next;
    struct node* child;
    char* symlinkTarget; // For symbolic links
} node;

// Function to create a new folder in the current directory
void mkdir(node* currentFolder, char* command);

// Function to create a new file in the current directory
void touch(node* currentFolder, char* command);

// Function to list files and folders in the current directory
void ls(node* currentFolder);

// Function to recursively list files and folders in the current directory
void lsrecursive(node* currentFolder, int indentCount);

// Function to edit the content of an existing file
void edit(node* currentFolder, char* command);

// Function to print the current directory's full path
void pwd(char* path);

// Function to change the current directory
node* cd(node* currentFolder, char* command, char** path);

// Function to move up to the parent directory
node* cdup(node* currentFolder, char** path);

// Function to free a node (and its children)
void freeNode(node* freeingNode);

// Function to remove a node (file or folder)
void removeNode(node* removingNode);

// Function to remove a file or folder
void rm(node* currentFolder, char* command);

// Function to move a node (file or folder) to another location
void mov(node* currentFolder, char* command);

// Function to count the total number of files in the entire directory tree
int countFiles(node* folder);

// Function to save the directory structure to a file or compressed file
void saveDirectory(node* folder, FILE* file);

// Function to load the directory structure from a file or compressed file
node* loadDirectory(FILE* file, node* parent);

// Function to merge two directories, resolving conflicts interactively
void mergeDirectories(node* destFolder, node* srcFolder);

// Function to create a symbolic link to an existing file or folder
void createSymlink(node* currentFolder, char* target, char* linkName);

// Function to sort files and folders in the current directory by name or date
void sortDirectory(node* folder, const char* criterion);

// Function to compress the entire directory structure into a compressed file
void compressDirectory(node* folder, const char* filename);

// Function to decompress a file and restore the directory structure
node* decompressDirectory(const char* filename);


char* getString() {
    size_t size = 10;
    char* str = (char*)malloc(size);
    size_t len = 0;
    int ch;

    while ((ch = fgetc(stdin)) != EOF && ch != '\n') {
        if (len + 1 >= size) {
            size += 16;
            str = (char*)realloc(str, size);
        }
        str[len++] = ch;
    }
    str[len] = '\0';
    return str;
}

// Week 2: Count Total Files
int countFiles(node* folder) {
    if (!folder) return 0;

    int count = 0;
    if (folder->type == File) {
        count++;
    }

    node* currentNode = folder->child;
    while (currentNode) {
        count += countFiles(currentNode);
        currentNode = currentNode->next;
    }
    return count;
}

// Week 2: Save Directory Structure
// Function to save the directory to either a regular file or compressed file
void saveDirectory(node* folder, FILE* file) {
    if (!folder) return;

    // Save folder or file details
    fprintf(file, "%d %s %ld %ld\n", folder->type, folder->name, folder->size, folder->date);

    // Save file content if it's a file
    if (folder->type == File && folder->content) {
        fprintf(file, "CONTENT:%s\n", folder->content);
    }

    // Recursively save child nodes
    node* currentNode = folder->child;
    while (currentNode) {
        saveDirectory(currentNode, file);
        currentNode = currentNode->next;
    }

    // Mark the end of this folder's children
    fprintf(file, "END\n");
}

// Week 2: Load Directory Structure
// Function to load the directory from either a regular file or compressed file
node* loadDirectory(FILE* file, node* parent) {
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "END", 3) == 0) break;

        node* newNode = (node*)malloc(sizeof(node));
        newNode->parent = parent;
        newNode->child = NULL;
        newNode->next = NULL;
        newNode->previous = NULL;
        newNode->symlinkTarget = NULL;

        // Parse node details
        sscanf(line, "%d %ms %ld %ld", (int*)&newNode->type, &newNode->name, &newNode->size, &newNode->date);

        // Read content if available
        if (newNode->type == File && fgets(line, sizeof(line), file) && strncmp(line, "CONTENT:", 8) == 0) {
            newNode->content = strdup(line + 8);
            newNode->content[strlen(newNode->content) - 1] = '\0'; // Remove newline
        } else {
            newNode->content = NULL;
        }

        // Recursively load children
        newNode->child = loadDirectory(file, newNode);

        // Add to parent's children
        if (parent->child == NULL) {
            parent->child = newNode;
        } else {
            node* last = parent->child;
            while (last->next) last = last->next;
            last->next = newNode;
            newNode->previous = last;
        }
    }

    return parent ? parent->child : NULL;
}

// Week 3: Rename Node
void renameNode(node* currentNode, const char* newName) {
    if (!currentNode) return;

    free(currentNode->name);
    currentNode->name = strdup(newName);
    printf("Renamed to '%s'\n", currentNode->name);
}

// Week 3: Display Full Path
void displayFullPath(node* currentNode) {
    if (!currentNode) return;

    if (currentNode->parent) {
        displayFullPath(currentNode->parent);
    }
    printf("/%s", currentNode->name);
}

node* getNode(node *currentFolder, char* name, enum nodeType type) {

    if (currentFolder->child != NULL) {

        node *currentNode = currentFolder->child;

        while (currentNode->next != NULL) {

            if (strcmp(name, currentNode->name) == 0 && currentNode->type == type) {
                return currentNode;
            }
            currentNode = currentNode->next;
        }

        if (strcmp(name, currentNode->name) == 0 && currentNode->type == type) {
            return currentNode;
        } else return NULL;

    } else return NULL;
}

node* getNodeTypeless(node *currentFolder, char* name) {

    if (currentFolder->child != NULL) {

        node *currentNode = currentFolder->child;

        while (currentNode->next != NULL) {

            if (strcmp(name, currentNode->name) == 0) {
                return currentNode;
            }
            currentNode = currentNode->next;
        }

        if (strcmp(name, currentNode->name) == 0) {
            return currentNode;
        } else return NULL;

    } else return NULL;
}

void mkdir(node *currentFolder, char *command) {
    if (strtok(command, " ") != NULL) {
        char* folderName = strtok(NULL, " ");
        if (folderName != NULL) {
            if (getNodeTypeless(currentFolder, folderName) == NULL) {

                currentFolder->numberOfItems++;
                node *newFolder = (node*) malloc(sizeof(node));

                if (currentFolder->child == NULL) {
                    currentFolder->child = newFolder;
                    newFolder->previous = NULL;
                    newFolder->parent = currentFolder;
                } else {

                    node *currentNode = currentFolder->child;

                    while (currentNode->next != NULL) {
                        currentNode = currentNode->next;
                    }
                    currentNode->next = newFolder;
                    newFolder->previous = currentNode;
                    newFolder->parent = NULL;
                }

                char* newFolderName = (char*) malloc(sizeof(char)*(strlen(folderName)+1));
                strcpy(newFolderName, folderName);

                newFolder->name = newFolderName;
                newFolder->type = Folder;
                newFolder->numberOfItems = 0;
                newFolder->size = 0;
                newFolder->date = time(NULL);
                newFolder->content = NULL;
                newFolder->next = NULL;
                newFolder->child = NULL;

                printf("Folder '%s' added\n", newFolder->name);
            } else {
                fprintf(stderr, "'%s' is already exist in current directory!\n",  folderName);
            }
        }
    }
}

void touch(node *currentFolder, char *command) {

    if (strtok(command, " ") != NULL) {
        char *fileName = strtok(NULL, " ");
        if (fileName != NULL) {
            node* existing = getNodeTypeless(currentFolder, fileName);
            if (existing) {
                printf("Conflict detected: %s already exists. Choose an option:\n", fileName);
                printf("1. Skip\n2. Rename\n3. Overwrite\n");
                int choice;
                scanf("%d", &choice);
                getchar(); // Consume the newline character

                if (choice == 1) {
                    printf("Skipping %s\n", fileName);
                    return;
                } else if (choice == 2) {
                    char newName[256];
                    printf("Enter a new name for %s: ", fileName);
                    fgets(newName, sizeof(newName), stdin);
                    newName[strcspn(newName, "\n")] = '\0'; // Remove newline
                    fileName = strdup(newName);
                    printf("Renamed to %s\n", fileName);
                } else if (choice == 3) {
                    printf("Overwriting %s\n", fileName);
                    removeNode(existing); // Remove the existing file
                } else {
                    printf("Invalid choice. Skipping %s.\n", fileName);
                    return;
                }
            }
            else if (existing == NULL){

                currentFolder->numberOfItems++;

                node *newFile = (node *) malloc(sizeof(node));

                if (currentFolder->child == NULL) {
                    currentFolder->child = newFile;
                    newFile->previous = NULL;
                    newFile->parent = currentFolder;
                } else {

                    node *currentNode = currentFolder->child;

                    while (currentNode->next != NULL) {
                        currentNode = currentNode->next;
                    }
                    currentNode->next = newFile;
                    newFile->previous = currentNode;
                    newFile->parent = NULL;
                }

                char* newFileName = (char*) malloc(sizeof(char)*(strlen(fileName)+1));
                strcpy(newFileName, fileName);

                newFile->name = newFileName;
                newFile->type = File;
                newFile->numberOfItems = 0;
                newFile->size = 0;
                newFile->date = time(NULL);
                newFile->content = NULL;
                newFile->next = NULL;
                newFile->child = NULL;

                printf("File '%s' added\n", newFile->name);
            } else {
                fprintf(stderr, "'%s' is already exist in current directory!\n", fileName);
            }
        }
    }
}

void ls(node *currentFolder) {
    if (currentFolder->child == NULL) {
        printf("___Empty____\n");
    } else {

        node *currentNode = currentFolder->child;

        while (currentNode->next != NULL) {

            struct tm *date_time = localtime(&currentNode->date);
            char dateString[26];
            strftime(dateString, 26, "%d %b %H:%M", date_time);

            if (currentNode->type == Folder) {
                printf("%d items\t%s\t%s\n", currentNode->numberOfItems, dateString, currentNode->name);
            } else {
                printf("%dB\t%s\t%s\n", (int)currentNode->size, dateString, currentNode->name);
            }

            currentNode = currentNode->next;
        }

        struct tm *date_time = localtime(&currentNode->date);
        char dateString[26];
        strftime(dateString, 26, "%d %b %H:%M", date_time);

        if (currentNode->type == Folder) {
            printf("%d items\t%s\t%s\n", currentNode->numberOfItems, dateString, currentNode->name);
        } else {
            printf("%dB\t%s\t%s\n", (int)currentNode->size, dateString, currentNode->name);
        }
    }
}

void lsrecursive(node *currentFolder, int indentCount) {

    if (currentFolder->child == NULL) {
        for (int i = 0; i < indentCount; ++i) {
            printf("\t");
        }
        if (indentCount != 0 ) {
            printf("|_");
        }
        printf("___Empty____\n");
    } else {

        node *currentNode = currentFolder->child;

        while (currentNode->next != NULL) {

            for (int i = 0; i < indentCount; ++i) {
                printf("\t");
            }
            if (indentCount != 0 ) {
                printf("|_");
            }
            struct tm *date_time = localtime(&currentNode->date);
            char dateString[26];
            strftime(dateString, 26, "%d %b %H:%M", date_time);

            if (currentNode->type == Folder) {
                printf("%d items\t%s\t%s\n", currentNode->numberOfItems, dateString, currentNode->name);
            } else {
                printf("%dB\t%s\t%s\n", (int)currentNode->size, dateString, currentNode->name);
            }
            if (currentNode->type == Folder){
                lsrecursive(currentNode, indentCount+1);
            }
            currentNode = currentNode->next;
        }
        for (int i = 0; i < indentCount; ++i) {
            printf("\t");
        }
        if (indentCount != 0 ) {
            printf("|_");
        }
        struct tm *date_time = localtime(&currentNode->date);
        char dateString[26];
        strftime(dateString, 26, "%d %b %H:%M", date_time);

        if (currentNode->type == Folder) {
            printf("%d items\t%s\t%s\n", currentNode->numberOfItems, dateString, currentNode->name);
        } else {
            printf("%dB\t%s\t%s\n", (int)currentNode->size, dateString, currentNode->name);
        }
        if (currentNode->type == Folder){
            lsrecursive(currentNode, indentCount+1);
        }
    }
}

void edit(node *currentFolder, char *command) {

    if (strtok(command, " ") != NULL) {
        char *fileName = strtok(NULL, " ");
        if (fileName != NULL) {
            node * editingNode = getNode(currentFolder, fileName, File);
            if (editingNode != NULL) {
                printf("%s\n", "Please enter some text to overwrite the file content:");
                if (editingNode->content != NULL){
                    free(editingNode->content);
                }
                char* content = getString();
                if (content != NULL) {
                    editingNode->content = content;
                    editingNode->size = strlen(editingNode->content);
                    editingNode->date = time(NULL);
                }
            }
        }
    }
}

void pwd(char *path) {
    if (strlen(path) != 1){

        for (int i = 0; i < strlen(path)-1 ; ++i) {
            printf("%c", path[i]);
        }
        printf("\n");
    } else {
        printf("%s\n", path);
    }
}

node* cd(node *currentFolder, char *command, char **path) {

    if (strtok(command, " ") != NULL) {
        char* folderName = strtok(NULL, " ");
        if (folderName != NULL) {

            node *destinationFolder = getNode(currentFolder, folderName, Folder);

            if ( destinationFolder != NULL) {

                size_t newPathLength = strlen(*path) + strlen(destinationFolder->name) + 2;

                *path = (char *) realloc(*path, sizeof(char)* newPathLength);

                strcat(strcat(*path, destinationFolder->name), "/");

                return destinationFolder;
            } else {
                fprintf(stderr, "There is no '%s' folder in current directory!\n",  folderName);
                return currentFolder;
            }
        } else {
            return currentFolder;
        }
    } else {
        return currentFolder;
    }
}

node* cdup(node *currentFolder, char **path) {

    size_t newPathLength = strlen(*path) - strlen(currentFolder->name);

    while (currentFolder->previous != NULL) {
        currentFolder = currentFolder->previous;
    }
    if (currentFolder->parent != NULL ) {

        *path = (char *) realloc(*path, sizeof(char)* newPathLength);
        (*path)[newPathLength-1] = '\0';

        currentFolder = currentFolder->parent;
        return currentFolder;
    } else {
        return currentFolder;
    }
}

void freeNode(node *freeingNode) {

    if (freeingNode->child != NULL) {

        node* currentNode = freeingNode->child;

        while (currentNode->next != NULL) {
            node* nextNode = currentNode->next;
            freeNode(currentNode);
            currentNode = nextNode;
        }
        freeNode(currentNode);
    }
    free(freeingNode->name);
    free(freeingNode->content);
    free(freeingNode);

}

void removeNode(node *removingNode) {
    if (removingNode->parent != NULL){
        if (removingNode->next != NULL) {
            removingNode->next->parent = removingNode->parent;
            removingNode->parent->child = removingNode->next;
            removingNode->next->previous = NULL;
        } else {
            removingNode->parent->child = NULL;
        }
    } else {
        if (removingNode->next != NULL) {
            removingNode->previous->next = removingNode->next;
            removingNode->next->previous  = removingNode->previous;
        } else {
            removingNode->previous->next = NULL;
        }
    }
}

void rm(node *currentFolder, char *command) {

    if (strtok(command, " ") != NULL) {
        char *nodeName = strtok(NULL, " ");
        if (nodeName != NULL) {
            node *removingNode = getNodeTypeless(currentFolder, nodeName);

            if (removingNode != NULL) {

                printf("Do you really want to remove %s and all of its content? (y/n)\n", removingNode->name);

                char *answer = getString();
                if (strcmp(answer, "y") == 0 ) {
                    currentFolder->numberOfItems--;
                    removeNode(removingNode);
                    printf("%s and its all content is removed!\n", removingNode->name);
                    freeNode(removingNode);
                }
                free(answer);
            } else {
                fprintf(stderr, "'%s' is not exist in current directory!\n",  nodeName);
            }
        }
    }
}

void moveNode(node *movingNode, node *destinationFolder) {

    if (destinationFolder->child == NULL) {
        destinationFolder->child = movingNode;
        movingNode->previous = NULL;
        movingNode->parent = destinationFolder;
        movingNode->next = NULL;
    } else {

        node *currentNode = destinationFolder->child;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }

        currentNode->next = movingNode;
        movingNode->previous = currentNode;
        movingNode->parent = NULL;
        movingNode->next = NULL;
    }
    destinationFolder->numberOfItems++;
}

void mov(node *currentFolder, char *command) {

    char* nodeName;
    char* destinationName;

    if (strtok(command, " ") != NULL) {
        nodeName = strtok(NULL, " ");
        if (nodeName != NULL) {
            destinationName = strtok(NULL, " ");
            if (destinationName != NULL) {
                if (strtok(NULL, " ")) {
                    return;
                } else {

                    node* movingNode = getNodeTypeless(currentFolder, nodeName);
                    node* destinationFolder = getNode(currentFolder, destinationName, Folder);

                    if (destinationFolder != NULL && movingNode != NULL && destinationFolder != movingNode) {

                        removeNode(movingNode);
                        moveNode(movingNode, destinationFolder);
                    } else {
                        fprintf(stderr, "Something you made wrong!\n");
                    }
                }
            }
        }
    }
}

// Helper functions for comparing and swapping nodes (used for sorting)
int compareNodesByName(const void* a, const void* b) {
    node* nodeA = *(node**)a;
    node* nodeB = *(node**)b;
    return strcmp(nodeA->name, nodeB->name);
}

int compareNodesByDate(const void* a, const void* b) {
    node* nodeA = *(node**)a;
    node* nodeB = *(node**)b;
    return difftime(nodeA->date, nodeB->date);
}

void sortDirectory(node* folder, const char* criterion) {
    if (!folder || folder->child == NULL) return;

    // Count the number of child nodes
    int count = 0;
    node* current = folder->child;
    while (current) {
        count++;
        current = current->next;
    }

    // Populate an array of child nodes
    node** nodesArray = malloc(count * sizeof(node*));
    current = folder->child;
    for (int i = 0; i < count; i++) {
        nodesArray[i] = current;
        current = current->next;
    }

    // Sort the array based on the criterion
    if (strcmp(criterion, "name") == 0) {
        qsort(nodesArray, count, sizeof(node*), compareNodesByName);
    } else if (strcmp(criterion, "date") == 0) {
        qsort(nodesArray, count, sizeof(node*), compareNodesByDate);
    }

    // Re-link the sorted nodes back into the tree
    folder->child = nodesArray[0];
    folder->child->previous = NULL;
    for (int i = 0; i < count - 1; i++) {
        nodesArray[i]->next = nodesArray[i + 1];
        nodesArray[i + 1]->previous = nodesArray[i];
    }
    nodesArray[count - 1]->next = NULL;

    free(nodesArray);
    printf("Directory sorted by %s.\n", criterion);
}

// Function to merge two directories, resolving any conflicts interactively
void mergeDirectories(node* destFolder, node* srcFolder) {
    if (!destFolder || !srcFolder || srcFolder->type != Folder || destFolder->type != Folder) return;

    node* current = srcFolder->child;
    while (current) {
        node* next = current->next;
        int choice;
        // Check for conflicts (same name)
        node* existing = getNodeTypeless(destFolder, current->name);
        if (existing) {
            printf("Conflict detected: %s already exists. Choose an option:\n", current->name);
            printf("1. Skip\n2. Rename\n3. Overwrite\n");
            
            // Declare and initialize the choice variable
            scanf("%d", &choice);
            getchar(); // Consume the newline character

            if (choice == 1) {
                // Skip the conflicting file/folder
                printf("Skipping %s\n", current->name);
            } else if (choice == 2) {
                // Rename the new file/folder
                char newName[256];
                printf("Enter a new name for %s: ", current->name);
                fgets(newName, sizeof(newName), stdin);
                newName[strcspn(newName, "\n")] = '\0'; // Remove newline
                free(current->name);
                current->name = strdup(newName);
                printf("Renamed to %s\n", current->name);
            } else if (choice == 3) {
                // Overwrite the existing file/folder
                printf("Overwriting %s\n", current->name);
                removeNode(existing); // Remove the existing node
            } else {
                // Handle invalid input
                printf("Invalid choice. Skipping %s.\n", current->name);
                return;
            }
        }

        // Move the current node to the destination folder
        if (!existing || choice == 3) {
            current->parent = destFolder;
            if (destFolder->child == NULL) {
                destFolder->child = current;
                current->previous = NULL;
            } else {
                node* last = destFolder->child;
                while (last->next) last = last->next;
                last->next = current;
                current->previous = last;
            }
        }
        current = next;
    }
    printf("Directories merged.\n");
}

// Handle symbolic links
void createSymlink(node* currentFolder, char* target, char* linkName) {
    if (!currentFolder) return;

    node* targetNode = getNodeTypeless(currentFolder, target);
    if (!targetNode) {
        printf("Target not found: %s\n", target);
        return;
    }

    if (getNodeTypeless(currentFolder, linkName)) {
        printf("Conflict: Symlink name already exists: %s\n", linkName);
        return;
    }

    node* symlinkNode = malloc(sizeof(node));
    symlinkNode->type = Symlink;
    symlinkNode->name = strdup(linkName);
    symlinkNode->symlinkTarget = strdup(target);
    symlinkNode->parent = currentFolder;
    symlinkNode->next = symlinkNode->child = NULL;
    symlinkNode->previous = NULL;

    // Add symlink to the directory
    if (currentFolder->child == NULL) {
        currentFolder->child = symlinkNode;
    } else {
        node* last = currentFolder->child;
        while (last->next) last = last->next;
        last->next = symlinkNode;
        symlinkNode->previous = last;
    }

    printf("Symbolic link '%s' -> '%s' created.\n", linkName, target);
}

// Compression (using zlib)
// void compressDirectory(node* folder, const char* filename) {
//     if (!folder) return;

//     FILE* file = fopen(filename, "wb");
//     if (!file) {
//         printf("Error: Unable to create compressed file.\n");
//         return;
//     }

//     gzFile gzfile = gzdopen(fileno(file), "wb");
//     if (!gzfile) {
//         fclose(file);
//         printf("Error: Unable to open compressed stream.\n");
//         return;
//     }

//     saveDirectory(folder, gzfile); // Save directory structure into compressed stream
//     gzclose(gzfile);

//     printf("Directory compressed to %s.\n", filename);
// }

// Decompression (using zlib)
// node* decompressDirectory(const char* filename) {
//     FILE* file = fopen(filename, "rb");
//     if (!file) {
//         printf("Error: Unable to open compressed file.\n");
//         return NULL;
//     }

//     gzFile gzfile = gzdopen(fileno(file), "rb");
//     if (!gzfile) {
//         fclose(file);
//         printf("Error: Unable to open compressed stream.\n");
//         return NULL;
//     }

//     node* folder = malloc(sizeof(node));
//     folder->child = loadDirectory(gzfile, folder); // Load directory structure from compressed stream
//     gzclose(gzfile);

//     printf("Directory decompressed from %s.\n", filename);
//     return folder;
// }

int main() {

    node *root = (node*) malloc(sizeof(node));

    char *rootName = (char *) malloc(sizeof(char)*2);
    strcpy(rootName, "/");
    root->type = Folder;
    root->name = rootName;
    root->numberOfItems = 0;
    root->size = 0;
    root->date = time(NULL);
    root->content =NULL;
    root->previous = NULL;
    root->parent = NULL;
    root->next = NULL;
    root->child = NULL;

    node *currentFolder = root;

    char *path = (char *) malloc(sizeof(char)*2);
    strcpy(path, "/");

    while (1) {

        printf("> ");
        char *command = getString();

        if (strncmp(command, "mkdir", 5) == 0) {
            mkdir(currentFolder, command);
        } else if (strncmp(command, "touch", 5) == 0) {
            touch(currentFolder, command);
        } else if (strcmp(command, "ls") == 0) {
            ls(currentFolder);
        } else if (strcmp(command, "lsrecursive") == 0) {
            lsrecursive(currentFolder, 0);
        } else if (strncmp(command, "edit", 4) == 0 ) {
            edit(currentFolder, command);
        } else if (strcmp(command, "pwd") == 0) {
            pwd(path);
        } else if (strcmp(command, "cdup") == 0) {
            currentFolder = cdup(currentFolder, &path);
        } else if (strncmp(command, "cd", 2) == 0){
            currentFolder = cd(currentFolder, command, &path);
        } else if (strncmp(command, "rm", 2) == 0) {
            rm(currentFolder, command);
        } else if (strncmp(command, "mov", 3) == 0) {
            mov(currentFolder, command);
        } else if (strcmp(command, "countFiles") == 0) {
            printf("Total files: %d\n", countFiles(root));
        } else if (strncmp(command, "save", 4) == 0) {
            char* filename = strtok(command + 5, " ");
            if (filename) {
                FILE* file = fopen(filename, "w");
                if (file) {
                    saveDirectory(root, file);
                    fclose(file);
                    printf("Directory structure saved to %s\n", filename);
                } else {
                    printf("Error: Could not open file for saving.\n");
                }
            }
        } else if (strncmp(command, "load", 4) == 0) {
            char* filename = strtok(command + 5, " ");
            if (filename) {
                FILE* file = fopen(filename, "r");
                if (file) {
                    freeNode(root); // Free the current directory tree
                    root = (node*)malloc(sizeof(node));
                    root->type = Folder;
                    root->name = strdup("/");
                    root->child = loadDirectory(file, root);
                    fclose(file);
                    currentFolder = root;
                    printf("Directory structure loaded from %s\n", filename);
                } else {
                    printf("Error: Could not open file for loading.\n");
                }
            }
        } else if (strncmp(command, "merge", 5) == 0) {
            char* srcName = strtok(command + 6, " ");
            char* destName = strtok(NULL, " ");
            if (srcName && destName) {
                node* srcFolder = getNode(currentFolder, srcName, Folder);
                node* destFolder = getNode(currentFolder, destName, Folder);
                if (srcFolder && destFolder) {
                    mergeDirectories(destFolder, srcFolder);
                } else {
                    printf("Error: One or both directories not found.\n");
                }
            }
        } else if (strncmp(command, "symlink", 7) == 0) {
            char* target = strtok(command + 8, " ");
            char* linkName = strtok(NULL, " ");
            if (target && linkName) {
                createSymlink(currentFolder, target, linkName);
            }
        } else if (strncmp(command, "sortBy", 6) == 0) {
            char* criterion = strtok(command + 7, " ");
            if (criterion && (strcmp(criterion, "name") == 0 || strcmp(criterion, "date") == 0)) {
                sortDirectory(currentFolder, criterion);
            } else {
                printf("Error: Sort criterion must be 'name' or 'date'.\n");
            }
        } else if (strncmp(command, "compress", 8) == 0) {
            char* filename = strtok(command + 9, " ");
            // if (filename) {
            //     compressDirectory(root, filename);
            // }
        } else if (strncmp(command, "decompress", 10) == 0) {
            char* filename = strtok(command + 11, " ");
            // if (filename) {
            //     node* decompressedRoot = decompressDirectory(filename);
            //     if (decompressedRoot) {
            //         freeNode(root);
            //         root = decompressedRoot;
            //         currentFolder = root;
            //     }
            // }
        } else if (strncmp(command, "rename", 6) == 0) {
            char* oldName = strtok(command + 7, " ");
            char* newName = strtok(NULL, " ");
            if (oldName && newName) {
                node* targetNode = NULL; // Find target node logic
                if (targetNode) renameNode(targetNode, newName);
            }
        } else if (strncmp(command, "fullpath", 8) == 0) {
            displayFullPath(currentFolder);
            printf("\n");
        } else if (strcmp(command, "exit") == 0){
            free(command);
            freeNode(root);
            free(path);
            break;
        } else {
            printf("Unknown command: %s\n", command);
        }

        free(command);
    }

    return 0;
}

//==11180== HEAP SUMMARY:
//==11180==     in use at exit: 0 bytes in 0 blocks
//==11180==   total heap usage: 182 allocs, 182 frees, 10,723 bytes allocated
//==11180==
//==11180== All heap blocks were freed -- no leaks are possible
//==11180==
//==11180== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
//==11180== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
