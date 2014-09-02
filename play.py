import os
t=[0,39,41,43,44,46,48,50,51,53,55,56,58,60,62,63,65,67,68,70,72,74,75]
def trans(x): return t[int(x)]
def con(x,y): return str(x)+','+str(y)
os.system('firefox 198.199.106.123/scoreur/backend/gen.html?#'+reduce(con,map(trans,raw_input().split(' '))))
