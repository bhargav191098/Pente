import threading
import time

import subprocess



'''
At this stage, I have done till white player - plays the first move. Then the command flows to the agent
The agent makes the next move.
There is no timer as of now.
Also need to populate  the grid from the playdata.txt
Need to write code to capture
need to write code to count black and white coins captured.

'''

firstWhiteMove = True

'''
def makeBoardAndTest():
	print("Make board and test called !!!")
	global board
	string_board = [
	    "...................",
	    "...................",
	    "...................",
	    "...................",
	    "...................",
	    "...................",
	    "........w..........",
	    "...................",
	    ".........b.w.......",
	    ".........wb........",
	    ".........b.........",
	    "........w..........",
	    "...................",
	    "...................",
	    "...................",
	    "...................",
	    "...................",
	    "...................",
	    "..................."
	]
	for i in range(19):
		for j in range(19):
			board[i][j]  = string_board[i][j]
	captureCoins(8,11)
	print("White captured : ",white_captured)
	print("Black captured :",black_captured)
'''


def execute_cpp_program(cpp_file_path):
    try:
        # Compile the C++ program
        compile_command = ['g++', cpp_file_path, '-o', 'output']
        subprocess.run(compile_command, check=True)

        # Execute the compiled program and capture its output
        execution_command = ['./output']
        completed_process = subprocess.run(execution_command, stdout=subprocess.PIPE, text=True, check=True)
        
        # Get the output of the program
        program_output = completed_process.stdout

        return program_output

    except subprocess.CalledProcessError as e:
        print("Error:", e)
        return None


def isSafe(coord):
	if(coord>=19 or coord<0):
		return False
	return True 

def captureCoins(x,y):
	global white_captured,black_captured,player_coin,agent_coin,agent_coins

	player = player_coin
	opponent = agent_coin
	print("PLAYER ",player_coin)
	print("opponent ",opponent)

	moveCaptureCount = 0

	if(isSafe(y-3) and board[x][y-3]==player):
	    if(board[x][y-2]==opponent and board[x][y-2] == board[x][y-1]):
	        board[x][y-2] = '.'
	        board[x][y-1] = '.'
	        moveCaptureCount += 2
	
	#Horizontal-Right Capture
	if(isSafe(y+3) and board[x][y+3]==player):
	    if(board[x][y+2]==opponent and board[x][y+2] == board[x][y+1]):
	        board[x][y+2] = '.'
	        board[x][y+1] = '.'
	        moveCaptureCount += 2
	#Vertical-Down capture 
	if(isSafe(x+3) and board[x+3][y]==player):
	    if(board[x+2][y] == opponent and board[x+2][y]==board[x+1][y]):
	        board[x+2][y] = '.'
	        board[x+1][y] = '.'
	        moveCaptureCount += 2
	        
	#Vertical-Up capture
	if(isSafe(x-3) and board[x-3][y] == player):
	    if(board[x-2][y] == opponent and board[x-2][y] == board[x-1][y]):
	        board[x-2][y] = '.'
	        board[x-1][y] = '.'
	        moveCaptureCount += 2
	        
	#Diagonal-LeftUp
	if(isSafe(x-3) and isSafe(y-3) and board[x-3][y-3] == player):
	    if(board[x-2][y-2] == opponent and board[x-2][y-2] == board[x-1][y-1]):
	        board[x-2][y-2] = '.'
	        board[x-1][y-1] = '.'
	        moveCaptureCount += 2
	        
	#Diagonal-RightDown
	if(isSafe(x+3) and isSafe(y+3) and board[x+3][y+3] == player):
	    if(board[x+2][y+2] == opponent and board[x+2][y+2] == board[x+1][y+1]):
	        board[x+2][y+2] = '.'
	        board[x+1][y+1] = '.'
	        moveCaptureCount += 2
	        
	#Diagonal-RightUp
	if(isSafe(x-3) and isSafe(y+3) and board[x-3][y+3] == player):
	    if(board[x-1][y+1] == opponent and board[x-1][y+1] == board[x-2][y+2]):
	        board[x-1][y+1] = '.'
	        board[x-2][y+2] = '.'
	        moveCaptureCount += 2
	        
	#Diagonal-LeftDown
	if(isSafe(x+3) and isSafe(y-3) and board[x+3][y-3] == player):
	    if(board[x+2][y-2] == opponent and board[x+1][y-1] == board[x+2][y-2]):
	        board[x+2][y-2] = '.'
	        board[x+1][y-1] = '.'
	        moveCaptureCount += 2

	        
	#capturedMap.insert(make_pair(make_pair(x,y),capturedCoordinates));
	print("Move capture count: ",moveCaptureCount)
	if(player_coin == 'b'):
		white_captured += moveCaptureCount
	else:
		black_captured += moveCaptureCount
	#Since the agents coins are only captured throught this function.
	#The only entry point for this function is after the player move. Hence the direct piece of code.
	agent_coins -= moveCaptureCount

