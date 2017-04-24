# Heterogenous-IO
* Understanding the IO Performance gap between KNL and Haswell
* Design Heterogenous IO solution along ExaScale Computing Roadmap

# To visualize the result: 

* IO Gap caused by CPU frequencies Example below, More on [Notebook](https://github.com/NERSC/heterogeneous-IO/blob/master/cpu/plot/hio.ipynb):
		
	![Alt text](https://cloud.githubusercontent.com/assets/1396867/25308126/2d8d406e-2763-11e7-88f5-323c7e73b7b5.png)
	
	This figure shows how I/O on different nodes is affected by CPU frequencies, when page cache is turned on and IO less than page cache
* IO Gap caused by page cache, Example below, More on [Notebook](https://github.com/NERSC/heterogeneous-IO/blob/master/cpu/plot/iostack.ipynb):

	![Alt text](https://cloud.githubusercontent.com/assets/1396867/25308203/7c3f7e74-2764-11e7-803f-e3329b469e1c.png)		
		
	This figure shows how close the IO on different nodes is , when page cache is turned off
* IO Gap caused by MPI and HDF5:

	
# To reproduce the result:

* cd cpu/script

* sbatch master.sh
