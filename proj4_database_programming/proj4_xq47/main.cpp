#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include "exerciser.h"
#include <sstream>
#include <fstream>


using namespace std;
using namespace pqxx;


void buildTables(string filename, connection *C){
  string sql,line;
  ifstream file(filename.c_str());
  if(file.is_open()){
    while(getline(file,line)){
      sql += line;
    }
    file.close();
  }
  else{
    cout<< "cannot open file"<<endl;
  }
  work W(*C);
  W.exec(sql);
  W.commit();

  // cout <<"create table successfully" <<endl;
}

void buildPlayer(string filename, connection *C){
  string line,first_name,last_name;
  int player_id,team_id,uniform_num,mpg,ppg,rpg,apg;
  double spg,bpg;
  ifstream file(filename.c_str());
  if(file.is_open()){
    while(getline(file,line)){
      stringstream ss;
      ss << line;
      ss >> player_id >> team_id >> uniform_num >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg >>bpg;
      add_player(C,team_id,uniform_num,first_name,last_name,mpg,ppg,rpg,apg,spg,bpg);

    }
    file.close();
  }
  else{
    cout<< "cannot open file"<<endl;
  }
}
void buildTeam(string filename, connection *C){
  string line,name;
  int team_id,state_id,color_id,wins,losses;
  ifstream file(filename.c_str());
  if(file.is_open()){
    while(getline(file,line)){
      stringstream ss;
      ss << line;
      ss >> team_id >> name >> state_id >> color_id >> wins >> losses;
      add_team(C,name, state_id,color_id,wins,losses);

    }
    file.close();
  }
  else{
    cout<< "cannot open file"<<endl;
  }
}

void buildColor(string filename, connection *C){
  string line,name;
  int color_id;
  ifstream file(filename.c_str());
  if(file.is_open()){
    while(getline(file,line)){
      stringstream ss;
      ss << line;
      ss >> color_id >> name;
      add_color(C,name);
    }
    file.close();
  }
  else{
    cout<< "cannot open file"<<endl;
  } 
}

void buildState(string filename, connection *C){
  string line,name;
  int state_id;
  ifstream file(filename.c_str());
  if(file.is_open()){
    while(getline(file,line)){
      stringstream ss;
      ss << line;
      ss >> state_id >> name;
      add_state(C,name);
    }
    file.close();
  }
  else{
    cout<< "cannot open file"<<endl;
  } 
}
int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files
  buildTables("create_table.sql",C);


  buildState("state.txt",C);
  // cout <<"create state successfully" <<endl;
  buildColor("color.txt",C);
  // cout <<"create color successfully" <<endl;
  buildTeam("team.txt",C);
  // cout <<"create team successfully" <<endl;
  buildPlayer("player.txt",C);
  // cout <<"create player successfully" <<endl;

  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


