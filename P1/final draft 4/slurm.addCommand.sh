#! /bin/bash

# EDIT THIS FOR SACCT TO SHOW A DIFFERENT JOBNAME
#SBATCH --job-name="CHRIS1"

#SBATCH -p r630-hpc3
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=1G

# EDIT THIS FOR JOB TIME (d-hh:mm:ss) 2 days is max and will usuaally take longer to start
#SBATCH --time=2-00:00:00

# ###########################################################################################
# !!!!!!!Replace command line arguments following 'srun' below !!!!!!!!!!!!!!!!!!!!!!!
# ###########################################################################################

./pathway sickle_cell_data_discretized.txt 47323 250 61 sickle_cell_output.txt
