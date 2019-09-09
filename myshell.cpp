#include <assign1.h>



int main(){

    struct pcb_less *parent_pcb;
    struct pcb_less *child_pcb;
    pid_t child_pid;
    int statloc;
    unordered_map<string, string> aliases;
    struct builtin *builtin_cmds;
    struct builtin *builtin_cmd;
    builtin_cmds = init_builtins();
    vector<char*> history;
    int HISTSIZE;

	//1. get user details
    unordered_map<string, string> curr_user_details= get_user_details();
    //cout << curr_user_details["uid"] << " : " << curr_user_details["uname"] << " : " << curr_user_details["homedir"] << endl;
    string username = curr_user_details["uname"];
    string homedir = curr_user_details["homedir"];
    string uidstr = curr_user_details["uid"];
    int uid = atoi(uidstr.c_str());
    
    //2. load myrc & get aliases
    string rcfile_path = homedir + "/" + ".myrc";
    pair<unordered_map<string, string>,unordered_map<string, string>> env_aliases = parse_initrc_file(rcfile_path);
    aliases=env_aliases.first;


    //3. read Environment variables
    unordered_map<string, string> environ = get_environ_vars();

    //4. merge environ and myrc
    environ.insert(env_aliases.second.begin(), env_aliases.second.end());
    unordered_map<string, string> defaults;
    defaults = get_defaults();

    parse_for_default_apps(environ);

    string HISTSIZE_str;
    if(environ.find("HISTSIZE") != environ.end()){
        HISTSIZE_str=environ["HISTSIZE"];
    }
    else{
        HISTSIZE_str=defaults["HISTSIZE"];
    }
    HISTSIZE=atoi(HISTSIZE_str.c_str());
    
    //cout << "HISTSIZE = " << HISTSIZE;
    //print_env(environ);
    


	while (1) {
		parent_pcb = get_pcb_less();
		//register_signals_for_myshell();
    	proc_related_id_init(parent_pcb);
       
        if (sigsetjmp(jmpbuf, 1) == CODE_SIGINT) {
            printf("\n");
            continue;
        }

        //5. prompt change for non-root and root user
		string prompt = username;
        if(uid==0)
            prompt=prompt+"#";
        else
            prompt=prompt+"$";

		cout << prompt;
		//char*  commandline = getline(prompt);
		struct retl ret = getline();
		//cout  << "got line" << endl;
		if (ret.eof == true) {
		    //cout << "exiting" << endl;  
            printf("\n");
            exit(0);
        }

        char *commandline = ret.line;

        if(strlen(commandline) == 0)
            continue;



		string com = string(commandline);
        char* comcp = clone(commandline);

        add_history(comcp,history,10);
        char *argv[COMMAND_BUFFER];
        char* cp2 = clone(commandline);
        int argc = parse2(cp2,argv);

        //add alias command
        //cout << "command = " << commandline << endl;
        if(string("$$").compare(string(commandline)) == 0){
            cout << getpid() << endl;
            continue;
        }
        if(string("$?").compare(string(commandline)) == 0){
            cout << statloc << endl;
            continue;
        }

        commandline = substitute_tilde(commandline,environ);
        commandline = substitute_envvars(commandline,environ);

        //6. Alias
        if(contains_alias(commandline))
        {
            //cout << "contains alias: " << endl;
            pair<string, string> p = get_alias(commandline);
            aliases[trim(p.first)] = p.second;
            continue;
        }
        //is alias
        if(aliases.find(com) != aliases.end())
        {
           commandline = (char *)(aliases[com]).c_str(); 
           char *argv[COMMAND_BUFFER];
           char* cp3 = clone(commandline);
           argc = parse2(cp2,argv);
        }
        else{
            //cout << "not in alias map" <<endl;
        }

        //7. default app
        if(strcmp(argv[0], "xdg-open") == 0){
            //char* extn = extract_extension(argv[1]);
            if(environ.find("MEDIA") != environ.end()){
                char* default_app = parse_for_default_apps(environ);
                argv[0] = default_app;
                string s(default_app); 
                s.append(" " +string(argv[1])); 
                commandline = (char*)s.c_str();
            }
            
        }

        //8. Builtin commands
        builtin_cmd = builtin_exists(builtin_cmds, argv[0]);
        if (builtin_cmd != NULL) { 
            //cout << "is a builtin" << endl;
            builtin_exec(builtin_cmd, argv);
            continue;
        }

        //9. history
        if(string("history").compare(string(commandline)) == 0)
        {
            print_history(history);
            continue;
        }

		child_pid = fork();
		if (child_pid == 0) {
			child_pcb = get_pcb_less();
            register_signals_for_child();

            struct command_object command_objects[COMMAND_BUFFER];
            int commandsCount = parseCommand(commandline, command_objects);
            //10. Pipes and Redirection
            execute (commandsCount, command_objects);

			throw_error(
                    tcsetpgrp(STDIN_FILENO, child_pcb->pgid),
                    "tcsetpgrp failed"
                );
            

            throw_error(
                execvp(argv[0], argv),
                argv[0]
            );
		} 
		else 
		{
			child_pcb = get_pcb_less();
            setpgid(child_pcb->pid, child_pcb->pid);
            child_pcb->pgid = getpgid(child_pcb->pid);

            throw_error(
                   tcsetpgrp(STDIN_FILENO, child_pcb->pgid),
                   "tcsetpgrp failed"
                );

            waitpid(child_pid, &statloc, WUNTRACED);
                tcsetpgrp(STDIN_FILENO, parent_pcb->pgid);

		}
	}


}