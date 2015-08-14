db.iliad.drop()
load("iliad-en-inserts.js");
db.iliad.createIndex({ line : "text"});
for(i = 0; i < 10; i++) {
	load("iliad-en-finds.js");
}
db.iliadfr.drop()
load("iliad-fr-inserts.js");



db.iliadfr.createIndex({ line : "text"});
sleep(5);


for(i = 0; i < 10; i++) {
	load("iliad-fr-finds.js");
}
sleep(5);