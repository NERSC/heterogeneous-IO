# Heterogenous-IO
* Understanding the IO Performance gap between KNL and Haswell
* Design Heterogenous IO solution along ExaScale Computing Roadmap

# To visualize the result: 

* IO Gap caused by CPU frequencies [Notebook](https://github.com/NERSC/heterogeneous-IO/blob/master/cpu/plot/hio.ipynb):
	
	Example:	
	![Alt text](https://cloud.githubusercontent.com/assets/1396867/25308126/2d8d406e-2763-11e7-88f5-323c7e73b7b5.png)
* IO Gap caused by page cache:

		https://github.com/NERSC/heterogeneous-IO/blob/master/cpu/plot/iostack.ipynb
		
* IO Gap caused by MPI and HDF5:


# To reproduce the result:

* cd cpu/script

* sbatch master.sh
