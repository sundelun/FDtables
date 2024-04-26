#include "header.h"


Node *newNode(pid_t pid, int fd, int fd_cnt, char *name, ino_t inode){
    Node *newcreate = NULL;
    newcreate = (Node*)calloc(1,sizeof(Node));
    if (newcreate == NULL){
        return NULL;
    }
    newcreate->pid = pid;
    newcreate->fd = fd;
    newcreate->fd_cnt = fd_cnt;
    strcpy(newcreate->name, name);
    newcreate->inode = inode;
    newcreate->next = NULL;
    return newcreate;
}

void insert(Node **node, pid_t pid, int fd, int fd_cnt, char *name, ino_t inode){
    Node *newone = NULL;
    newone = newNode(pid, fd, fd_cnt, name, inode);
    if (*node == NULL){
        *node = newone;
        return;
    }
    Node *last = *node;
    while (last->next != NULL){
        last = last->next;
    }
    last->next = newone;
}
bool isInteger(const char *str) {
    // Check if the string is empty after a sign or if it was empty to start with
    if (!*str) {
        return false;
    }

    // Check each character to ensure it's a digit
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false; // Found a non-digit character
        }
        str++; // Move to the next character
    }
    return true; // Only digit characters were found
}


ino_t get_symlink_inode(char *pathLink) {
    struct stat linkInfo;
    if (stat(pathLink, &linkInfo) == -1) {
        //perror("lstat");
        return UINTMAX_MAX; 
    }
    return linkInfo.st_ino; 
}

int processuid(pid_t id){
    char filePath[MAX_LEN];
    char line[MAX_LEN];
    snprintf(filePath, sizeof(filePath), "/proc/%d/status", id);

    FILE *file = fopen(filePath, "r");
    if (!file) {
        return -1; // Unable to open the file
    }

    uid_t uid = (uid_t)-1; // Initialize uid with an invalid value
    while (fgets(line, MAX_LEN, file)) { // Read each line in the file
        if (strncmp(line, "Uid:", 4) == 0) { // Check if the line contains UID information
            sscanf(line + 4, "%d", &uid); // Use sscanf to extract uid directly
            break;
        }
    }

    fclose(file); // Always close the file descriptor
    return uid; // Return the found UID or -1 if not found
}

int handleFDs(Node **node, pid_t pid) {
    char fdDirPath[MAX_LEN];
    int cnt = 0;

    int access = snprintf(NULL, 0, "/proc/%d/fd", pid);
    if(access < 0) return -1; 

    // Generate path for the /proc/[pid]/fd directory
    snprintf(fdDirPath, sizeof(fdDirPath), "/proc/%d/fd", pid);
    DIR *directory = opendir(fdDirPath);
    if (!directory) {
        //perror("Failed to open FD directory");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        int access2 = snprintf(NULL, 0, "/proc/%d/fd/%s", pid, entry->d_name);
        if (access2 < 0) continue;
        char filePath[1280];
        snprintf(filePath, sizeof(filePath), "%s/%s", fdDirPath, entry->d_name);

        ino_t inode = get_symlink_inode(filePath);
        if (inode == UINTMAX_MAX) {
            continue;
        }
        char linkTarget[MAX_LEN] = {'\0'};
        if (readlink(filePath, linkTarget, sizeof(linkTarget) - 1) == -1) {
            perror("Failed to read link target");
            continue;
        }

        // Assuming NOTHING is a defined constant and atoi is appropriate here
        insert(node, pid, atoi(entry->d_name), NOTHING, linkTarget, inode);
        cnt++;
    }
    closedir(directory);
    return cnt;
}

void handle(Node **node, Node **nodethreshold, int threshold_num, int typePID){
    DIR *dir;
    dir = opendir("/proc");
    struct dirent *entry;
    // get current userid
    uid_t currentuserid = getuid();
    if (dir == NULL){
        perror("opendir failed");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        // skip non-digit, parent and current directory
        if (!isdigit(*(entry->d_name)) || strcmp(entry->d_name,"..") == 0 || strcmp(entry->d_name,".") == 0){
            continue;
        }
        
        int pid = atoi(entry->d_name);
        if (typePID > 0 && pid != typePID){
            continue;
        }
        int process_id = processuid(pid); 
        if (process_id == -1){
            continue;
        }
        if (process_id == currentuserid){
            int cnt = handleFDs(node, pid);
            if (threshold_num != -1 && cnt > threshold_num){
                insert(nodethreshold, pid, NOTHING, cnt, "" , 0);
            }
        }
        

    }
    closedir(dir);
}
void freememory(Node *node){
    Node *tmp;
    while (node != NULL){
        tmp = node;
        node = node->next;
        free(tmp);
    }
}
int main(int argc, char **argv){
    bool composite, per_process, system_wide, vnodes, threshold, txt, bin;
    composite = per_process = system_wide = vnodes = threshold = bin = txt = false;
    int thresholdnum = -1;
    int typeinPID = -1;
    if (argc==1){
        composite = true;
    }
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--per-process") == 0){
            per_process = true;
        }
        else if (strcmp(argv[i], "--systemWide") == 0){
            system_wide = true;
        }
        else if (strcmp(argv[i], "--Vnodes") == 0){
            vnodes = true;
        }
        else if (strcmp(argv[i], "--composite") == 0){
            composite = true;
        }
        else if (strcmp(argv[i], "--output_TXT") == 0){
            txt = true;
        }
        else if (strcmp(argv[i], "--output_binary") == 0){
            bin = true;
        }
        else if (strncmp(argv[i], "--threshold=", 12) == 0){
            char *thresholdStr = argv[i] + 12;
            long val = strtol(thresholdStr, NULL, 10);
            thresholdnum = (int)val;
            threshold = true;
        }
        else if (isInteger(argv[i])){
            typeinPID = atoi(argv[i]);
            if (argc == 2){
                composite = true;
            }
        }
        else{
            perror("Invalid argument\n");
            return 1;
        }
    }
    Node* head = NULL;
    Node *head_threshold = NULL;
    handle(&head, &head_threshold, thresholdnum, typeinPID);
    //if (composite) printf("iloveuywt");
    if (typeinPID != -1) printf(">>> Target PID: %d\n", typeinPID);
    if (composite) composite_print(head);
    if (per_process) per_process_print(head);
    if (system_wide) system_wide_print(head);
    if (vnodes) vnodes_print(head);
    if (threshold) threshold_print(head_threshold);
    if (txt) output_txt(head);
    if (bin) output_bin(head);
    freememory(head);
    freememory(head_threshold);
    return 0;
}