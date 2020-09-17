#!/bin/bash
#SBATCH --time=12:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=20
#SBATCH --mem=20gb
#SBATCH --job-name=MissionControl
#SBATCH --partition=cs

source /etc/profile.d/modules.sh

module load singularity

singularity exec -B /tmp:/tmp -B /home/minhyuk2/git_repos/QuickScripts:/home/minhyuk2/git_repos/QuickScripts -B /scratch:/scratch /scratch/users/minhyuk2/singularity_images/bio.sif /bin/bash ./scripts/batch_script.run
