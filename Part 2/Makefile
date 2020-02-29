obj-m += my_timer.o

all:
        make -C /lib/modules/4.15.0-76-generic/build M=/usr/src/my_timer modules

clean:
        make -C /lib/modules/4.15.0-76-generic/build M=/usr/src/my_timer clean
