import onedollar

# read gesture data from file
# each line must contain x, y, z values
# separated by spaces
def file2gesture(fname):
	f = open(fname, "r")
	data = f.readlines()
	gesture = []
	for line in data:
		coords = line.split(' ')
		p = onedollar.Point(float(coords[0]), float(coords[1]), float(coords[2].strip('\n')))
		gesture.append(p)
	#recognizer preprocessing
	gesture = onedollar.resample(gesture, 64) #magic number for the start
	gesture = onedollar.rotate_to_zero(gesture)
	gesture = onedollar.scale_to_square(gesture, 100) #magic number from 3$ paper
	gesture = onedollar.translate_to_origin(gesture)
	f.close()
	return gesture

#start by sorting the recorded samples
zorro = []
circle = []
pigtail = []

for i in range(1, 246):
	path = "sample_data/" + str(i) + ".txt"
	if (i <= 10) or ((i >= 31) and (i <= 40)) or ((i >= 63) and (i <= 72)) or ((i >= 93) and (i <= 102)) or ((i >=123) and (i <= 132)) or ((i >= 153) and (i <= 162)) or ((i >= 183) and (i <= 192)) or ((i >= 215) and (i <= 224)):  #zorro
		if i != 95: #niete
			zorro.append(file2gesture(path))
	elif ((i >= 11) and (i <= 20)) or ((i >= 41) and (i <= 52)) or ((i >= 73) and (i <= 82)) or ((i >= 103) and (i <= 112)) or ((i >= 133) and (i <= 142)) or ((i >= 163) and (i <= 172)) or ((i >= 193) and (i <= 202)) or ((i >= 225) and (i <= 234)): #circle
		circle.append(file2gesture(path))
	elif (i != 95) and (i != 213) and (i != 214): #nieten ausfiltern
		pigtail.append(file2gesture(path))

#print("zorro : " + str(len(zorro)))
#print("circle: " + str(len(circle)))
#print("pigtail: " + str(len(pigtail)))
print("Data conversion successful!")

data = [circle, pigtail, zorro]
names = ["circle", "pigtail", "zorro"]

max_correct = [0, 0, 0]
id_correct = [0, 0, 0]
min_fp = [200, 200, 200]
id_fp = [0, 0, 0]
best_ratio = [0, 0, 0]
id_ratio = [0, 0, 0]

#templates = [circle[0], circle[45], pigtail[0], pigtail[43], zorro[0], zorro[6]]
#blacklist = [[45], [43], [6]]
templates = [circle[0], pigtail[0],  zorro[0]]
blacklist = []
for k in range(len(data)):
	cur_list = data[k]
	for j in range(len(cur_list)): # pick new template and substitute
		correct_matches = 0
		false_positives = 0
		tmpl_index = k * (int(len(templates) / len(data)))
		templates[tmpl_index] = cur_list[j]
		for l in range(len(data)): #iterate through all gestures and classify
			if data[l] == cur_list:
				is_own = True
			else:
				is_own = False
			for i in range(len(data[l])):
				result = onedollar.recognize(data[l][i], templates, 100)
				if is_own:
					if templates.index(result[0]) == tmpl_index:
						correct_matches += 1
					elif templates.index(result[0]) == tmpl_index + 1:
						correct_matches += 1
				else:
					if templates.index(result[0]) == tmpl_index:
						false_positives += 1
					elif templates.index(result[0]) == tmpl_index + 1:
						false_positives += 1
		print(names[k] + "[" + str(j) + "]: correct " + str(correct_matches) + " false positives: " + str(false_positives))
		#if j not in blacklist[k]:
		if True:
			if correct_matches > max_correct[k]:
				max_correct[k] = correct_matches
				id_correct[k] = j
				print("NEW best match: " + str(max_correct[k]))
			if false_positives < min_fp[k]:
				min_fp[k] = false_positives
				id_fp[k] = j
				print("NEW least error: " + str(min_fp[k]))
			if (false_positives != 0) and (correct_matches / false_positives > best_ratio[k]):
				best_ratio[k] = correct_matches / false_positives
				id_ratio[k] = j
				print("NEW best ratio: " + str(best_ratio[k]))
			if false_positives == 0: #perfect match
				best_ratio[k] = 1
				id_ratio[k] = j
				break
		print("--------------------")
	
	print("SUMMARY")
	print("Best match: " + str(id_correct[k]) + " with " + str(float(max_correct[k])/len(cur_list)) + " (" + str(max_correct[k]) + "/" + str(len(cur_list)) + ") matches")
	print("Least false positives: " + str(id_fp[k]) + " with " + str(float(min_fp[k])/(len(data[0]) + len(data[1]) + len(data[2]) - len(cur_list))) + " (" + str(min_fp[k]) + "/" + str((len(data[0]) + len(data[1]) + len(data[2]) - len(cur_list))) + ") false positives")
	print("Best ratio: " + str(id_ratio[k]) + " with " + str(best_ratio[k]))
	#templates[k] = cur_list[0] #reset for equal conditions?
	print("")

print("--------------------")
print("DONE!")
print("Best templates are: ")
for i in range(3):
	print(names[i] + " template " + str(id_correct[i]) + " with " + str(max_correct[i]) + "/" + str(len(data[i])) + " correct matches")
	print(names[i] + " template " + str(id_fp[i]) + " with " + str(min_fp[i]) + "/" + str(len(data[(i+1)%3]) + len(data[(i+2)%3])) + " false positives")
print("")
print(":)")
