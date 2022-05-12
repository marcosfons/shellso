# TODO

## Important

- Fix broken pipe when 3 commands are connected. The program finishes instead of
	returning and asking a new prompt. Eg.:
		```shell
		Enter a command: cat file.txt | grep some-word | grep some
		```
- Implement tests for `shell.c` and `background_jobs.c`
- Implement shell builtin commands:
		- cd
		- fg
		- jobs
		- time (maybe)
		- fim
		- exit (alias to fim, maybe)
		- help

- Implement handling for builtin commands (Hash table and code execution)
		- Hash table already implemented. Still missing code execution from it
- Implement better code for file redirection

## Error handling

- Check erorr handling in `command_parser.c`
- Implement error handling in `shell.c`


# Others

- Free all memory used and check memory leaks with `Valgrind`