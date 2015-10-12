LIBS = -lpthread

all: BarberShop CigaretteSmokers DiningPhilosopher DiningPhilosophers DiningSavages NoStarveRdWr ProducerConsumer

BarberShop: BarberShop.c
	$(CC) $? $(LIBS) -o $@

CigaretteSmokers: CigaretteSmokers.c
	$(CC) $? $(LIBS) -o $@

DiningPhilosopher: DiningPhilosopher.c
	$(CC) $? $(LIBS) -o $@

DiningPhilosophers: DiningPhilosophers.c
	$(CC) $? $(LIBS) -o $@

DiningSavages: DiningSavages.c
	$(CC) $? $(LIBS) -o $@

NoStarveRdWr: NoStarveRdWr.c
	$(CC) $? $(LIBS) -o $@

ProducerConsumer: ProducerConsumer.c
	$(CC) $? $(LIBS) -o $@
