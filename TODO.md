# TODO


## Important

- [x] Fix broken pipe when 3 commands are connected. The program finishes instead of
	returning and asking a new prompt. Eg.:
		```shell
		Enter a command: cat file.txt | grep some-word | grep some
		```
- [ ] Implement tests for `shell.c` and `background_jobs.c`
- [ ] Implement shell builtin commands:
	- [x] cd
	- [x] fg
	- [x] jobs
	- [ ] time (maybe)
	- [x] exit
	- [x] fim (alias to exit, maybe)
	- [ ] help
	- [x] alias

- [ ] Create a process group for each command chain. In this case,
The signal handling works for each command

- [x] Implement handling for builtin commands (Hash table and code execution)
		- [x] Hash table already implemented. Still missing code execution from it
- [x] Implement better code for file redirection


## Error handling

- [x] Check error handling in `command_parser.c`
- [x] Implement error handling in `shell.c`


# Others

- [x] Free all memory used and check memory leaks with `Valgrind`
- [x] Support aliases
