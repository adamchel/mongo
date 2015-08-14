f_en = open("iliad-en.txt")
f_fr = open("iliad-fr.txt")

f_en_out = open("iliad-en-inserts.js", 'w')
pgno = 1
for line in f_en:
	if(line.strip() != ""):
		clean = line.strip().replace("\"", "\\\"")
		f_en_out.write("db.iliad.insert({ pg : " + str(pgno) + ", line : \"" + clean + "\"});\n")
	pgno += 1
f_en_out.close()

f_fr_out = open("iliad-fr-inserts.js", 'w')
pgno = 1
for line in f_fr:
	if(line.strip() != ""):
		clean = line.strip().replace("\"", "\\\"")
		f_fr_out.write("db.iliadfr.insert({ pg : " + str(pgno) + ", line : \"" + clean + "\"});\n")
	pgno += 1
f_fr_out.close()


