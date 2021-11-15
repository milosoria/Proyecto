#include "conection.h"

void recollect(int player, PlayersInfo * players_info);

void log_all(char * message,int pkg_id,PlayersInfo* players_info);

int play(int player,char* payload,PlayersInfo *players_info); 

// Plays available
void log_info(int player, PlayersInfo* players_info);
void create_villager(int player, PlayersInfo* players_info);
void level_up(int player, PlayersInfo* players_info);
int attack(int player, PlayersInfo* players_info);
void spy(int player, PlayersInfo *players_info);
void steal(int player, PlayersInfo* players_info);
void surrender(int player, PlayersInfo* players_info);
void pass(int player, PlayersInfo *players_info);
