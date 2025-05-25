#include <pthread.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <queue>


#define DEBUGLOG(msg) cout << "#DEBUG : " << msg <<" #\n";


using std::cout, std::cin, std::endl, std::string, std::to_string, std::queue;


int handle_TUI_display(int selection);
int handle_TUI_selection_input(string msg);

void* clientThread(void* thdata);
void* serverThread(void* thdata);


pthread_t server;
pthread_t* clients;
int clientsNum;
bool serverOnline;
string serverLog;
string* chatHisoryClient;

struct Message {
    int clientID;
    string text;
};

queue<Message> msgQueue;
pthread_mutex_t msgQueueMutex;
pthread_cond_t msgQueueConditional;



void* serverThread(void* thdata){
    delete (int*)thdata;

    while (true) {
        if (!serverOnline) break;

        Message msg;

        //Message queue handling
        pthread_mutex_lock(&msgQueueMutex);
        while (msgQueue.empty() && serverOnline){
            pthread_cond_wait(&msgQueueConditional, &msgQueueMutex);
        }
        //Break server loop
        if (msgQueue.empty() && !serverOnline){
            pthread_mutex_unlock(&msgQueueMutex);
            break;
        }

        msg = msgQueue.front();
        msgQueue.pop();
        pthread_mutex_unlock(&msgQueueMutex);

        serverLog += "From Client " + std::to_string(msg.clientID) + ": " + msg.text + "\n";

        //Update clients' chat history
        for (int i = 0; i < clientsNum; ++i){
            if (i != msg.clientID){
                chatHisoryClient[i] += "Client " + std::to_string(msg.clientID) + ": " + msg.text + "\n";
            }
        }
    }


    pthread_exit(NULL);
}



void* clientThread(void* thdata){
    int clientID = *(int*)thdata;
    delete (int*)thdata;

    string name = "Client " + to_string(clientID);

    for (int i = 0; i < 3; ++i) {
        string msg = name + " message " + to_string(i + 1);

        pthread_mutex_lock(&msgQueueMutex);
        msgQueue.push({clientID, msg});
        pthread_mutex_unlock(&msgQueueMutex);

        pthread_cond_signal(&msgQueueConditional);

        usleep(1000);
    }

    pthread_exit(NULL);
}



int main(int argc, char* argv[]){
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


    pthread_mutex_init(&msgQueueMutex, NULL);
    pthread_cond_init(&msgQueueConditional, NULL);
    //Creating server thread
    serverOnline = true;
    rt = pthread_create(&server, NULL, serverThread, NULL);
    //Creating client threads
    clients = new pthread_t[clientsNum];
    chatHisoryClient = new string[clientsNum];
    if (rt){
        printf("\n ERROR: Return code from pthread_create : %d \n", rt);
        exit(10);
    }
    for (int i = 0; i < clientsNum; i++){
        int* clientID = new int(i);
        rt = pthread_create(&clients[i], NULL, clientThread, (void*)clientID);
        if (rt){
            printf("\n ERROR: Return code from pthread_create : %d - For iteration : %d \n", rt, i);
            exit(20);
        }
    }

    usleep(100000);
    int usrInpt = -1;
    while (usrInpt){
        usrInpt = handle_TUI_display(usrInpt);
    }

    //Joining client threads
    void* thStatus;
    for (int i = 0; i < clientsNum; i++){
        rt = pthread_join(clients[i], &thStatus);
        DEBUGLOG("cl:"<<i<<"-rt:"<<rt);
        if (rt){
            printf("\n ERROR: Return code from pthread_join : %d \n", rt);
            exit(21);
        }
        //printf("Thread %llu joined with status : %d \n", clients[i], *(int*)thStatus);//???
    }
    //Joining server thread
    serverOnline = false;
    DEBUGLOG(serverOnline)
    pthread_mutex_lock(&msgQueueMutex);
    pthread_cond_signal(&msgQueueConditional);  //Wake up server so it detect that serverOnline == false
    pthread_mutex_unlock(&msgQueueMutex);
    rt = pthread_join(server, &thStatus);
    DEBUGLOG("serv:"<<"rt:"<<rt);
    if (rt){
        printf("\n ERROR: Return code from pthread_join : %d \n", rt);
        exit(11);
    }
    //printf("Thread %llu joined with status : %d \n", server, *(int*)thStatus);//???
    
    pthread_mutex_destroy(&msgQueueMutex);
    pthread_cond_destroy(&msgQueueConditional);
    delete[] clients;
    delete[] chatHisoryClient;

    cout << "\nEnding main thread.";
    cout << "\n>>> Program termination <<<\n";
    pthread_exit(NULL);
}



int handle_TUI_display(int selection){
    switch (selection)
    {
    default:
        cout << "======================== \n"
                "Select option: \n"
                "1) Open server logs \n"
                "2) Open client interface \n"
                "0) Exit\n";
                "------------------------ \n";
        selection = handle_TUI_selection_input("Enter option: ");
        
        break;
    case 0:
        break;
    case 1:
        cout << "=========================== \n"
                "       SERVER LOGS \n"
                "--------------------------- \n"
             << serverLog
             << "--------------------------- \n"
             ;
        while (selection != 9) 
            selection = handle_TUI_selection_input("Enter 9 to continue: ");
        break;
    case 2:
        cout << "=========================== \n"
                "       User's menu \n"
                "--------------------------- \n"
                "21) Open clients logs \n"
                " 9) Back to menu \n"
                "--------------------------- \n"
             ;
        while (!(selection == 9 || selection == 21)) 
            selection = handle_TUI_selection_input("Enter option: ");
        break;
    case 21:
        cout << "--------------------------- \n";
        selection = handle_TUI_selection_input("Enter user's ID (0 - "+ to_string(clientsNum-1) +"): ");
        while (selection < 0 || selection > clientsNum-1) 
            selection = handle_TUI_selection_input("User's ID must be valid (from 0 to " + to_string(clientsNum-1) + "): ");
        cout << "--------------------------- \n"
             << chatHisoryClient[selection]
             << "--------------------------- \n";
        selection = -1
        while (selection != 9) 
            selection = handle_TUI_selection_input("Enter 9 to continue: ");
            break;
    }
    return selection;
}

int handle_TUI_selection_input(string msg){
    string inputStr;
    int inputNum;
    while (true){
        cout << msg;
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