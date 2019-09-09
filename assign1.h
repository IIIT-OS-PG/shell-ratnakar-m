#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include<tuple>
#include <unistd.h>
#include <unordered_map> 
#include <vector>
#include <string>
#include <pwd.h>
#include <fstream>
#include <regex>
#include <setjmp.h> 
#include <list>
#define COMMAND_BUFFER 2048


using namespace std;

char* getcommandline(string prompt);
string trim(const string& s);
pair<char**,int> split_line(char *line, char* delim);
void print_command(char** tokens, int l);
char * getcommandline2(string prompt);
struct retl getline();
unordered_map<string, string> get_user_details();
unordered_map<string, uid_t> get_user_id();
unordered_map<string, string> get_user_name();
//unordered_map<string, string> parse_initrc_file(string rcfile_path);
pair<unordered_map<string, string>,unordered_map<string, string>> parse_initrc_file(string rcfile_path);
char** parse_default_apps_props(char* default_app_val);
char* parse_for_default_apps(unordered_map<string, string> environ);
char * extract_extension(char* file_path);
char* get_default_app(char* extn, unordered_map<string, string> environ);
unordered_map<string, string> get_environ_vars();
unordered_map<string, string> get_defaults();
char* remove_square_brackets(char* input);
bool contains_alias(char * line);
string trim(const string& s);


pair<string, string> get_alias(char* line);

struct pcb_less *get_pcb_less();
int proc_related_id_init(struct pcb_less *ptr);

int exec_in_fork (int in, int out, struct command_object *cmd);
int execute (int n, struct command_object *cmd);
int parseCommand(char commandline[], struct command_object command_objects[]);
int  parse(char *line, char **argv);
int parse2(char input[],char *arr[]);

struct command_object {
	char* cmd;
	char** args;
	char* input= {"\0"} ;
	bool has_input = false;
	char* output= {"\0"};
	bool has_output = false;
	char* fcaas;
	int in;
	int out;
	bool is_bg = false;
};



struct pcb_less {
    pid_t pid;
    pid_t pgid;
    pid_t fgid;
};

struct retl
{
    char line[COMMAND_BUFFER];
    int len;
    bool tab;
    bool eof;
};

struct builtin {
    char *command;
    int (*function) (char **);
};

struct builtin *init_builtins();
struct builtin *builtin_exists(struct builtin *, char *);
void builtin_exec(struct builtin *, char **);
int cd(char **);
int exit(char **);
void *emalloc(int size);

void throw_error(int result, char *msg);
extern sigjmp_buf jmpbuf;
#define CODE_SIGINT 1002
int register_signals_for_myshell();
int register_signals_for_child();
char* substitute_tilde_envvars(char* command, string homedir, unordered_map<string, string> environment);
char* substitute_envvars(char* command, unordered_map<string, string> environment);
char* substitute_tilde(char* commandline, unordered_map<string, string> environment);
void add_history(char* commandline, vector<char*> &history, int hist_size);
void print_history(vector<char*> q);
char* clone(char* orig);
void print_aliases(unordered_map<string, string> environ);
void print_env(unordered_map<string, string> environ);
