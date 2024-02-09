#!/bin/bash

rm -f tests.out 2>&1 >/dev/null
make
input="./tests.in"
while IFS= read -r line
do
  echo "./prog1 $line"
  ./prog1 $line | tee -a tests.out
done < "$input"
diff -iBw tests.reference tests.out
rc=$?
if [[ $rc == 0 ]]; then
  echo "All tests passed"
else
  echo "One or more tests failed"
fi
exit $rc
