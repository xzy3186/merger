import subprocess
import sys
from multiprocessing import Pool

nuclei = {"CuNi":["Ni78","Cu80","Cu77","Cu78","Cu79","Ni75","Ni76","Ni77","Ni79"],"GaZn":["Ga87","Ga86","Ga85","Ga84","Ga83","Zn80","Zn81","Zn82","Zn83","Zn84"],"GeAsCo":["Ge85","Ge86","Ge87","Ge88","Ge89","As88","As89","As90","As91","Co73","Co74","Co75"]}
nuclei["As"] = ["As88","As89","As90","As91"]
nuclei["Ge"] = ["Ge85","Ge86","Ge87","Ge88","Ge89"]
nuclei["Ga"] = ["Ga83","Ga84","Ga85","Ga86","Ga87"]
nuclei["Ga85"] = ["Ga85"]
nuclei["Ga84"] = ["Ga84"]
nuclei["Ga83"] = ["Ga83"]
nuclei["Zn"] = ["Zn80","Zn81","Zn82","Zn83","Zn84"]
nuclei["Cu"] = ["Cu77","Cu78","Cu79","Cu80","Cu81"]
nuclei["Ni"] = ["Ni75","Ni76","Ni77","Ni78","Ni79"]
nuclei["Co"] = ["Co73","Co74","Co75"]
nuclei["lrqdctest"] = ["Cu82"]

def merge(run_argv):
	exe = 'merger_main'
	#exe = 'merger_pid'
	conf_file = './config/configs/config' + str(run_argv[0]) + '_' + run_argv[1] + '.yaml'
	log_file = './logs/merger_' + str(run_argv[0]) + '_' + run_argv[1]
	cmd = exe + ' -c ' + conf_file + ' >> ' + log_file + '.log 2>> ' + log_file + '.errlog'
	print (cmd)
	sp = subprocess.call([cmd], shell=True)

if __name__ == "__main__":
	key = sys.argv[4]
	p = Pool(int(sys.argv[3]))
	scan_data = []
	for nucl in nuclei[key]:
		for i in range(int(sys.argv[1]),int(sys.argv[2])):
			scan_data.append([i,nucl])
	
	p.map(merge, scan_data)