def returnOpponent(colorOfCoin):
	if(colorOfCoin=='b'):
		return 'w'
	else:
		return 'b'


def checkVerticalGrowth(lastMoveX,lastMoveY):
    count = 1
    breakLeftGrowth = False
    leftDotCount = 0
    rightDotCount = 0
    #Rationale: I'll be making a call from the last move made. It can be mine or my opponent. The pattern across is it generalised. Grow till blocked by an opponent.
    #Count how many coins I have already placed and how many I can do. See how far I'm away from the win.
    #At the end of the growth, see what coin's growth was checked and based on this return the point -> Return the character that made this growth. Check in evaluation function.
    colorCoin = board[lastMoveX][lastMoveY]
    opponent = returnOpponent(colorCoin)
    coinUpGrowthBreak = False;
    coinDownGrowthBreak = False;
    stopCheckingMyColor = False;
    
    for i in range(1,5):
        if(breakLeftGrowth):
        	break
        if(count == 5):
            return True 
        if(isSafe(lastMoveX-i)):
            if(board[lastMoveX-i][lastMoveY]== returnOpponent(colorCoin)):
                breakLeftGrowth = True
                continue
            
            #safe to grow.
            #Can I grow my consecutive coin count?
            #lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX-i][lastMoveY] == board[lastMoveX][lastMoveY] and not coinUpGrowthBreak):
                count+=1
            elif(board[lastMoveX-i][lastMoveY]=='.'):
                leftDotCount+=1
                coinUpGrowthBreak = True
        else:
            #Not safe to grow.
            breakLeftGrowth = True

    breakRightGrowth = False
    for i in range(1,5):
        if(breakRightGrowth):
        	break
        if(count == 5):
            return 0
        if(isSafe(lastMoveX+i)):
            if(board[lastMoveX+i][lastMoveY] == opponent ):
                breakRightGrowth = True
                continue
            if(board[lastMoveX+i][lastMoveY] == board[lastMoveX][lastMoveY] and not coinDownGrowthBreak):
                count+=1
            elif(board[lastMoveX+i][lastMoveY]=='.'):
                rightDotCount+=1
                coinDownGrowthBreak = True
        else:
            breakRightGrowth = True
    #cout<<"Vertical Check:"<<count<<"--"<<dotCount<<endl;
    return count == 5


def checkHorizontalGrowth(lastMoveX,lastMoveY):
    count = 1;
    breakLeftGrowth = False;
    leftDotCount = 0;
    rightDotCount = 0;
    colorCoin = board[lastMoveX][lastMoveY];
    coinLeftGrowthBreak = False;
    coinRightGrowthBreak = False;
    opponent = returnOpponent(colorCoin);

    for i in (range(1,5)):
    	if(breakLeftGrowth):
    		break;
    	if(count == 5):
            return True
        
    	if(isSafe(lastMoveY-i)):
            if(board[lastMoveX][lastMoveY-i]== returnOpponent(colorCoin)):
                breakLeftGrowth = True
                continue
            
            #safe to grow.
            #Can I grow my consecutive coin count?
            #lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX][lastMoveY-i] == board[lastMoveX][lastMoveY] and not coinLeftGrowthBreak):
                count+=1
            
            elif(board[lastMoveX][lastMoveY-i]=='.'):
                leftDotCount+=1
                coinLeftGrowthBreak = True
    	else:
            #Not safe to grow.
            breakLeftGrowth = True

    breakRightGrowth = False;
    for i in range(1,5):
    	if(breakRightGrowth):
    		break;
    	if(count == 5):
            return 0
    	if(isSafe(lastMoveY+i)):
        	if(board[lastMoveX][lastMoveY+i] == opponent ):
        		breakRightGrowth = True
        		continue
        	if(board[lastMoveX][lastMoveY+i] == board[lastMoveX][lastMoveY] and not coinRightGrowthBreak):
        		count+=1
            
       		elif(board[lastMoveX][lastMoveY+i]=='.'):
        		rightDotCount+=1
        		coinRightGrowthBreak = True
    	else:
            breakRightGrowth = True
        
    
    return count == 5




