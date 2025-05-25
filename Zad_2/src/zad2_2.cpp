#include <pthread.h>
#include <iostream>
#include <string>


#define DEBUGLOG(msg) cout << "#DEBUG : " << msg <<" #\n";


using std::cout, std::cin, std::endl, std::string;


int handle_TUI_display(int selection);
int handle_TUI_selection_input();

void* clientThread(void* thdata);
void* serverThread(void* thdata);


pthread_t server;
pthread_t* clients;
int clientsNum;
string serverLog;
string* chatHisClient;


void* serverThread(void* thdata){
    delete (int*)thdata;
        
    
    pthread_exit(NULL);
}

void* clientThread(void* thdata){
    int clientID = *(int*)thdata;
    delete (int*)thdata;
    string name = "Client " + std::to_string(clientID);
    
    
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    clientsNum = 0;
    int rt;
    if (argc < 2){
        printf(
            "\nERROR: Not enough program arguments.\n"
            "# Please provide arguments as follows : /PROGRAM_NAME/ " 
            "<Number of clients (as int)>\n");
        exit(1);
    }
    clientsNum = atoi(argv[1]);
    if (clientsNum < 1){
        printf("\n ERROR : Provided too small number of clents : %s \n", argv[1]);
        exit(2);
    }


    clients = new pthread_t[clientsNum];
    

    int usrInpt = -1;
    while (usrInpt){
        usrInpt = handle_TUI_display(usrInpt);
    }


    for (int i = 0; i < clientsNum; i++){
        int* clientID = new int(i);
        rt = pthread_create(&clients[i], NULL, clientThread, (void*)clientID);
        if (rt){
            printf("\n ERROR: Return code from pthread_create : %d - For iteration : %d \n", rt, i);
            exit(10);
        }
        
    }
    void* thStatus;
    for (int i = 0; i < clientsNum; i++)
    {
        rt = pthread_join(clients[i], &thStatus);
        if (rt){
            printf("\n ERROR: Return code from pthread_join : %d \n", rt);
            exit(11);
        }
        printf("Thread %llu joined with status : %d \n", clients[i], *(int*)thStatus);
    }
    
    delete[] clients;
    
    cout << "\nEnding main thread.";
    cout << "\n>>> Program termination <<<\n";
    pthread_exit(NULL);
}



int handle_TUI_display(int selection){
    switch (selection)
    {
    default:
        cout << "-------------------- \n"
                "Select option: \n"
                "1) Print server logs \n"
                "2) Open client interface \n"
                "0) Exit\n";
        selection = handle_TUI_selection_input();
        break;
    case 0:
        break;
    }
    return selection;
}

int handle_TUI_selection_input(){
    string inputStr;
    int inputNum;
    while (true){
        cout << "Enter option:";
        try
        {
            cin >> inputStr;
            inputNum = stoi(inputStr);
        }
        catch(const std::exception& e)
        {
            cout << ">!< Invalid input. Please enter an integer number.\n";
            continue;
        }
        break;
    }
    return inputNum;
}