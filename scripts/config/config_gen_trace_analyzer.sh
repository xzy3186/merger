#!bin/bash
WORKDIR=/home/ryokoyam/vandle/trace_yso/work
INPUT=${WORKDIR}/config/config_trace_analyzer.yaml

#for((i=0; i<20; i++))
for((i=0; i<30; i++))
do
    #((a = 8000 + i*100 ))
    ((a = 5000 + i*1000 ))
    OUTPUT=${WORKDIR}/config/config_trace_analyzer/config${a}_newfunc.yaml
    python ${WORKDIR}/config/config_gen.py -i ${INPUT} -o ${OUTPUT} -b PspmtAnalyzer -k TraceMaxQDCRatio -v ${a}
done

