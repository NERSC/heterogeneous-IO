# Heterogenous-IO
* Understanding the IO Performance gap between KNL and Haswell
* Design Heterogenous IO solution along ExaScale Computing Roadmap

# To visualize the result: 

* IO Gap caused by CPU frequencies:

		https://github.com/NERSC/heterogeneous-IO/blob/master/cpu/plot/hio.ipynb

* IO Gap caused by page cache:

		https://github.com/NERSC/heterogeneous-IO/blob/master/cpu/plot/iostack.ipynb
		
* IO Gap caused by MPI and HDF5:


# To reproduce the result:

* cd cpu/script

* sbatch master.sh
