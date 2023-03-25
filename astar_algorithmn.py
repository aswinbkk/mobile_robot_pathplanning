from pyamaze import maze, agent, COLOR, textLabel
from queue import PriorityQueue
import serial


def h(cell1, cell2):
    x1, y1 = cell1
    x2, y2 = cell2
    return (abs(x1 - x2) + abs(y1 - y2))


def aStar(m):
    (m.rowS, m.colS) = (1, 1)
    (m.rowG, m.colG) = (5, 5)
    print("Star Point:", (m.rowS, m.colS))
    ser = serial.Serial('COM5', 9600, timeout=1)
    ser.write(str(m.rowS).encode())
    while True:
        response = ser.readline().decode().strip()
        if response == '0':
            break
        elif response:
            print('Unexpected response:', response)
    ser.write(str(m.colS).encode())

    while True:
        response = ser.readline().decode().strip()
        if response == '1':
            break
        elif response:
            print('Unexpected response:', response)

    print("End Point:", (m.rowG, m.colG))
    # create a serial port object with the appropriate parameters for the xbee module
    ser.write(str(m.rowG).encode())
    while True:
        response = ser.readline().decode().strip()
        if response == '0':
            break
        elif response:
            print('Unexpected response:', response)
    ser.write(str(m.colG).encode())

    open = PriorityQueue()
    open.put((h((m.rowG, m.colG), (m.rowS, m.colS)), h((m.rowG, m.colG), (m.rowS, m.colS)), (m.rowG, m.colG)))
    aPath = {}
    g_score = {row: float("inf") for row in m.grid}
    g_score[(m.rowG, m.colG)] = 0
    f_score = {row: float("inf") for row in m.grid}
    f_score[(m.rowG, m.colG)] = h((m.rowG, m.colG), (m.rowS, m.colS))
    searchPath = [(m.rowG, m.colG)]
    while not open.empty():
        currCell = open.get()[2]
        searchPath.append(currCell)
        if currCell == (m.rowS, m.colS):
            break
        for d in 'ESNW':
            if m.maze_map[currCell][d] == True:
                if d == 'E':
                    childCell = (currCell[0], currCell[1] + 1)
                elif d == 'W':
                    childCell = (currCell[0], currCell[1] - 1)
                elif d == 'N':
                    childCell = (currCell[0] - 1, currCell[1])
                elif d == 'S':
                    childCell = (currCell[0] + 1, currCell[1])

                temp_g_score = g_score[currCell] + 1
                temp_f_score = temp_g_score + h(childCell, (m.rowS, m.colS))

                if temp_f_score < f_score[childCell]:
                    aPath[childCell] = currCell
                    g_score[childCell] = temp_g_score
                    f_score[childCell] = temp_g_score + h(childCell, (m.rowS, m.colS))
                    open.put((f_score[childCell], h(childCell, (m.rowS, m.colS)), childCell))

    fwdPath = {}
    cell = (m.rowS, m.colS)
    while cell != (m.rowG, m.colG):
        fwdPath[aPath[cell]] = cell
        cell = aPath[cell]
    return searchPath, aPath, fwdPath


if __name__ == '__main__':
    m = maze(6, 6)
    m.CreateMaze(loadMaze='map.csv')

    searchPath, aPath, fwdPath = aStar(m)
    a = agent(m, footprints=True, color=COLOR.blue, filled=True)
    b = agent(m, m.rowS, m.colS, footprints=True, color=COLOR.yellow, filled=True, goal=(m.rowG, m.colG))
    c = agent(m, footprints=True, color=COLOR.red)

    m.tracePath({a: searchPath}, delay=300)
    m.tracePath({b: aPath}, delay=300)
    m.tracePath({c: fwdPath}, delay=300)

    shortest_path = [b.goal]
    cell = b.goal
    while cell != (m.rowS, m.colS):
        shortest_path.append(fwdPath[cell])
        cell = fwdPath[cell]
    shortest_path.reverse()
    print("Forward Path:", shortest_path)

    # create a serial port object with the appropriate parameters for the xbee module
    ser = serial.Serial('COM5', 9600, timeout=1)
    l = textLabel(m, 'A Star Path Length', len(fwdPath) + 1)
    l = textLabel(m, 'A Star Search Length', len(searchPath))
    # Show the animation first
    m.run()

    # Send the cell address after the animation is complete
    for cell in shortest_path:
        # send the x-coordinate to the microcontroller
        ser.write(str(cell[0]).encode())

        # wait for confirmation from the microcontroller
        while True:
            response = ser.readline().decode().strip()
            if response == '0':
                break
            elif response:
                print('Unexpected response:', response)

        # send the y-coordinate to the microcontroller
        ser.write(str(cell[1]).encode())
        print("Move to ➡️", cell)

        # wait for confirmation from the microcontroller
        while True:
            response = ser.readline().decode().strip()
            if response == '1':
                break
            elif response:
                print('Unexpected response:', response)

    print("Reverse Path:", shortest_path[::-1])

    for cell in reversed(shortest_path[:-1]):
        # send the x-coordinate to the microcontroller
        ser.write(str(cell[0]).encode())

        # wait for confirmation from the microcontroller
        while True:
            response = ser.readline().decode().strip()
            if response == '0':
                break
            elif response:
                print('Unexpected response:', response)

        # send the y-coordinate to the microcontroller
        ser.write(str(cell[1]).encode())
        print("Move to ➡️", cell)

        # wait for confirmation from the microcontroller
        while True:
            response = ser.readline().decode().strip()
            if response == '1':
                break
            elif response:
                print('Unexpected response:', response)

    # read the response from the microcontroller
    response = ser.readline().decode()

    # print the response
    print(response)
    # close the serial port
    ser.close()

