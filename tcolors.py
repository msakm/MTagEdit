import os

BOLD   =""
RED    =""
GREEN  =""
YELLOW =""
CYAN   =""
NORMAL =""

def testcolors():
    print ('BOLD test :  '+BOLD + "Bold" +NORMAL)
    print ('RED test :  '+RED + "Red" +NORMAL)
    print ('GREEN test :  '+GREEN + "Green" +NORMAL)

def initColors():
    global BOLD, RED, GREEN, YELLOW, CYAN, NORMAL
    try:
        tcolors = int(os.popen("tput colors 2>&0").read().strip())
    except:
        tcolors = 0
    print('Colors supported: ' + str(tcolors))
    
    if (tcolors > 0):
        BOLD   ="\033[1m"
        RED    ="\033[91m"
        GREEN  ="\033[92m"
        YELLOW ="\033[93m" # unreadable on white backgrounds
        CYAN   ="\033[96m"
        NORMAL ="\033[0m"
