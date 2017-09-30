#include <bits/stdc++.h>
#include <unistd.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <csignal>
#include <sys/wait.h>
#include <fstream>

using namespace std;

// function initialisations
vector<string> to_vector(string);
void create_child_process(string);
void execute(vector<string>);
void list_directory(vector<string>);
vector<char*> items_in_dir(const char*);
void change_directory(vector<string>);
void make_directory(vector<string>);
void rem_directory(vector<string>);
void rem_except(vector<string>);
void try_remove(const char*, bool, bool, bool);
void history(vector<string>);
void issue(vector<string>);
void run_program(vector<string>);
void create_file(vector<string> command);
void save_command(vector<string>);
void terminate(int);
string read_file();
int is_dir(const char*);


// file to which commands history will be stored
string CMD_FILE_PATH = "/commands.txt";

// main function
int main(){
    // declarations
    string command;
    char current_dir[50];

    // welcome
    cout << "Welcome to the matrix!\n";

    // event loop
    while(true){
        // print current directory
        getcwd(current_dir, sizeof(current_dir));
        cout << "User@" << current_dir;

        // get command input
        cout << " >> ";
        getline(cin, command);

        // execute the command if not exit
        if(command == "exit") break;
        else create_child_process(command);
    }

    cout << "Exit\n";
}

void create_child_process(string command){
    int pid;

    // convert to vector of commands
    vector<string> cmd = to_vector(command);

    // creating child process
    pid = fork();

    // child process
    if(pid == 0){
        // execute the command
        execute(cmd);

        // exit child process
        exit(1);

    }else{
        // change directory of parent process
        if(cmd[0] == "cd") change_directory(cmd);
        else if(cmd[0] == "issue") issue(cmd);

        // wait for child process
        wait(NULL);
    }
}

vector<string> to_vector(string command){
    vector<string> cmd;
    // generate string stream for command string
    istringstream ss(command);

    do{
        // get substring
        string subs;
        ss >> subs;

        // append to vector
        if(subs != "") cmd.push_back(subs);
    }while(ss);

    // return vector
    return cmd;
}

// execute separate commands for each command line
void execute(vector<string> command){
    if(command[0] == "ls") list_directory(command);
    else if(command[0] == "mkdir") make_directory(command);
    else if(command[0] == "rm") rem_directory(command);
    else if(command[0] == "rmexcept") rem_except(command);
    else if(command[0] == "history") history(command);
    else if(command[0] == "touch") create_file(command);
    else if(command[0] != "cd" && command[0]!= "issue") run_program(command);
}

void change_directory(vector<string> command){
    // check if too many arguments
    if(command.size()>2){
        cout << "Too many arguments.\n";
        return;
    }else if(command.size()==2){
        const char* dir;

        // getting new directory information
        dir = command[1].c_str();

        // change the directory
        if(chdir(dir) == -1){
            cout << "No such file or directory exists.\n";
        };

        // save command
        save_command(command);
    }else{
        cout << "No directory given.\n";
    }
}

void list_directory(vector<string> command){
    vector<char*> listing;

    // check if too many arguments
    if(command.size()>1){
        cout << "ls does not accept arguments.\n";
        return ;
    }

    // get directory listing
    listing = items_in_dir(".");

    for(string item:listing){
        cout << item << "\t";
    }
    cout << "\n";

    // save command
    save_command(command);
}

vector<char*> items_in_dir(const char * path){
    // initialisations
    DIR* dir;
    struct dirent *dir_item;
    vector<char*> listing;

    // open current directory
    dir = opendir(path);

    // list directory
    if(dir){
        // for self and parent directories

        // for actual files and folders
        while((dir_item = readdir(dir)) != NULL){
            if(strcmp(dir_item->d_name,".")!=0 && strcmp(dir_item->d_name,"..")!=0)
                listing.push_back(dir_item->d_name);
        }
        closedir(dir);
    }

    return listing;
}

void make_directory(vector<string> command){
    if(command.size()>2){
        cout << "Too many arguments.\n";
        return;
    }else if(command.size()==2){
        const char* dir;

        // get directory name
        dir = command[1].c_str();

        // make directory
        mkdir(dir, S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXOTH);

        // save command
        save_command(command);
    }else{
        cout << "No directory provided.\n";
    }
}

void rem_directory(vector<string> command){
    if(command.size()>1){
        const char* path;
        bool verbose = false; // -v
        bool recursive = false; // -r
        bool force = false; // -f

        // for all arguments
        for(unsigned int i=1;i<command.size();i++){
            // check if special commands used
            if(command[i] == "-r"){
                recursive = true;
                continue;
            }else if(command[i] == "-f"){
                force = true;
                continue;
            }else if(command[i] == "-v"){
                verbose = true;
                continue;
            }

            // get directory name or file path
            path = command[i].c_str();

            // remove file or directory
            try_remove(path, recursive, force, verbose);
        }

        // save command
        save_command(command);
    }
}

