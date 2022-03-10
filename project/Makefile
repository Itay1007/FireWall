obj-m += firewall.o
firewall-objs := ./src/hw3secws.o ./src/filtering.o ./src/logs.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean