import subprocess
import sys

if __name__ == "__main__":
	nuclei = ["Ni78","Cu81","Cu80","Cu77","Cu78","Cu79","Ni75","Ni76","Ni77","Ni79","Ga87","Ga86","Ga85","Ga84","Ga83","Zn80","Zn81","Zn82","Zn83","Zn84","Ge85","Ge86","Ge87","Ge88","Ge89","As88","As89","As90","As91","Co73","Co74","Co75"]
	for nucl in nuclei:
		cmd = "anamerger_main -c config/anamerger/config_"+nucl+".yaml -o anamerger_outputs/anamerger_output_"+nucl+"_r0.4.root >> logs/anamerger_"+nucl+".log 2>> logs/anamerger_"+nucl+".errlog"
		sp = subprocess.call([cmd], shell=True)
	
