#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define NOTHING -1
#define MAX_LEN 1024
/**
 * A new defined linked-list structure used for storing information about file descriptor
*/
typedef struct nodestruct{
    pid_t pid; // specific pid
    int fd; // FD number
    int fd_cnt; // count of a specific fd_cnt
    char name[MAX_LEN]; // name of process
    ino_t inode; //inode
    struct nodestruct* next; //pointer to 
}Node;

/**
 * @brief Creates a new Node instance and initializes it with provided parameters.
 * 
 * Allocates memory for a new Node structure and sets its properties based on the passed parameters.
 * Initializes the 'next' pointer to NULL, indicating the end of the list. If memory allocation fails, returns NULL.
 *
 * @param pid The process ID to associate with the new node.
 * @param fd The file descriptor to store in the node.
 * @param fd_cnt The count of file descriptors for the process, used in threshold logic.
 * @param name A string representing the name associated with the node, typically a file or socket name.
 * @param inode The inode number associated with the name.
 * @return Node* A pointer to the newly created Node instance, or NULL if memory allocation fails.
 */
Node *newNode(pid_t pid, int fd, int fd_cnt, char *name, ino_t inode);

/**
 * @brief Inserts a new Node into the linked list.
 * 
 * Creates a new Node with the provided parameters and appends it to the end of the linked list.
 * If the list is empty (indicated by a NULL head pointer), the new node becomes the head of the list.
 *
 * @param node Double pointer to the head of the linked list.
 * @param pid Process ID for the new node.
 * @param fd File descriptor for the new node.
 * @param fd_cnt Count of file descriptors for the process.
 * @param name Name associated with the file descriptor.
 * @param inode Inode number associated with the name.
 * @return void
 */
void insert(Node **node, pid_t pid, int fd, int fd_cnt, char *name, ino_t inode);

/**
 * @brief Checks if a string represents an integer value.
 * 
 * Evaluates each character of the string to determine if it is a digit (0-9).
 * The function returns false if the string is empty, contains non-digit characters, or only consists of a sign (+/-) without digits.
 *
 * @param str The string to evaluate.
 * @return bool True if the string represents an integer, otherwise false.
 */
bool isInteger(const char *str);

/**
 * @brief Frees the memory allocated for the linked list.
 * 
 * Iterates through the linked list, freeing each node's allocated memory to prevent memory leaks.
 * This function should be called once the linked list is no longer needed.
 *
 * @param node Pointer to the head of the linked list to be freed.
 * @return void
 */
void freememory(Node *node);

/**
 * @brief Displays a composite view of the linked list to the console.
 * 
 * Iterates through the linked list, printing each node's properties (index, PID, FD, name, inode) in a tabular format.
 * This function is intended for a comprehensive overview of process file descriptor usage.
 *
 * @param node Pointer to the head of the linked list.
 * @return void
 */
void composite_print(Node *node);

/**
 * @brief Prints a simplified view of the linked list, focusing on process IDs and their file descriptors.
 * 
 * Iterates through the linked list, displaying only the process ID and file descriptor for each node.
 * This view is useful for identifying open file descriptors on a per-process basis.
 *
 * @param node Pointer to the head of the linked list.
 * @return void
 */
void per_process_print(Node *node);

/**
 * @brief Prints a system-wide overview of file descriptor usage, including process IDs, file descriptors, and associated names.
 * 
 * Iterates through the linked list, printing the process ID, file descriptor, and name for each node. This function provides
 * a broad perspective on file descriptor usage across the system.
 *
 * @param node Pointer to the head of the linked list.
 * @return void
 */
void system_wide_print(Node *node);

/**
 * @brief Displays file descriptors and their associated inode numbers from the linked list.
 * 
 * Iterates through the linked list, printing the file descriptor and inode number for each node.
 * This function is aimed at inspecting the inode usage of file descriptors.
 *
 * @param node Pointer to the head of the linked list.
 * @return void
 */
void vnodes_print(Node *node);