def checkLeftRightDiagonal(lastMoveX,lastMoveY):
    count = 1
    breakLeftGrowth = False
    leftDotCount = 0
    rightDotCount = 0
    colorCoin = board[lastMoveX][lastMoveY]
    coinLeftGrowthBreak = False
    coinRightGrowthBreak = False
    opponent = returnOpponent(colorCoin)

    for i in range(1,5):

        if(breakLeftGrowth):
    	    break;
    
        if(count == 5):
            return True
     
        if(isSafe(lastMoveX-i) and isSafe(lastMoveY-i)):
            if(board[lastMoveX-i][lastMoveY-i]== returnOpponent(colorCoin)):
                breakLeftGrowth = True
                continue
            
            #safe to grow.
            #Can I grow my consecutive coin count?
            #lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX-i][lastMoveY-i] == board[lastMoveX][lastMoveY] and not coinLeftGrowthBreak):
                count+=1
            
            elif(board[lastMoveX-i][lastMoveY-i]=='.'):
                leftDotCount+=1
                coinLeftGrowthBreak = True
            
        
        else:
            #Not safe to grow.
            breakLeftGrowth = True
        
    

    breakRightGrowth = False
    for i in range(1,5):
    	if(breakRightGrowth):
    		break
    	if(count == 5):
    		return True 

    	if(isSafe(lastMoveX+i) and isSafe(lastMoveY+i)):
            if(board[lastMoveX+i][lastMoveY+i] == opponent ):
                breakRightGrowth = True
                continue
            
            if(board[lastMoveX+i][lastMoveY+i] == board[lastMoveX][lastMoveY] and not coinRightGrowthBreak):
                count+=1
            
            elif(board[lastMoveX+i][lastMoveY+i]=='.'):
                rightDotCount+=1
                coinRightGrowthBreak = True
           
    	else:
            breakRightGrowth = True
        
    
    return count == 5    


def checkRightLeftDiagonal(lastMoveX,lastMoveY):

    count = 1;
    breakLeftGrowth = False;
    leftDotCount = 0;
    rightDotCount = 0;
    colorCoin = board[lastMoveX][lastMoveY];
    coinLeftGrowthBreak = False;
    coinRightGrowthBreak = False;
    opponent = returnOpponent(colorCoin);

    for i in range(1,5):
        if(breakLeftGrowth):
    	    break
        if(count == 5):
            return 0
        if(isSafe(lastMoveX+i) and isSafe(lastMoveY-i)):
            if(board[lastMoveX+i][lastMoveY-i]== returnOpponent(colorCoin)):
                breakLeftGrowth = True
                continue
            #safe to grow.
            #Can I grow my consecutive coin count?
            #lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX+i][lastMoveY-i] == board[lastMoveX][lastMoveY] and not coinLeftGrowthBreak):
                count+=1
           
            elif(board[lastMoveX+i][lastMoveY-i]=='.'):
                leftDotCount+=1
                coinLeftGrowthBreak = True
        else:
            #Not safe to grow.
            breakLeftGrowth = True
    breakRightGrowth = False
    for i in range(1,5):
    
        if(breakRightGrowth):
    	    break
        if(count == 5):
            return True
          
        if(isSafe(lastMoveX-i) and isSafe(lastMoveY+i)):
            if(board[lastMoveX-i][lastMoveY+i] == opponent):
                breakRightGrowth = True
                continue
            
            if(board[lastMoveX-i][lastMoveY+i] == board[lastMoveX][lastMoveY] and not coinRightGrowthBreak):
                count+=1
            
            elif(board[lastMoveX-i][lastMoveY+i]=='.'):
                rightDotCount+=1
                coinRightGrowthBreak = True
           
     
        else:
            breakRightGrowth = True
        
    
    return count == 5



