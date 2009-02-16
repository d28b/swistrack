
all:
	cd 3rdParty/ && tar xf cs2-4.4.tar
	cd 3rdParty/cs2-4.4/ && make
	cd code/gui && make

clean:
	cd code/batch && make clean
	cd code/camshift_wrapper && make clean
	cd code/core && make clean
	cd code/gui && make clean
	cd code/libtsai && make clean
