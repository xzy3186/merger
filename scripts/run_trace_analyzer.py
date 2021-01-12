import subprocess
import sys
from multiprocessing import Pool


def scan(i):
    inputname = "./rootfiles_vandle/17N_"+str(i)
    fname = "17N_pspmt_"+str(i)+".root"
    tmpoutput = "./outfiles/"
    logfile = "./logs/trace_analyzer_main_"+str(i)
    conf = "./config/config_trace_analyzer.yaml"
    cmd="trace_analyzer_main "+ inputname + " " + tmpoutput + fname + " " + conf + " >> " + logfile + ".log 2>> " + logfile + ".errlog"
    print (cmd)
    sp = subprocess.call([cmd], shell=True)
	
if __name__ == "__main__":
	first = int(sys.argv[1])
	last = int(sys.argv[2])

	p = Pool(int(sys.argv[3]))
	scan_data = []
	for i in range(first,last):
		scan_data.append(i)
	
	p.map(scan, scan_data)
	p.join()

