#include <assign1.h>


extern char **environ;
extern sigjmp_buf jmpbuf;

unordered_map<string, string> get_environ_vars(){
    unordered_map<string, string> environment;
    char **ep;
    for (ep = environ; *ep != NULL; ep++)
    {
        vector<string> v;
        string s = string(*ep);
        //cout << s << endl;
        regex reList("([^=]*)=(.*)");

        sregex_iterator iter(s.begin(), s.end(), reList);
        sregex_iterator end;
        for ( ; iter != end; ++iter)
        {
            smatch const& match = *iter;   
            //cout << match[0] << endl;
            environment[trim(match[1])]=trim(match[2]);
         }
    }

    return environment;
}

unordered_map<string, string> get_defaults(){
    unordered_map<string, string> defaults;
    defaults["HISTSIZE"]="20";
    //defaults["MEDIA"]="[vlc,/usr/bin/vlc, mp4]";
    //cout << "def size: " << defaults.size() << endl;;
    return defaults;
}


pair<unordered_map<string, string>,unordered_map<string, string>> parse_initrc_file(string rcfile_path){
    //vector <string> mysh_env; 
    unordered_map<string, string> aliases;
    unordered_map<string, string> env;
    //cout << rcfile_path << endl;
    ifstream rcfile_stream(rcfile_path);
    
    //rcfile_stream.open(rcfile_path);
    if (!rcfile_stream) {
        cout << ".myrc is not yet created in home dir"  << endl;
    }
    if (rcfile_stream.is_open())
    {
        cout << "initializing shell..." << endl;
        string line;
        while ( getline (rcfile_stream,line) )
        {
            //mysh_env.push_back(line); 
            //cout << line << endl;
             //if (regex_match (line,regex("\\b(alias)([^ ]*)")))
            if (regex_match (line.c_str(),regex("(\\s*)(alias)(.*)")))
             {
                //cout << "line contains alias\n";
                char* ch = (char *)line.c_str();
                pair<string, string> p = get_alias(ch);
                //cout << p.first << " -> " << p.second << endl;
                string ftrim = trim(p.first);
                aliases[ftrim] = p.second;
             }

            regex reList("([^=]*)=(.*)");
            //cout << "line = " << line << endl;

            sregex_iterator iter(line.begin(), line.end(), reList);
            sregex_iterator end;
            for ( ; iter != end; ++iter)
            {
                smatch const& match = *iter;   
                //cout << match[0] << endl;
                env[trim(match[1])]=trim(match[2]);
             }
             
            //cout << line << '\n';
        }
        rcfile_stream.close();
    }

    return make_pair(aliases,env);
}

bool contains_alias(char * line){
    return (regex_match (line,regex("(\\s*)(alias)(.*)")));
     
}

char* parse_for_default_apps(unordered_map<string, string> environ){
    unordered_map<string, string> default_apps;
    if(environ.find("MEDIA") != environ.end()){
        string media_val = environ["MEDIA"];
        //cout << "MEDIA VALUE: " << media_val << endl;
        char** entries = parse_default_apps_props((char*)media_val.c_str());
        return entries[1];
    }

    return "";

}

char* get_default_app(char* extn, unordered_map<string, string> environ){
    if(environ.find(extn) != environ.end()){
        return (char*)environ[extn].c_str();
    }
    else 
    {
        return "";
    }
}

char** parse_default_apps_props(char* default_app_val){
    unordered_map<string, string> m;
    char* value = remove_square_brackets(default_app_val);

      char* value_cloned = clone(value);
      char** entryArgs = (char ** ) malloc(COMMAND_BUFFER * sizeof(char * )) ;
      //char** entries =  = (char ** ) malloc(COMMAND_BUFFER * sizeof(char * )) ;

      char* token=strtok(value_cloned,",");
      int aCount = 0;
      while(token!=NULL)
      {
            token[strlen(token)]='\0';
            //cout << token << endl;
            entryArgs[aCount]=token;
            aCount++;
            token=strtok(NULL,",");
      }
      entryArgs[aCount++]=token;
      entryArgs[aCount] = NULL;
      aCount = aCount - 1;
      
      return entryArgs;

}

