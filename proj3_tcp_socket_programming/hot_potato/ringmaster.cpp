
#include "potato.h"

using namespace std;
//ringmaster <port_num> <num_players> <num_hops> (example: ./ringmaster 1234 3 100)

//same as the TCP server.cpp example


int main(int argc, char *argv[])
{
    if (argc < 4) {
        cout << "Please check the input! example: ./ringmaster <port_num> <num_players> <num_hops> \n" << endl;
        return 1;
    }
    const char * server_port = argv[1];
    int num_player = stoi(argv[2]);
    int num_hop = stoi(argv[3]);
    if (num_hop > 512){
       cerr << "Error: numbe of hops cannot larger than 512 "<< endl;
            return -1; 
    }
    if (num_player <= 1){
        cerr << "Error: numbe of players should be larger than 1 "<< endl;
        return -1;
    }
    int master_fd = open_server(server_port);

    vector<int> player_fd_list;
    vector<string> player_ip_list;
    vector<int> player_port_list;
    cout << "Potato Ringmaster" << endl << "Players = " << num_player << endl << "Hops = " << num_hop << endl;

    //connect to players
    for (int i = 0; i < num_player; ++i) {
        struct sockaddr_storage socket_addr = {};
        socklen_t addr_len = sizeof(socket_addr);
        int new_fd = accept(master_fd, (struct  sockaddr *)&socket_addr, &addr_len);
        int player_port;
        if (new_fd  == -1) {
            cerr << "Error: cannot create socket for " << "player " << i << endl;
            return -1;
        }
        player_fd_list.push_back(new_fd);
        struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;
        string ip = inet_ntoa(addr->sin_addr);
        player_ip_list.push_back(ip);
        //send player id
        long status = send(new_fd, &i, sizeof(i), 0);
        if (status != sizeof(i)){
            cerr << "Error: send player id failed!" << endl;
        }
        //send number of player
        status = send(new_fd, &num_player, sizeof(num_player),0);
        if (status != sizeof(num_player)){
            cerr << "Error: send number of player failed!" << endl;
        }
        //receive player port number
        recv(new_fd, &player_port,sizeof(player_port),0);
        player_port_list.push_back(player_port);
        cout << "Player "<< i << " is ready to play" << endl;
    }
//    for (int i = 0; i < num_player; ++i) {
//        cout << "Player "<< i << player_port_list[i] << endl;
//        cout << "Player "<< i << player_ip_list[i] << endl;
//    }
    for (int i = 0; i < num_player; ++i) {
        int next_player = (i+1)%num_player;
//        const char * next_ip = player_ip_list[next_player].c_str();
        char next_ip[100];
        strcpy(next_ip, player_ip_list[next_player].c_str());
//        cout << "next_ip "<< next_ip << endl;
        int next_port = player_port_list[next_player];
        send(player_fd_list[i],&next_ip,sizeof(next_ip),0);
//        cout << i << "send next ip to player"<< next_player << endl;

//        cout << i << "send port number to player"<< next_player << endl;
        send(player_fd_list[i],&next_port,sizeof(next_port),0);
//        cout << "send port number"<< next_port << endl;
    }

    struct hot_potato potato = {};
    potato.num_hop = num_hop;

    if (num_hop != 0) {
        srand((unsigned int) time(NULL) + num_player);
        int random = rand() % num_player;
        send(player_fd_list[random], &potato, sizeof(potato), MSG_WAITALL);
        cout << "Ready to start the game, sending potato to player " << random << endl;

        fd_set rfds;
        int status;
        int max_fd = -1;

        FD_ZERO(&rfds);
        for(int i=0;i<num_player;i++){
            FD_SET(player_fd_list[i], &rfds);
            max_fd = max(max_fd,player_fd_list[i]);
        }
        status = select(max_fd+1, &rfds, NULL, NULL, NULL);
        if (status == -1){
            cerr<<"select() has error"<<endl;
        }
        for(int i=0;i<num_player;i++){
            if(FD_ISSET(player_fd_list[i], &rfds)){
                recv(player_fd_list[i],&potato,sizeof(potato),MSG_WAITALL);
                break;
            }
        }
        FD_ZERO(&rfds);

    }
    cout<<"Trace of potato:"<<endl;
    for(int i=0;i<num_hop;i++){
        if(i==0){
            cout<<potato.path[i];
        }
        else{
            cout<<","<<potato.path[i];
        }
    }
    cout<<endl;

   
    close(master_fd);

    for (int i = 0; i < num_player; i++) {
    close(player_fd_list[i]);
}
    return 0;

}