def checkCaptureCriteria():
	if(white_captured == 10):
		if(player_coin=='w'):
			print("Yay! You win!")
			exit()
		else:
			print("O.o You lost!")
			exit()
	elif(black_captured == 10):
		if(player_coin=='b'):
			print("Yay! You win!")
			exit()
		else:
			print("O.o You lost!")
			exit()

def check5piecesCriteria(x,y):
	return (checkHorizontalGrowth(x,y) or checkVerticalGrowth(x,y) or checkLeftRightDiagonal(x,y) or checkRightLeftDiagonal(x,y))


player = input("What color do you want to play?")
agent_side = ""
player_side = ""
player_coin = ""
agent_coin = ""
player_coins = 0
agent_coins = 0
white_move_number = 0 



if(player.lower()=="black"):
	agent_side = "WHITE"
	player_side = "BLACK"
	player_coin = 'b'
	agent_coin = 'w'
elif(player.lower()=="white"):
	agent_side = "BLACK"
	player_side = "WHITE"
	player_coin = 'w'
	agent_coin = 'b'
else:
	print("Please enter a proper player name")
	exit()


print("You have 300 seconds in total. The AI agent also has 300 seconds.\n")
print("If you don't make a move within your 300 seconds, it is automatically considered forfeiting.")
print("By default, if you are white, your first move will be placing a white coin at the center of the board.")
print("Game starts! ********************************************\n")

agent_time = 100.0

white_captured = 0
black_captured = 0


lastMoveX = -1
lastMoveY = -1

gameRun = True
moveMade = False


board = [['.' for _ in range(19)] for _ in range(19)]


def timer_function(interval, callback):
    time.sleep(interval)
    callback()

def my_callback_function():
	global gameRun
	gameRun = False

	print("Timer completed! This function is called.")

def agent_callback():
	if(not moveMade):
		print("Agent forfeit because too much time taken. You win!!")
		exit()
	else:
		print("Thread callback but not doing anything!")



# Set the interval for the timer in seconds
timer_interval = 10

# Create a thread for the timer function
#timer_thread = threading.Thread(target=timer_function, args=(timer_interval, my_callback_function))




'''
rowData = [-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
colData = {'A':0,'B':1,'C':2,'D':3,'E':4,'F':5,'G':6,'H':7,'J':8,
		   'K':9,'L':10,'M':11,'N':12,'O':13,'P':14,''
		  }'''

def checkForCutCoins(side):
	global player_coins,agent_coins,black_captured,white_captured
	count_side= 0
	for i in range(19):
		for j in range(19):
			if(board[i][j]==side):
				count_side+=1
	difference = 0
	if(side == player_coin):
		difference = player_coins - count_side
		player_coins = count_side
	else:
		difference = agent_coins - count_side
		agent_coins = count_side

	difference = max(0,difference)
	if(side=='b'):
		black_captured += difference
	elif(side=='w'):
		white_captured += difference


def updateAgentMove():
	print("Processing the move of agent: \n")
	playdata_file = open('playdata.txt','r')
	line_number = 0
	agent_move_x = 0
	agent_move_y =0

	while True:
		line_number += 1

		line = playdata_file.readline()

		if not line:
			break

		if(line_number < 20):
			#print(str(line_number-1),":",line,"\n")
			for itr in range(19):
				board[line_number-1][itr]=line[itr]
		else:
			if(line_number==20):
				agent_move_x = line
			else:
				agent_move_y = line

	print_pente_board()
	print("Agent move Coordinates : ",row_names[int(agent_move_x)],",",col_names[int(agent_move_y)],"\n")
	return int(agent_move_x),int(agent_move_y)