char * extract_extension(char* file_path){
      char* file_path_cloned = clone(file_path);
      char** cmdArgs = (char ** ) malloc(COMMAND_BUFFER * sizeof(char * )) ;

      char* token=strtok(file_path_cloned,".");
      int aCount = 0;
      while(token!=NULL)
      {
            token[strlen(token)]='\0';
            //cout << token << endl;
            cmdArgs[aCount]=token;
            aCount++;
            token=strtok(NULL,".");
      }
      cmdArgs[aCount++]=token;
      cmdArgs[aCount] = NULL;
      aCount = aCount - 1;
      //cout << "aCount = " << aCount << endl;
      //cout << "required = "<<cmdArgs[1] << endl;

      return cmdArgs[1];
}

char* remove_square_brackets(char* input){
    //char space_removed_input[COMMAND_BUFFER] = {0}; 
    //cout << "input = " << input << endl;
    char* brackets_removed_input = (char *) malloc(COMMAND_BUFFER);
    int orig_idx = 0;
    int temp_idx = 0;
    while(input[orig_idx]!='\0')
    {
        if(input[orig_idx] != '[' && input[orig_idx] != ']')
        {
            //cout << input[orig_idx];
            brackets_removed_input[temp_idx]=input[orig_idx];
            temp_idx++;
        }

        orig_idx++;
    }
    //cout << endl;
    brackets_removed_input[temp_idx] = '\0';
    //cout << "space removed: " << space_removed_input << endl;
    return brackets_removed_input;
}

char* substitute_envvars(char* command, unordered_map<string, string> environment){
    
    regex h2("\\$(.+)\\s*");

    smatch m;
    string u = string(command);
    string n;
    bool matches = false;
    while (regex_search (u,m,h2)) {
        matches = true;
        string x = m[1];
        //cout << "mapped val: " << environment[x] << endl;
        regex h3("(.*)(\\$.*\\s*)(.*)");
        string t = string("$1")+string(" ")+environment[x]+string(" ")+string("$3");
        n = regex_replace(u,h3,t);
        //cout << "new string: " << n << endl;
        u = m.suffix();
    }
    if(!matches)
        return command;
    return clone((char*)n.c_str());
}

void print_env(unordered_map<string, string> environ){
    for (auto i = environ.begin(); i != environ.end(); i++) 
        cout << i->first << "->" << i->second << endl;
}

void print_aliases(unordered_map<string, string> aliases){
    for (auto i = aliases.begin(); i != aliases.end(); i++) 
        cout << i->first << "      " << i->second << endl;
}

char* substitute_tilde(char* commandline, unordered_map<string, string> environment){
    
    //cout << "commandline BEFORE: " << commandline << endl;
    string u1 = string(commandline);
    regex h("(.*)(~)(.*)");
    bool matches = false;
    string n;
    smatch m;
    if (regex_search (u1,m,h)) {
        matches = true;
        string x = m[1];
        //cout << "x = " << x << endl;
        string t = string("$1")+string(" ")+environment["HOME"]+string(" ")+string("$3");
        n = regex_replace(u1,h,t);
        //cout << "new string: " << n << endl;
        u1 = m.suffix();
    }
    if(!matches)
    {
        return commandline;
    }
    return clone((char*)n.c_str());
}


pair<string, string> get_alias(char* line){
    char *token = strtok(line, "="); 
    vector<string> tokens; 
    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    while (token != NULL) 
    { 
        string s (token);
        regex e ("alias"); 
        string rt = regex_replace (token,e,"");
        //cout << "replaced str: " << rt <<endl;
        tokens.push_back(string(rt));
        token = strtok(NULL, "="); 
        
    } 

    return make_pair(tokens[0],tokens[1]);
}

