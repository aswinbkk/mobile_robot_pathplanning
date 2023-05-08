def add_new_face(request, row, col):
	from pyamaze import maze, agent, COLOR, textLabel
	from queue import PriorityQueue

	def h(cell1, cell2):
		x1, y1 = cell1
		x2, y2 = cell2
		return (abs(x1 - x2) + abs(y1 - y2))

	def aStar(m):
		(m.rowS, m.colS) = (1, 1)
		(m.rowG, m.colG) = (row, col)
		print("Star Point:", (m.rowS, m.colS))
		print("End Point:", (m.rowG, m.colG))
		messages.success(request, f'Star Point: Row {m.rowS}, Column {m.colS}')
		messages.success(request, f'End Point: Row {m.rowG}, Column {m.colG}')
		# create a serial port object with the appropriate parameters for the xbee module

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

		l = textLabel(m, 'A Star Path Length', len(fwdPath) + 1)
		l = textLabel(m, 'A Star Search Length', len(searchPath))
		# Show the animation first
		m.run()
		messages.success(request, f'Navigation begins')
		return redirect('add-new-face-instructions')
	else:
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
		message = f'Forward Path: {", ".join(str(i) for i in shortest_path)}'
		messages.success(request, message)

		print("Reverse Path:", shortest_path[::-1])
		message = f'Reverse Path: {", ".join(str(i) for i in shortest_path[::-1])}'
		messages.success(request, message)

		l = textLabel(m, 'A Star Path Length', len(fwdPath) + 1)
		l = textLabel(m, 'A Star Search Length', len(searchPath))
		l = textLabel(m, 'Star Point:', (m.rowS, m.colS))
		l = textLabel(m, 'End Point:', (m.rowG, m.colG))
		# Show the animation first
		m.run()

		return redirect('add-new-face-instructions')