void rem_except(vector<string> command){
        const char* path;
        bool verbose = false; // -v
        bool recursive = false; // -r
        bool force = false; // -f

        vector<char*> dir_items = items_in_dir(".");

        // for all arguments
        for(unsigned int i=1;i<command.size();i++){
            // check if special commands used
            if(command[i] == "-r"){
                recursive = true;
                continue;
            }else if(command[i] == "-f"){
                force = true;
                continue;
            }else if(command[i] == "-v"){
                verbose = true;
                continue;
            }

            // convert to constant character
            path=command[i].c_str();

            // remove selected files
            for(unsigned int j=0;j<dir_items.size();j++){
                if(strcmp(dir_items[j],path) == 0){
                    dir_items.erase(dir_items.begin()+j);
                }
            }
        }

        for(const char* item:dir_items){
            // converting to valid type casts
            string s(item);
            path = s.c_str();

            // remove remaining paths
            try_remove(path, recursive, force, verbose);
        }

        // save command
        save_command(command);
}

void try_remove(const char* path, bool recursive, bool force, bool verbose){
    // check if the path is a directory
    if(is_dir(path)){
        if(recursive){
            vector<char*> items = items_in_dir(path);
            for(char *item : items){
                char buff[] = "aslkdfjlaksjdfkljasldkfjlkasjdflkajsdflasldf";
                strcpy(buff, path);
                try_remove(strcat(strcat((char*) buff,"/"),item), recursive, force, verbose);
            }
            rmdir(path);
        }else{
            if(!force){
                if(rmdir(path) != 0)
                    cout << "'"<< path <<"' is not an empty directory. Use -r.\n";
            }
        }
    }else{
        // if could not remove
        if(unlink(path)!=0){
            if(!force)
                cout << "No such file or directory named'"<<path<<"' exists.\n";
        }else{
            if(!force && verbose)
                cout << "File named '" << path << "' removed.\n";
        }
    }
}

void history(vector<string> command){
    string line;
    int n;
    bool n_flag = false;

    // check for too many arguments
    if(command.size()>2){
        cout << "Too many arguments.\n";
        return;
    }

    // getting n
    if(command.size()>1){
        string::size_type sz;
        n = stoi(command[1], &sz);
        n_flag = true;
    }

    // opening commands file stream
    ifstream cmd_file(CMD_FILE_PATH, ios::in);

    // reading file
    if(cmd_file.is_open()){
        // output each line till n
        while(getline(cmd_file, line)){
            cout << line << "\n";
            if(n_flag == true) if(--n <= 0) break;
        }
        // close file
        cmd_file.close();
    }
}

void issue(vector<string> command){
    if(command.size()>2){
        cout << "Too many arguments.\n";
        return;
    } else if(command.size()==2){
        string line;
        string nth_cmd;

        // getting n
        int n;
        string::size_type sz;
        n = stoi(command[1], &sz);

        // opening commands file stream
        ifstream cmd_file(CMD_FILE_PATH, ios::in);

        // reading file
        if(cmd_file.is_open()){
            while(getline(cmd_file, line)){
                nth_cmd = line;
                if(--n <= 0) break;
            }
            cmd_file.close();
        }

        // execute command
        cout << "Command Executed: " << nth_cmd << endl;
        create_child_process(nth_cmd);

    }else{
        cout << "Please provide n.\n";
    }
}

void run_program(vector<string> command){
    if(command.size()>2){
        cout << "Too many arguments.\n";
    }else if(command.size()>=1){
        const char*path;

        // getting n
        int n=-1;

        if(command.size() == 2){
            string::size_type sz;
            n = stoi(command[1], &sz);
        }

        signal(SIGALRM, terminate);
        alarm(n);

        // get directory name or file path
        path = command[0].c_str();
        execl(path, path,(char*) NULL);
    }
}

void terminate(int){
    abort();
}

void create_file(vector<string> command){
    if(command.size() > 1){
        ofstream new_file;
        for(unsigned int i = 1; i<command.size();i++){
            new_file.open(command[i], ios::app);
            new_file.close();
        }
    }else{
        cout << "No arguments provided.\n";
    }
}

void save_command(vector<string> cmd_vector){
    string command = "";
    string history;

    // converting command vector to string for storage
    for(auto cmd:cmd_vector){
        command += cmd+" ";
    }
    command += "\n";

    // read cmd_history
    history = read_file();

    // opening command file stream
    ofstream cmd_file(CMD_FILE_PATH);

    // writing to file
    if(cmd_file.is_open()){
        // appending new command to start of file
        cmd_file << command + history;
        // close file
        cmd_file.close();
    }else{
        cout << "Shell Error\nERR_CMD_FILE\n";
    }
}

string read_file(){
    string line;
    string page;

    // opening commands file stream
    ifstream cmd_file(CMD_FILE_PATH, ios::in);

    // reading file
    if(cmd_file.is_open()){
        while(getline(cmd_file, line)){
            page += line + "\n";
        }
        cmd_file.close();
    }
    return page;
}

int is_dir(const char *path){
    struct stat statbuf;
    if(stat(path, &statbuf) != 0){
        return 0;
    }
    
    return S_ISDIR(statbuf.st_mode);
}
