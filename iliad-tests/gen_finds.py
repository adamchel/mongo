import random

f_en = open("iliad-en.txt")

f_en_out = open("iliad-en-finds.js", 'w')

words = []

pgno = 1
for line in f_en:
	if(line.strip() != ""):
		clean = line.strip().replace("\"", "")
		for word in clean.split():
			words.append(word)

quantities = [1,2,3,4,5]
for i in range(10000):
	query = ""
	for j in range(random.choice(quantities)):
		query += random.choice(words) + " "
	query = query.strip()
	f_en_out.write('db.iliad.find({ $text : { $search : "' + query + '" }});\n')

f_en_out.close()

f_fr = open("iliad-fr.txt")

f_fr_out = open("iliad-fr-finds.js", 'w')

words = []

pgno = 1
for line in f_fr:
	if(line.strip() != ""):
		clean = line.strip().replace("\"", "")
		for word in clean.split():
			words.append(word)

quantities = [1,2,3,4,5]
for i in range(10000):
	query = ""
	for j in range(random.choice(quantities)):
		query += random.choice(words) + " "
	query = query.strip()
	f_fr_out.write('db.iliadfr.find({ $text : { $search : "' + query + '" }});\n')

f_fr_out.close()