unordered_map<string, string> get_user_details(){

    unordered_map<string, string> user_map;

    unordered_map<string, uid_t> id_map = get_user_id();
    for (auto& i: id_map) 
            user_map[i.first] = to_string((int)i.second); 
    
    unordered_map<string, string> name_map = get_user_name();
    for (auto& i: name_map) 
            user_map[i.first] = i.second; 

    return user_map;
}


unordered_map<string, uid_t> get_user_id(){

    unordered_map<string, uid_t> umap; 
    umap[string("uid")] = getuid(); 
    umap[string("euid")] = geteuid();
    return umap;
}

unordered_map<string, string> get_user_name(){

    unordered_map<string, string> umap; 
    umap[string("uname")] = (getpwuid(getuid()))->pw_name;
    umap[string("euname")] = (getpwuid(geteuid())->pw_name); 
    umap[string("homedir")] = (getpwuid(getuid()))->pw_dir;
    return umap;
}

char* getcommandline(string prompt){
    char *buffer;
    size_t bufsize = COMMAND_BUFFER;
    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    cout << prompt;
    characters = getline(&buffer,&bufsize,stdin);

    return buffer;

}

struct retl getline()
{
	struct retl ret;
	char line[COMMAND_BUFFER]={'\0'};
	int max=COMMAND_BUFFER;
	int nch = 0;
	int c;
	max = max - 1;

	while((c = getchar()) != EOF)
	{
		if(c == '\n')
			break;

        /*if(c == '\t')
        {
            cout << "Tab captured: " << endl;
        }*/

		if(nch < max)
			{
			line[nch] = c;
			nch = nch + 1;
			}
	}

	if(c == EOF && nch == 0)
	{
		ret.len = nch;
		ret.tab = false;
		ret.eof = true;

		return ret;
	}

	line[nch] = '\0';

	strcpy(ret.line, line);
	ret.len = nch;
	ret.tab = false;
	ret.eof = false;

	
	return ret;
}

char * getcommandline2(string prompt){

    cout << prompt;
    string str;
    std::getline(cin, str);
    cout << "eneterd: " << str << endl;
    return (char *)str.c_str();

}

void print_command(char** tokens, int l){
	char** tk;
	//cout << "TYPE!" << endl;
	for (tk = tokens; *tk != NULL; tk++)
    {
        string s = string(*tk);
        //cout << s << " -> " << s.length()<<endl;
    }
    
    /*for (int j = 0; j<l; j++)
    {
        cout << string(tokens[j]) << endl;
        cout << s << " -> " << s.length()<<endl;
    }*/
}


int parse2(char input[],char *arr[])
{
    char *token=strtok(input," ");
    int parse_count = 0;
    while(token!=NULL)
    {
        int len=strlen(token);
        token[len]='\0';
        arr[parse_count++]=token;
        token=strtok(NULL," ");
    }
    arr[parse_count]=token;
    parse_count++;
    arr[parse_count] = NULL;
    return (parse_count-1);
}

