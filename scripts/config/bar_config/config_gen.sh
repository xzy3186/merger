#!bin/bash
WORKDIR=/nics/b/home/ryokoyam/vandle_work/config/bar_config
CONFIGDIR=/nics/b/home/ryokoyam/opt/merger/config
INPUT=${WORKDIR}/config_vandle_corrector_source.yaml
OUTPUT=${WORKDIR}/config_vandle_corrector.yaml

python ${CONFIGDIR}/config_gen.py -i ${INPUT} -o ${OUTPUT} -b VANDLEBars -k ToFOffset -v ${1}

