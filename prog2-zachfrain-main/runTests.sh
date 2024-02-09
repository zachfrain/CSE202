
#!/bin/bash
rm -f tests.out 2>&1 >/dev/null
echo -e "\nRunning make clean"
make -f makefile clean
echo -e "\nRunning make prog2"
make
rc=$?
total=0
if [[ $rc == 0 ]]; then
  tests=( "abs 12"
          "abs -25"  
          "average 25 12"
          "average 3000000000 2000000000"
          "quadratic 1 -7 10"
          "quadratic 1 6 5"
          "quadratic 1 -2 1"
          "quadratic 1 4 5"
          "min 100 50 25 75 100"
          "min 500 400 300 200 100"
          "sum Emma 1000 Patrick 1000 Jacques 1000 Helen 1000"
          "sum Monday 65 Tuesday 71 Wednesday 78 Thursday 68"
          "product 3.5 4.5 2.5 2.0 2.0 2.0"
          "product 3.5 4.5 2.5 0.5 0.5 0.5"
          "product 0.5 2 0.5 2.0 0.5 2.0"
        )

  for test in "${tests[@]}"; do
    total=$((total + 1))
    echo -e "\nTest $total with args: $test"
    ./prog2 $test | tee -a tests.out
  done
paste -d '|' tests.out tests.reference | grep -Po '^(.*)\|+\K\1$' > out
count=0
input="./out"
while IFS= read -r line
do
  count=$((count + 1))
done < "$input"

if [[ $count == 15 ]]; then
   echo -e "\nAll tests passed"
   echo -e "Score: 90/90\n"
   exit 0
else
   echo -e "\n$count out of 15 passed"
   count=$((count * 6))
   echo -e "Score: $count /90\n"
   exit 1
fi

fi
