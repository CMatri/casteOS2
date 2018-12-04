OBJS:=\
test2.txt \
test.txt \

initrd:
	gcc -o initrd_gen initrd_gen.c
	./initrd_gen $(OBJS)  

install: initrd
	cp initrd.img ../isodir/modules

clean:
	rm initrd_gen *.img