pair<char**,int> split_line(char *commandline, char* delim)
{
	char line[COMMAND_BUFFER];   	
    strcpy(line,commandline);

	int bufsize = COMMAND_BUFFER, position = 0;
	char **tokens = (char**) malloc(bufsize * sizeof(char*));
	char *token;
	if (!tokens) {
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, delim);
	while (token != NULL) {
		//cout << "TOKEN: " << string(token) << endl;
        token[strlen(token)]='\0';
		tokens[position] = token;
		position++;
		if (position >= bufsize) {
			bufsize += COMMAND_BUFFER;
			tokens = (char**) realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	tokens[position] = NULL;

	return make_pair(tokens,position);
}

string ltrim(const string& s) {
    return regex_replace(s, regex("^\\s+"), string(""));
}

string rtrim(const string& s) {
    return regex_replace(s, regex("\\s+$"), string(""));
}

string trim(const string& s) {
    return ltrim(rtrim(s));
}

//PROC RELATED
struct pcb_less *get_pcb_less() {
    static struct pcb_less pcb;
    pcb.pid = getpid();

    pcb.pgid = getpgid(0);
    pcb.fgid = tcgetpgrp(STDIN_FILENO);

    return &pcb;
}

int proc_related_id_init(struct pcb_less *ptr) {

    int result = setpgid(ptr->pid, ptr->pid);
    if (result < 0) {
        perror("Error setup_job_control - setpgid");
        return result;
    }

    if ((ptr->pgid = getpgid(0)) < 0) {
        perror("Error setup_job_control - getpgid");
        return ptr->pgid;
    }

    result = tcsetpgrp(STDIN_FILENO, ptr->pgid);
    if (result < 0) {
        perror("Error setup_job_control - tcsetpgrp");
        return result;
    }

    if ((ptr->fgid = tcgetpgrp(STDIN_FILENO)) < 0) {
        perror("Error setup_job_control - tcgetpgrp");
        return ptr->fgid;
    }

    return 0;
}

//SIGNALS
sigjmp_buf jmpbuf;
void sigint_handler(int signo __attribute__((unused))) {
    siglongjmp(jmpbuf, CODE_SIGINT);
}

void sigint_handler2(int signo __attribute__((unused))) {
    siglongjmp(jmpbuf, CODE_SIGINT);
}


static int setup_signal(int signal, void (*handler)(int)) {
    struct sigaction s;
    s.sa_handler = handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    return sigaction(signal, &s, NULL);
}


int register_signals_for_myshell() {

    setup_signal(SIGINT, sigint_handler);
    setup_signal(SIGTSTP, sigint_handler);
    setup_signal(SIGTTIN, SIG_IGN);
    setup_signal(SIGTTOU, SIG_IGN);
    setup_signal(SIGCHLD, SIG_IGN);
    setup_signal(SIGQUIT, sigint_handler);

    return 0;
}

int register_signals_for_child() {

    setup_signal(SIGINT, sigint_handler);
    setup_signal(SIGTSTP, sigint_handler);
    setup_signal(SIGTTIN, SIG_IGN);
    setup_signal(SIGTTOU, SIG_IGN);
    setup_signal(SIGCHLD, SIG_IGN);
    setup_signal(SIGQUIT, sigint_handler);

    return 0;
}

void throw_error(int result, char *msg) {
    if (result < 0) {
        if (msg != NULL) {
            perror(msg);
        }

        exit(1);
    }
}

int  parseCommand(char commandline[], struct command_object* command_objects){
    int commandsCount = 0;                  
    char *temp = NULL, *commands[COMMAND_BUFFER] ;
    char *token=strtok(commandline,"|");
    while(token!=NULL)
    {
        token[strlen(token)]='\0';
        commands[commandsCount]=token;
        commandsCount++;
        token=strtok(NULL,"|");
    }
    commands[commandsCount++]=token;
    commands[commandsCount]=NULL;
    commandsCount = commandsCount - 1;

    
    char* fcaas[COMMAND_BUFFER];
    for(int z = 0; z < commandsCount; z++) 
    {
        char* command_temp = (char *) malloc(COMMAND_BUFFER);
        for(int x=0; x<strlen(commands[z]); x++)
            command_temp[x]=commands[z][x];
        strcpy(command_temp, commands[z]);
        fcaas[z] = command_temp;
        //cout << "command full -> " << fcaas[z] << endl;
        //cout << "addresses -> " << &command_temp << ", " << &commands[z] << endl;
    }

    //cout << "commandsCount : " << commandsCount << endl;
    vector<int> argCounts;
    char** cds[COMMAND_BUFFER];
    int i = 0;
    for(i = 0; i < commandsCount; i++) 
    {
        //char* cmdArgs[COMMAND_BUFFER];
        char** cmdArgs = (char ** ) malloc(COMMAND_BUFFER * sizeof(char * )) ;

        int aCount = 0;
        token=strtok(commands[i]," ");
        while(token!=NULL)
        {
            token[strlen(token)]='\0';
            //cout << token << endl;
            cmdArgs[aCount]=token;
            aCount++;
            token=strtok(NULL," ");
        }
        cmdArgs[aCount++]=token;
        cmdArgs[aCount] = NULL;
        aCount = aCount - 1;
        argCounts.push_back(aCount);
        //cout << "arg count: " << aCount<< endl;

        command_objects[i].cmd=cmdArgs[0];
        command_objects[i].args=cmdArgs;
        command_objects[i].input="test";
        command_objects[i].fcaas=fcaas[i];
        cds[i] = cmdArgs;

    }


    cds[i] = NULL;

    for(int j = 0; j < commandsCount; j++) 
    {
        char** argsv = cds[j];
        char** t;
        /*for (t = argsv; *t != NULL; t++);
            cout << *t << endl;*/
    }

    for(int l = 0; l < commandsCount; l++) 
    {
        char* c = command_objects[l].cmd;
        //cout << "command (only): "<< c << endl;
        char* inp = command_objects[l].input;
        //cout << "input: "<< c << endl;
        char** ags = command_objects[l].args;
        char** r;
        /*for (r = ags; *r != NULL; r++)
            cout << *r << endl;*/
        //cout << "command (full): "<< command_objects[l].fcaas << endl;
    }

    char* input_val = command_objects[0].fcaas;
    char* output_val = command_objects[commandsCount-1].fcaas;
    bool has_input_redir = false;
    bool has_output_redir = false;
    char* first_command = {"\0"};
    char* last_command = {"\0"};
    char* input_src = {"\0"};
    char* output_dest = {"\0"};
    bool is_bg = false;
    for(int f=0;f<strlen(input_val);f++)
    {   
        if(input_val[f]=='<')
        {
            has_input_redir=true;
            token=strtok(input_val,"<");
            token[strlen(token)]='\0';
            first_command = token;
            token=strtok(NULL," ");
            token[strlen(token)]='\0';
            input_src = token;

            //cout << "first command: " << first_command << endl;
            //cout << "input source: " << input_src << "\nHas input redir: " << has_input_redir << endl;


            break;
        }
    }

    for(int f=0;f<strlen(output_val);f++)
    {
        if(output_val[f]=='&')
        {    
            is_bg=true;
            token=strtok(output_val,"&");
            token[strlen(token)]='\0';
            output_val =  token;
        }
    }

    for(int f=0;f<strlen(output_val);f++)
    {
        if(output_val[f]=='>')
        {
            has_output_redir=true;
            token=strtok(output_val,">");
            token[strlen(token)]='\0';
            last_command =  token;
            token=strtok(NULL," ");
            token[strlen(token)]='\0';
            output_dest = token;
            //cout << "last command: " << last_command << endl;
            //cout << "output dest: " << output_dest << "\nHas output redir: " << has_output_redir <<endl;
            break;
        }
    }

    int in, out, pipefd[2]; 

    if(is_bg)
        command_objects[commandsCount-1].is_bg=is_bg;

    if(has_input_redir){
        char** new_first = (char ** ) malloc(COMMAND_BUFFER * sizeof(char * )) ;

        int aCount = 0;
        token=strtok(first_command," ");
        while(token!=NULL)
        {
            token[strlen(token)]='\0';
            //cout << token << endl;
            new_first[aCount]=token;
            aCount++;
            token=strtok(NULL," ");
        }
        new_first[aCount++]=token;
        new_first[aCount] = NULL;

        cds[0]=new_first;
        command_objects[0].args=new_first;
        command_objects[0].has_input=true;
        command_objects[0].input = input_src;

        // open the input file
        in = open(input_src, O_RDONLY);
        if (in < 0)
        {
            cout << "Error opening input file" << endl;
            exit(1);
        }

        command_objects[0].in=in;

        
    }

    if(has_output_redir){
        char** new_last = (char ** ) malloc(COMMAND_BUFFER * sizeof(char * )) ;

        int aCount = 0;
        token=strtok(last_command," ");
        while(token!=NULL)
        {
            token[strlen(token)]='\0';
            //cout << token << endl;
            new_last[aCount]=token;
            aCount++;
            token=strtok(NULL," ");
        }
        new_last[aCount++]=token;
        new_last[aCount] = NULL;

        cds[commandsCount]=new_last;
        command_objects[commandsCount-1].args=new_last;
        command_objects[commandsCount-1].has_output=true;
        command_objects[0].output = output_dest;

        // open the output file
        out = open(output_dest, O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
        if (out < 0)
        {
            cout << "Error opening output file" << endl;
            exit(1);
        }

        command_objects[commandsCount-1].out=out;
    }

    return commandsCount;   
    
}

int execute (int n, struct command_object *cmd)
{
  int i;
  pid_t pid;
  int in, fd [2];

  in = 0;

  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);
      if(i==0 && cmd[i].has_input)
        dup2(cmd[i].in, 0);
      exec_in_fork (in, fd [1], cmd + i);
      close (fd [1]);
      in = fd [0];
    }

  if (in != 0)
    dup2 (in, 0);
  if(cmd[i].has_output)
    dup2(cmd[i].out, 1);

  return execvp (cmd[i].cmd, (char * const *)cmd[i].args);
}

int exec_in_fork (int in, int out, struct command_object *cmd)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execvp (cmd->cmd, (char * const *)cmd->args);
    }

  return pid;
}

