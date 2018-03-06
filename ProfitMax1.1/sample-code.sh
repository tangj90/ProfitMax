#!/bin/bash
echo Begin

cmd="./ProfitMax1.1.o"
${cmd} -func=0 -gname=facebook
read -rsp $'Press any key to continue...\n' -n 1 key
#${cmd} -func=1 -gname=facebook -alg=random
#${cmd} -func=1 -gname=facebook -alg=highdegree
#${cmd} -func=1 -gname=facebook -alg=infmax
${cmd} -func=1 -gname=facebook -ptype=0 -alg=greedy
#${cmd} -func=1 -gname=facebook -ptype=0 -alg=doublegreedy
#${cmd} -func=1 -gname=facebook -ptype=1 -alg=greedy
${cmd} -func=1 -gname=facebook -ptype=1 -alg=doublegreedy
