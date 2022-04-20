#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define MODE_R 'r'
#define MODE_W 'w'
#define MODE_X 'x'

#define MODE_B 'b'
#define MODE_C 'c'
#define MODE_D 'd'

#define L_BRACKET '['
#define R_BRACKET ']'

#define MIDDLE_LEVEL "├── "
#define BOTTOM_LEVEL "└── "
#define LEFT_LEVEL "│   "
#define EMPTY_LEVEL "   "

#define EXIT_FAILURE 1
#define DOT_STR "."
#define DOT_CHR '.'

#define END_LINE '\0'
#define INIT_MODE_TEXT "----------"
#define MODE_LEN 10
#define MALLOC_MSG "Cannot perfom malloc"
#define PRINT_DET_FORMAT "%c%s %-8s%-10s%8ld%c"


typedef struct dir {
  char *name;
  char *details;
  int is_dir;
  struct dir *next;
} dir_entry;

void malloc_error() {
    perror(MALLOC_MSG);
    exit(EXIT_FAILURE);
}

static int free_uid = 0;
char * get_uid_name(uid_t id) {
    struct passwd * getpwuid(), * pw_ptr;
    char * result;
    if((pw_ptr = getpwuid(id)) == NULL) {
        free_uid = 1;
        char * num_str = (char*)malloc(MODE_LEN);
        if(!num_str) malloc_error();
        sprintf(num_str, "%d", id);
        result = num_str;
    } else {
        free_uid = 0;
        result = pw_ptr->pw_name;
    }
    return result;
}

static int free_gid = 0;
char* get_gid_name(gid_t id) {
    struct group *getggrid(), *grp_ptr;
    char * result;
    if((grp_ptr = getgrgid(id)) == NULL) {
        free_gid = 1;
        char* num_str = (char*)malloc(MODE_LEN);
        if(!num_str) malloc_error();
        sprintf(num_str, "%d", id);
        result = num_str;
    } else {
        free_gid = 0;
        result = grp_ptr->gr_name;
    }
    return result;
}

void get_modes(int mode, char * str) {
    strcpy(str, INIT_MODE_TEXT);
    if(S_ISDIR(mode)) str[0]= MODE_B;
    if(S_ISCHR(mode)) str[0]= MODE_C;
    if(S_ISBLK(mode)) str[0]= MODE_B;
    if(mode & S_IRUSR) str[1] = MODE_R;
    if(mode & S_IWUSR) str[2] = MODE_W;
    if(mode & S_IXUSR) str[3] = MODE_X;
    if(mode & S_IRGRP) str[4] = MODE_R;
    if(mode & S_IWGRP) str[5] = MODE_W;
    if(mode & S_IXGRP) str[6] = MODE_X;
    if(mode & S_IROTH) str[7] = MODE_R;
    if(mode & S_IWOTH) str[8] = MODE_W;
    if(mode & S_IXOTH) str[9] = MODE_X;
    str[MODE_LEN] = END_LINE;
}

char * create_name(dir_entry * det, char * path) {
    char * name = (char*)malloc(strlen(path) + strlen(det->name) + 2);
    if(!name) malloc_error();
    return name;
}

void get_dir_details(dir_entry * det , char * path){
    char *name = det->name; // getting name
    if(strcmp(DOT_STR, path) != 0 ) {
        name = create_name(det, path);
        sprintf(name, "%s/%s", path, det->name);
    }

    char *uid, *gid;
    struct stat info;
    if(stat(name, &info) == -1) {
        perror(name);
        exit(-1);
    }
    
    struct stat * info_p = &info;
    char modestr[MODE_LEN + 1];
    get_modes(info_p->st_mode, modestr);

    // get len of info pointer
    long len = (long) info_p->st_size;

    // get uid and gid
    uid = get_uid_name(info_p->st_uid);
    gid = get_gid_name(info_p->st_gid);

    // create details
    det->details = malloc(sizeof(char) + strlen(modestr) + strlen(uid) + strlen(gid) + sizeof(long) + sizeof(char) + 6);
    if(!det->details) malloc_error();
    sprintf(det->details, PRINT_DET_FORMAT, L_BRACKET, modestr, uid, gid, len, R_BRACKET);
 
    // free details
    if(free_uid) free(uid);
    if(free_gid) free(gid);
    if(strcmp(DOT_STR, path) != 0) free(name);
}

void go_throw_dirs(char *path, char * history, size_t * num_of_dirs, size_t * num_of_files) {
    (*num_of_dirs)++;
    DIR * dir;
    size_t counter = 0;

    dir = opendir(path); // getting the current directory
    if(!dir) {
        perror("[-] cannot open dir");
        exit(EXIT_FAILURE);
    }
    struct dirent * s_file; // file struct
    dir_entry * curr, * head = NULL, *ptr;
 
    while((s_file = readdir(dir)) != NULL){
        if(s_file->d_name[0] == DOT_CHR) {
            continue;
        }
        curr = (dir_entry*)malloc(sizeof(dir_entry));
        if(!curr) malloc_error();

        curr->name = (char*)malloc(strlen(s_file->d_name) + 1);
        if(!curr->name) malloc_error();

        strcpy(curr->name, s_file->d_name); // contains '\0'
        curr->is_dir = 0;
        if(s_file->d_type == DT_DIR) curr->is_dir++;

        curr->next = NULL;
        get_dir_details(curr, path);

        if(head) {
            ptr->next = curr;
            ptr = ptr->next;
        } else head = ptr = curr;
        counter++;
    }

    if(closedir(dir)) {
        perror("Cannot close directory");
        exit(EXIT_FAILURE);
    }

    char * same_level, * in_level;
    ptr = head;    
    for(size_t i = 0; i < counter; i++) {
        if(i < counter - 1) {
            same_level = MIDDLE_LEVEL;
            in_level = LEFT_LEVEL;
        } else {
            same_level = BOTTOM_LEVEL;
            in_level = EMPTY_LEVEL;
        }
        
        printf("%s%s%s  %s\n", history, same_level ,ptr->details, ptr->name);
        if(ptr->is_dir) {
            char *new_history = (char*) malloc(strlen(history) + strlen(in_level) + 2);
            if(!new_history) malloc_error();
            sprintf(new_history,"%s%s", history, in_level);
            char* new_path = (char*)malloc(strlen(path) + strlen(ptr->name)+2);
            if(!new_path) malloc_error();
                sprintf(new_path,"%s/%s",path, ptr->name);
                go_throw_dirs(new_path,new_history, num_of_dirs, num_of_files);
                free(new_history);
                free(new_path);
        } else {
            (*num_of_files)++;
        }
        dir_entry * entry_to_free = ptr;
        ptr = ptr->next; // go to next pointer

        // free last pointer
        free(entry_to_free->name);
        free(entry_to_free->details);
        free(entry_to_free);
    }
}

// main function
int main(int argc, char ** argv) {
    size_t num_of_dirs = 0, num_of_files = 0;
    char * dir;
    if(argc > 1) {
        dir = argv[1]; // dir as argument
    } else {
        dir = DOT_STR; // current dir
    }
    printf("%s\n", dir);
    go_throw_dirs(dir, "", &num_of_dirs, &num_of_files);
    printf("\n%ld directories, %ld files\n", num_of_dirs - 1, num_of_files);
    return 0;
}