/**
 * @brief Prints the process IDs and their file descriptor counts that exceed a specified threshold.
 * 
 * This function iterates through a linked list of processes, each represented as a node containing
 * a process ID and the count of file descriptors associated with that process. It is designed to 
 * highlight "offending" processes that have a number of open file descriptors exceeding a predetermined 
 * threshold. The output is formatted to list each qualifying process ID alongside its file descriptor count, 
 * separated by commas.
 * 
 * The function begins by printing a header "## Offending processes:" to denote the start of the list. 
 * For each node in the list, it prints the process ID followed by the count of file descriptors in 
 * parentheses. If there is more than one node in the list, each node's information is separated from the next 
 * with a comma. The list is terminated with a newline character.
 *
 * @param node Pointer to the head of the linked list of nodes, each representing a process with its 
 *             file descriptor count.
 * @return void
 */
void threshold_print(Node *node);

/**
 * @brief Retrieves the inode number of a symbolic link.
 * 
 * Uses the stat system call to fetch the inode number of the file or symbolic link specified by pathLink.
 * Returns UINTMAX_MAX if the operation fails, indicating an error or non-existent file.
 *
 * @param pathLink The path to the symbolic link or file.
 * @return ino_t The inode number of the symbolic link, or UINTMAX_MAX on failure.
 */
ino_t get_symlink_inode(char *pathLink);

/**
 * @brief Fetches the UID of a process given its PID.
 * 
 * Opens and reads the /proc/[pid]/status file to extract the UID of the process.
 * Returns -1 if the file cannot be opened or the UID cannot be found.
 *
 * @param id The process ID of the target process.
 * @return int The UID of the process, or -1 on failure.
 */
int processuid(pid_t id);

/**
 * @brief Processes file descriptors for a given process ID and adds them to the linked list.
 * 
 * Opens the /proc/[pid]/fd directory to enumerate all open file descriptors for the process.
 * For each file descriptor, retrieves the target of the symbolic link (if applicable) and the inode number, then inserts a new node into the linked list.
 * Returns the count of processed file descriptors or -1 if the directory cannot be opened.
 *
 * @param node Double pointer to the head of the linked list.
 * @param pid The process ID whose file descriptors are to be processed.
 * @return int The count of processed file descriptors, or -1 on failure.
 */
int handleFDs(Node **node, pid_t pid) ;

/**
 * @brief Main function to handle processing of processes and their file descriptors based on user-specified criteria.
 * 
 * Opens the /proc directory to enumerate all processes. Filters processes based on the current user's UID and optional PID.
 * For each process, processes its open file descriptors and applies threshold logic if specified. Results are stored in linked lists for printing.
 *
 * @param node Double pointer to the head of the main linked list for storing process and file descriptor information.
 * @param nodethreshold Double pointer to the head of a separate linked list for storing processes exceeding specified thresholds.
 * @param threshold_num The threshold number of file descriptors to trigger separate listing.
 * @param typePID Optional process ID to filter the processing. Use -1 to process all processes.
 * @return void
 */
void handle(Node **node, Node **nodethreshold, int threshold_num, int typePID);

/**
 * @brief Writes the contents of the linked list to a text file named "composite.txt".
 * 
 * Iterates through the linked list starting from the node passed as a parameter. For each node, 
 * it writes the node's index, process ID, file descriptor, name, and inode number in a tab-separated format to the file.
 * This function creates or overwrites the "composite.txt" file in the current working directory.
 * If the file cannot be opened, it prints an error message to stderr and returns immediately.
 *
 * @param node Pointer to the head of the linked list containing the data to be written to the file.
 * @return void
 */
void output_txt(Node *node);

/**
 * @brief Writes the contents of the linked list to a text file named "composite.txt".
 * 
 * Iterates through the linked list starting from the node passed as a parameter. For each node, 
 * it writes the node's index, process ID, file descriptor, name, and inode number in a tab-separated format to the file.
 * This function creates or overwrites the "composite.txt" file in the current working directory.
 * If the file cannot be opened, it prints an error message to stderr and returns immediately.
 *
 * @param node Pointer to the head of the linked list containing the data to be written to the file.
 * @return void
 */
void output_bin(Node *node);

#endif