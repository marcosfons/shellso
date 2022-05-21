echo "Testing" > testing.txt
ls
cat testing.txt
rm testing.txt

ls | grep b | grep bin > out.txt
cat out.txt
rm out.txt

ls | grep b > input.txt

grep bin < input.txt > out.txt
cat out.txt

ls > out.txt
cat out.txt

grep b < out.txt

ls | wc -l > out.txt

cat < out.txt && cat < out.txt
cat < out.txt > out2.txt && cat < out2.txt

rm out.txt out2.txt input.txt

cat src/main.c

