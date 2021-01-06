import subprocess
import sys

nuclei = {"CuNi":["Ni78","Cu80","Cu77","Cu78","Cu79","Ni75","Ni76","Ni77","Ni79"],"GaZn":["Ga87","Ga86","Ga85","Ga84","Ga83","Zn80","Zn81","Zn82","Zn83","Zn84"],"GeAsCo":["Ge85","Ge86","Ge87","Ge88","Ge89","As88","As89","As90","As91","Co73","Co74","Co75"]}
nuclei["As"] = ["As88","As89","As90","As91"]
nuclei["Ge"] = ["Ge85","Ge86","Ge87","Ge88","Ge89"]
nuclei["Ga"] = ["Ga83","Ga84","Ga85","Ga86","Ga87"]
nuclei["Ga85"] = ["Ga85"]
nuclei["Ga84"] = ["Ga84"]
nuclei["Ga83"] = ["Ga83"]
nuclei["Zn"] = ["Zn80","Zn81","Zn82","Zn83","Zn84"]
nuclei["Cu"] = ["Cu77","Cu78","Cu79","Cu80","Cu81"]
nuclei["Cu82"] = ["Cu82"]
nuclei["Ni"] = ["Ni75","Ni76","Ni77","Ni78","Ni79"]
nuclei["Co"] = ["Co73","Co74","Co75"]
nuclei["lrqdctest"] = ["Cu82"]

if __name__ == "__main__":
	key = sys.argv[1]
	for nucl in nuclei[key]:
		cmd = "correction_main -c config/corrector/config_"+nucl+".yaml -o mergedCorrectedBeta_"+nucl+"_r0.2 >> logs/corrector_"+nucl+".log 2>> logs/corrector_"+nucl+".errlog"
		#cmd = "correction_main -c config/corrector/config_"+nucl+".yaml -o corrected_rootfiles/mergedCorrectedBeta_"+nucl+"_r0.2.root"
		sp = subprocess.call([cmd], shell=True)
	
