# Test the cd builtin command

ls

cd src
ls

cd shell
ls

cd ..
ls

cd ..
ls

cd src && cd shell
ls

cd .. && cd .. && cd src && cd shell
ls

cd .. || cd ..
pwd
ls
