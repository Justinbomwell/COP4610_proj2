obj-m := elevator.o

PWD := $(shell pwd)
KDIR := /lib/modules/4.19.98/build

default:
  $(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
  rm -f *.o *.ko *.mod Module.* modules.*
