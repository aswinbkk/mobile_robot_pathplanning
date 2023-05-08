# import cv2
# import numpy as np
# from os import listdir
# from os.path import isfile, join
# import serial
# import time
# import pyttsx3
from django.shortcuts import render,redirect
from django.contrib import messages
# from django.http import HttpResponse
# from django.shortcuts import render
# from django.contrib.auth.decorators import login_required


def home(request):
	return render(request, 'recognition/home.html')


# @login_required
def dashboard(request):
	if(request.user.username == 'aswinbkk'):
		print("admin")
		return render(request, 'recognition/admin_dashboard.html')


def contact_us(request):
	return render(request, "recognition/contact.html")
	return redirect('home')

def add_new_face_instructions(request):
	return render(request, "recognition/add_new_face_instructions.html")
	return redirect('home')

def confirm_your_face_instructions(request):
	return render(request, "recognition/confirm_your_face_instructions.html")
	return redirect('home')

def confirm_your_face(request):

	import cv2
	import numpy as np

	# get user input for number of rows and columns in the grid
	num_rows = 6
	num_cols = 6

	# initialize video capture object
	cap = cv2.VideoCapture(0)

	# loop through video frames
	while True:
		# read frame from video capture object
		ret, frame = cap.read()

		# check if frame was successfully read
		if not ret:
			break

		# convert frame to HSV color space
		hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

		# define range of red color in HSV
		lower_red = np.array([0, 100, 100])
		upper_red = np.array([10, 255, 255])
		mask1 = cv2.inRange(hsv, lower_red, upper_red)

		lower_red = np.array([160, 100, 100])
		upper_red = np.array([179, 255, 255])
		mask2 = cv2.inRange(hsv, lower_red, upper_red)

		# combine masks
		mask = mask1 + mask2

		# apply Gaussian blur to mask
		mask = cv2.GaussianBlur(mask, (5, 5), 0)

		# find contours in mask
		contours, hierarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

		# loop through contours
		for cnt in contours:
			# find the center of the contour
			M = cv2.moments(cnt)
			if M["m00"] != 0:
				cX = int(M["m10"] / M["m00"])
				cY = int(M["m01"] / M["m00"])

				# find the row and column of the center of the contour
				row = int(cY / (frame.shape[0] / num_rows)) + 1
				col = int(cX / (frame.shape[1] / num_cols)) + 1

				# draw the contour and label on the frame
				cv2.drawContours(frame, [cnt], -1, (0, 255, 0), 2)
				cv2.putText(frame, "Red object", (cX, cY), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

		cap.release()
		cv2.destroyAllWindows()
		messages.success(request, f'Object found at Row {row}, Column {col}')
		add_new_face(request, row, col)
		return redirect('add-new-face-instructions')

		# calculate width and height of grid cells
		cell_width = frame.shape[1] // num_cols
		cell_height = frame.shape[0] // num_rows

		# draw grid on frame
		for row in range(num_rows):
			for col in range(num_cols):
				x1 = col * cell_width
				y1 = row * cell_height
				x2 = (col + 1) * cell_width
				y2 = (row + 1) * cell_height
				cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 1)

		# display frame
		cv2.imshow("frame", frame)

		# wait for user input to exit
		if cv2.waitKey(1) == 13:
			break

	# release video capture object and close all windows
	cap.release()
	cv2.destroyAllWindows()
	messages.success(request, f'Object found at Row {row}, Column {col}')
	add_new_face(request, row, col)
	return redirect('add-new-face-instructions')



# @login_required
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
		# Show the animation first
		m.run()

		return redirect('add-new-face-instructions')
