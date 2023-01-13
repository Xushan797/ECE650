import os 
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mydatabase.settings")

import django 
django.setup()

from database.models import Player,Team,State,Color

def add_state(name):
    State.objects.get_or_create(name = name)
    return

def add_color(name):
    Color.objects.get_or_create(name = name)

def add_player(team_id, uniform_num, first_name, last_name, mpg,ppg,rpg,apg):
    Player.objects.get_or_create(team_id=Team.objects.get(team_id = team_id), uniform_num = uniform_num, first_name = first_name, last_name = last_name, mpg = mpg, ppg = ppg, rpg = rpg, apg = apg, spg = spg, bpg = bpg)
    return

def add_team(name,state_id, color_id, wins, losses):
    Team.objects.get_or_create(name = name, state_id = State.objects.get(state_id = state_id), color_id = Color.objects.get(color_id = color_id), wins = wins, losses = losses)
    return

def query1(use_mpg, min_mpg, max_mpg, use_ppg, min_ppg, max_ppg, use_rpg, min_rpg, max_rpg, use_apg, min_apg, max_apg, use_spg, min_spg, max_spg, use_bpg, min_bpg, max_bpg):
    res= Player.objects.all()
    if use_mpg:
        res = res.filter(mpg__lte = max_mpg, mpg__gte = min_mpg)
    if use_ppg: 
        res = res.filter(ppg__lte = max_ppg, ppg__gte = min_ppg)
    if use_rpg:
        res = res.filter(rpg__lte = max_rpg, rpg__gte = min_rpg)
    if use_apg: 
        res = res.filter(apg__lte = max_apg, apg__gte = min_apg)
    if use_spg:
        res = res.filter(spg__lte = max_spg, spg__gte = min_spg)
    if use_bpg: 
        res = res.filter(bpg__lte = max_bpg, bpg__gte = min_bpg)
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    for player in res:
        print(player.player_id, player.team_id.team_id, player.uniform_num, player.first_name, player.last_name, player.mpg, player.ppg,player.rpg, player.apg,player.spg, player.bpg)
    return

def query2(team_color):
    res = Team.objects.filter(color_id__name = team_color)
    print("NAME")
    for team in res:
        print(team.name)
    return

def query3(team_name):
    res = Player.objects.filter(team_id__name = team_name).order_by('-ppg')
    print("FIRST_NAME LAST_NAME")
    for player in res:
        print(player.first_name, player.last_name)
    return

def query4(team_state, team_color):
    res = Player.objects.filter(team_id__state_id__name = team_state, team_id__color_id__name = team_color).order_by('player_id')
    print("UNIFORM_NUM FIRST_NAME LAST_NAME")
    for player in res:
        print(player.uniform_num, player.first_name,player.last_name)
    return

def query5(num_wins):
    res = Player.objects.filter(team_id__wins__gt = num_wins)
    print("FIRST_NAME LAST_NAME NAME WINS")
    for player in res:
        print(player.first_name, player.last_name, player.team_id.name, player.team_id.wins)
    return