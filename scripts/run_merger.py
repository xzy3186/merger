import subprocess
import sys
from multiprocessing import Pool

def merge(run_argv):
	exe = 'merger_main'
	conf_file = './config/configs/config' + str(run_argv[0]) + '.yaml'
	log_file = './logs/merger_' + str(run_argv[0])
	cmd = exe + ' -c ' + conf_file + ' >> ' + log_file + '.log 2>> ' + log_file + '.errlog'
	print (cmd)
	sp = subprocess.call([cmd], shell=True)

if __name__ == "__main__":
	key = sys.argv[4]
	p = Pool(int(sys.argv[3]))
	scan_data = []
	for i in range(int(sys.argv[1]),int(sys.argv[2])):
		scan_data.append([i])
	
	p.map(merge, scan_data)
