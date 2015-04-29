LIBS = -lpthread

all: ProducerConsumer DiningPhilosopher CigaretteSmokers

ProducerConsumer: ProducerConsumer.c
	$(CC) $? $(LIBS) -o $@

DiningPhilosopher: DiningPhilosopher.c
	$(CC) $? $(LIBS) -o $@

CigaretteSmokers: CigaretteSmokers.c
	$(CC) $? $(LIBS) -o $@

