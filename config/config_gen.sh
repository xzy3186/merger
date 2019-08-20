#!bin/bash
WORKDIR=/path_to_work_dir
INPUT=${WORKDIR}/config/config.yaml
OUTPUT=${WORKDIR}/config/configs/config${1}.yaml
BRIPS=${WORKDIR}/rootfiles_bigrips/run${2}ga83_84_ni_86_gated.root
PIXIE=${WORKDIR}/processed_rootfiles/production_${1}_trace.root
IMPMERGED=${WORKDIR}/merged_rootfiles/mergedImp${1}_${3}.root
BETAMERGED=${WORKDIR}/merged_rootfiles/mergedBeta${1}_${3}.root
IMPBETAMERGED=${WORKDIR}/merged_rootfiles/mergedImpBeta${1}_${3}.root
TREE=${3}

python ${WORKDIR}/../merger/config/config_gen.py -i ${INPUT} -o ${OUTPUT} -b BigRIPSTSScannor -k InputFileName -v ${BRIPS}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BigRIPSTSScannor -k TreeName -v ${TREE}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b ImplantTSScannor -k InputFileName -v ${PIXIE}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BigRIPSImplantMerger -k OutputFileName -v ${IMPMERGED}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b MergedImplantTSScannor -k InputFileName -v ${IMPMERGED}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BetaTSScannor -k InputFileName -v ${PIXIE}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b ImplantBetaMerger -k OutputFileName -v ${BETAMERGED}
python ${WORKDIR}/../merger/config/config_gen.py -i ${OUTPUT} -o ${OUTPUT} -b BetaImplantMerger -k OutputFileName -v ${IMPBETAMERGED}
