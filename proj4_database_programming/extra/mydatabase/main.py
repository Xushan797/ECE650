import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE","mydatabase.settings")

import django
django.setup()

from database.models import Player,Team,State,Color

from query_func import query1,query2,query3,query4,query5

def addState():
    file = open('state.txt')
    for line in file:
        state_id, name = line.strip("\n").split(' ')
        State.objects.get_or_create(name = name)
    file.close()
    return

def addColor():
    file = open('color.txt')
    for line in file:
        color_id, name = line.strip("\n").split(' ')
        Color.objects.get_or_create(name = name)
    file.close()
    return   

def addTeam():
    file = open('team.txt')
    for line in file:
        team_id, name, state_id, color_id, wins, losses = line.split(' ')
        Team.objects.get_or_create(name = name, state_id = State.objects.get(state_id = state_id), color_id = Color.objects.get(color_id=color_id),wins = wins, losses = losses)
    file.close()
    return
        

def addPlayer():
    file = open('player.txt')
    for line in file:
        player_id, team_id, uniform_num, first_name, last_name, mpg,ppg,rpg,apg,spg,bpg = line.split(' ')
        Player.objects.get_or_create(team_id = Team.objects.get(team_id=team_id),uniform_num = uniform_num,first_name = first_name, last_name = last_name, mpg = mpg, ppg = ppg, rpg = rpg, apg = apg, spg = spg, bpg = bpg)
    file.close()
    return


if __name__ == "__main__":
    addState()
    addColor()
    addTeam()
    addPlayer()