struct builtin *init_builtins() {
    static struct builtin *s;
    s = (builtin*)emalloc(3 * sizeof(struct builtin));
    s[0].command = "cd";
    s[0].function = cd;
    s[1].command = "exit";
    s[1].function = exit;
    s[2].command = NULL;
    s[2].function = NULL;
    return s;
}

int  parse(char *line, char **argv)
{
    int argc = 0;
     while (*line != '\0') {      
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     
          *argv++ = line;          
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;
          argc++;             
     }
     *argv = '\0';  
     return  argc;              
}



struct builtin *builtin_exists(struct builtin *builtins_ptr, char *command) {
    while(builtins_ptr->command != NULL) {
        //cout << "builtins_ptr->command" << endl;
        //cout << "*command" << *command;
        if (strcmp(builtins_ptr->command, command) == 0) {
            return builtins_ptr;
        }

        builtins_ptr++;
    }

    return NULL;
}

void builtin_exec(struct builtin *builtin_ptr, char **command) {
    if (builtin_ptr->function(command) < 0) {
        perror(builtin_ptr->command);
    }
}

int cd(char **command) {
    return chdir(command[1]);
}

int exit(char **command) {
    exit(0);
    return 0;
}

void *emalloc(int size) {
    void *ptr = malloc(size);
    if (!ptr) {
        perror("Out of memory Error");
        exit(1);
    }

    return ptr;
}

void add_history(char* commandline, vector<char*> &history, int hist_size){
    if(history.size() < hist_size)
    {
        history.insert(history.begin(),commandline);
    }
    else
    {
        history.pop_back();
        history.insert(history.begin(),commandline);
    }
}

void print_history(vector<char*> q){
    for (int index = q.size() - 1; index >= 0; --index)
    {
        cout << q[index] << endl;
    }
}

char* clone(char* orig){
    char* cl = (char *) malloc(COMMAND_BUFFER);
    int g = 0;
    while(orig[g] != '\0')
    {
        cl[g] = orig[g];
        g++;
    }
    cl[g]='\0';

    return cl;
}


