import turtle
import math
import sys

argc = len(sys.argv)
in_filename = sys.argv[1]
fin = open(in_filename, mode = 'r')
text = fin.read()
fin.close()

tokens = text.split()
ptr = 0

def ReadInt():
	global ptr
	x = int(tokens[ptr])
	ptr += 1
	return x
	
def ReadFloat():
	global ptr
	x = float(tokens[ptr])
	ptr += 1
	return x

crane_cnt = 0
bays = ReadInt()
bay_plan = []
bay_teu = []
max_time = 0.0

for i in range(bays):
	bay_plan.append([])
	actions = ReadInt()
	tot_teu = 0
	for j in range(actions):
		crane, teu, bg_time, ed_time = ReadInt(), ReadInt(), ReadFloat(), ReadFloat()
		bay_plan[i].append([crane, teu, bg_time, ed_time])
		tot_teu += teu
		max_time = max(max_time, ed_time)
		crane_cnt = max(crane_cnt, crane + 1)
	bay_teu.append(tot_teu)

colors = ['pink', 'aqua', 'yellow', 'bisque', 'greenyellow', 'orange', 'skyblue']

def PaintAxis(X, Y):
	turtle.speed(0)
	turtle.setworldcoordinates(-1, -Y - 1, X + 1, 1)
	turtle.fd(X)
	for i in range(X):
		turtle.goto(i + 0.25, 0)
		turtle.write(bay_teu[i])
	turtle.goto(0, 0)
	turtle.right(90)
	turtle.fd(Y)
	for i in range(Y + 1):
		turtle.penup()
		turtle.goto(0, -i)
		turtle.pendown()
		turtle.goto(-0.1, -i)
		turtle.penup()
		turtle.goto(-0.6, -i - 0.25)
		turtle.pendown()
		turtle.write(str(i) + 'h')
		
def Fill(crane, bay, teu, bg_time, ed_time):
	turtle.penup()
	turtle.goto(bay, -bg_time)
	turtle.pendown()
	turtle.fillcolor(colors[crane % len(colors)])
	turtle.begin_fill()
	turtle.goto(bay, -ed_time)
	turtle.goto(bay + 1, -ed_time)
	turtle.goto(bay + 1, -bg_time)
	turtle.goto(bay, -bg_time)
	turtle.end_fill()
	turtle.penup()
	turtle.goto(bay + 0.25, -(bg_time + ed_time) / 2 - 0.25)
	turtle.pendown()
	turtle.write(teu)

PaintAxis(bays, math.ceil(max_time))

for i in range(bays):
	sz = len(bay_plan[i])
	for j in range(sz):
		crane, teu, bg_time, ed_time = bay_plan[i][j]
		Fill(crane, i, teu, bg_time, ed_time)

turtle.fillcolor('white')
turtle.penup()
turtle.goto(0, -max_time)
turtle.pendown()
turtle.goto(bays, -max_time)
turtle.goto(bays * (5.0 / 6.0), -max_time)
turtle.write('max time : ' + str(max_time) + 'h')

turtle.penup()
turtle.goto(bays, -max_time)
turtle.pendown()

turtle.done()