#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "potato.h"
#include "stdio.h"
using namespace std;

//player <machine_name> <port_num>
//(example: ./player vcm-xxxx.vm.duke.edu 1234)


int main(int argc, char *argv[])
{
    const char *hostname = argv[1];
    const char *hostport = argv[2];
    int player_id = 0;
    int num_player;
    int next_player_port;

    int status;


    if (argc < 3) {
        cout << "Syntax: player <machine_name> <port_num>\n" << endl;
        return 1;
    }
    int server_fd = open_client(hostname,hostport);
    struct sockaddr_in addr_in;
    socklen_t len = sizeof(addr_in);
    if (getsockname(server_fd, (struct sockaddr *)&addr_in, &len) == -1) {
        cerr << "Error: cannot getsockname" << endl;
        return -1;
    }
    status = recv(server_fd,&player_id,sizeof(player_id),0);
    if(status == -1){
        cerr << "Error: cannot receive player_id" << endl;
        return -1;
    }
    status = recv(server_fd,&num_player,sizeof(num_player),0);
    if(status == -1){
        cerr << "Error: cannot receive number of players" << endl;
        return -1;
    }


    int player_fd = open_server("");
    //
    struct sockaddr_in addr_in_player;
    socklen_t len_player = sizeof(addr_in);
    if (getsockname(player_fd, (struct sockaddr *)&addr_in_player, &len_player) == -1) {
        cerr << "Error: cannot getsockname" << endl;
        return -1;
    }
    int player_port_player = ntohs(addr_in_player.sin_port);

    status = send(server_fd,&player_port_player,sizeof(player_port_player),0);
    if(status == -1){
        cerr << "Error: cannot send player port number" << endl;
        return -1;
    }
    cout << "Connected as player " << player_id << " out of " << num_player<< " total players" << endl;
    // receive next neighbor ip and port number



    char next_player_ip[100] = "A";
    status = recv(server_fd,&next_player_ip,sizeof(next_player_ip),0);
//    cout << " next ip "<<next_player_ip<< endl;
    if(status == -1){
        cerr << "Error: cannot receive next player ip number" << endl;
    }
    status = recv(server_fd,&next_player_port,sizeof(next_player_port),0);
//    cout << " next_player_port "<< next_player_port<< endl;
    if(status == -1){
        cerr << "Error: cannot receive next player port number" << endl;
    }


    //
//    cout << "open as server "<< endl;
    // connect to the next neighbor as client
    char nextplayer_port[10];
    sprintf(nextplayer_port,"%d", next_player_port);
//    cout << "nextplayer_port "<<nextplayer_port<< endl;
    int next_neighbor_fd = open_client(next_player_ip,nextplayer_port);
//    cerr << next_neighbor_fd  << endl;
    if (next_neighbor_fd  == -1) {
        cerr << "Error: cannot create socket for next player "  << endl;
        return -1;}
//    cout << "Connected to next player "<< endl;

    // connect to the former neighbor as server

    struct sockaddr_storage socket_addr = {};
    socklen_t addr_len = sizeof(socket_addr);
    int former_fd = accept(player_fd, (struct  sockaddr *)&socket_addr, &addr_len);
    if (former_fd  == -1) {
        cerr << "Error: cannot create socket for former player "  << endl;
        return -1;}
//    cout << "Connected to former player "<< endl;

    struct hot_potato potato = {};

    //potato game, receive the potato
    fd_set rfds;
    srand((unsigned int)time(NULL) + player_id);
    int max_fd = max(next_neighbor_fd,former_fd);
    max_fd = max(max_fd,server_fd);
    while(true){
        FD_ZERO(&rfds);
        FD_SET(server_fd, &rfds);
        FD_SET(next_neighbor_fd, &rfds);
        FD_SET(former_fd, &rfds);
        status = select(max_fd+1, &rfds, NULL, NULL, NULL);
        if(status == -1){
            cerr<<"select has error"<<endl;
        }
        int signal;
        if(FD_ISSET(server_fd, &rfds)){
            signal = recv(server_fd, &potato, sizeof(potato), MSG_WAITALL);
        }
        if(FD_ISSET(former_fd, &rfds)){
            signal = recv(former_fd, &potato, sizeof(potato), MSG_WAITALL);
        }
        if(FD_ISSET(next_neighbor_fd, &rfds)){
            signal = recv(next_neighbor_fd, &potato, sizeof(potato), MSG_WAITALL);
        }
        if (signal == 0){
            break;
        }
        else if(potato.num_hop == 1){
            potato.num_hop -= 1;
            potato.path[potato.cnt] = player_id;
            potato.cnt += 1;
            cout << "I'm it" << endl;
            send(server_fd,&potato,sizeof (potato),MSG_WAITALL);
            close(server_fd);
            break;
        }
        
        else{
            potato.num_hop -= 1;
            potato.path[potato.cnt] = player_id;
            potato.cnt += 1;
            int k = rand()%2;
//            cout<<"k = "<< k <<endl;
            if(k == 0){
                cout<<"Sending potato to "<< (player_id+1)%num_player <<endl;
                send(next_neighbor_fd,&potato,sizeof(potato),MSG_WAITALL);
            }
            else{
                cout<<"Sending potato to "<< (player_id-1+num_player)%num_player <<endl;
                send(former_fd,&potato,sizeof(potato),MSG_WAITALL);
            }
        }
        FD_ZERO(&rfds);
    }
    
    close(next_neighbor_fd);
    close(former_fd);
    
    return 0;
}
