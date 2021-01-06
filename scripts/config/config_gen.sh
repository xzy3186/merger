#!/bin/bash
DISTANCE=${4}
WORKDIR=/nics/b/home/ryokoyam/vandle_work
CONFIGDIR=/nics/b/home/ryokoyam/opt/merger/config
INPUT=${WORKDIR}/config/config.yaml
OUTPUT=${WORKDIR}/config/configs/config${1}_${3}.yaml
BRIPS=${WORKDIR}/bigrips_rootfiles/run${2}isotope_gated_widegate.root
PIXIE=${WORKDIR}/processed_rootfiles_lrqdc/production_${1}.root
IMPMERGED=${WORKDIR}/scratch/mergedImp${1}_${3}.root
BETAMERGED=${WORKDIR}/scratch/mergedBeta${1}_${3}_r${DISTANCE}.root
IMPBETAMERGED=${WORKDIR}/scratch/mergedImpBeta${1}_${3}_r${DISTANCE}.root
TREE=${3}

python ${CONFIGDIR}/config_gen.py -i ${INPUT} -o ${OUTPUT} -b BigRIPSTSScannor -k InputFileName -v ${BRIPS}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BigRIPSTSScannor -k TreeName -v ${TREE}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b ImplantTSScannor -k InputFileName -v ${PIXIE}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BigRIPSImplantMerger -k OutputFileName -v ${IMPMERGED}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b MergedImplantTSScannor -k InputFileName -v ${IMPMERGED}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BetaTSScannor -k InputFileName -v ${PIXIE}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b ImplantBetaMerger -k OutputFileName -v ${BETAMERGED}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BetaImplantMerger -k OutputFileName -v ${IMPBETAMERGED}
python ${CONFIGDIR}/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BetaTreeMerger -k CorrelationRadius -v ${DISTANCE}