def agentMakeMove():
	print("Agent making move ....\n")
	global moveMade,agent_time,agent_coins
	'''
	agent_thread = threading.Thread(target=timer_function,args=(agent_time,agent_callback))
	
	# Start the timer thread
	agent_thread.start()
	'''
	t = threading.Timer(agent_time,agent_callback)
	t.start()
	moveMade = False
	agent_action_time_begin = time.time()
	output = execute_cpp_program('homework.cpp')
	agent_action_time_end = time.time()
	agent_action_time = agent_action_time_end - agent_action_time_begin
	agent_time = agent_time - agent_action_time
	print("Time left after move : ",agent_time)
	t.cancel()
	moveMade = True
	agent_coins += 1
	moveX,moveY = updateAgentMove()
	checkCaptureCriteria()
	if(check5piecesCriteria(moveX,moveY)):
		print("Agent wins! ")
		exit()
	playerMakeMove()




def writeToInputFile():
	with open('input.txt','w') as file:
		file.write(agent_side)
		file.write('\n')
		file.write(str(agent_time))
		file.write('\n')
		file.write(str(white_captured)+','+str(black_captured))
		file.write('\n')
		for row in board:
			for item in row:
				file.write(str(item))
			#file.write(row)
			#file.write(str(item) for item in row])
			file.write('\n')
		

def isSecondWhiteMoveSafe(x,y):
	if((x >= 7 and x <= 11) and (y>=7 and y<=11)):
		return False
	return True

col_names = ['A','B','C','D','E','F','G','H','J','K','L','M','N','O','P','Q','R','S','T']
row_names = [19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1]

def print_pente_board():
    size = len(board)

    # Print column names (letters A to O)
    print("    " + "  ".join([col_names[i]  for i in range(size)]))

    # Print rows with row names and the board itself
    for i in range(size):
        row_name = str(row_names[i]).rjust(2)  # Convert row number to string and right-justify
        row = [" " + board[i][j] for j in range(size)]  # Add a space before each cell value

        # Combine row name and cells to create the row string
        row_string = row_name + " " + " ".join(row)

        # Print the row
        print(row_string)


def playerMakeMove():
	row = 0
	col = 0
	print("Player should make the move : \n")
	global firstWhiteMove,player_coins,white_move_number
	if(player_coin == 'w'):
		white_move_number += 1
	if(player_side=="WHITE" and (white_move_number==1)):
		print("Sorry, the first move for a white is always at the center!")
		firstWhiteMove = False
		board[9][9]='w'
		writeToInputFile()
		agentMakeMove()
	elif(player_side =="WHITE" and (white_move_number == 2)):
		print("CAUTION: This is your second white move. For the second move, there is a rule enforced: \n The second move of White (first player) should be atleast more than 3 intersections away from their first piece (center).")
		print("\nIf a move is made violating these terms, the game will be considered forfeited.\n")
		x = int(row_names.index(int(input("Enter row: "))))
		col_input = input("Enter column: ")
		y = int(col_names.index(col_input.upper()))

		if(isSecondWhiteMoveSafe(x,y)):
			board[x][y] = player_coin
			player_coins += 1
			captureCoins(x,y)
			writeToInputFile()
			checkCaptureCriteria()
			if(check5piecesCriteria(x,y)):
				print("You win!")
				exit()
			agentMakeMove()
		else:
			print("You violated the second move rule! You lose.")
			exit()


	else:
		print('Second move')
		
		checkForCutCoins(player_coin)

		print("These are the board information.\n")
		if(player_coin=='b'):
			print("Pairs of coins cut for your opponent ",white_captured/2)
			print("Pairs of your coins cut:",black_captured/2)
		else:
			print("Pairs of coins cut for your opponent ",black_captured/2)
			print("Pairs of your coins cut:",white_captured/2)


		x = int(row_names.index(int(input("Enter row: "))))
		col_input = input("Enter column: ")
		y = int(col_names.index(col_input.upper()))

		board[x][y] = player_coin
		player_coins += 1
		captureCoins(x,y)
		writeToInputFile()
		checkCaptureCriteria()
		if(check5piecesCriteria(x,y)):
			print("You win!")
			exit()
		agentMakeMove()





print("Game run value ",gameRun)
remaining_agent_time = agent_time

firstWhiteMove = True
print("Main part of the code running")

if(player_side == "WHITE"):
	playerMakeMove()
else:
	#first move made by agent.
	writeToInputFile()
	agentMakeMove()

