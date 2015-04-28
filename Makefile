LIBS = -lpthread

all: DiningPhilosopher CigaretteSmokers

DiningPhilosopher: DiningPhilosopher.c
	$(CC) $? $(LIBS) -o $@

CigaretteSmokers: CigaretteSmokers.c
	$(CC) $? $(LIBS) -o $@

