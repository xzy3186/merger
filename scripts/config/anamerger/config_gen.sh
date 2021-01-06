#!bin/bash
WORKDIR=/nics/b/home/ryokoyam/vandle_work
CONFIGDIR=/nics/b/home/ryokoyam/opt/merger/config
INPUT=${WORKDIR}/config/anamerger/anamerger_config.yaml
OUTPUT=${WORKDIR}/config/anamerger/config_${1}.yaml
MERGERLISTNAME=${WORKDIR}/anamergerLists/anamergerList${1}.txt
TIMEWINDOW=${2}

python ${CONFIGDIR}/config_gen.py -i ${INPUT} -o ${OUTPUT} -b AnamergerMain -k MergerListName -v ${MERGERLISTNAME}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b AnamergerMain -k TimeWindow -v ${TIMEWINDOW}
