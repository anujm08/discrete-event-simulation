randomfile="randomseeds.txt"
seed=999;
printlog=0;
numUsers=50;
bufferSize=200;
numCores=8;
threadLimit=100;

tQuantum=0.1;
csTime=0.001;
totalSimulationTime=3000;
transientTime=200;

thinkMean=10;
thinkVariance=4;

serviceConst=2;
serviceUniformMin=1;
serviceUniformMax=3;
serviceExpMean=2;
serviceProb1=0.2;
serviceProb2=0.5;

timeOutMinm=10;
timeOutExpMean=5;

i=1

make
mkdir -p input
mkdir -p output
> results.txt
for numUsers in ` seq 5 5 500`
do
	for testcase in ` seq 0 9`
	do
		seed=$(head -n $i $randomfile | tail -n 1 | awk '{print $1}')

		> "input/input-$numUsers-$testcase.in"
		echo "$seed $printlog" >> "input/input-$numUsers-$testcase.in"
		echo "$numUsers $numCores $bufferSize $threadLimit" >> "input/input-$numUsers-$testcase.in"
		echo "$tQuantum $csTime" >> "input/input-$numUsers-$testcase.in"
		echo "$transientTime $totalSimulationTime" >> "input/input-$numUsers-$testcase.in"
		echo "$thinkMean $thinkVariance" >> "input/input-$numUsers-$testcase.in"
		echo "$serviceConst $serviceProb1" >> "input/input-$numUsers-$testcase.in"
		echo "$serviceUniformMin $serviceUniformMax $serviceProb2" >> "input/input-$numUsers-$testcase.in"
		echo "$serviceExpMean" >> "input/input-$numUsers-$testcase.in"
		echo "$timeOutMinm $timeOutExpMean" >> "input/input-$numUsers-$testcase.in"

		./simulation < "input/input-$numUsers-$testcase.in" > "output/output$numUsers-$testcase.out"

		echo "$numUsers $testcase $(cat "output/output$numUsers-$testcase.out" | awk '{print $NF}' | tr '\n' ' ')"  >> results.txt
		i=$((i+1))
	done
done

# python plot.py