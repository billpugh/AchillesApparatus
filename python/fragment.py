oldR = 0
oldC = 0
newR = 0
newC = 0

# ----------------------------------------------------------
# Main loop
# ----------------------------------------------------------

while True:
    
    newR = input("New blank row: ")
    newC = input("New blank col: ")
    
    print("Old: ", oldR, ",", oldC)
    print("New: ", newR, ",", newC)
    
    # If the number of holes is more then one, mis-alignment or errors.
    # Count 'em for awhile and then complain
    if (oldR == newR):
        print("No change:", oldR, ",", newR)
    else:
        print("Change: ", oldr, ",", newR)
 